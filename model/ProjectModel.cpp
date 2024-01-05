#include <algorithm>
#include <cassert>

#include <QFont>

#include "model/ProjectModel.h"
#include "GPManager.h"

ProjectModel::ProjectModel(GPManager &manager)
	: QAbstractTableModel(&manager)
	, m_manager{manager}
{}

void ProjectModel::clear()
{
	beginResetModel();
	m_projects.clear();
	endResetModel();
}

void ProjectModel::addProject(gpr::api::Project::Data projectData)
{
	if (auto const pos = std::ranges::find(m_projects, projectData.id, &gpr::api::Project::id); pos != m_projects.cend())
	{
		(*pos)->update(std::move(projectData));
	}
	else
	{
		beginResetModel();
		auto project = new gpr::api::Project(m_manager, std::move(projectData), this);
		connectProject(project);

		connect(project, &gpr::api::Project::modified, [this, project] { onProjectUpdated(project); });

		m_projects.push_back(std::move(project));
		endResetModel();
	}
}

std::vector<QPointer<gpr::api::Project>> const &ProjectModel::projects() const
{
	return m_projects;
}

QPointer<gpr::api::Project> ProjectModel::findProject(int projectId) const
{
	if (auto const pos = std::ranges::find(m_projects, projectId, &gpr::api::Project::id); pos != m_projects.cend())
	{
		return *pos;
	}

	return nullptr;
}

int ProjectModel::rowCount(QModelIndex const &) const
{
	return std::ranges::ssize(m_projects);
}

int ProjectModel::columnCount(QModelIndex const &) const
{
	return Column::Count;
}

QVariant ProjectModel::data(QModelIndex const &index, int role) const
{
	if (!checkIndex(index)) return {};

	auto const &prj = m_projects[index.row()];

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
	else if (role == Role::ProjectIdRole)
	{
		return prj->id();
	}
	else if (role == Role::ProjectUrlRole)
	{
		return prj->url();
	}
	else if (role == Role::ProjectAvatarUrlRole)
	{
		return prj->avatarUrl();
	}
	else if (role == Role::HasUnreadNotesRole)
	{
		return std::ranges::any_of(
			prj->openMRs(), 
			[this](QPointer<gpr::api::MR> const &mr)
			{
				return mr->isUserInvolved(m_manager.getCurrentUser()) && mr->hasUnreadNotes();
			});
	}
	else if (role == Role::HasCurrentUserMRsRole)
	{
		return std::ranges::any_of(
			prj->openMRs(),
			[this](auto const &mr) { return mr->isUserInvolved(m_manager.getCurrentUser()); });
	}

	return QVariant();
}

QHash<int, QByteArray> ProjectModel::roleNames() const
{
	auto names = QAbstractTableModel::roleNames();
	names.insert(Role::ProjectIdRole, "projectId");
	names.insert(Role::ProjectUrlRole, "projectUrl");
	names.insert(Role::ProjectAvatarUrlRole, "projectAvatarUrl");
	names.insert(Role::HasUnreadNotesRole, "hasUnreadNotes");
	names.insert(Role::HasCurrentUserMRsRole, "hasCurrentUserMRs");
	return names;
}

void ProjectModel::onProjectUpdated(QPointer<gpr::api::Project> project)
{
	if (auto const pos = std::ranges::find(m_projects, project); pos != m_projects.cend())
	{
		auto const row = std::ranges::distance(m_projects.cbegin(), pos);
		Q_EMIT dataChanged(index(row, 0), index(row, Column::Count - 1));
	}
	else
	{
		assert(false && "Invalid project");
	}
}

void ProjectModel::connectProject(QPointer<gpr::api::Project> project)
{
	connect(project, &gpr::api::Project::mrAdded,   [this, project](auto mr) { Q_EMIT projectMergeRequestAdded(project, mr); });
	connect(project, &gpr::api::Project::mrRemoved, [this, project](auto mr) { Q_EMIT projectMergeRequestRemoved(project, mr); });
	connect(project, &gpr::api::Project::mrUpdated, [this, project](auto mr) { Q_EMIT projectMergeRequestUpdated(project, mr); });

	connect(
		project,
		&gpr::api::Project::mrDiscussionAdded,
		[this, project](auto mr, auto discussion) { Q_EMIT projectMrDiscussionAdded(project, mr, discussion); });
	connect(
		project,
		&gpr::api::Project::mrDiscussionUpdated,
		[this, project](auto mr, auto discussion) { Q_EMIT projectMrDiscussionUpdated(project, mr, discussion); });
	connect(
		project,
		&gpr::api::Project::mrDiscussionRemoved,
		[this, project](auto mr, auto discussion) { Q_EMIT projectMrDiscussionRemoved(project, mr, discussion); });

	connect(
		project,
		&gpr::api::Project::mrDiscussionNoteAdded,
		[this, project](auto mr, auto discussion, auto note)
		{ Q_EMIT projectMrDiscussionNoteAdded(project, mr, discussion, note); });
	connect(
		project,
		&gpr::api::Project::mrDiscussionNoteUpdated,
		[this, project](auto mr, auto discussion, auto note)
		{ Q_EMIT projectMrDiscussionNoteUpdated(project, mr, discussion, note); });
	connect(
		project,
		&gpr::api::Project::mrDiscussionNoteRemoved,
		[this, project](auto mr, auto discussion, auto note)
		{ Q_EMIT projectMrDiscussionNoteRemoved(project, mr, discussion, note); });
}
