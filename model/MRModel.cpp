﻿#include <ranges>

#ifdef _DEBUG
#include <QAbstractItemModelTester>
#endif

#include "GPManager.h"
#include "model/MRModel.h"

namespace {
	struct StatusInfo
	{
		QString shortInfo;
		QString fullInfo;
		// TODO: Icon
	};

	auto getStatusInfo(QString const &status)
	{
		static std::map<QString, StatusInfo> const mergeStatuses = {
		{"blocked_status",           {"BLK",  "Blocked by another merge request."}},
		{"broken_status",            {"BRK",  "Can’t merge into the target branch due to a potential conflict."}},
		{"checking",                 {"GIT",  "Git is testing if a valid merge is possible."}},
		{"unchecked",                {"GIT",  "Git has not yet tested if a valid merge is possible."}},
		{"ci_must_pass",             {"CI",   "A CI/CD pipeline must succeed before merge."}},
		{"ci_still_running",         {"CI",   "A CI/CD pipeline is still running."}},
		{"discussions_not_resolved", {"🗨",  "All discussions must be resolved before merge."}},
		{"draft_status",             {"✎", "Can’t merge because the merge request is a draft."}},
		{"external_status_checks",   {"CHK",  "All status checks must pass before merge."}},
		{"mergeable",                {"✅",   "The branch can merge cleanly into the target branch."}},
		{"not_approved",             {"APP",  "Approval is required before merge."}},
		{"not_open",                 {"CLS",  "The merge request must be open before merge."}},
		{"policies_denied",          {"PLC",  "The merge request contains denied policies."}},
		};

		if(!mergeStatuses.contains(status)) return StatusInfo{"???", "Unknown status"};
		return mergeStatuses.at(status);
	}

	QString getPipelineStatusIcon(QString const &status)
	{
		static std::map<QString, QString> const icons = {
			{"created",              "⧖"},
			{"waiting_for_resource", "⧖"},
			{"preparing",            "⧖"},
			{"pending",              "⧖"},
			{"running",              "⏵"},
			{"success",              "✅"},
			{"failed",               "❌"},
			{"canceled",             "⏹"},
			{"skipped",              "⏩"},
			{"manual",               "🤚"},
			{"schedule",             "🕑"},
		};

		if(!icons.contains(status)) return {};
		return icons.at(status);
	}
}

MRModel::MRModel(GPManager &manager)
	: QAbstractTableModel(&manager)
	, m_manager{manager}
{
#ifdef _DEBUG
	new QAbstractItemModelTester(this, this);
#endif
}

void MRModel::clear()
{
	setProject(nullptr);
}

void MRModel::setProject(gpr::api::Project *project)
{
	beginResetModel();

	if (m_project) disconnectProject(m_project);
	m_project = std::move(project);
	if (m_project) connectProject(m_project);

	endResetModel();
}

int MRModel::rowCount(QModelIndex const &parent) const
{
	if(parent.isValid()) return 0;

	if (!m_project) return 0;
	return std::ranges::ssize(m_project->openMRs());
}

int MRModel::columnCount(QModelIndex const &) const
{
	return Column::Count;
}

QVariant MRModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Orientation::Horizontal || section < 0 || section >= Column::Count || role != Qt::DisplayRole)
	{
		return QAbstractTableModel::headerData(section, orientation, role);
	}

	switch (section)
	{
		case Column::Iid: return "IID";
		case Column::Title: return "Title";
		case Column::Status: return "?";
		case Column::Pipeline: return "PL";
		case Column::Author: return "Author";
		case Column::Discussions: return "🗨";
		case Column::Assignee: return "Assignee";
		case Column::Reviewer: return "Reviewer";
		case Column::SourceBranch: return "Source";
		case Column::TargetBranch: return "Target";
		case Column::Created: return "Created";
		case Column::Updated: return "Updated";
		default: break;
	}
	return {};
}

