#pragma once

#include <QObject>

#include "model/classes/GPClasses.h"

class GPManager;

namespace gpr::api
{
	class Pipeline;

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
			QString runner;
			QStringList tags;
			double duration{};
			QDateTime created;
			QDateTime started;
			QDateTime finished;
			User user;
		};

		Job(Data data, Pipeline &pipeline);

		void update(Data data);

		int id() const;

		Pipeline &pipeline();
		Pipeline const &pipeline() const;

		QString name() const;
		QString stage() const;
		QString status() const;
		QString url() const;
		QString runner() const;
		QStringList tags() const;
		double duration() const;
		QDateTime created() const;
		QDateTime started() const;
		QDateTime finished() const;
		User const &user() const;

	Q_SIGNALS:
		void modified();

	private:
		Data m_data;
		Pipeline &m_pipeline;
	};
} // namespace gpr::api
