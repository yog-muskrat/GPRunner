#pragma once

#include <QAbstractTableModel>

#include "GPClasses.h"

class MRModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	enum Column
	{
		Id,
		Title,
		Author,
		Assignee,
		Reviewer,
		SourceBranch,
		TargetBranch,
		Created,
		Updated,
		Count
	};

	MRModel(QObject *parent = nullptr)
		: QAbstractTableModel(parent)
	{}

	void clear();
	void setMRs(std::vector<gpr::MR> mrs);

	int rowCount(QModelIndex const & = {}) const override;
	int columnCount(QModelIndex const & = {}) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	QVariant data(QModelIndex const &index, int role = Qt::ItemDataRole::DisplayRole) const override;

private:
	std::vector<gpr::MR> m_mrs;
};
