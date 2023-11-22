#include "GPManager.h"

#ifdef _DEBUG
#include <QAbstractItemModelTester>
#endif

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include <thread>
#include <ranges>
#include <format>

#include "model/classes/Parser.h"
#include "ImageProvider.h"

namespace
{
	std::chrono::milliseconds const UpdateInterval{3000};
}

GPManager::GPManager(ImageProvider &imageProvider, QObject *parent)
	: QObject(parent)
	, m_imageProvider{imageProvider}
	, m_projectModel{*this}
	, m_mrModel{*this}
	, m_discussionModel{*this}
{
	initModels();
	initUpdateTimer();
}

void GPManager::connect()
{
	loadCurrentUser();
	loadProjects();

	m_updateTimer.start();
}

void GPManager::setCurrentProject(int projectId)
{
	if (projectId != m_currentProject)
	{
		m_currentMR = 0;
		
		m_pipelineModel.clear();
		m_mrModel.clear();
		m_variableModel.clear();
		m_discussionModel.clear();
	}

	m_currentProject = projectId;

	auto prj = m_projectModel.findProject(projectId);

	m_pipelineModel.setProject(prj);
	m_mrModel.setProject(prj);
}

void GPManager::setCurrentMR(int projectId, int mrId)
{
	auto project = m_projectModel.findProject(projectId);
	assert(project);

	auto mr = project->findMR(mrId);
	assert(mr);

	m_currentMR = mrId;
	m_discussionModel.setMR(mr);
}

void GPManager::loadProjects()
{
	m_client.requestProjects(std::bind_front(&GPManager::parseProjects, this));
}

void GPManager::loadProjectPipelines(int projectId)
{
	m_client.requestProjectPipelines(projectId, std::bind_front(&GPManager::parsePipelines, this, projectId));
}

void GPManager::runPipeline(int projectId, QString const &ref)
{
	m_client.runPipeline(projectId, ref, m_variableModel.variables());
}

QStringList GPManager::getProjectBranches(int projectId)
{
	if (auto prj = m_projectModel.findProject(projectId)) return prj->branches();
	return QStringList{"master"};
}

void GPManager::loadPipelineVariables(int projectId, QString const &ref)
{
	m_client.requestPipelineVariables(projectId, ref, std::bind_front(&GPManager::parseVariables, this));
}

void GPManager::loadPipelineStatistics(int projectId, QDateTime const &from, QDateTime const &to)
{
	std::ignore = projectId, from, to;
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
		m_client.requestMRDetails(projectId, mr->iid(), std::bind_front(&GPManager::parseMRDetails, this, projectId, mr->id()));
		m_client.requestMRApprovals(projectId, mr->iid(), std::bind_front(&GPManager::parseMRApprovals, this, projectId, mr->id()));

		if(mr->hasNotes())
		{
			m_client.requestMRDiscussions(projectId, mr->iid(), std::bind_front(&GPManager::parseMRDiscussions, this, projectId, mr->id()));
		}
	}
}

void GPManager::loadPipelineInfo(int projectId, int pipelineId)
{
	m_client.requestPipelineInfo(projectId, pipelineId, std::bind_front(&GPManager::parsePipelineInfo, this, projectId, pipelineId));
}

void GPManager::loadCurrentUser()
{
	m_client.requestCurrentUser(std::bind_front(&GPManager::parseCurrentUser, this));
}

void GPManager::onDiscussionAdded(
	QPointer<gpr::api::Project> project,
	QPointer<gpr::api::MR> mr,
	gpr::Discussion const &)
{
	if(mr->isUserInvolved(m_currentUser))
	{
		Q_EMIT notification("Новая дискуссия", QString("Новая дискуссия в %1/%2").arg(project->name()).arg(mr->title()));
		Q_EMIT newNotesReceived();
	}
}

void GPManager::onDiscussionNoteAdded(
	QPointer<gpr::api::Project> project,
	QPointer<gpr::api::MR> mr,
	gpr::Discussion const &,
	gpr::Note const &)
{
	if(mr->isUserInvolved(m_currentUser))
	{
		Q_EMIT notification("Новые сообщения", QString("Новые сообщение в %1/%2").arg(project->name()).arg(mr->title()));
		Q_EMIT newNotesReceived();
	}
}

void GPManager::onDiscussionNoteUpdated(
	QPointer<gpr::api::Project>,
	QPointer<gpr::api::MR> mr,
	gpr::Discussion const &,
	gpr::Note const &)
{
	if(mr->isUserInvolved(m_currentUser))
	{
		Q_EMIT newNotesReceived();
	}
}

void GPManager::onMergeRequestRemoved(QPointer<gpr::api::Project> project, QPointer<gpr::api::MR> mr)
{
	if(project->id() == m_currentProject && mr->id() == m_currentMR)
	{
		m_currentMR = -1;
		m_mrModel.clear();
	}
}

void GPManager::cancelPipeline(int pipelineId)
{
	m_client.cancelPipeline(m_currentProject, pipelineId);
}

void GPManager::retryPipeline(int pipelineId)
{
	m_client.retryPipeline(m_currentProject, pipelineId);
}

void GPManager::approveMR(int projectId, int mrIid)
{
	m_client.approveMR(projectId, mrIid);
}

void GPManager::unapproveMR(int projectId, int mrIid)
{
	m_client.unapproveMR(projectId, mrIid);
}

