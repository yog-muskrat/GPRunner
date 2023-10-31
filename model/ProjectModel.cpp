#include <algorithm>
#include <cassert>

#include "model/ProjectModel.h"

void ProjectModel::clear()
{
	beginResetModel();
	m_projects.clear();
	endResetModel();
}

void ProjectModel::addProject(gpr::api::Project::Data projectData)
{
	if (auto const pos = std::ranges::find(m_projects, projectData.id, &gpr::api::Project::id); pos != m_projects.cend())
	{
		auto const row = std::ranges::distance(m_projects.cbegin(), pos);
		(*pos)->update(std::move(projectData));
	}
	else
	{
		beginResetModel();
		auto project = new gpr::api::Project(std::move(projectData), this);

		connect(project, &gpr::api::Project::modified, [this, project] { onProjectUpdated(project); });

		m_projects.push_back(std::move(project));
		endResetModel();
	}
}

QPointer<gpr::api::Project> ProjectModel::findProject(int projectId) const
{
	if (auto const pos = std::ranges::find(m_projects, projectId, &gpr::api::Project::id); pos != m_projects.cend())
	{
		return *pos;
	}

	return nullptr;
}

int ProjectModel::rowCount(QModelIndex const &) const
{
	return m_projects.size();
}

int ProjectModel::columnCount(QModelIndex const &) const
{
	return Column::Count;
}

QVariant ProjectModel::data(QModelIndex const &index, int role) const
{
	if (!checkIndex(index)) return {};

	auto const &prj = m_projects[index.row()];

	if (role == Qt::ItemDataRole::DisplayRole)
	{
		if (index.column() == Column::Name)
		{
			auto result = prj->name();

			if (!prj->openMRs().empty()) result += QString("[MR: %1]").arg(prj->openMRs().size());

			auto const runningPipelines = std::ranges::count(prj->pipelines(), QString("running"), &gpr::api::Pipeline::status);
			if (runningPipelines > 0) result += QString("[PL: %1]").arg(runningPipelines);

			return result;
		}
	}
	else if (role == Role::ProjectIdRole)
	{
		return prj->id();
	}

	return QVariant();
}

QHash<int, QByteArray> ProjectModel::roleNames() const
{
	auto names = QAbstractTableModel::roleNames();
	names.insert(Role::ProjectIdRole, "projectId");
	return names;
}

void ProjectModel::onProjectUpdated(QPointer<gpr::api::Project> project)
{
	if (auto const pos = std::ranges::find(m_projects, project); pos != m_projects.cend())
	{
		auto const row = std::ranges::distance(m_projects.cbegin(), pos);
		Q_EMIT dataChanged(index(row, 0), index(row, Column::Count - 1));
	}
	else
	{
		assert(false && "Invalid project");
	}
}
