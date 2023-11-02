#include "GPManager.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include <thread>
#include <ranges>
#include <format>

#include "model/classes/Parser.h"

namespace
{
	std::chrono::milliseconds const UpdateInterval{3000};
}

GPManager::GPManager(QObject *parent)
	: QObject(parent)
{
	initModels();
	initUpdateTimer();
}

void GPManager::connect()
{
	loadCurrentUser();

	m_client.requestProjects(std::bind_front(&GPManager::parseProjects, this));

	m_updateTimer.start();
}

void GPManager::setCurrentProject(int projectId)
{
	if (projectId != m_currentProject)
	{
		m_pipelineModel->clear();
		m_mrModel->clear();
		m_variableModel->clear();
	}

	m_currentProject = projectId;

	auto prj = m_projectModel->findProject(projectId);

	m_pipelineModel->setProject(prj);
	m_mrModel->setProject(prj);
}

void GPManager::loadProjects()
{
	m_client.requestProjects(std::bind_front(&GPManager::parseProjects, this));
}

void GPManager::loadProjectPipelines(int projectId)
{
	m_client.requestProjectPipelines(projectId, std::bind_front(&GPManager::parsePipelines, this, projectId));
}

void GPManager::runPipeline(QString const &ref)
{
	m_client.runPipeline(m_currentProject, ref, m_variableModel->variables());
}

QStringList GPManager::getProjectBranches(int projectId)
{
	if (auto prj = m_projectModel->findProject(projectId)) return prj->branches();
	return QStringList{"master"};
}

void GPManager::loadPipelineVariables(QString const &ref)
{
	m_client.requestPipelineVariables(m_currentProject, ref, std::bind_front(&GPManager::parseVariables, this));
}

void GPManager::loadProjectBranches(int projectId)
{
	m_client.requestProjectBranches(projectId, std::bind_front(&GPManager::parseBranches, this, projectId));
}

void GPManager::loadProjectMRs(int projectId)
{
	m_client.requestProjectMRs(projectId, std::bind_front(&GPManager::parseMRs, this, projectId));
}

void GPManager::loadCurrentUser()
{
	m_client.requestCurrentUser(std::bind_front(&GPManager::parseCurrentUser, this));
}

void GPManager::cancelPipeline(int pipelineId)
{
	m_client.cancelPipeline(m_currentProject, pipelineId);
}

void GPManager::retryPipeline(int pipelineId)
{
	m_client.retryPipeline(m_currentProject, pipelineId);
}

PipelineModel *GPManager::getPipelineModel() const
{
	return m_pipelineModel.get();
}

MRModel *GPManager::getMRModel() const
{
	return m_mrModel.get();
}

VariableModel *GPManager::getVariableModel() const
{
	return m_variableModel.get();
}

void GPManager::addVariable()
{
	m_variableModel->addVariable({.key = "variable", .value = "value", .used = false});
}

void GPManager::removeVariable(int index)
{
	m_variableModel->removeRow(index);
}

ProjectModel *GPManager::getProjectModel() const
{
	return m_projectModel.get();
}

void GPManager::initModels()
{
	m_projectModel = new ProjectModel(*this, this);
	m_pipelineModel = new PipelineModel(this);
	m_mrModel = new MRModel(*this, this);
	m_variableModel = new VariableModel(this);
}

void GPManager::initUpdateTimer()
{
	m_updateTimer.setSingleShot(true);
	m_updateTimer.setInterval(UpdateInterval);

	QObject::connect(&m_updateTimer, &QTimer::timeout, this, &GPManager::update);
}

void GPManager::parseProjects(QJsonDocument const &doc)
{
	m_projectModel->clear();

	for (auto prj : doc.array() | std::views::transform(&QJsonValueRef::toObject) | std::views::transform(gpr::api::parseProject))
	{
		m_projectModel->addProject(prj);
		loadProjectBranches(prj.id);
	}
}

void GPManager::parsePipelines(int projectId, QJsonDocument const &doc)
{
	auto project = m_projectModel->findProject(projectId);
	if (!project)
	{
		qDebug() << "Invalid project id: " << projectId;
		return;
	}

	auto pipelines = doc.array() | std::views::transform(&QJsonValueRef::toObject)
	               | std::views::transform(gpr::api::parsePipeline) | std::ranges::to<std::vector>();

	project->updatePipelines(std::move(pipelines));
}

void GPManager::parseMRs(int projectId, QJsonDocument const &doc)
{
	auto project = m_projectModel->findProject(projectId);
	if (!project)
	{
		qDebug() << "Invalid project id: " << projectId;
		return;
	}

	auto mrs = doc.array() | std::views::transform(&QJsonValueRef::toObject) | std::views::transform(gpr::api::parseMR)
	         | std::ranges::to<std::vector>();

	project->updateMRs(std::move(mrs));
}

void GPManager::parseVariables(QJsonDocument const &doc)
{
	if (auto const content = doc.object().value("content"); !content.isNull())
	{
		auto varsDoc = QJsonDocument::fromJson(QByteArray::fromBase64(content.toString().toUtf8()));

		if (!varsDoc.isArray())
		{
			qDebug() << "Invalid variables configuration format";
			return;
		}

		std::vector<gpr::Variable> vars;
		for (auto const var : varsDoc.array())
		{
			vars.push_back(
				{.key = var.toObject().value("key").toString(),
			     .value = var.toObject().value("value").toString(),
			     .used = var.toObject().value("required").toBool()});
		}
		m_variableModel->setVariables(std::move(vars));
	}
	else
	{
		qDebug() << "Failed to get variables configuration";
	}
}

void GPManager::parseBranches(int projectId, QJsonDocument const &doc)
{
	QStringList result;
	for (auto const &branchValue : doc.array())
	{
		auto const branch = branchValue.toObject();
		auto name = branch.value("name").toString();

		if (branch.value("default").toBool())
		{
			result.prepend(std::move(name));
		}
		else
		{
			result << std::move(name);
		}
	}

	if (auto prj = m_projectModel->findProject(projectId))
	{
		prj->setBranches(std::move(result));
	}
}

void GPManager::parseCurrentUser(QJsonDocument const &doc)
{
	auto const obj = doc.object();

	m_currentUser = obj.value("username").toString();

	Q_EMIT currentUserChanged(m_currentUser);
}

void GPManager::update()
{
	m_updateTimer.stop();

	for (auto const id :
	     std::views::iota(0, m_projectModel->rowCount())
	         | std::views::transform([this](int row)
	                                 { return m_projectModel->index(row, 0).data(ProjectModel::Role::ProjectIdRole).toInt(); }))
	{
		loadProjectMRs(id);
		loadProjectPipelines(id);
		loadProjectBranches(id);
	}

	m_updateTimer.start();
}
