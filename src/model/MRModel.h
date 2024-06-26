#pragma once

#include <QAbstractTableModel>
#include <QQmlEngine>

#include "model/classes/Project.h"

class GPManager;

class MRModel : public QAbstractTableModel
{
	Q_OBJECT
	QML_ELEMENT
public:
	enum Column
	{
		Title,
		Pipeline,
		Status,
		Users,
		Discussions,
		Branches,
		Count,
	};
	Q_ENUM(Column)

	enum Role
	{
		MrRole = Qt::UserRole,
		UpdatedRole
	};

	MRModel(GPManager &manager);
	~MRModel() override = default;

	Q_INVOKABLE void clear();
	Q_INVOKABLE void setProject(gpr::api::Project *project);

	int rowCount(QModelIndex const & = {}) const override;
	int columnCount(QModelIndex const & = {}) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	QVariant data(QModelIndex const &index, int role = Qt::ItemDataRole::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;

private:
	QVariant editRole(gpr::api::MR const &mr, Column column) const;
	QVariant displayRole(gpr::api::MR const &mr, Column column) const;
	QVariant toolTipRole(gpr::api::MR const &mr, Column column) const;

	QString getDiscussionsString(gpr::api::MR const &mr) const;

	void connectProject(QPointer<gpr::api::Project> project);
	void disconnectProject(QPointer<gpr::api::Project> project);
	void connectMR(QPointer<gpr::api::MR> mr);
	void disconnectMR(QPointer<gpr::api::MR> mr);

	void onMRAdded(QPointer<gpr::api::MR> mr);
	void onMRRemoved(QPointer<gpr::api::MR> mr);
	void onMRUpdated();

	int getMRIndex(QPointer<gpr::api::MR> mr);

	QPointer<gpr::api::Project> m_project;
	GPManager &m_manager;
};
 