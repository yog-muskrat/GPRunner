#include <cassert>
#include <ranges>

#include "model/classes/Pipeline.h"
#include "model/classes/Project.h"
#include "model/classes/MR.h"
#include "GPManager.h"

namespace gpr::api
{
	Project::Project(GPManager &manager, Data data, QObject *parent)
		: QObject(parent)
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

	QString Project::name() const
	{
		return m_data.name;
	}

	void Project::setName(QString name)
	{
		m_data.name = std::move(name);
		Q_EMIT modified();
	}

	QString Project::url() const
	{
		return m_data.url;
	}

	void Project::setUrl(QString url)
	{
		m_data.url= std::move(url);
		Q_EMIT modified();
	}

	QString Project::avatarUrl() const
	{
		return m_data.avatarUrl;
	}

	void Project::setAvatarUrl(QString avatarUrl)
	{
		m_data.avatarUrl= std::move(avatarUrl);
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
			if (auto mr = findMRById(mrData.id))
			{
				mr->update(std::move(mrData));
				Q_EMIT mrUpdated(mr);
			}
			else
			{
				auto newMr = new MR(m_manager, std::move(mrData), *this);
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
			auto newPipeline = new Pipeline(m_manager, std::move(data), this);
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

	void Project::connectMR(QPointer<MR> mr)
	{
		connect(mr, &MR::discussionAdded, [this, mr](auto discussion) { Q_EMIT mrDiscussionAdded(mr, discussion); });
		connect(mr, &MR::discussionUpdated, [this, mr](auto discussion) { Q_EMIT mrDiscussionUpdated(mr, discussion); });
		connect(mr, &MR::discussionRemoved, [this, mr](auto discussion) { Q_EMIT mrDiscussionRemoved(mr, discussion); });

		connect(
			mr,
			&MR::discussionNoteAdded,
			[this, mr](auto discussion, auto note) { Q_EMIT mrDiscussionNoteAdded(mr, discussion, note); });
		connect(
			mr,
			&MR::discussionNoteUpdated,
			[this, mr](auto discussion, auto note) { Q_EMIT mrDiscussionNoteUpdated(mr, discussion, note); });
		connect(
			mr,
			&MR::discussionNoteRemoved,
			[this, mr](auto discussion, auto note) { Q_EMIT mrDiscussionNoteRemoved(mr, discussion, note); });
	}
} // namespace gpr::api
