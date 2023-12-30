#pragma once

#include <QObject>
#include <QDateTime>
#include <QQmlEngine>

#include "model/classes/GPClasses.h"
#include "model/classes/Job.h"

class GPManager;

namespace gpr::api
{
	class Pipeline : public QObject
	{
		Q_OBJECT
		QML_ELEMENT
	public:
		struct Data
		{
			int id {-1};

			QString status;
			QString source;
			User user;
			QString ref;
			QString url;
			QDateTime created;
			QDateTime updated;

			friend bool operator==(Data const &, Data const &) = default;
		};

		Pipeline(GPManager &manager, Data data, QObject *parent);

		int id() const;

		void update(Data data);

		QString status() const;
		void setStatus(QString status);

		QString source() const;
		void setSource(QString source);

		User const &user() const;
		void setUser(User user);

		QString ref() const;
		void setRef(QString ref);

		QString url() const;
		void setUrl(QString url);

		QDateTime createdAt() const;
		void setCreatedAt(QDateTime created);

		QDateTime updatedAt() const;
		void setUpdatedAt(QDateTime updated);

		std::vector<QPointer<Job>> const &jobs() const;
		void updateJobs(std::vector<Job::Data> data);

		//friend auto operator<=>(Pipeline const &, Pipeline const &) = default;

	Q_SIGNALS:
		void modified();
		void jobAdded(QPointer<Job>);
		void jobUpdated(QPointer<Job>);
		void jobRemoved(QPointer<Job>);

	private:
		Data m_data;
		GPManager &m_manager;
		std::vector<QPointer<Job>> m_jobs;
	};
} // namespace gpr::api
