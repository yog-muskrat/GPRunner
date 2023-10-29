#include <ranges>

#include "MRModel.h"

void MRModel::clear()
{
	beginResetModel();
	m_mrs.clear();
	endResetModel();
}

void MRModel::setMRs(std::vector<gpr::MR> mrs)
{
	for (int row = rowCount() - 1; row >= 0; --row)
	{
		if (!std::ranges::contains(mrs, m_mrs[row].id, &gpr::MR::id))
		{
			beginRemoveRows({}, row, row);
			m_mrs.erase(std::next(m_mrs.begin(), row));
			endRemoveRows();
		}
	}

	if (mrs.size() != m_mrs.size())
	{
		beginResetModel();
		m_mrs = std::move(mrs);
		std::ranges::sort(m_mrs, std::ranges::greater{}, &gpr::MR::updated);
		endResetModel();
	}

	for (auto &mr : mrs)
	{
		if (auto const pos = std::ranges::find(m_mrs, mr.id, &gpr::MR::id); pos != m_mrs.cend())
		{
			if (*pos != mr)
			{
				auto const row = std::ranges::distance(m_mrs.cbegin(), pos);
				*pos = std::move(mr);
				Q_EMIT dataChanged(index(row, 0), index(row, Column::Count - 1));
			}
		}
	}
}

int MRModel::rowCount(QModelIndex const &) const
{
	return m_mrs.size();
}

int MRModel::columnCount(QModelIndex const &) const
{
	return Column::Count;
}

QVariant MRModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Orientation::Horizontal || section < 0 || section >= Column::Count)
	{
		return QAbstractTableModel::headerData(section, orientation, role);
	}

	switch (section)
	{
		case Column::Id: return "ID";
		case Column::Title: return "Title";
		case Column::Author: return "Author";
		case Column::Assignee: return "Assignee";
		case Column::Reviewer: return "Reviewer";
		case Column::SourceBranch: return "Source branch";
		case Column::TargetBranch: return "Target branch";
		case Column::Created: return "Created";
		case Column::Updated: return "Updated";
		default: break;
	}
	return {};
}

QVariant MRModel::data(QModelIndex const &index, int role) const
{
	if (!checkIndex(index)) return {};

	auto const &mr = m_mrs[index.row()];

	if (role == Qt::DisplayRole)
	{
		switch (index.column())
		{
			case Column::Id: return mr.id;
			case Column::Title: return mr.title;
			case Column::Author: return mr.author;
			case Column::Assignee: return mr.assignee;
			case Column::Reviewer: return mr.reviewer;
			case Column::SourceBranch: return mr.sourceBranch;
			case Column::TargetBranch: return mr.targetBranch;
			case Column::Created:
			{
				auto const format = mr.created.date() == QDate::currentDate() ? "hh:mm" : "dd.MM.yyyy hh:mm";
				return mr.created.toLocalTime().toString(format);
			}
			case Column::Updated:
			{
				auto const format = mr.updated.date() == QDate::currentDate() ? "hh:mm" : "dd.MM.yyyy hh:mm";
				return mr.updated.toLocalTime().toString(format);
			}
		}
	}

	return QVariant();
}
