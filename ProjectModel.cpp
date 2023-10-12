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
        Q_EMIT dataChanged(index(row, 0), index(row, Column::Count));
        return;
    }

    auto const row = m_projects.size();

    beginInsertRows({}, row, row);
    m_projects.push_back(std::move(project));
    endInsertRows();
}

int ProjectModel::rowCount(QModelIndex const &) const
{
    return m_projects.size();
}

int ProjectModel::columnCount(QModelIndex const&) const
{
    return Column::Count;
}

QVariant ProjectModel::data(QModelIndex const& index, int role) const
{
    if (!checkIndex(index)) return {};

    auto const& prj = m_projects[index.row()];

    if(role == Qt::ItemDataRole::DisplayRole)
    {
        if (index.column() == Column::Name) return prj.name;
    }
    else if(role == Role::ProjectIdRole) 
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
