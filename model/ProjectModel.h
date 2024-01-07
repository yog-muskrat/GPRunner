#pragma once

#include <QAbstractTableModel>

#include "model/classes/GPClasses.h"
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
		ProjectRole = Qt::ItemDataRole::UserRole,
		HasUnreadNotesRole,
		HasCurrentUserMRsRole
	};

	ProjectModel(GPManager &manager);

	void clear();

	QPointer<gpr::api::Project> addProject(gpr::api::Project::Data projectData);

	std::vector<QPointer<gpr::api::Project>> const &projects() const;

	QPointer<gpr::api::Project> findProject(int projectId) const;

	int rowCount(QModelIndex const & = {}) const override;
	int columnCount(QModelIndex const & = {}) const override;

	QVariant data(QModelIndex const &index, int role = Qt::ItemDataRole::DisplayRole) const override;

	QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
	void projectMergeRequestAdded(QPointer<gpr::api::MR>);
	void projectMergeRequestRemoved(QPointer<gpr::api::MR>);
	void projectMergeRequestUpdated(QPointer<gpr::api::MR>);

	void projectMrDiscussionAdded(QPointer<gpr::api::Discussion>);
	void projectMrDiscussionUpdated(QPointer<gpr::api::Discussion>);
	void projectMrDiscussionRemoved(QPointer<gpr::api::Discussion>);

	void projectMrDiscussionNoteAdded(QPointer<gpr::api::Note>);
	void projectMrDiscussionNoteUpdated(QPointer<gpr::api::Note>);
	void projectMrDiscussionNoteRemoved(QPointer<gpr::api::Note>);

private:
	void onProjectUpdated(QPointer<gpr::api::Project> project);
	void connectProject(QPointer<gpr::api::Project> project);

	std::vector<QPointer<gpr::api::Project>> m_projects;
	GPManager &m_manager;
};
