#pragma once

#include <QObject>

#include "model/classes/GPClasses.h"

class GPManager;

namespace gpr::api
{
	class Job : public QObject
	{
		Q_OBJECT
	public:
		struct Data
		{
			int id{};
			QString name;
			QString stage;
			QString status;
			QString url;
			double duration{};
			QDateTime created;
			QDateTime started;
			QDateTime finished;
			User user;
			//TODO: tags
			//TODO: runner
		};

		Job(GPManager &manager, Data data, QObject *parent = nullptr);

		void update(Data data);

		int id() const;

		QString name() const;
		QString stage() const;
		QString status() const;
		QString url() const;
		double duration() const;
		QDateTime created() const;
		QDateTime started() const;
		QDateTime finished() const;
		User const &user() const;

	Q_SIGNALS:
		void modified();

	private:
		Data m_data;
		GPManager &m_manager;
	};
} // namespace gpr::api
