#pragma once

#include <QAbstractTableModel>

#include "GPClasses.h"

class ProjectModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Column
    {
        Name,
        Count
    };

    enum Role
    {
        ProjectIdRole = Qt::ItemDataRole::UserRole
    };

    ProjectModel(QObject *parent = nullptr) : QAbstractTableModel(parent) {}

    void clear();

    void addProject(gpr::Project project);

    std::optional<gpr::Project> findProject(int projectId);

    int rowCount(QModelIndex const & = {}) const override;
    int columnCount(QModelIndex const& = {}) const override;

    QVariant data(QModelIndex const &index, int role = Qt::ItemDataRole::DisplayRole) const override;
    
    QHash<int, QByteArray> roleNames() const override;

private:
    QVector<gpr::Project> m_projects;
};
