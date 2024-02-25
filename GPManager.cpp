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

void GPManager::loadProjectPipelines(QPointer<gpr::api::Project> project)
{
	m_client.requestProjectPipelines(project->id(), std::bind_front(&GPManager::parsePipelines, this, project));
}

void GPManager::loadProjectPipelineVariables(QPointer<gpr::api::Project> project, QString const &ref)
{
	m_client.requestPipelineVariables(project->id(), ref, std::bind_front(&GPManager::parseVariables, this, project));
}

void GPManager::loadPipelineStatistics(QPointer<gpr::api::Project> project, QDateTime const &from, QDateTime const &to)
{
	std::ignore = project, from, to;
}

void GPManager::loadProjectBranches(QPointer<gpr::api::Project> project)
{
	m_client.requestProjectBranches(project->id(), std::bind_front(&GPManager::parseBranches, this, project));
}

void GPManager::loadProjectMRs(QPointer<gpr::api::Project> project)
{
	m_client.requestProjectMRs(project->id(), std::bind_front(&GPManager::parseMRs, this, project));
}

void GPManager::loadProjectMRInfo(QPointer<gpr::api::Project> project)
{
	for (auto const &mr : project->openMRs())
	{
		m_client.requestMRDetails(project->id(), mr->iid(), std::bind_front(&GPManager::parseMRDetails, this, mr));
		m_client.requestMRApprovals(project->id(), mr->iid(), std::bind_front(&GPManager::parseMRApprovals, this, mr));

		if(mr->hasNotes())
		{
			m_client.requestMRDiscussions(project->id(), mr->iid(), std::bind_front(&GPManager::parseMRDiscussions, this, mr));
		}
	}
}

void GPManager::loadPipelineInfo(QPointer<gpr::api::Project> project, int pipelineId)
{
	m_client.requestPipelineInfo(project->id(), pipelineId, std::bind_front(&GPManager::parsePipelineInfo, this, project, pipelineId));
}

void GPManager::loadPipelineJobs(QPointer<gpr::api::Pipeline> pipeline)
{
	m_client.requestPipelineJobs(pipeline->project().id(), pipeline->id(), std::bind_front(&GPManager::parsePipelineJobs, this, pipeline));
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
		Q_EMIT newNotesChanged();
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
		Q_EMIT newNotesChanged();
	}

	m_client.requestMRNoteEmojis(
		note->discussion().mr().project().id(),
		note->discussion().mr().iid(),
		note->id(),
		std::bind_front(&GPManager::parseMRNoteEmojis, this, &note->discussion().mr(), note->discussion().id(), note->id()));
}

void GPManager::onDiscussionNoteUpdated(QPointer<gpr::api::Note> note)
{
	Q_EMIT newNotesChanged();

	m_client.requestMRNoteEmojis(
		note->discussion().mr().project().id(),
		note->discussion().mr().iid(),
		note->id(),
		std::bind_front(&GPManager::parseMRNoteEmojis, this, &note->discussion().mr(), note->discussion().id(), note->id()));
}

QAbstractItemModel *GPManager::getPipelineModel()
{
	return &m_pipelineProxyModel;
}

QAbstractItemModel *GPManager::getMRModel()
{
	return &m_mrProxyModel;
}

QList<gpr::User> GPManager::getActiveUsers() const
{
	return m_activeUsers;
}

bool GPManager::hasNewNotes() const
{
	auto notes = m_projects
		| std::views::transform(&gpr::api::Project::openMRs)
		| std::views::join
		| std::views::filter([this](auto const &mr) { return mr->isUserInvolved(getCurrentUser()); })
		| std::views::transform(&gpr::api::MR::discussions)
		| std::views::join
		| std::views::transform(&gpr::api::Discussion::notes)
		| std::views::join;

	return !std::ranges::all_of(notes, &gpr::api::Note::isRead);
}

gpr::Client &GPManager::client()
{
	return m_client;
}

void GPManager::initModels()
{
	m_pipelineProxyModel.setSourceModel(&m_pipelineModel);
	m_pipelineProxyModel.setDynamicSortFilter(true);
	m_pipelineProxyModel.setSortRole(Qt::ItemDataRole::EditRole);
	m_pipelineProxyModel.sort(PipelineModel::Column::Created, Qt::SortOrder::DescendingOrder);

	m_mrProxyModel.setSourceModel(&m_mrModel);
	m_mrProxyModel.setDynamicSortFilter(true);
	m_mrProxyModel.setSortRole(Qt::ItemDataRole::EditRole);
	m_mrProxyModel.sort(MRModel::Column::Updated, Qt::SortOrder::DescendingOrder);
}

