#pragma once

#include <QObject>
#include <QDateTime>

#include "GPClasses.h"

namespace gpr::api
{
	class MR : public QObject
	{
		Q_OBJECT
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

		MR(Data data, QObject *parent = 0);

		int id() const;
		int iid() const;

		void update(Data data);

		QDateTime createdAt() const;
		void setCreatedAt(QDateTime time);

		QDateTime updatedAt() const;
		void setUpdatedAt(QDateTime time);

		QString title() const;
		void setTitle(QString title);

		User author() const;
		void setAuthor(User author);

		User assignee() const;
		void setAssignee(User assignee);

		User reviewer() const;
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

		QString pipelineStatus() const;
		void setPipelineStatus(QString status);

		std::vector<QString> const &approvedBy() const;
		void setApprovedBy(std::vector<QString> list);
		bool isApprovedBy(User const &user) const;
		bool isApprovedBy(QString const &username) const;

		std::vector<Discussion> const &discussions() const;
		bool discussionsLoaded() const;
		void updateDiscussions(std::vector<Discussion> discussions);

		bool isUserInvolved(QString const &username) const;
		bool isUserInvolved(User const &user) const;

		friend auto operator<=>(MR const &, MR const &) = default;

	Q_SIGNALS:
		void modified();

		void discussionAdded(Discussion const &);
		void discussionUpdated(Discussion const &);
		void discussionRemoved(Discussion const &);

		void discussionNoteAdded(Discussion const &, Note const &);
		void discussionNoteUpdated(Discussion const &, Note const &);
		void discussionNoteRemoved(Discussion const &, Note const &);

	private:
		void updateDiscussionNotes(Discussion &discussion, std::vector<Note> notes);

		Discussion *findDiscussion(QString const &id);
		Note *findDiscussionNote(Discussion &discussion, int noteId) const;

		Data m_data;

		// NOTE: Флаг для отличия ситуаций изначальной загрузки дискуссий и создания новых.
		bool m_discussionsLoaded{false};
		std::vector<Discussion> m_discussions;
		std::vector<QString> m_approvedBy;
		QString m_pipelineStatus;
	};
} // namespace gpr::api
