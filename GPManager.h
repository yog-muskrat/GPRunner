#pragma once

#include <QTimer>
#include <QObject>
#include <QPointer>
#include <QNetworkAccessManager>

#include "ProjectModel.h"
#include "PipelineModel.h"
#include "VariableModel.h"

class QJsonDocument;
class QJsonArray;

class GPManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QObject *projectModel READ getProjectModel NOTIFY projectModelChanged)
    Q_PROPERTY(QObject *pipelineModel READ getPipelineModel NOTIFY pipelineModelChanged)
    Q_PROPERTY(QObject *variableModel READ getVariableModel NOTIFY variableModelChanged)

public:
    GPManager(QObject* parent = nullptr);

    Q_INVOKABLE void loadProjects();
    Q_INVOKABLE void loadPipelines(int projectId);
    Q_INVOKABLE void runPipeline(QString const &ref);
    Q_INVOKABLE QStringList getProjectBranches(int projectId);
    
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

    ProjectModel * getProjectModel() const;
    PipelineModel * getPipelineModel() const;
    VariableModel * getVariableModel() const;

    Q_INVOKABLE void addVariable();
	Q_INVOKABLE void removeVariable(int index);

signals:
    void pipelineModelChanged();
    void projectModelChanged();
    void variableModelChanged();

private:
    void parseProjects(QJsonDocument const& doc);
	void loadProjectBranches(int projectId);

    void parsePipelines(QJsonDocument const& doc);
    void readSettings();

    QJsonArray prepareVariables() const;

    QPointer<ProjectModel> m_projectModel;
    QPointer<PipelineModel> m_pipelineModel;
    QPointer<VariableModel> m_variableModel;
    QPointer<QNetworkAccessManager> m_networkManager;
    QTimer m_pipelineUpdateTimer;
    int m_currentProject{ -1 };

    gpr::Settings m_settings;
};
