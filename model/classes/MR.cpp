﻿#include <cassert>
#include <ranges>

#include "model/classes/MR.h"
#include "model/classes/Project.h"

namespace gpr::api
{
	MR::MR(GPManager &manager, Data data, Project &project)
		: QObject(&project)
		, m_data{std::move(data)}
		, m_manager{manager}
		, m_project{project}
	{}

	int MR::id() const
	{
		return m_data.id;
	}

	int MR::iid() const
	{
		return m_data.iid;
	}

	void MR::update(Data data)
	{
		if (data.id != m_data.id)
		{
			assert(false && "Invalid MR data");
			return;
		}

		m_data = std::move(data);
		Q_EMIT modified();
	}

	Project &MR::project()
	{
		return m_project;
	}

	Project const &MR::project() const
	{
		return m_project;
	}

	QDateTime MR::createdAt() const
	{
		return m_data.created;
	}

	QDateTime MR::updatedAt() const
	{
		return m_data.updated;
	}

	QString MR::title() const
	{
		return m_data.title;
	}

	User const &MR::author() const
	{
		return m_data.author;
	}

	User const &MR::assignee() const
	{
		return m_data.assignee;
	}

	void MR::setAssignee(User assignee)
	{
		m_data.assignee = std::move(assignee);
		Q_EMIT modified();
	}

	User const &MR::reviewer() const
	{
		return m_data.reviewer;
	}

	void MR::setReviewer(User reviewer)
	{
		m_data.reviewer = std::move(reviewer);
		Q_EMIT modified();
	}

	QString MR::sourceBranch() const
	{
		return m_data.sourceBranch;
	}

	QString MR::targetBranch() const
	{
		return m_data.targetBranch;
	}

	std::vector<QPointer<Discussion>> const &MR::discussions() const
	{
		return m_discussions;
	}

	QString MR::url() const
	{
		return m_data.url;
	}

	QString MR::mergeStatus() const
	{
		return m_data.mergeStatus;
	}

	bool MR::hasNotes() const
	{
		return m_data.hasNotes;
	}

	QPointer<Pipeline> MR::pipeline() const
	{
		return m_pipeline;
	}

	void MR::setPipeline(QPointer<Pipeline> pipeline)
	{
		if(m_pipeline != pipeline)
		{
			m_pipeline = pipeline;
			Q_EMIT modified();
		}
	}

	bool MR::hasUnreadNotes() const
	{
		auto notes = m_discussions | std::views::transform(&Discussion::notes) | std::views::join;
		return !std::ranges::all_of(notes, &Note::isRead);
	}

	bool MR::discussionsLoaded() const
	{
		return m_discussionsLoaded;
	}

	std::vector<QString> const &MR::approvedBy() const
	{
		return m_approvedBy;
	}

	bool MR::isApprovedBy(User const &user) const
	{
		return isApprovedBy(user.username);
	}

	bool MR::isApprovedBy(QString const &username) const
	{
		return std::ranges::contains(m_approvedBy, username);
	}

	void MR::setApprovedBy(std::vector<QString> list)
	{
		if(m_approvedBy != list)
		{
			m_approvedBy = std::move(list);
			Q_EMIT modified();
		}
	}

	void MR::updateDiscussions(std::vector<std::pair<Discussion::Data, std::vector<Note::Data>>> discussions)
	{
		auto const removed =
			m_discussions
			| std::views::filter(
				[&discussions](auto discussion)
				{ return !std::ranges::contains(discussions, discussion->id(), [](auto const &p) { return p.first.id; }); })
			| std::views::transform(&Discussion::id) | std::ranges::to<std::vector>();

		for (auto &[discussionData, notes] : discussions)
		{
			QPointer<Discussion> discussion;

			if (discussion = findDiscussion(discussionData.id))
			{
				discussion->update(std::move(discussionData));
			}
			else
			{
				discussion = m_discussions.emplace_back(new Discussion(m_manager, std::move(discussionData), *this));
				connectDiscussion(discussion);
				Q_EMIT discussionAdded(discussion);
			}
			discussion->updateNotes(std::move(notes));
		}

		for (auto const &discussionId : removed)
		{
			auto pos = std::ranges::find(m_discussions, discussionId, &Discussion::id);
			assert(pos != m_discussions.cend());
			Q_EMIT discussionRemoved(*pos);
			m_discussions.erase(pos);
		}

		Q_EMIT modified();

		if(!m_discussionsLoaded)
		{
			std::ranges::for_each(m_discussions, &Discussion::setLoadFinished);
			m_discussionsLoaded = true;
		}
	}

	void MR::markDiscussionsRead()
	{
		std::ranges::for_each(m_discussions, &Discussion::markRead);
		Q_EMIT modified();
	}

	QString MR::noteUrl(gpr::api::Note const &note) const
	{
		return QString{"%1#note_%2"}.arg(url()).arg(note.id());
	}

	bool MR::isUserInvolved(User const &user) const
	{
		return isUserInvolved(user.username);
	}

	bool MR::userCanApprove(QString const &username) const
	{
		return !username.isEmpty() && (m_data.reviewer.username == username || m_data.assignee.username == username);
	}

	bool MR::userCanApprove(User const &user) const
	{
		return userCanApprove(user.username);
	}

	bool MR::isUserInvolved(QString const &username) const
	{
		// TODO: Проверять упоминания пользователя в дискуссиях.
		return m_data.author.username == username || m_data.assignee.username == username || m_data.reviewer.username == username;
	}

	QPointer<Discussion> MR::findDiscussion(QString const &id) const
	{
		if(auto const pos = std::ranges::find(m_discussions, id, &Discussion::id); pos != m_discussions.end())
		{
			return *pos;
		}

		return nullptr;
	}

	void MR::connectDiscussion(QPointer<Discussion> discussion)
	{
		connect(discussion, &Discussion::modified, [this, discussion]{ Q_EMIT discussionUpdated(discussion); });

		connect(discussion, &Discussion::noteAdded  , [this, discussion](QPointer<Note> note){ Q_EMIT discussionNoteAdded(discussion, note); });
		connect(discussion, &Discussion::noteUpdated, [this, discussion](QPointer<Note> note){ Q_EMIT discussionNoteUpdated(discussion, note); });
		connect(discussion, &Discussion::noteRemoved, [this, discussion](QPointer<Note> note){ Q_EMIT discussionNoteRemoved(discussion, note); });
	}

} // namespace gpr::api
