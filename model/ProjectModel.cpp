#include <algorithm>
#include <cassert>

#ifdef _DEBUG
#include <QAbstractItemModelTester>
#endif

#include "model/ProjectModel.h"

ProjectModel::ProjectModel(QObject *parent)
	: QAbstractTableModel(parent)
{
#ifdef _DEBUG
	new QAbstractItemModelTester(this, this);
#endif
}

void ProjectModel::setGPManager(GPManager *manager)
{
	m_manager = manager;
	connect(m_manager, &GPManager::projectsLoaded, this, &ProjectModel::onProjectsReady);
}

int ProjectModel::rowCount(QModelIndex const &) const
{
	if(!m_manager) return 0;
	return std::ranges::ssize(m_manager->projects());
}

int ProjectModel::columnCount(QModelIndex const &) const
{
	return Column::Count;
}

QVariant ProjectModel::data(QModelIndex const &index, int role) const
{
	if (!checkIndex(index)) return {};

	auto const &prj = m_manager->projects()[index.row()];

	if (role == Qt::ItemDataRole::DisplayRole)
	{
		if (index.column() == Column::Name)
		{
			auto result = prj->name();

			if (!prj->openMRs().empty()) result += QString("[MR: %1]").arg(prj->openMRs().size());

			auto const runningPipelines = std::ranges::count(prj->pipelines(), QString("running"), &gpr::api::Pipeline::status);
			if (runningPipelines > 0) result += QString("[PL: %1]").arg(runningPipelines);

			return result;
		}
	}
	else if (role == Role::ProjectRole)
	{
		return QVariant::fromValue(prj.get());
	}
	else if (role == Role::HasUnreadNotesRole)
	{
		return std::ranges::any_of(
			prj->openMRs(), 
			[this](QPointer<gpr::api::MR> const &mr)
			{
				return mr->isUserInvolved(m_manager->getCurrentUser()) && mr->hasUnreadNotes();
			});
	}
	else if (role == Role::HasCurrentUserMRsRole)
	{
		return std::ranges::any_of(
			prj->openMRs(),
			[this](auto const &mr) { return mr->isUserInvolved(m_manager->getCurrentUser()); });
	}

	return QVariant();
}

QHash<int, QByteArray> ProjectModel::roleNames() const
{
	auto names = QAbstractTableModel::roleNames();
	names.insert(Role::ProjectRole, "project");
	names.insert(Role::HasUnreadNotesRole, "hasUnreadNotes");
	names.insert(Role::HasCurrentUserMRsRole, "hasCurrentUserMRs");
	return names;
}

void ProjectModel::onProjectsReady()
{
	beginResetModel();
	for(auto prj : m_manager->projects())
	{
		connect(prj, &gpr::api::Project::modified, [this, prj]{ onProjectModified(prj); });
	}

	endResetModel();
}

void ProjectModel::onProjectModified(gpr::api::Project *prj)
{
	auto const &projects = m_manager->projects();

	auto const pos = std::ranges::find(projects, prj, &QPointer<gpr::api::Project>::get);

	if(pos == projects.cend()) return;

	auto const row = static_cast<int>(std::ranges::distance(projects.cbegin(), pos));

	Q_EMIT dataChanged(index(row, 0), index(row, columnCount() - 1));
}
