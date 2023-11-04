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
	, m_projectModel(*this)
	, m_mrModel(*this)
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
		m_pipelineModel.clear();
		m_mrModel.clear();
		m_variableModel.clear();
	}

	m_currentProject = projectId;

	auto prj = m_projectModel.findProject(projectId);

	m_pipelineModel.setProject(prj);
	m_mrModel.setProject(prj);
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
	m_client.runPipeline(m_currentProject, ref, m_variableModel.variables());
}

QStringList GPManager::getProjectBranches(int projectId)
{
	if (auto prj = m_projectModel.findProject(projectId)) return prj->branches();
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

void GPManager::loadProjectMRInfo(int projectId)
{
	auto prj = m_projectModel.findProject(projectId);
	if (!prj) return;

	for (auto const &mr : prj->openMRs())
	{
		if(mr->hasNotes())
		{
			m_client.requestMRDiscussions(projectId, mr->iid(), std::bind_front(&GPManager::parseMRDiscussions, this, projectId, mr->id()));
		}

		m_client.requestMRApprovals(projectId, mr->iid(), std::bind_front(&GPManager::parseMRApprovals, this, projectId, mr->id()));
	}
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

QAbstractItemModel *GPManager::getProjectModel()
{
	return &m_projectProxyModel;
}

QAbstractItemModel *GPManager::getPipelineModel()
{
	return &m_pipelineProxyModel;
}

QAbstractItemModel *GPManager::getMRModel()
{
	return &m_mrProxyModel;
}

QAbstractItemModel *GPManager::getVariableModel()
{
	return &m_variableProxyModel;
}

void GPManager::addVariable()
{
	m_variableModel.addVariable({.key = "variable", .value = "value", .used = false});
}

void GPManager::removeVariable(int index)
{
	m_variableModel.removeRow(index);
}

void GPManager::initModels()
{
	m_projectProxyModel.setSourceModel(&m_projectModel);
	m_projectProxyModel.setDynamicSortFilter(true);
	m_projectProxyModel.setSortCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
	m_projectProxyModel.sort(ProjectModel::Column::Name);

	m_pipelineProxyModel.setSourceModel(&m_pipelineModel);
	m_pipelineProxyModel.setDynamicSortFilter(true);
	m_pipelineProxyModel.setSortRole(Qt::ItemDataRole::EditRole);
	m_pipelineProxyModel.sort(PipelineModel::Column::Created, Qt::SortOrder::DescendingOrder);

	m_mrProxyModel.setSourceModel(&m_mrModel);
	m_mrProxyModel.setDynamicSortFilter(true);
	m_mrProxyModel.setSortRole(Qt::ItemDataRole::EditRole);
	m_mrProxyModel.sort(MRModel::Column::Updated, Qt::SortOrder::DescendingOrder);

	m_variableProxyModel.setSourceModel(&m_variableModel);
}

void GPManager::initUpdateTimer()
{
	m_updateTimer.setSingleShot(true);
	m_updateTimer.setInterval(UpdateInterval);

	QObject::connect(&m_updateTimer, &QTimer::timeout, this, &GPManager::update);
}

void GPManager::parseProjects(QJsonDocument const &doc)
{
	m_projectModel.clear();

	for (auto prj : doc.array() | std::views::transform(&QJsonValueRef::toObject) | std::views::transform(gpr::api::parseProject))
	{
		m_projectModel.addProject(prj);
		loadProjectBranches(prj.id);
	}
}

void GPManager::parsePipelines(int projectId, QJsonDocument const &doc)
{
	auto project = m_projectModel.findProject(projectId);
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
	auto project = m_projectModel.findProject(projectId);
	if (!project)
	{
		qDebug() << "Invalid project id: " << projectId;
		return;
	}

	auto mrs = doc.array()
		| std::views::transform(&QJsonValueRef::toObject)
		| std::views::transform(gpr::api::parseMR)
		| std::ranges::to<std::vector>();

	project->updateMRs(std::move(mrs));
}

void GPManager::parseMRDiscussions(int projectId, int mrId, QJsonDocument const &doc)
{
	auto project = m_projectModel.findProject(projectId);
	if (!project)
	{
		qDebug() << "Invalid project id: " << projectId;
		return;
	}

	auto mr = project->findMR(mrId);
	if(!mr)
	{
		qDebug() << "Invalid MR id: " << mrId;
		return;
	}

	auto discussions = doc.array()
		| std::views::transform(&QJsonValueRef::toObject)
		| std::views::transform(gpr::api::parseDiscussion)
		| std::views::filter(std::not_fn(&gpr::Discussion::isEmpty))
		| std::ranges::to<std::vector>();

	mr->updateDiscussions(std::move(discussions));
}

void GPManager::parseMRApprovals(int projectId, int mrId, QJsonDocument const &doc)
{
	auto project = m_projectModel.findProject(projectId);
	if (!project)
	{
		qDebug() << "Invalid project id: " << projectId;
		return;
	}

	auto mr = project->findMR(mrId);
	if(!mr)
	{
		qDebug() << "Invalid MR id: " << mrId;
		return;
	}

	mr->setApprovedBy(gpr::api::parseApprovals(doc.object()));
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
		m_variableModel.setVariables(std::move(vars));
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

	if (auto prj = m_projectModel.findProject(projectId))
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
	     std::views::iota(0, m_projectModel.rowCount())
	         | std::views::transform([this](int row)
	                                 { return m_projectModel.index(row, 0).data(ProjectModel::Role::ProjectIdRole).toInt(); }))
	{
		loadProjectMRs(id);
		loadProjectPipelines(id);
		loadProjectBranches(id);
		loadProjectMRInfo(id);
	}

	m_updateTimer.start();
}