void GPManager::resolveMRDiscussion(int projectId, int mrIid, QString const &discussionId)
{
	m_client.resolveDiscussion(projectId, mrIid, discussionId);
}

void GPManager::unresolveMRDiscussion(int projectId, int mrIid, QString const &discussionId)
{
	m_client.unresolveDiscussion(projectId, mrIid, discussionId);
}

void GPManager::addMRDiscussionNote(int projectId, int mrIid, QString const &discussionId, QString const &note)
{
	m_client.addDiscussionNote(projectId, mrIid, discussionId, note);
}

void GPManager::addMRDiscussion(int projectId, int mrIid, QString const &note)
{
	m_client.addDiscussion(projectId, mrIid, note);
}

void GPManager::editMRDiscussionNote(int projectId, int mrIid, QString const &discussionId, int noteId, QString const &note)
{
	m_client.editDiscussionNote(projectId, mrIid, discussionId, noteId, note);
}

void GPManager::removeMRDiscussionNote(int projectId, int mrIid, QString const &discussionId, int noteId)
{
	m_client.removeDiscussionNote(projectId, mrIid, discussionId, noteId);
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

QAbstractItemModel *GPManager::getDiscussionModel()
{
	return &m_discussionProxyModel;
}

bool GPManager::hasNewNotes() const
{
	auto notes = m_projectModel.projects()
		| std::views::transform(&gpr::api::Project::openMRs)
		| std::views::join
		| std::views::filter([this](auto const &mr) { return mr->isUserInvolved(getCurrentUser()); })
		| std::views::transform(&gpr::api::MR::discussions)
		| std::views::join
		| std::views::transform(&gpr::Discussion::notes)
		| std::views::join;

	return !std::ranges::all_of(notes, &gpr::Note::wasShown);
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

	m_discussionProxyModel.setSourceModel(&m_discussionModel);

	QObject::connect(&m_projectModel, &ProjectModel::projectMergeRequestRemoved, this, &GPManager::onMergeRequestRemoved);
	QObject::connect(&m_projectModel, &ProjectModel::projectMrDiscussionAdded, this, &GPManager::onDiscussionAdded);
	QObject::connect(&m_projectModel, &ProjectModel::projectMrDiscussionNoteAdded, this, &GPManager::onDiscussionNoteAdded);
	QObject::connect(&m_projectModel, &ProjectModel::projectMrDiscussionNoteUpdated, this, &GPManager::onDiscussionNoteUpdated);

#ifdef _DEBUG
	new QAbstractItemModelTester(&m_projectModel, this);
	new QAbstractItemModelTester(&m_pipelineModel, this);
	new QAbstractItemModelTester(&m_mrModel, this);
	new QAbstractItemModelTester(&m_variableModel, this);
	new QAbstractItemModelTester(&m_discussionModel, this);
#endif
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

		if (!prj.avatarUrl.isEmpty())
		{
			m_client.requestFileDownload(
				prj.avatarUrl,
				std::bind_front(&GPManager::parseProviderImage, this, QString("project_%1").arg(prj.id)));
		}
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
	               | std::views::transform(gpr::api::parseProjectPipeline) | std::ranges::to<std::vector>();

	project->updatePipelines(std::move(pipelines));

	for (auto const &pipeline :
	     project->pipelines() | std::views::filter([](auto const &p) { return p->user().username.isEmpty(); }))
	{
		loadPipelineInfo(projectId, pipeline->id());
	}
}

void GPManager::parsePipelineInfo(int projectId, int pipelineId, QJsonDocument const &doc)
{
	auto project = m_projectModel.findProject(projectId);
	if (!project)
	{
		qDebug() << "Invalid project id: " << projectId;
		return;
	}

	auto pipeline = project->findPipeline(pipelineId);
	if (!pipeline)
	{
		qDebug() << "Invalid pipeline id: " << pipelineId;
		return;
	}

	auto data = gpr::api::parsePipelineInfo(doc.object());
	pipeline->setUser(std::move(data.user));
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

void GPManager::parseMRDetails(int projectId, int mrId, QJsonDocument const &doc)
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

	mr->setPipelineStatus(gpr::api::parseMRDetails(doc.object()));
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

		m_variableModel.setVariables(gpr::api::parseVariables(varsDoc));
	}
	else
	{
		qDebug() << "Failed to get variables configuration";
	}
}

void GPManager::parseBranches(int projectId, QJsonDocument const &doc)
{
	if (auto prj = m_projectModel.findProject(projectId))
	{
		prj->setBranches(gpr::api::parseBranches(doc));
	}
}

void GPManager::parseCurrentUser(QJsonDocument const &doc)
{
	auto const obj = doc.object();

	m_currentUser.username = obj.value("username").toString();
	m_currentUser.avatarUrl = obj["avatar_url"].toString();

	Q_EMIT currentUserChanged(m_currentUser);
}

void GPManager::parseProviderImage(QString const &id, QByteArray data)
{
	if(QPixmap pm(std::move(data)); !pm.isNull())
	{
		m_imageProvider.addPixmap(id, std::move(pm));
	}
}

void GPManager::update()
{
	m_updateTimer.stop();

	for (auto const id : m_projectModel.projects() | std::views::transform(&gpr::api::Project::id))
	{
		loadProjectMRs(id);
		loadProjectPipelines(id);
		loadProjectBranches(id);
		loadProjectMRInfo(id);
	}

	m_updateTimer.start();
}
