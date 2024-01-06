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
	class Pipeline;

	class MR : public QObject
	{
		Q_OBJECT
		Q_PROPERTY(int id READ id)
		Q_PROPERTY(int iid READ iid)
		Q_PROPERTY(QString url READ url NOTIFY modified)
		Q_PROPERTY(User author READ author NOTIFY modified)
		Q_PROPERTY(User assignee READ assignee NOTIFY modified)
		Q_PROPERTY(User reviewer READ reviewer NOTIFY modified)
		Q_PROPERTY(Pipeline * pipeline READ pipeline NOTIFY modified)
		Q_PROPERTY(bool hasUnreadNotes READ hasUnreadNotes NOTIFY modified)
		

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

		Q_INVOKABLE void approve();
		Q_INVOKABLE void unapprove();
		Q_INVOKABLE void addDiscussion(QString noteText);
		Q_INVOKABLE void markDiscussionsRead();
		Q_INVOKABLE bool isApprovedBy(User const &user) const;
		Q_INVOKABLE bool isApprovedBy(QString const &username) const;
		Q_INVOKABLE bool isUserInvolved(QString const &username) const;
		Q_INVOKABLE bool isUserInvolved(User const &user) const;
		Q_INVOKABLE bool userCanApprove(QString const &username) const;
		Q_INVOKABLE bool userCanApprove(User const &user) const;

		QDateTime createdAt() const;
		QDateTime updatedAt() const;

		QString title() const;

		User const &author() const;

		User const &assignee() const;
		void setAssignee(User assignee);

		User const &reviewer() const;
		void setReviewer(User reviewer);

		QString sourceBranch() const;

		QString targetBranch() const;

		QString url() const;

		QString mergeStatus() const;

		bool hasNotes() const;
		bool hasUnreadNotes() const;

		QPointer<Pipeline> pipeline() const;
		void setPipeline(QPointer<Pipeline> pipeline);

		std::vector<QString> const &approvedBy() const;
		void setApprovedBy(std::vector<QString> list);

		std::vector<QPointer<Discussion>> const &discussions() const;

		bool discussionsLoaded() const;
		void updateDiscussions(std::vector<std::pair<Discussion::Data, std::vector<Note::Data>>> discussions);
		QPointer<Discussion> findDiscussion(QString const &id) const;

		QString noteUrl(gpr::api::Note const &note) const;

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
		QPointer<Pipeline> m_pipeline;
	};
} // namespace gpr::api
