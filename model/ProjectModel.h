#pragma once

#include <QAbstractTableModel>

#include "GPClasses.h"
#include "model/classes/Project.h"

class GPManager;

class ProjectModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	enum Column
	{
		Name,
		Count
	};

	enum Role
	{
		ProjectIdRole = Qt::ItemDataRole::UserRole,
		ProjectUrlRole,
		ProjectAvatarUrlRole,
		HasUnreadNotesRole,
		HasCurrentUserMRsRole
	};

	ProjectModel(GPManager &manager);

	void clear();

	void addProject(gpr::api::Project::Data projectData);

	std::vector<QPointer<gpr::api::Project>> const &projects() const;

	QPointer<gpr::api::Project> findProject(int projectId) const;

	int rowCount(QModelIndex const & = {}) const override;
	int columnCount(QModelIndex const & = {}) const override;

	QVariant data(QModelIndex const &index, int role = Qt::ItemDataRole::DisplayRole) const override;

	QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
	void projectMergeRequestAdded(QPointer<gpr::api::Project>, QPointer<gpr::api::MR>);
	void projectMergeRequestRemoved(QPointer<gpr::api::Project>, QPointer<gpr::api::MR>);
	void projectMergeRequestUpdated(QPointer<gpr::api::Project>, QPointer<gpr::api::MR>);

	void projectMrDiscussionAdded(QPointer<gpr::api::Project>, QPointer<gpr::api::MR>, gpr::Discussion const &);
	void projectMrDiscussionUpdated(QPointer<gpr::api::Project>, QPointer<gpr::api::MR>, gpr::Discussion const &);
	void projectMrDiscussionRemoved(QPointer<gpr::api::Project>, QPointer<gpr::api::MR>, gpr::Discussion const &);

	void projectMrDiscussionNoteAdded(QPointer<gpr::api::Project>, QPointer<gpr::api::MR>, gpr::Discussion const &, gpr::Note const &);
	void projectMrDiscussionNoteUpdated(QPointer<gpr::api::Project>, QPointer<gpr::api::MR>, gpr::Discussion const &, gpr::Note const &);
	void projectMrDiscussionNoteRemoved(QPointer<gpr::api::Project>, QPointer<gpr::api::MR>, gpr::Discussion const &, gpr::Note const &);

private:
	void onProjectUpdated(QPointer<gpr::api::Project> project);
	void connectProject(QPointer<gpr::api::Project> project);

	std::vector<QPointer<gpr::api::Project>> m_projects;
	GPManager &m_manager;
};
