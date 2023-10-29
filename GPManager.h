#pragma once

#include <QTimer>
#include <QObject>
#include <QPointer>
#include <QNetworkAccessManager>

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

	Q_INVOKABLE void loadProjects();
	Q_INVOKABLE void setCurrentProject(int projectId);
	Q_INVOKABLE void runPipeline(QString const &ref);
	Q_INVOKABLE QStringList getProjectBranches(int projectId);
	Q_INVOKABLE void loadPipelineVariables(QString const &ref);

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

	ProjectModel *getProjectModel() const;
	PipelineModel *getPipelineModel() const;
	MRModel *getMRModel() const;
	VariableModel *getVariableModel() const;
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
	void parseProjects(QJsonDocument const &doc);
	void parsePipelines(int projectId, QJsonDocument const &doc);
	void parseMRs(int projectId, QJsonDocument const &doc);
	void parseVariables(QJsonDocument const &doc);
	void parseBranches(int projectId, QJsonDocument const &doc);
	void parseCurrentUser(QJsonDocument const &doc);

	void loadProjectBranches(int projectId);
	void loadProjectAvatar(int projectId, QString const &avatarUrl);
	void loadProjectPipelines(int projectId);
	void loadProjectMRs(int projectId);

	void loadCurrentUser();

	void readSettings();

	void update();

	QJsonArray prepareVariables() const;

	QPointer<ProjectModel> m_projectModel;
	QPointer<PipelineModel> m_pipelineModel;
	QPointer<MRModel> m_mrModel;
	QPointer<VariableModel> m_variableModel;
	QPointer<QNetworkAccessManager> m_networkManager;

	QTimer m_updateTimer;
	
	QString m_currentUser;

	int m_currentProject{-1};

	gpr::Settings m_settings;
};
