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
		Status,
		Stage,
		Name,
		Runner,
		Tags,
		Duration,
		Started,
		Finished,
		Count,
	};
	Q_ENUM(Column)

	enum Role
	{
		UrlRole = Qt::UserRole,
		JobRole,
	};

	JobModel(QObject *parent = nullptr);
	~JobModel() override = default;

	Q_INVOKABLE void setPipeline(gpr::api::Pipeline *pipeline);
	Q_INVOKABLE void clear();

private:
	// Inherited via QAbstractTableModel
	int rowCount(QModelIndex const & = {}) const override;
	int columnCount(QModelIndex const & = {}) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	QVariant data(QModelIndex const &index, int role) const override;
	QHash<int, QByteArray> roleNames() const override;

	QString toDateTimeString(QDateTime const &dt) const;
	QString toTimeString(double secs) const;

	void disconnectPipeline(QPointer<gpr::api::Pipeline> pipeline);
	void connectPipeline(QPointer<gpr::api::Pipeline> pipeline);

	void onJobAdded(QPointer<gpr::api::Job> job);
	void onJobRemoved(QPointer<gpr::api::Job> job);
	void onJobUpdated(QPointer<gpr::api::Job> job);

	int getJobRow(QPointer<gpr::api::Job> job) const;

	QPointer<gpr::api::Pipeline> m_pipeline;
};
