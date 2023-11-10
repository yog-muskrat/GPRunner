#include <cassert>
#include <ranges>

#include "model/classes/Pipeline.h"
#include "model/classes/Project.h"
#include "model/classes/MR.h"

namespace gpr::api
{
	Project::Project(Data data, QObject *parent)
		: QObject(parent)
		, m_data{std::move(data)}
	{}

	int Project::id() const
	{
		return m_data.id;
	}

	void Project::update(Data data)
	{
		if (data.id != m_data.id)
		{
			assert(false && "Invalid project data");
			return;
		}

		m_data = std::move(data);
		Q_EMIT modified();
	}

	QString Project::name() const
	{
		return m_data.name;
	}

	void Project::setName(QString name)
	{
		m_data.name = std::move(name);
		Q_EMIT modified();
	}

	QStringList const &Project::branches() const
	{
		return m_branches;
	}

	void Project::setBranches(QStringList branches)
	{
		m_branches = std::move(branches);
		Q_EMIT modified();
	}

	std::vector<QPointer<MR>> const &Project::openMRs() const
	{
		return m_openMRs;
	}

	void Project::updateMRs(std::vector<MR::Data> mrs)
	{
		std::vector<QPointer<MR>> const removed = m_openMRs
			| std::views::filter([&mrs](auto const &mr) { return !std::ranges::contains(mrs, mr->id(), &MR::Data::id); })
			| std::ranges::to<std::vector>();

		for (auto &mrData : mrs)
		{
			if (auto mr = findMR(mrData.id))
			{
				mr->update(std::move(mrData));
				Q_EMIT mrUpdated(mr);
			}
			else
			{
				auto newMr = new MR(std::move(mrData), this);
				m_openMRs.push_back(newMr);
				connectMR(newMr);

				Q_EMIT mrAdded(newMr);
			}
		}

		for (auto const &mr : removed)
		{
			Q_EMIT mrRemoved(mr);
			std::erase(m_openMRs, mr);
			mr->deleteLater();
		}

		Q_EMIT modified();
	}

	QPointer<MR> Project::findMR(int mrId) const
	{
		if (auto const pos = std::ranges::find(m_openMRs, mrId, &MR::id); pos != m_openMRs.cend())
		{
			return *pos;
		}
		return nullptr;
	}

	std::vector<QPointer<Pipeline>> const &Project::pipelines() const
	{
		return m_pipelines;
	}

	void Project::updatePipelines(std::vector<Pipeline::Data> pipelines)
	{
		std::vector<QPointer<Pipeline>> const removed = m_pipelines
			| std::views::filter([&pipelines](auto const &pipeline) { return !std::ranges::contains(pipelines, pipeline->id(), &Pipeline::Data::id); })
			| std::ranges::to<std::vector>();

		for (auto &pipelineData : pipelines)
		{
			if (auto pipeline = findPipeline(pipelineData.id))
			{
				pipeline->update(std::move(pipelineData));
				Q_EMIT pipelineUpdated(pipeline);
			}
			else
			{
				auto newPipeline = new Pipeline(std::move(pipelineData), this);
				m_pipelines.push_back(newPipeline);
				Q_EMIT pipelineAdded(newPipeline);
			}
		}

		for (auto const &pipeline : removed)
		{
			Q_EMIT pipelineRemoved(pipeline);
			std::erase(m_pipelines, pipeline);
			pipeline->deleteLater();
		}

		Q_EMIT modified();
	}

	QPointer<Pipeline> Project::findPipeline(int pipelineId) const
	{
		if (auto const pos = std::ranges::find(m_pipelines, pipelineId, &Pipeline::id); pos != m_pipelines.cend())
		{
			return *pos;
		}
		return nullptr;
	}

	void Project::connectMR(QPointer<MR> mr)
	{
		connect(mr, &MR::discussionAdded, [this, mr](Discussion const &discussion) { Q_EMIT mrDiscussionAdded(mr, discussion); });
		connect(mr, &MR::discussionUpdated, [this, mr](Discussion const &discussion) { Q_EMIT mrDiscussionUpdated(mr, discussion); });
		connect(mr, &MR::discussionRemoved, [this, mr](Discussion const &discussion) { Q_EMIT mrDiscussionRemoved(mr, discussion); });
	}
} // namespace gpr::api
