#pragma once

#include <QAbstractTableModel>
#include <QQmlEngine>

#include "GPManager.h"

namespace gpr::api
{
	class Pipeline;
}

class JobModel : public QAbstractTableModel
{
	Q_OBJECT
	QML_ELEMENT

public:
	enum Column
	{
		Id,
		Stage,
		Name,
		Runner,
		Status,
		Duration,
		Started,
		Finished,
		Count,
	};
	Q_ENUM(Column)

	enum Role
	{
		UrlRole = Qt::UserRole,
	};

	JobModel(GPManager &manager);
	~JobModel() override = default;

	Q_INVOKABLE void setPipeline(QPointer<gpr::api::Pipeline> pipeline);
	Q_INVOKABLE void clear();

private:
	// Inherited via QAbstractTableModel
	int rowCount(QModelIndex const & = {}) const override;
	int columnCount(QModelIndex const & = {}) const override;
	QVariant data(QModelIndex const &index, int role) const override;
	QHash<int, QByteArray> roleNames() const override;

	void disconnectPipeline(QPointer<gpr::api::Pipeline> pipeline);
	void connectPipeline(QPointer<gpr::api::Pipeline> pipeline);

	void onJobAdded(QPointer<gpr::api::Job> job);
	void onJobRemoved(QPointer<gpr::api::Job> job);
	void onJobUpdated(QPointer<gpr::api::Job> job);

	int getJobRow(QPointer<gpr::api::Job> job) const;

	GPManager &m_manager;
	QPointer<gpr::api::Pipeline> m_pipeline;
};
