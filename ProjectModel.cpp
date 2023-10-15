#include "ProjectModel.h"

#include <algorithm>

void ProjectModel::clear()
{
	beginResetModel();
	m_projects.clear();
	endResetModel();
}

void ProjectModel::addProject(gpr::Project project)
{
	if (auto const pos = std::ranges::find(m_projects, project.id, &gpr::Project::id); pos != m_projects.cend())
	{
		auto const row = std::ranges::distance(m_projects.cbegin(), pos);
		*pos = project;
		Q_EMIT dataChanged(index(row, 0), index(row, Column::Count - 1));
		return;
	}

	beginResetModel();
	m_projects.push_back(std::move(project));
	std::ranges::sort(m_projects, {}, &gpr::Project::name);
	endResetModel();
}

std::optional<gpr::Project> ProjectModel::findProject(int projectId) const
{
	if (auto pos = std::ranges::find(m_projects, projectId, &gpr::Project::id); pos != m_projects.cend())
	{
		return *pos;
	}
	return std::nullopt;
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
			auto result = prj.name;

			if (!prj.openMRs.empty()) result += QString("[MR: %1]").arg(prj.openMRs.size());
			
			auto const runningPipelines = std::ranges::count(prj.pipelines, QString("running"), &gpr::Pipeline::status);
			if (runningPipelines > 0) result += QString("[PL: %1]").arg(runningPipelines);
			
			return result;
		}
	}
	else if (role == Role::ProjectIdRole)
	{
		return prj.id;
	}

	return QVariant();
}

QHash<int, QByteArray> ProjectModel::roleNames() const
{
	auto names = QAbstractTableModel::roleNames();
	names.insert(Role::ProjectIdRole, "projectId");
	return names;
}