void GPManager::initUpdateTimer()
{
	m_updateTimer.setSingleShot(true);
	m_updateTimer.setInterval(UpdateInterval);

	QObject::connect(&m_updateTimer, &QTimer::timeout, this, &GPManager::update);
}

void GPManager::connectProject(QPointer<gpr::api::Project> project)
{
	QObject::connect(project, &gpr::api::Project::mrDiscussionAdded, this, &GPManager::onDiscussionAdded);
	QObject::connect(project, &gpr::api::Project::mrDiscussionNoteAdded, this, &GPManager::onDiscussionNoteAdded);
	QObject::connect(project, &gpr::api::Project::mrDiscussionNoteUpdated, this, &GPManager::onDiscussionNoteUpdated);
}

void GPManager::parseProjects(QJsonDocument const &doc)
{
	m_projects.clear();

	for (auto prjData : doc.array() | std::views::transform(&QJsonValueRef::toObject) | std::views::transform(gpr::api::parseProject))
	{
		auto project = new gpr::api::Project(*this, std::move(prjData));
		loadProjectBranches(project);
		loadProjectPipelineVariables(project);
		m_projects << project;

		connectProject(project);

		//TODO
		//if (!project->avatarUrl().isEmpty())
		//{
		//	m_client.requestFileDownload(
		//		project->avatarUrl(),
		//		std::bind_front(&GPManager::parseProviderImage, this, QString("project_%1").arg(project->id())));
		//}
	}

	std::ranges::sort(
		m_projects,
		[](auto const &l, auto const &r) { return l.compare(r, Qt::CaseInsensitive) < 0; },
		&gpr::api::Project::name);

	Q_EMIT projectsLoaded();
}

void GPManager::parsePipelines(QPointer<gpr::api::Project> project, QJsonDocument const &doc)
{
	auto pipelines = doc.array() | std::views::transform(&QJsonValueRef::toObject)
	               | std::views::transform(gpr::api::parseProjectPipeline) | std::ranges::to<std::vector>();

	project->updatePipelines(std::move(pipelines));

	for (auto const &pipeline :
	     project->pipelines() | std::views::filter([](auto const &p) { return p->user().username.isEmpty(); }))
	{
		loadPipelineInfo(project, pipeline->id());
		loadPipelineJobs(pipeline);
	}
}

void GPManager::parsePipelineInfo(QPointer<gpr::api::Project> project, int pipelineId, QJsonDocument const &doc)
{
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

void GPManager::parsePipelineJobs(QPointer<gpr::api::Pipeline> pipeline, QJsonDocument const &doc)
{
	auto jobs = doc.array() | std::views::transform(&QJsonValueRef::toObject)
		| std::views::transform(gpr::api::parseJobInfo)
		| std::ranges::to<std::vector>();
	pipeline->updateJobs(std::move(jobs));
}

void GPManager::parseMRs(QPointer<gpr::api::Project> project, QJsonDocument const &doc)
{
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
		loadPipelineInfo(&mr->project(), headPipelineId);
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
		| std::views::transform([this](auto const &obj) { return gpr::api::parseDiscussion(obj, *this); })
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

	note->setReactions(gpr::api::parseNoteEmojis(doc, *this));
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

void GPManager::parseVariables(QPointer<gpr::api::Project> project, QJsonDocument const &doc)
{
	if(!project)
	{
		assert(false && "Invalid project");
		return;
	}

	if (auto const content = doc.object().value("content"); !content.isNull())
	{
		auto varsDoc = QJsonDocument::fromJson(QByteArray::fromBase64(content.toString().toUtf8()));

		project->setVariables(gpr::api::parseVariables(varsDoc));
	}
	else
	{
		qDebug() << "Failed to get variables configuration";
	}
}

void GPManager::parseBranches(QPointer<gpr::api::Project> project, QJsonDocument const &doc)
{
	project->setBranches(gpr::api::parseBranches(doc));
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

	for (auto prj : m_projects)
	{
		loadProjectMRs(prj);
		loadProjectPipelines(prj);
		loadProjectBranches(prj);
		loadProjectMRInfo(prj);
	}

	m_updateTimer.start();
}
