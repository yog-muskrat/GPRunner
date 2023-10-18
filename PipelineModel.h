#pragma once

#include <QAbstractTableModel>

#include "GPClasses.h"

class PipelineModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	enum Column
	{
		Id,
		Ref,
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
	void addPipeline(gpr::Pipeline pipeline);

	int rowCount(QModelIndex const & = {}) const override;
	int columnCount(QModelIndex const & = {}) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	QVariant data(QModelIndex const &index, int role = Qt::ItemDataRole::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;

private:
	std::vector<gpr::Pipeline> m_pipelines;
};
