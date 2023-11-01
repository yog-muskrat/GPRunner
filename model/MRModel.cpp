#include <ranges>

#include <QFont>

#include "GPManager.h"
#include "MRModel.h"

void MRModel::clear()
{
	setProject(nullptr);
}

void MRModel::setProject(QPointer<gpr::api::Project> project)
{
	beginResetModel();

	if (m_project) disconnectProject(m_project);
	m_project = std::move(project);
	if (m_project) connectProject(m_project);

	endResetModel();
}

int MRModel::rowCount(QModelIndex const &) const
{
	if (!m_project) return 0;
	return m_project->openMRs().size();
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

	auto const mr = m_project->openMRs().at(index.row());

	if (role == Qt::DisplayRole)
	{
		switch (index.column())
		{
			case Column::Id: return mr->id();
			case Column::Title: return mr->title();
			case Column::Author: return mr->author();
			case Column::Assignee: return mr->assignee();
			case Column::Reviewer: return mr->reviewer();
			case Column::SourceBranch: return mr->sourceBranch();
			case Column::TargetBranch: return mr->targetBranch();
			case Column::Created:
			{
				auto const format = mr->createdAt().date() == QDate::currentDate() ? "hh:mm" : "dd.MM.yyyy hh:mm";
				return mr->createdAt().toLocalTime().toString(format);
			}
			case Column::Updated:
			{
				auto const format = mr->updatedAt().date() == QDate::currentDate() ? "hh:mm" : "dd.MM.yyyy hh:mm";
				return mr->updatedAt().toLocalTime().toString(format);
			}
		}
	}
	else if (role == Qt::ItemDataRole::FontRole)
	{
		QFont font;

		switch (index.column())
		{
			case Column::Author:   font.setBold(mr->author()   == m_manager.getCurrentUser()); break;
			case Column::Assignee: font.setBold(mr->assignee() == m_manager.getCurrentUser()); break;
			case Column::Reviewer: font.setBold(mr->reviewer() == m_manager.getCurrentUser()); break;
			default: break;
		}

		return font;
	}

	return QVariant();
}

QHash<int, QByteArray> MRModel::roleNames() const
{
	auto names = QAbstractTableModel::roleNames();
	names.insert(Qt::FontRole, "font");

	return names;
}

void MRModel::connectProject(QPointer<gpr::api::Project> project)
{
	connect(project, &gpr::api::Project::mrAdded, this, &MRModel::onMRAdded);
	connect(project, &gpr::api::Project::mrRemoved, this, &MRModel::onMRRemoved);

	for (auto const &mr : project->openMRs()) connectMR(mr);
}

void MRModel::disconnectProject(QPointer<gpr::api::Project> project)
{
	disconnect(project, &gpr::api::Project::mrAdded, this, &MRModel::onMRAdded);
	disconnect(project, &gpr::api::Project::mrRemoved, this, &MRModel::onMRRemoved);

	for (auto const &mr : project->openMRs()) disconnectMR(mr);
}

void MRModel::connectMR(QPointer<gpr::api::MR> mr)
{
	connect(mr, &gpr::api::MR::modified, this, &MRModel::onMRUpdated);
}

void MRModel::disconnectMR(QPointer<gpr::api::MR> mr)
{
	disconnect(mr, &gpr::api::MR::modified, this, &MRModel::onMRUpdated);
}

void MRModel::onMRAdded(QPointer<gpr::api::MR> mr)
{
	auto const idx = getMRIndex(mr);

	beginInsertRows({}, idx, idx);
	connectMR(mr);
	endInsertRows();
}

void MRModel::onMRRemoved(QPointer<gpr::api::MR> mr)
{
	auto const idx = getMRIndex(mr);

	beginRemoveRows({}, idx, idx);
	disconnectMR(mr);
	endRemoveRows();
}

void MRModel::onMRUpdated()
{
	auto mr = qobject_cast<gpr::api::MR*>(sender());
	if (!mr) return;

	auto const idx = getMRIndex(mr);

	Q_EMIT dataChanged(index(idx, 0), index(idx, Column::Count - 1));
}

int MRModel::getMRIndex(QPointer<gpr::api::MR> mr)
{
	auto const &mrs = m_project->openMRs();
	auto const pos = std::ranges::find(mrs, mr);
	if (pos == mrs.cend())
	{
		assert(false && "Invalid mr");
		return -1;
	}

	return static_cast<int>(std::ranges::distance(mrs.cbegin(), pos));
}
