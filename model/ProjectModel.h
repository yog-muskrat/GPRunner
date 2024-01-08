#pragma once

#include <QAbstractTableModel>
#include <QQmlEngine>

#include "model/classes/GPClasses.h"
#include "model/classes/Project.h"

#include "GPManager.h"

class GPManager;

class ProjectModel : public QAbstractTableModel
{
	Q_OBJECT
	QML_ELEMENT
	Q_PROPERTY(GPManager* manager WRITE setGPManager READ gpManager)

public:
	enum Column
	{
		Name,
		Count
	};

	enum Role
	{
		ProjectRole = Qt::ItemDataRole::UserRole,
		HasUnreadNotesRole,
		HasCurrentUserMRsRole
	};

	ProjectModel(QObject *parent = nullptr);

	void setGPManager(GPManager *manager);
	GPManager *gpManager() const { return m_manager; }

	int rowCount(QModelIndex const & = {}) const override;
	int columnCount(QModelIndex const & = {}) const override;

	QVariant data(QModelIndex const &index, int role = Qt::ItemDataRole::DisplayRole) const override;

	QHash<int, QByteArray> roleNames() const override;

private:
	void onProjectsReady();

	QPointer<GPManager> m_manager;
};
