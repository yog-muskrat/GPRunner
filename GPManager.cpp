#include "GPManager.h"

#include <QFile>
#include <QJsonDocument>

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
{
	initModels();
	initUpdateTimer();
	readEmojis();
}

void GPManager::connect()
{
	loadCurrentUser();
	loadActiveUsers();
	loadProjects();

	m_updateTimer.start();
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
		m_client.requestMRDetails(projectId, mr->iid(), std::bind_front(&GPManager::parseMRDetails, this, mr));
		m_client.requestMRApprovals(projectId, mr->iid(), std::bind_front(&GPManager::parseMRApprovals, this, mr));

		if(mr->hasNotes())
		{
			m_client.requestMRDiscussions(projectId, mr->iid(), std::bind_front(&GPManager::parseMRDiscussions, this, mr));
		}
	}
}

void GPManager::loadPipelineInfo(int projectId, int pipelineId)
{
	m_client.requestPipelineInfo(projectId, pipelineId, std::bind_front(&GPManager::parsePipelineInfo, this, projectId, pipelineId));
}

void GPManager::loadPipelineJobs(int projectId, int pipelineId)
{
	m_client.requestPipelineJobs(projectId, pipelineId, std::bind_front(&GPManager::parsePipelineJobs, this, projectId, pipelineId));
}

void GPManager::loadCurrentUser()
{
	m_client.requestCurrentUser(std::bind_front(&GPManager::parseCurrentUser, this));
}

void GPManager::loadActiveUsers()
{
	m_client.requestActiveUsers(std::bind_front(&GPManager::parseActiveUsers, this));
}

void GPManager::onDiscussionAdded(QPointer<gpr::api::Discussion> discussion)
{
	if(discussion->mr().discussionsLoaded() && discussion->mr().isUserInvolved(m_currentUser) && discussion->author() != m_currentUser)
	{
		Q_EMIT notification(
			"Новая дискуссия",
			QString("Новая дискуссия в %1/%2").arg(discussion->mr().project().name()).arg(discussion->mr().title()));
		Q_EMIT newNotesReceived();
	}
}

void GPManager::onDiscussionNoteAdded(QPointer<gpr::api::Note> note)
{
	if(note->discussion().mr().discussionsLoaded() && note->discussion().mr().isUserInvolved(m_currentUser) && note->author() != m_currentUser)
	{
		Q_EMIT notification(
			"Новые сообщения",
			QString("Новые сообщение в %1/%2")
				.arg(note->discussion().mr().project().name())
				.arg(note->discussion().mr().title()));
		Q_EMIT newNotesReceived();
	}

	m_client.requestMRNoteEmojis(
		note->discussion().mr().project().id(),
		note->discussion().mr().iid(),
		note->id(),
		std::bind_front(&GPManager::parseMRNoteEmojis, this, &note->discussion().mr(), note->discussion().id(), note->id()));
}

