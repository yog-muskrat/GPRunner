#include "model/classes/Job.h"

namespace gpr::api
{
	Job::Job(GPManager &manager, Data data, QObject *parent)
		: QObject(parent)
		, m_data{std::move(data)}
		, m_manager{manager}
	{}

	void Job::update(Data data)
	{
		if (data.id != id())
		{
			assert(false && "Invalid data id");
			return;
		}

		m_data = std::move(data);
		Q_EMIT modified();
	}

	int Job::id() const
	{
		return m_data.id;
	}

	QString Job::name() const
	{
		return m_data.name;
	}

	QString Job::stage() const
	{
		return m_data.stage;
	}

	QString Job::status() const
	{
		return m_data.status;
	}

	QString Job::url() const
	{
		return m_data.url;
	}

	QString Job::runner() const
	{
		return m_data.runner;
	}

	QStringList Job::tags() const
	{
		return m_data.tags;
	}

	double Job::duration() const
	{
		return m_data.duration;
	}

	QDateTime Job::created() const
	{
		return m_data.created;
	}

	QDateTime Job::started() const
	{
		return m_data.started;
	}

	QDateTime Job::finished() const
	{
		return m_data.finished;
	}

	User const &Job::user() const
	{
		return m_data.user;
	}

} // namespace gpr::api