QVariant MRModel::data(QModelIndex const &index, int role) const
{
	if (!checkIndex(index, CheckIndexOption::IndexIsValid))
	{
		return {};
	}

	auto mr = m_project->openMRs().at(index.row());
	auto const column = static_cast<Column>(index.column());

	if (role == Qt::DisplayRole)      return displayRole(*mr, column);
	if (role == Qt::EditRole)         return editRole(*mr, column);
	if (role == Qt::ToolTipRole)      return toolTipRole(*mr, column);
	if (role == Role::MrRole)         return QVariant::fromValue(mr.get());

	return QVariant();
}

bool MRModel::setData(QModelIndex const &index, QVariant const &value, int role)
{
	if (!checkIndex(index, CheckIndexOption::IndexIsValid))
	{
		return false;
	}

	auto const mr = m_project->openMRs().at(index.row());

	if (role == Qt::EditRole)
	{
		auto const user = value.canConvert<gpr::User>() ? value.value<gpr::User>() : gpr::User{.id = 0};

		if (index.column() == Column::Reviewer)
		{
			mr->setReviewer(user);
		}
		else if (index.column() == Column::Assignee)
		{
			mr->setAssignee(user);
		}
	}

	return false;
}

QHash<int, QByteArray> MRModel::roleNames() const
{
	auto names = QAbstractTableModel::roleNames();
	names.emplace(Role::MrRole, "mr");

	return names;
}

Qt::ItemFlags MRModel::flags(QModelIndex const &index) const
{
	if(!index.isValid()) return {};

	auto &mr = *m_project->openMRs().at(index.row());
	auto flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	if((index.column() == Column::Reviewer || index.column() == Column::Assignee) && mr.author() == m_manager.getCurrentUser())
	{
		flags |= Qt::ItemIsEditable;
	}
	return flags;
}

QVariant MRModel::editRole(gpr::api::MR const &mr, Column column) const
{
	switch (column)
	{
		case Column::Iid:
		case Column::Title:
		case Column::Author:
		case Column::Assignee:
		case Column::Reviewer:
		case Column::Discussions:
		case Column::SourceBranch:
		case Column::TargetBranch: return displayRole(mr, column);
		case Column::Status:       return mr.mergeStatus();
		case Column::Pipeline:     return mr.pipeline() ? mr.pipeline()->status() : QString{""};
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
		case Column::Iid:          return mr.iid();
		case Column::Title:        return mr.title();
		case Column::Status:       return getStatusInfo(mr.mergeStatus()).shortInfo;
		case Column::Pipeline:     return getPipelineStatusIcon(mr.pipeline() ? mr.pipeline()->status() : QString{""});
		case Column::Author:       return mr.author().username;
		case Column::Discussions:  return getDiscussionsString(mr);
		case Column::Assignee:     return mr.assignee().username;
		case Column::Reviewer:     return mr.reviewer().username;
		case Column::SourceBranch: return mr.sourceBranch();
		case Column::TargetBranch: return mr.targetBranch();
		case Column::Created:      return getDateTimeString(mr.createdAt());
		case Column::Updated:      return getDateTimeString(mr.updatedAt());
		default: break;
	}
	return {};
}

QVariant MRModel::toolTipRole(gpr::api::MR const &mr, Column column) const
{
	switch (column)
	{
		case Column::Status:   return getStatusInfo(mr.mergeStatus()).fullInfo;
		case Column::Pipeline: return mr.pipeline() ? mr.pipeline()->status() : QString{""};
		default: break;
	}
	return QString{""};
}

QString MRModel::getDiscussionsString(gpr::api::MR const &mr) const
{
	QString result;
	auto const count = std::ranges::ssize(mr.discussions());
	if (count > 0)
	{
		auto const resolvable = std::ranges::count_if(mr.discussions(), &gpr::api::Discussion::isResolvable);

		if (resolvable != count)
		{
			result += QString("[%1]").arg(count);
		}

		if (resolvable > 0)
		{
			auto const resolved = std::ranges::count_if(mr.discussions(), &gpr::api::Discussion::isResolved);
			result += QString("[%1/%2]").arg(resolved).arg(resolvable);
		}
	}
	return result;
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
