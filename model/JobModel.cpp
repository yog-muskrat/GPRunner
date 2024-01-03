#include <ranges>

#include "model/JobModel.h"

#include "model/classes/Pipeline.h"
#include "model/classes/Job.h"

JobModel::JobModel(QObject *parent) : QAbstractTableModel(parent)
{

}

void JobModel::setPipeline(gpr::api::Pipeline *pipeline)
{
	beginResetModel();

	disconnectPipeline(m_pipeline);
	m_pipeline = pipeline;
	connectPipeline(m_pipeline);

	endResetModel();
}

void JobModel::clear()
{
	setPipeline(nullptr);
}

int JobModel::rowCount(const QModelIndex &parent) const
{
	if(parent.isValid() || !m_pipeline) return 0;

	return std::ssize(m_pipeline->jobs());
}

int JobModel::columnCount(const QModelIndex &) const
{
	return Column::Count;
}

QVariant JobModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(orientation != Qt::Horizontal || role != Qt::DisplayRole)
	{
		return QAbstractTableModel::headerData(section, orientation, role);
	}

	switch(section)
	{
		case Column::Id:       return QString{"Id"};
		case Column::Stage:    return QString{"Stage"};
		case Column::Name:     return QString{"Name"};
		case Column::Runner:   return QString{"Runner"};
		case Column::Tags:     return QString{"Tags"};
		case Column::Status:   return QString{"Status"};
		case Column::Duration: return QString{"Duration"};
		case Column::Started:  return QString{"Started"};
		case Column::Finished: return QString{"Finished"};
		default: break;
	}

	return QVariant();
}

QVariant JobModel::data(const QModelIndex &index, int role) const
{
	if(index.row() < 0 || index.row() >= rowCount() || index.column() < 0 || index.column() >= columnCount()) return {};

	auto const &job = m_pipeline->jobs().at(index.row());

	if(role == Qt::DisplayRole)
	{
		switch(index.column())
		{
			case Column::Id:       return job->id();
			case Column::Stage:    return job->stage();
			case Column::Name:     return job->name();
			case Column::Runner:   return job->runner();
			case Column::Tags:     return job->tags().join(", ");
			case Column::Status:   return job->status();
			case Column::Duration: return toTimeString(job->duration());
			case Column::Started:  return toDateTimeString(job->started());
			case Column::Finished: return toDateTimeString(job->finished());
			default:
				assert(false && "Unknown column");
				break;
		}
	}
	if(role == Qt::EditRole)
	{
		switch(index.column())
		{
			case Column::Id:       return job->id();
			case Column::Stage:    return job->stage();
			case Column::Name:     return job->name();
			case Column::Runner:   return job->runner();
			case Column::Tags:     return job->tags();
			case Column::Status:   return job->status();
			case Column::Duration: return job->duration();
			case Column::Started:  return job->started();
			case Column::Finished: return job->finished();
			default:
				assert(false && "Unknown column");
				break;
		}
	}
	else if(role == Role::UrlRole)
	{
		if(index.column() == Column::Id) return job->url();
		return QString{};
	}

	return {};
}

QHash<int, QByteArray> JobModel::roleNames() const
{
	auto names = QAbstractTableModel::roleNames();
	names.emplace(Role::UrlRole, "url");
	return names;
}

QString JobModel::toDateTimeString(QDateTime const &dt) const
{
	if(!dt.isValid()) return {};

	auto const format = dt.date() == QDate::currentDate() ? "hh:mm" : "dd.MM.yyyy hh:mm";
	return dt.toLocalTime().toString(format);
}

QString JobModel::toTimeString(double secs) const
{
	return QTime{0, 0}.addSecs(static_cast<int>(secs)).toString("mm:ss");
}

void JobModel::disconnectPipeline(QPointer<gpr::api::Pipeline> pipeline)
{
	if(!pipeline) return;

	disconnect(pipeline, &gpr::api::Pipeline::jobAdded, this, &JobModel::onJobAdded);
	disconnect(pipeline, &gpr::api::Pipeline::jobRemoved, this, &JobModel::onJobRemoved);
	disconnect(pipeline, &gpr::api::Pipeline::jobUpdated, this, &JobModel::onJobUpdated);
}

void JobModel::connectPipeline(QPointer<gpr::api::Pipeline> pipeline)
{
	if(!pipeline) return;

	connect(pipeline, &gpr::api::Pipeline::jobAdded, this, &JobModel::onJobAdded);
	connect(pipeline, &gpr::api::Pipeline::jobRemoved, this, &JobModel::onJobRemoved);
	connect(pipeline, &gpr::api::Pipeline::jobUpdated, this, &JobModel::onJobUpdated);
}

void JobModel::onJobAdded(QPointer<gpr::api::Job> job)
{
	auto const row = getJobRow(job);

	beginInsertRows({}, row, row);
	endInsertRows();
}

void JobModel::onJobRemoved(QPointer<gpr::api::Job> job)
{
	auto const row = getJobRow(job);

	beginRemoveRows({}, row, row);
	endRemoveRows();
}

void JobModel::onJobUpdated(QPointer<gpr::api::Job> job)
{
	auto const row = getJobRow(job);

	Q_EMIT dataChanged(index(row, 0), index(row, Column::Count - 1));
}

int JobModel::getJobRow(QPointer<gpr::api::Job> job) const
{
	assert(m_pipeline);
	auto const pos = std::ranges::find(m_pipeline->jobs(), job);
	assert(pos != std::cend(m_pipeline->jobs()));

	return static_cast<int>(std::ranges::distance(m_pipeline->jobs().cbegin(), pos));
}
