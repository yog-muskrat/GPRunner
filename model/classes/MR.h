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
			QString author;
			QString assignee;
			QString reviewer;
			QString sourceBranch;
			QString targetBranch;
			QString url;
			QStringList approvedBy;
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

		QString author() const;
		void setAuthor(QString author);

		QString assignee() const;
		void setAssignee(QString assignee);

		QString reviewer() const;
		void setReviewer(QString reviewer);

		QString sourceBranch() const;
		void setSourceBranch(QString sourceBranch);

		QString targetBranch() const;
		void setTargetBranch(QString targetBranch);

		QString url() const;
		void setUrl(QString url);

		QStringList approvedBy() const;
		void setApprovedBy(QStringList list);

		std::vector<Discussion> const &discussions() const;
		void updateDiscussions(std::vector<Discussion> discussions);

		friend auto operator<=>(MR const &, MR const &) = default;

	Q_SIGNALS:
		void modified();

	private:
		Data m_data;

		std::vector<Discussion> m_discussions;
	};
} // namespace gpr::api
