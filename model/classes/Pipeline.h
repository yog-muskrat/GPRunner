#pragma once

#include <QObject>
#include <QDateTime>

#include "GPClasses.h"

namespace gpr::api
{
	class Pipeline : public QObject
	{
		Q_OBJECT

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
		};

		Pipeline(Data data, QObject *parent);

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

		friend auto operator<=>(Pipeline const &, Pipeline const &) = default;

	Q_SIGNALS:
		void modified();

	private:
		Data m_data;
	};
} // namespace gpr::api
