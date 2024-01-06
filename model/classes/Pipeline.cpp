#include <ranges>

#include "model/classes/Pipeline.h"
#include "model/classes/Project.h"
#include "GPManager.h"

namespace gpr::api
{
	Pipeline::Pipeline(Data data, Project &project)
		: QObject(&project)
		, m_data{std::move(data)}
		, m_project{project}
	{}

	int Pipeline::id() const
	{
		return m_data.id;
	}

	void Pipeline::update(Data data)
	{
		if (data.id != m_data.id)
		{
			assert(false && "Invalid pipeline data");
			return;
		}

		m_data = std::move(data);
		Q_EMIT modified();
	}

	Project &Pipeline::project()
	{
		return m_project;
	}

	Project const &Pipeline::project() const
	{
		return m_project;
	}

	void Pipeline::cancel()
	{
		project().manager().client().cancelPipeline(project().id(), id());
	}

	void Pipeline::retry()
	{
		project().manager().client().retryPipeline(project().id(), id());
	}

	Pipeline::State Pipeline::state() const
	{
		if(auto const st = m_data.status.toLower();
			    st == "failed")               return State::Failed;
		else if(st == "warning")              return State::Warning;
		else if(st == "canceled")             return State::Canceled;
		else if(st == "skipped")              return State::Skipped;
		else if(st == "success")              return State::Success;
		else if(st == "pending")              return State::Pending;
		else if(st == "running")              return State::Running;
		else if(st == "manual")               return State::Manual;
		else if(st == "scheduled")            return State::Scheduled;
		else if(st == "created")              return State::Created;
		else if(st == "preparing")            return State::Preparing;
		else if(st == "waiting_for_resource") return State::WaitingForResource;

		return State::Unknown;
	}

	QString Pipeline::status() const
	{
		return m_data.status;
	}

	void Pipeline::setStatus(QString status)
	{
		m_data.status = std::move(status);
		Q_EMIT modified();
	}

	QString Pipeline::source() const
	{
		return m_data.source;
	}

	void Pipeline::setSource(QString source)
	{
		m_data.source = std::move(source);
		Q_EMIT modified();
	}

	User const &Pipeline::user() const
	{
		return m_data.user;
	}

	void Pipeline::setUser(User user)
	{
		m_data.user = std::move(user);
		Q_EMIT modified();
	}

	QString Pipeline::ref() const
	{
		return m_data.ref;
	}

	void Pipeline::setRef(QString ref)
	{
		m_data.ref = std::move(ref);
		Q_EMIT modified();
	}

	QString Pipeline::url() const
	{
		return m_data.url;
	}

	void Pipeline::setUrl(QString url)
	{
		m_data.url = url;
		Q_EMIT modified();
	}

	QDateTime Pipeline::createdAt() const
	{
		return m_data.created;
	}

	void Pipeline::setCreatedAt(QDateTime created)
	{
		m_data.created = created;
		Q_EMIT modified();
	}

	QDateTime Pipeline::updatedAt() const
	{
		return m_data.updated;
	}

	void Pipeline::setUpdatedAt(QDateTime updated)
	{
		m_data.updated = updated;
		Q_EMIT modified();
	}

	std::vector<QPointer<Job>> const &Pipeline::jobs() const
	{
		return m_jobs;
	}

	void Pipeline::updateJobs(std::vector<Job::Data> data)
	{
		auto const removed = m_jobs
			| std::views::filter([&data](auto const &job){ return !std::ranges::contains(data, job->id(), &Job::Data::id); })
			| std::ranges::to<std::vector>();

		bool wasModified{false};

		for(auto job : data)
		{
			if(auto pos = std::ranges::find(m_jobs, job.id, &Job::id); pos != m_jobs.cend())
			{
				if(job.duration > (*pos)->duration())
				{
					(*pos)->update(std::move(job));
					Q_EMIT jobUpdated(*pos);
					wasModified = true;
				}
			}
			else
			{
				auto newJob = m_jobs.emplace_back(new Job(std::move(job), *this));
				Q_EMIT jobAdded(newJob);
				wasModified = true;
			}
		}

		for(auto const &job: removed)
		{
			Q_EMIT jobRemoved(job);
			std::erase(m_jobs, job);
			job->deleteLater();
			wasModified = true;
		}

		if(wasModified)
		{
			Q_EMIT modified();
		}
	}

} // namespace gpr::api
