#include <cassert>
#include <ranges>

#include "model/classes/Pipeline.h"
#include "model/classes/Project.h"
#include "model/classes/MR.h"
#include "GPManager.h"

namespace gpr::api
{
	Project::Project(GPManager &manager, Data data)
		: QObject(&manager)
		, m_data{std::move(data)}
		, m_manager{manager}
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

	GPManager &Project::manager()
	{
		return m_manager;
	}

	GPManager const &Project::manager() const
	{
		return m_manager;
	}

	void Project::markDiscussionsRead()
	{
		std::ranges::for_each(m_openMRs, &gpr::api::MR::markDiscussionsRead);
	}

	void Project::runPipeline(QList<gpr::Variable> const &variables, QString const &branch)
	{
		manager().client().runPipeline(id(), branch, variables);
	}

	QString Project::name() const
	{
		return m_data.name;
	}

	QString Project::url() const
	{
		return m_data.url;
	}

	QString Project::avatarUrl() const
	{
		return m_data.avatarUrl;
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
			if (auto mr = findMRById(mrData.id))
			{
				mr->update(std::move(mrData));
				Q_EMIT mrUpdated(mr);
			}
			else
			{
				auto newMr = new MR(std::move(mrData), *this);
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

	QPointer<MR> Project::findMRByIid(int mrIid) const
	{
		if (auto const pos = std::ranges::find(m_openMRs, mrIid, &MR::iid); pos != m_openMRs.cend())
		{
			return *pos;
		}
		return nullptr;
	}

	QPointer<MR> Project::findMRById(int mrId) const
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
		for (auto &pipelineData : pipelines)
		{
			updatePipeline(std::move(pipelineData));
		}

		// TODO: Проверять удаленные пайплайны

		Q_EMIT modified();
	}

	void Project::updatePipeline(Pipeline::Data data)
	{
		if(auto pipeline = findPipeline(data.id))
		{
			// NOTE: Имя пользователя получается отдельно, поэтому, чтобы оно не обнулилось, нужно сохранить его вручную.
			auto user = pipeline->user();
			pipeline->update(std::move(data));
			pipeline->setUser(std::move(user));
		}
		else
		{
			auto newPipeline = new Pipeline(std::move(data), *this);
			m_pipelines.push_back(newPipeline);
			Q_EMIT pipelineAdded(newPipeline);
		}
	}

	QPointer<Pipeline> Project::findPipeline(int pipelineId) const
	{
		if (auto const pos = std::ranges::find(m_pipelines, pipelineId, &Pipeline::id); pos != m_pipelines.cend())
		{
			return *pos;
		}
		return nullptr;
	}

	QList<Variable> Project::variables() const
	{
		return m_variables;
	}

	void Project::setVariables(QList<Variable> variables)
	{
		m_variables = std::move(variables);
		Q_EMIT modified();
	}

	void Project::connectMR(QPointer<MR> mr)
	{
		connect(mr, &MR::discussionAdded, this, &Project::mrDiscussionAdded);
		connect(mr, &MR::discussionUpdated, this, &Project::mrDiscussionUpdated);
		connect(mr, &MR::discussionRemoved, this, &Project::mrDiscussionRemoved);

		connect(mr, &MR::discussionNoteAdded, this, &Project::mrDiscussionNoteAdded);
		connect(mr, &MR::discussionNoteUpdated, this, &Project::mrDiscussionNoteUpdated);
		connect(mr, &MR::discussionNoteRemoved, this, &Project::mrDiscussionNoteRemoved);
	}
} // namespace gpr::api
