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

class GPManager : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QObject *projectModel READ getProjectModel NOTIFY projectModelChanged)
	Q_PROPERTY(QObject *pipelineModel READ getPipelineModel NOTIFY pipelineModelChanged)
	Q_PROPERTY(QObject *mrModel READ getMRModel NOTIFY mrModelChanged)
	Q_PROPERTY(QObject *variableModel READ getVariableModel NOTIFY variableModelChanged)
	Q_PROPERTY(QString currentUser READ getCurrentUser NOTIFY currentUserChanged)

public:
	GPManager(QObject *parent = nullptr);

	Q_INVOKABLE void connect();
	Q_INVOKABLE void setCurrentProject(int projectId);
	Q_INVOKABLE void runPipeline(QString const &ref);
	Q_INVOKABLE QStringList getProjectBranches(int projectId);
	Q_INVOKABLE void loadPipelineVariables(QString const &ref);
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

	QAbstractItemModel *getProjectModel();
	QAbstractItemModel *getPipelineModel();
	QAbstractItemModel *getMRModel();
	QAbstractItemModel *getVariableModel();

	QString getCurrentUser() const { return m_currentUser; }

	Q_INVOKABLE void addVariable();
	Q_INVOKABLE void removeVariable(int index);

signals:
	void pipelineModelChanged();
	void mrModelChanged();
	void projectModelChanged();
	void variableModelChanged();
	void currentUserChanged(QString);

private:
	void initModels();
	void initUpdateTimer();

	void parseProjects(QJsonDocument const &doc);
	void parsePipelines(int projectId, QJsonDocument const &doc);
	void parseMRs(int projectId, QJsonDocument const &doc);
	void parseMRDetails(int projectId, int mrId, QJsonDocument const &doc);
	void parseMRDiscussions(int projectId, int mrId, QJsonDocument const &doc);
	void parseMRApprovals(int projectId, int mrId, QJsonDocument const &doc);
	void parseVariables(QJsonDocument const &doc);
	void parseBranches(int projectId, QJsonDocument const &doc);
	void parseCurrentUser(QJsonDocument const &doc);

	void loadProjects();
	void loadProjectBranches(int projectId);
	void loadProjectPipelines(int projectId);
	void loadProjectMRs(int projectId);
	void loadProjectMRInfo(int projectId);
	void loadCurrentUser();

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
	
	QString m_currentUser;

	int m_currentProject{-1};
};
