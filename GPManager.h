#pragma once

#include <QTimer>
#include <QObject>
#include <QPointer>
#include <QSortFilterProxyModel>

#include "client/Client.h"

#include "model/ProjectModel.h"
#include "model/PipelineModel.h"
#include "model/VariableModel.h"
#include "model/MRModel.h"

class QJsonDocument;
class QJsonArray;
class ImageProvider;

class GPManager : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QObject *projectModel READ getProjectModel NOTIFY projectModelChanged)
	Q_PROPERTY(QObject *pipelineModel READ getPipelineModel NOTIFY pipelineModelChanged)
	Q_PROPERTY(QObject *mrModel READ getMRModel NOTIFY mrModelChanged)
	Q_PROPERTY(QObject *variableModel READ getVariableModel NOTIFY variableModelChanged)
	Q_PROPERTY(gpr::User currentUser READ getCurrentUser NOTIFY currentUserChanged)
	Q_PROPERTY(QVariantList activeUsers READ getActiveUsers NOTIFY activeUsersChanged)
	Q_PROPERTY(bool hasNewNotes READ hasNewNotes NOTIFY newNotesReceived)

public:
	GPManager(ImageProvider &imageProvider, QObject *parent = nullptr);

	Q_INVOKABLE void connect();

	Q_INVOKABLE void setCurrentProject(int projectId);

	Q_INVOKABLE void runPipeline(int projectId, QString const &ref);
	Q_INVOKABLE QStringList getProjectBranches(int projectId);
	Q_INVOKABLE void loadPipelineVariables(int projectId, QString const &ref);
	Q_INVOKABLE void loadPipelineStatistics(int projectId, QDateTime const &from, QDateTime const &to);

	/**
	 * @brief Остановить пайплайн (в проекте m_currentProject)
	 * @param pipelineId идентификатор пайплайна
	 */
	Q_INVOKABLE void cancelPipeline(int pipelineId);

	/**
	 * @brief Перезапустить джобы пайплайна (в проекте m_currentProject)
	 * @param pipelineId идентификатор пайплайна
	 */
	Q_INVOKABLE void retryPipeline(int pipelineId);

	void setMRReviewer(QPointer<gpr::api::MR> mr, int userId);
	void setMRAssignee(QPointer<gpr::api::MR> mr, int userId);

	Q_INVOKABLE void resolveMRDiscussion(int projectId, int mrIid, QString const &discussionId);
	Q_INVOKABLE void unresolveMRDiscussion(int projectId, int mrIid, QString const &discussionId);
	Q_INVOKABLE void addMRDiscussionNote(int projectId, int mrIid, QString const &discussionId, QString const &note);
	Q_INVOKABLE void addMRDiscussion(int projectId, int mrIid, QString const &note);
	Q_INVOKABLE void editMRDiscussionNote(int projectId, int mrIid, QString const &discussionId, int noteId, QString const &note);
	Q_INVOKABLE void removeMRDiscussionNote(int projectId, int mrIid, QString const &discussionId, int noteId);

	Q_INVOKABLE void markDiscussionsRead(int projectId);

	QAbstractItemModel *getProjectModel();
	QAbstractItemModel *getPipelineModel();
	QAbstractItemModel *getMRModel();
	QAbstractItemModel *getVariableModel();

	gpr::User getCurrentUser() const { return m_currentUser; }
	QVariantList getActiveUsers() const;
	bool hasNewNotes() const;

	Q_INVOKABLE void addVariable();
	Q_INVOKABLE void removeVariable(int index);

	std::map<QString, gpr::Emoji> const &emojiDict() const { return m_emojis; }

	gpr::Client &client();

Q_SIGNALS:
	void pipelineModelChanged();
	void mrModelChanged();
	void projectModelChanged();
	void variableModelChanged();
	void currentUserChanged(gpr::User const &);
	void activeUsersChanged(QVariantList const &);
	void newNotesReceived() const;
	void notification(QString title, QString message) const;

private:
	void initModels();
	void initUpdateTimer();

	void parseProjects(QJsonDocument const &doc);
	void parsePipelines(int projectId, QJsonDocument const &doc);
	void parsePipelineInfo(int projectId, int pipelineId, QJsonDocument const &doc);
	void parsePipelineJobs(int projectId, int pipelineId, QJsonDocument const &doc);
	void parseMRs(int projectId, QJsonDocument const &doc);
	void parseMRDetails(QPointer<gpr::api::MR> mr, QJsonDocument const &doc);
	void parseMRDiscussions(QPointer<gpr::api::MR> mr, QJsonDocument const &doc);
	void parseMRNoteEmojis(QPointer<gpr::api::MR> mr, QString const &discussionId, int noteId, QJsonDocument const &doc);
	void parseMRApprovals(QPointer<gpr::api::MR> mr, QJsonDocument const &doc);
	void parseVariables(QJsonDocument const &doc);
	void parseBranches(int projectId, QJsonDocument const &doc);
	void parseCurrentUser(QJsonDocument const &doc);
	void parseActiveUsers(QJsonDocument const &doc);

	void parseProviderImage(QString const&id, QByteArray data);

	void readEmojis();

	void loadProjects();
	void loadProjectBranches(int projectId);
	void loadProjectPipelines(int projectId);
	void loadProjectMRs(int projectId);
	void loadProjectMRInfo(int projectId);
	void loadPipelineInfo(int projectId, int pipelineId);
	void loadPipelineJobs(int projectId, int pipelineId);
	void loadCurrentUser();
	void loadActiveUsers();

	void onDiscussionAdded(
		QPointer<gpr::api::Project> project,
		QPointer<gpr::api::MR> mr,
		QPointer<gpr::api::Discussion> discussion);

	void onDiscussionNoteAdded(
		QPointer<gpr::api::Project> project,
		QPointer<gpr::api::MR> mr,
		QPointer<gpr::api::Discussion> discussion,
		QPointer<gpr::api::Note> note);

	void onDiscussionNoteUpdated(
		QPointer<gpr::api::Project> project,
		QPointer<gpr::api::MR> mr,
		QPointer<gpr::api::Discussion> discussion,
		QPointer<gpr::api::Note> note);

	void onMergeRequestRemoved(QPointer<gpr::api::Project> project, QPointer<gpr::api::MR> mr);

	void update();

	ProjectModel m_projectModel;
	QSortFilterProxyModel m_projectProxyModel;

	PipelineModel m_pipelineModel;
	QSortFilterProxyModel m_pipelineProxyModel;

	MRModel m_mrModel;
	QSortFilterProxyModel m_mrProxyModel;

	VariableModel m_variableModel;
	QSortFilterProxyModel m_variableProxyModel;

	gpr::Client m_client;

	QTimer m_updateTimer;
	
	gpr::User m_currentUser;

	QList<gpr::User> m_activeUsers;

	ImageProvider &m_imageProvider;

	std::map<QString, gpr::Emoji> m_emojis;

	int m_currentProject{-1};
};
