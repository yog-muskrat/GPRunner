#include <QFont>

#include "PipelineModel.h"

void PipelineModel::clear()
{
	setProject(nullptr);
}

void PipelineModel::setProject(QPointer<gpr::api::Project> project)
{
	beginResetModel();
	disconnectProject(m_project);
	m_project = project;
	connectProject(m_project);
	endResetModel();
}

QPointer<gpr::api::Pipeline> PipelineModel::pipelineAtIndex(QModelIndex const &index) const
{
	if(index.row() < 0 || index.row() >= rowCount()) return nullptr;

	return m_project->pipelines().at(index.row());
}

int PipelineModel::rowCount(QModelIndex const &) const
{
	if (!m_project) return 0;

	return std::ranges::ssize(m_project->pipelines());
}

int PipelineModel::columnCount(QModelIndex const &) const
{
	return Column::Count;
}

QVariant PipelineModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Orientation::Horizontal || section < 0 || section >= Column::Count || role != Qt::DisplayRole)
	{
		return QAbstractTableModel::headerData(section, orientation, role);
	}

	switch (section)
	{
		case Column::Id: return "ID";
		case Column::Ref: return "Ref";
		case Column::User: return "User";
		case Column::Jobs: return "Jobs";
		case Column::Source: return "Source";
		case Column::Status: return "Status";
		case Column::Created: return "Created";
		case Column::Updated: return "Updated";
		default: break;
	}
	return {};
}

QVariant PipelineModel::data(QModelIndex const &index, int role) const
{
	if (!checkIndex(index)) return {};

	auto const pipeline = m_project->pipelines().at(index.row());

	if (role == Qt::DisplayRole)
	{
		if (index.column() == Column::Id)     return pipeline->id();
		if (index.column() == Column::Ref)    return pipeline->ref();
		if (index.column() == Column::User)   return pipeline->user().username;
		if (index.column() == Column::Jobs)   return getJobsDisplayRole(*pipeline);
		if (index.column() == Column::Status) return pipeline->status();
		if (index.column() == Column::Source) return pipeline->source();
		if (index.column() == Column::Created)
		{
			auto const format = pipeline->createdAt().date() == QDate::currentDate() ? "hh:mm" : "dd.MM.yyyy hh:mm";

			return pipeline->createdAt().toLocalTime().toString(format);
		}
		if (index.column() == Column::Updated)
		{
			auto const format = pipeline->updatedAt().date() == QDate::currentDate() ? "hh:mm" : "dd.MM.yyyy hh:mm";

			return pipeline->updatedAt().toLocalTime().toString(format);
		}
	}
	else if (role == Qt::EditRole)
	{
		if (index.column() == Column::Id)      return pipeline->id();
		if (index.column() == Column::Ref)     return pipeline->ref();
		if (index.column() == Column::Status)  return pipeline->status();
		if (index.column() == Column::Source)  return pipeline->source();
		if (index.column() == Column::Created) return pipeline->createdAt();
		if (index.column() == Column::Updated) return pipeline->updatedAt();
	}

	if (role == Role::PipelineIdRole) return pipeline->id();
	if (role == Role::PipelineStatusRole)
	{
		return index.column() == Column::Status ? pipeline->status() : QString{};
	}
	if(role == Role::PipelineUrlRole)
	{
		return index.column() == Column::Id ? pipeline->url() : QString{};
	}
	if(role == Role::PipelineUserRole && index.column() == Column::User)
	{
		return QVariant::fromValue(pipeline->user());
	}

	return QVariant();
}

QHash<int, QByteArray> PipelineModel::roleNames() const
{
	auto names = QAbstractTableModel::roleNames();
	names.insert(Role::PipelineIdRole, "pipelineId");
	names.insert(Role::PipelineStatusRole, "pipelineStatus");
	names.insert(Role::PipelineUrlRole, "pipelineUrl");
	names.insert(Role::PipelineUserRole, "pipelineUser");

	return names;
}

QString PipelineModel::getJobsDisplayRole(gpr::api::Pipeline const &pipeline) const
{
	QStringList const finishedStates{
		"failed",
		"warning",
		"canceled",
		"skipped",
		"success",
	};

	QStringList const activeStates{
		"pending",
		"running",
		"manual",
		"scheduled",
		"created",
	};

	auto const total = pipeline.jobs().size();
	auto const finished = std::ranges::count_if(
		pipeline.jobs(),
		[&finishedStates](auto const &state) { return finishedStates.contains(state); },
		&gpr::api::Job::status);

	return QString("[%1/%2]").arg(finished).arg(total);	
}

QString PipelineModel::getPipelineStatusDisplayRole(gpr::api::Pipeline const &pipeline) const
{
	if(pipeline.status() == "success" && std::ranges::any_of(pipeline.jobs(), [](auto const &st){ return st == "warning"; }, &gpr::api::Job::status))
	{
		return "warning";
	}

	return pipeline.status();
}

void PipelineModel::connectProject(QPointer<gpr::api::Project> project)
{
	if(!project) return;

	connect(project, &gpr::api::Project::pipelineAdded, this, &PipelineModel::onPipelineAdded);
	connect(project, &gpr::api::Project::pipelineRemoved, this, &PipelineModel::onPipelineRemoved);
	connect(project, &gpr::api::Project::pipelineUpdated, this, &PipelineModel::onPipelineUpdated);
}

void PipelineModel::disconnectProject(QPointer<gpr::api::Project> project)
{
	if(!project) return;

	disconnect(project, &gpr::api::Project::pipelineAdded, this, &PipelineModel::onPipelineAdded);
	disconnect(project, &gpr::api::Project::pipelineRemoved, this, &PipelineModel::onPipelineRemoved);
	disconnect(project, &gpr::api::Project::pipelineUpdated, this, &PipelineModel::onPipelineUpdated);
}

void PipelineModel::onPipelineAdded(QPointer<gpr::api::Pipeline> pipeline)
{
	auto const idx = getPipelineIndex(pipeline);

	beginInsertRows({}, idx, idx);
	endInsertRows();
}

void PipelineModel::onPipelineRemoved(QPointer<gpr::api::Pipeline> pipeline)
{
	auto const idx = getPipelineIndex(pipeline);

	beginRemoveRows({}, idx, idx);
	endRemoveRows();
}

void PipelineModel::onPipelineUpdated(QPointer<gpr::api::Pipeline> pipeline)
{
	auto const idx = getPipelineIndex(pipeline);

	Q_EMIT dataChanged(index(idx, 0), index(idx, Column::Count - 1));
}

int PipelineModel::getPipelineIndex(QPointer<gpr::api::Pipeline> pipeline)
{
	auto const &pipelines = m_project->pipelines();
	auto const pos = std::ranges::find(pipelines, pipeline);
	if (pos == pipelines.cend())
	{
		assert(false && "Invalid mr");
		return -1;
	}

	return static_cast<int>(std::ranges::distance(pipelines.cbegin(), pos));
}
