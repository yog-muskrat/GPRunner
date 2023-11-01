#pragma once

#include <QAbstractTableModel>

#include "GPClasses.h"
#include "model/classes/Project.h"

class GPManager;

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

	ProjectModel(GPManager &manager, QObject *parent = nullptr)
		: QAbstractTableModel(parent), m_manager{manager}
	{}

	void clear();

	void addProject(gpr::api::Project::Data projectData);

	QPointer<gpr::api::Project> findProject(int projectId) const;

	int rowCount(QModelIndex const & = {}) const override;
	int columnCount(QModelIndex const & = {}) const override;

	QVariant data(QModelIndex const &index, int role = Qt::ItemDataRole::DisplayRole) const override;

	QHash<int, QByteArray> roleNames() const override;

private:
	void onProjectUpdated(QPointer<gpr::api::Project> project);

	std::vector<QPointer<gpr::api::Project>> m_projects;
	GPManager &m_manager;
};
