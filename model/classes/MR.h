#pragma once

#include <QObject>
#include <QPointer>
#include <QDateTime>

#include "model/classes/GPClasses.h"
#include "model/classes/Discussion.h"
#include "model/classes/Pipeline.h"

class GPManager;

namespace gpr::api
{
	class Project;

	class MR : public QObject
	{
		Q_OBJECT
		Q_PROPERTY(int id READ id)
		Q_PROPERTY(int iid READ iid)

	public:
		struct Data
		{
			int id {-1};
			int iid {-1};

			QDateTime created;
			QDateTime updated;
			QString title;
			QString status;
			User author;
			User assignee;
			User reviewer;
			QString sourceBranch;
			QString targetBranch;
			QString url;
			QString mergeStatus;
			bool hasNotes{};
		};

		MR(GPManager &manager, Data data, Project &project);

		int id() const;
		int iid() const;

		void update(Data data);

		Project &project();
		Project const &project() const;

		QDateTime createdAt() const;
		void setCreatedAt(QDateTime time);

		QDateTime updatedAt() const;
		void setUpdatedAt(QDateTime time);

		QString title() const;
		void setTitle(QString title);

		User const &author() const;
		void setAuthor(User author);

		User const &assignee() const;
		void setAssignee(User assignee);

		User const &reviewer() const;
		void setReviewer(User reviewer);

		QString sourceBranch() const;
		void setSourceBranch(QString sourceBranch);

		QString targetBranch() const;
		void setTargetBranch(QString targetBranch);

		QString url() const;
		void setUrl(QString url);

		QString mergeStatus() const;
		void setMergeStatus(QString mergeStatus);

		bool hasNotes() const;
		void setHasNotes(bool hasNotes);

		bool hasNewNotes() const;

		Pipeline::Data const pipeline() const;
		void setPipeline(Pipeline::Data data);

		std::vector<QString> const &approvedBy() const;
		void setApprovedBy(std::vector<QString> list);
		bool isApprovedBy(User const &user) const;
		bool isApprovedBy(QString const &username) const;

		std::vector<QPointer<Discussion>> const &discussions() const;

		bool discussionsLoaded() const;
		void updateDiscussions(std::vector<std::pair<Discussion::Data, std::vector<Note::Data>>> discussions);
		QPointer<Discussion> findDiscussion(QString const &id) const;

		Q_INVOKABLE void markDiscussionsRead();
		QString noteUrl(gpr::api::Note const &note) const;

		bool isUserInvolved(QString const &username) const;
		bool isUserInvolved(User const &user) const;

		friend auto operator<=>(MR const &, MR const &) = default;

	Q_SIGNALS:
		void modified();

		void discussionAdded(QPointer<Discussion>);
		void discussionUpdated(QPointer<Discussion>);
		void discussionRemoved(QPointer<Discussion>);

		void discussionNoteAdded(QPointer<Discussion>, QPointer<Note>);
		void discussionNoteUpdated(QPointer<Discussion>, QPointer<Note>);
		void discussionNoteRemoved(QPointer<Discussion>, QPointer<Note>);

	private:
		void connectDiscussion(QPointer<Discussion> discussion);

		Data m_data;
		GPManager &m_manager;
		Project &m_project;

		// NOTE: Флаг первоначальной загрузки дискуссий
		bool m_discussionsLoaded{false};
		std::vector<QPointer<Discussion>> m_discussions;
		std::vector<QString> m_approvedBy;
		Pipeline::Data m_pipeline;
	};
} // namespace gpr::api