void GPManager::onDiscussionNoteUpdated(QPointer<gpr::api::Note> note)
{
	if (note->discussion().mr().isUserInvolved(m_currentUser))
	{
		Q_EMIT newNotesReceived();
	}

	m_client.requestMRNoteEmojis(
		note->discussion().mr().project().id(),
		note->discussion().mr().iid(),
		note->id(),
		std::bind_front(&GPManager::parseMRNoteEmojis, this, &note->discussion().mr(), note->discussion().id(), note->id()));
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

QVariantList GPManager::getActiveUsers() const
{
	QVariantList result;
	result.reserve(m_activeUsers.size());
	for(auto v : m_activeUsers | std::views::transform([](auto const &u) { return QVariant::fromValue(u); }))
	{
		result.push_back(std::move(v));
	}
	return result;
}

bool GPManager::hasNewNotes() const
{
	auto notes = m_projectModel.projects()
		| std::views::transform(&gpr::api::Project::openMRs)
		| std::views::join
		| std::views::filter([this](auto const &mr) { return mr->isUserInvolved(getCurrentUser()); })
		| std::views::transform(&gpr::api::MR::discussions)
		| std::views::join
		| std::views::transform(&gpr::api::Discussion::notes)
		| std::views::join;

	return !std::ranges::all_of(notes, &gpr::api::Note::isRead);
}

void GPManager::addVariable()
{
	m_variableModel.addVariable({.key = "variable", .value = "value", .used = false});
}

void GPManager::removeVariable(int index)
{
	m_variableModel.removeRow(index);
}

gpr::Client &GPManager::client()
{
	return m_client;
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

	QObject::connect(&m_projectModel, &ProjectModel::projectMrDiscussionAdded, this, &GPManager::onDiscussionAdded);
	QObject::connect(&m_projectModel, &ProjectModel::projectMrDiscussionNoteAdded, this, &GPManager::onDiscussionNoteAdded);
	QObject::connect(&m_projectModel, &ProjectModel::projectMrDiscussionNoteUpdated, this, &GPManager::onDiscussionNoteUpdated);
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
		loadPipelineJobs(projectId, pipeline->id());
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

	auto data = gpr::api::parsePipelineInfo(doc.object());

	if (auto pipeline = project->findPipeline(pipelineId))
	{
		pipeline->setUser(std::move(data.user));
	}
	else
	{
		project->updatePipeline(std::move(data));
	}
}

void GPManager::parsePipelineJobs(int projectId, int pipelineId, QJsonDocument const &doc)
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

	auto jobs = doc.array() | std::views::transform(&QJsonValueRef::toObject)
		| std::views::transform(gpr::api::parseJobInfo)
		| std::ranges::to<std::vector>();
	pipeline->updateJobs(std::move(jobs));
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

void GPManager::parseMRDetails(QPointer<gpr::api::MR> mr, QJsonDocument const &doc)
{
	if(!mr)
	{
		qDebug() << "Invalid MR";
		return;
	}

	auto const headPipelineId = gpr::api::parseMRHeadPipeline(doc.object());

	if(auto const pos = std::ranges::find(mr->project().pipelines(), headPipelineId, &gpr::api::Pipeline::id); pos != mr->project().pipelines().cend())
	{
		mr->setPipeline(*pos);
	}
	else
	{
		loadPipelineInfo(mr->project().id(), headPipelineId);
	}
}

void GPManager::parseMRDiscussions(QPointer<gpr::api::MR> mr, QJsonDocument const &doc)
{
	if(!mr)
	{
		qDebug() << "Invalid MR";
		return;
	}

	auto discussions =  doc.array()
		| std::views::transform(&QJsonValueRef::toObject)
		| std::views::transform(gpr::api::parseDiscussion)
		| std::views::filter([](auto const &pair) { return !pair.second.empty(); })
		| std::ranges::to<std::vector>();

	mr->updateDiscussions(std::move(discussions));
}

void GPManager::parseMRNoteEmojis(QPointer<gpr::api::MR> mr, QString const &discussionId, int noteId, QJsonDocument const &doc)
{
	if(!mr)
	{
		qDebug() << "Invalid MR";
		return;
	}

	auto discussion = mr->findDiscussion(discussionId);
	if(!discussion)
	{
		qDebug() << "Invalid discussion id: " << discussionId;
		return;
	}

	auto note = discussion->findNote(noteId);
	if(!note)
	{
		qDebug() << "Invalid note id: " << noteId;
		return;
	}

	note->setReactions(gpr::api::parseNoteEmojis(doc, m_emojis));
}

void GPManager::parseMRApprovals(QPointer<gpr::api::MR> mr, QJsonDocument const &doc)
{
	if(!mr)
	{
		qDebug() << "Invalid MR";
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
	m_currentUser = gpr::api::parseUser(doc.object());
	Q_EMIT currentUserChanged(m_currentUser);
}

void GPManager::parseActiveUsers(QJsonDocument const &doc)
{
	m_activeUsers = doc.array() | std::views::transform(&QJsonValueRef::toObject) | std::views::transform(&gpr::api::parseUser) | std::ranges::to<QList>();
	std::ranges::sort(m_activeUsers, std::ranges::less{}, &gpr::User::username);
	
	Q_EMIT activeUsersChanged(getActiveUsers());
}

void GPManager::parseProviderImage(QString const &id, QByteArray data)
{
	if(QPixmap pm(std::move(data)); !pm.isNull())
	{
		m_imageProvider.addPixmap(id, std::move(pm));
	}
}

void GPManager::readEmojis()
{
	QFile f(":/icons/emojis.json");
	f.open(QIODevice::ReadOnly);

	m_emojis = gpr::api::parseEmojis(QJsonDocument::fromJson(f.readAll()));
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
