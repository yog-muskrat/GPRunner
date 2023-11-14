#pragma once

#include <QAbstractTableModel>

#include "model/classes/Project.h"

class PipelineModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	enum Column
	{
		Id,
		Ref,
		User,
		Status,
		Source,
		Created,
		Updated,
		Count
	};

	enum Role
	{
		PipelineIdRole = Qt::ItemDataRole::UserRole + 1,
		PipelineStatusRole,
		PipelineSourceRole
	};

	PipelineModel(QObject *parent = nullptr)
		: QAbstractTableModel(parent)
	{}

	void clear();
	void setProject(QPointer<gpr::api::Project> project);

	int rowCount(QModelIndex const & = {}) const override;
	int columnCount(QModelIndex const & = {}) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	QVariant data(QModelIndex const &index, int role = Qt::ItemDataRole::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;

private:
	void connectProject(QPointer<gpr::api::Project> project);
	void disconnectProject(QPointer<gpr::api::Project> project);
	void connectPipeline(QPointer<gpr::api::Pipeline> pipeline);
	void disconnectPipeline(QPointer<gpr::api::Pipeline> pipeline);

	void onPipelineAdded(QPointer<gpr::api::Pipeline> pipeline);
	void onPipelineRemoved(QPointer<gpr::api::Pipeline> pipeline);
	void onPipelineUpdated();

	int getPipelineIndex(QPointer<gpr::api::Pipeline> pipeline);

	QPointer<gpr::api::Project> m_project;
	std::vector<QMetaObject::Connection> m_connections;
};
