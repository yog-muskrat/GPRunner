#include <ranges>

#include <QFont>

#include "GPManager.h"
#include "MRModel.h"

MRModel::MRModel(GPManager &manager)
	: QAbstractTableModel(&manager)
	, m_manager{manager}
{}

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
		case Column::Discussions: return "Discussions";
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

	auto const &mr = *m_project->openMRs().at(index.row());
	auto const column = static_cast<Column>(index.column());

	if (role == Qt::DisplayRole) return displayRole(mr, column);
	if (role == Qt::EditRole) return editRole(mr, column);
	if (role == Qt::ItemDataRole::FontRole) return fontRole(mr, column);
	if (role == Role::Url) return mr.url();

	return QVariant();
}

QHash<int, QByteArray> MRModel::roleNames() const
{
	auto names = QAbstractTableModel::roleNames();
	names.insert(Qt::FontRole, "font");
	names.insert(Role::Url, "url");

	return names;
}

QVariant MRModel::editRole(gpr::api::MR const &mr, Column column) const
{
	switch (column)
	{
		case Column::Id:
		case Column::Title:
		case Column::Author:
		case Column::Discussions:
		case Column::SourceBranch:
		case Column::TargetBranch: return displayRole(mr, column);
		case Column::Assignee:     return mr.assignee();
		case Column::Reviewer:     return mr.reviewer();
		case Column::Created:      return mr.createdAt();
		case Column::Updated:      return mr.updatedAt();
		default: break;
	}
	return {};
}

QVariant MRModel::displayRole(gpr::api::MR const &mr, Column column) const
{
	switch (column)
	{
		case Column::Id:           return mr.id();
		case Column::Title:        return mr.title();
		case Column::Author:       return mr.author();
		case Column::Discussions:  return getDiscussionsString(mr);
		case Column::Assignee:     return getApproverString(mr, mr.assignee());
		case Column::Reviewer:     return getApproverString(mr, mr.reviewer());
		case Column::SourceBranch: return mr.sourceBranch();
		case Column::TargetBranch: return mr.targetBranch();
		case Column::Created:      return getDateTimeString(mr.createdAt());
		case Column::Updated:      return getDateTimeString(mr.updatedAt());
		default: break;
	}
	return {};
}

QVariant MRModel::fontRole(gpr::api::MR const &mr, Column column) const
{
	QFont font;

	switch (column)
	{
		case Column::Author:   font.setBold(mr.author() == m_manager.getCurrentUser());   break;
		case Column::Assignee: font.setBold(mr.assignee() == m_manager.getCurrentUser()); break;
		case Column::Reviewer: font.setBold(mr.reviewer() == m_manager.getCurrentUser()); break;
		default: break;
	}

	return font;
}

QString MRModel::getDiscussionsString(gpr::api::MR const &mr) const
{
	QString result;
	auto const count = mr.discussions().size();
	if (count > 0)
	{
		result += "🗨";
		auto const resolvable = std::ranges::count_if(mr.discussions(), &gpr::Discussion::isResolvable);

		if (resolvable != count)
		{
			result += QString("[%1]").arg(count);
		}

		if (resolvable > 0)
		{
			auto const resolved = std::ranges::count_if(mr.discussions(), &gpr::Discussion::isResolved);
			result += QString("[%1/%2]").arg(resolved).arg(resolvable);
		}
	}
	return result;
}

QString MRModel::getApproverString(gpr::api::MR const &mr, QString username) const
{
	if (std::ranges::contains(mr.approvedBy(), username)) username.prepend("✅");
	return username;
}

QString MRModel::getDateTimeString(QDateTime const &dt) const
{
	auto const format = dt.date() == QDate::currentDate() ? "hh:mm" : "dd.MM.yyyy hh:mm";
	return dt.toLocalTime().toString(format);
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
	auto mr = qobject_cast<gpr::api::MR *>(sender());
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
