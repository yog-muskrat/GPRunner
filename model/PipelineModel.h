#pragma once

#include <QAbstractTableModel>
#include <QQmlEngine>

#include "model/classes/Project.h"

class PipelineModel : public QAbstractTableModel
{
	Q_OBJECT
	QML_ELEMENT
public:
	enum Column
	{
		Id,
		Jobs,
		Status,
		User,
		Source,
		Ref,
		Created,
		Updated,
		Count
	};

	enum Role
	{
		PipelineIdRole = Qt::ItemDataRole::UserRole + 1,
		PipelineStatusRole,
		PipelineUrlRole,
		PipelineUserRole,
	};

	PipelineModel(QObject *parent = nullptr)
		: QAbstractTableModel(parent)
	{}

	void clear();
	void setProject(QPointer<gpr::api::Project> project);

	Q_INVOKABLE QPointer<gpr::api::Pipeline> pipelineAtIndex(QModelIndex const &index) const;

	int rowCount(QModelIndex const & = {}) const override;
	int columnCount(QModelIndex const & = {}) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	QVariant data(QModelIndex const &index, int role = Qt::ItemDataRole::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;

private:
	QString getJobsDisplayRole(gpr::api::Pipeline const &pipeline) const;
	QString getPipelineStatusDisplayRole(gpr::api::Pipeline const &pipeline) const;

	void connectProject(QPointer<gpr::api::Project> project);
	void disconnectProject(QPointer<gpr::api::Project> project);

	void onPipelineAdded(QPointer<gpr::api::Pipeline> pipeline);
	void onPipelineRemoved(QPointer<gpr::api::Pipeline> pipeline);
	void onPipelineUpdated(QPointer<gpr::api::Pipeline> pipeline);

	int getPipelineIndex(QPointer<gpr::api::Pipeline> pipeline);

	QPointer<gpr::api::Project> m_project;
	std::vector<QMetaObject::Connection> m_connections;
};
