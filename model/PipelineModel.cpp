#include <QFont>

#include "PipelineModel.h"

void PipelineModel::clear()
{
	setProject(nullptr);
}

void PipelineModel::setProject(QPointer<gpr::api::Project> project)
{
	beginResetModel();
	if (m_project) disconnectProject(m_project);

	m_project = project;

	if (m_project) connectProject(m_project);
	endResetModel();
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
		if (index.column() == Column::Id) return pipeline->id();
		if (index.column() == Column::Ref) return pipeline->ref();
		if (index.column() == Column::User) return pipeline->user().username;
		if (index.column() == Column::Jobs) return QString("[%1]").arg(pipeline->jobs().size());
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

void PipelineModel::connectProject(QPointer<gpr::api::Project> project)
{
	connect(project, &gpr::api::Project::pipelineAdded, this, &PipelineModel::onPipelineAdded);
	connect(project, &gpr::api::Project::pipelineRemoved, this, &PipelineModel::onPipelineRemoved);

	for (auto const &pipeline : project->pipelines()) connectPipeline(pipeline);
}

void PipelineModel::disconnectProject(QPointer<gpr::api::Project> project)
{
	disconnect(project, &gpr::api::Project::pipelineAdded, this, &PipelineModel::onPipelineAdded);
	disconnect(project, &gpr::api::Project::pipelineRemoved, this, &PipelineModel::onPipelineRemoved);

	for (auto const &pipeline : project->pipelines()) disconnectPipeline(pipeline);
}

void PipelineModel::connectPipeline(QPointer<gpr::api::Pipeline> pipeline)
{
	connect(pipeline, &gpr::api::Pipeline::modified, this, &PipelineModel::onPipelineUpdated);
}

void PipelineModel::disconnectPipeline(QPointer<gpr::api::Pipeline> pipeline)
{
	disconnect(pipeline, &gpr::api::Pipeline::modified, this, &PipelineModel::onPipelineUpdated);
}

void PipelineModel::onPipelineAdded(QPointer<gpr::api::Pipeline> pipeline)
{
	auto const idx = getPipelineIndex(pipeline);

	beginInsertRows({}, idx, idx);
	connectPipeline(pipeline);
	endInsertRows();
}

void PipelineModel::onPipelineRemoved(QPointer<gpr::api::Pipeline> pipeline)
{
	auto const idx = getPipelineIndex(pipeline);

	beginRemoveRows({}, idx, idx);
	disconnectPipeline(pipeline);
	endRemoveRows();
}

void PipelineModel::onPipelineUpdated()
{
	auto pipeline = qobject_cast<gpr::api::Pipeline*>(sender());
	if (!pipeline) return;

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
