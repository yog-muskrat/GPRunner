#include "GPManager.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>

#include <thread>
#include <ranges>
#include <format>

#include "model/classes/Parser.h"

namespace
{
	QString const SettingsFile{"./settings.json"};
	std::string_view const VariablesPath{"%2Egitlab%2Fci%2Fpipeline_variables%2Ejson"};
	std::chrono::milliseconds const UpdateInterval{3000};
}

GPManager::GPManager(QObject *parent)
	: QObject(parent)
{
	readSettings();

	m_projectModel = new ProjectModel(*this, this);
	m_pipelineModel = new PipelineModel(this);
	m_mrModel = new MRModel(*this, this);
	m_variableModel = new VariableModel(this);

	m_networkManager = new QNetworkAccessManager;
	m_networkManager->connectToHost(m_settings.gitlabRoot, 443);

	m_updateTimer.setSingleShot(true);
	m_updateTimer.setInterval(UpdateInterval);
	connect(&m_updateTimer, &QTimer::timeout, this, &GPManager::update);

	loadCurrentUser();
}

void GPManager::loadProjects()
{
	QNetworkRequest req;
	req.setRawHeader("PRIVATE-TOKEN", m_settings.privateToken.toUtf8());
	req.setUrl(QUrl(QString{"%1/api/v4/projects"}.arg(m_settings.gitlabRoot)));

	auto reply = m_networkManager->get(req);

	connect(
		reply,
		&QNetworkReply::finished,
		[this, reply]
		{
			parseProjects(QJsonDocument::fromJson(reply->readAll()));
			reply->deleteLater();
			m_updateTimer.start();
		});
}

void GPManager::setCurrentProject(int projectId)
{
	m_updateTimer.stop();

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
	
	m_updateTimer.start();
}

void GPManager::loadProjectPipelines(int projectId)
{
	QNetworkRequest req;
	req.setRawHeader("PRIVATE-TOKEN", m_settings.privateToken.toUtf8());
	req.setUrl(QUrl(QString("%1/api/v4/projects/%2/pipelines").arg(m_settings.gitlabRoot).arg(projectId)));

	auto reply = m_networkManager->get(req);

	connect(
		reply,
		&QNetworkReply::finished,
		[this, reply, projectId]
		{
			if (reply->error()) return;

			parsePipelines(projectId, QJsonDocument::fromJson(reply->readAll()));
			reply->deleteLater();
		});
}

void GPManager::runPipeline(QString const &ref)
{
	QNetworkRequest req;
	req.setRawHeader("PRIVATE-TOKEN", m_settings.privateToken.toUtf8());
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	req.setUrl(QUrl(QString("%1/api/v4/projects/%2/pipeline").arg(m_settings.gitlabRoot).arg(m_currentProject)));

	QJsonObject param;
	param.insert("ref", ref);
	param.insert("variables", prepareVariables());

	qDebug() << "Run pipeline variables: " << QJsonDocument(param).toJson(QJsonDocument::Compact);

	auto reply = m_networkManager->post(req, QJsonDocument(param).toJson(QJsonDocument::Compact));

	connect(
		reply,
		&QNetworkReply::finished,
		[this, reply]
		{
			qDebug() << "Run pipeline response: " << reply->readAll();

			reply->deleteLater();
		});
}

QStringList GPManager::getProjectBranches(int projectId)
{
	if (auto prj = m_projectModel->findProject(projectId)) return prj->branches();
	return QStringList{"master"};
}

void GPManager::loadPipelineVariables(QString const &ref)
{
	QNetworkRequest req;
	req.setRawHeader("PRIVATE-TOKEN", m_settings.privateToken.toUtf8());

	auto const url = QString::fromStdString(std::format("{}/api/v4/projects/{}/repository/files/{}?ref={}", 
	                     m_settings.gitlabRoot.toStdString(), m_currentProject, VariablesPath, ref.toStdString()));

	qDebug() << "Request pipeline variables: " << url;

	req.setUrl(QUrl(url));

	auto reply = m_networkManager->get(req);

	connect(
		reply,
		&QNetworkReply::finished,
		[this, reply]
		{
			auto doc = QJsonDocument::fromJson(reply->readAll());
			reply->deleteLater();
			qDebug() << "Get pipeline variables response: " << doc.toJson();

			parseVariables(doc);
		});
}

void GPManager::loadProjectBranches(int projectId)
{
	QNetworkRequest req;
	req.setRawHeader("PRIVATE-TOKEN", m_settings.privateToken.toUtf8());
	req.setUrl(QUrl(QString("%1/api/v4/projects/%2/repository/branches").arg(m_settings.gitlabRoot).arg(projectId)));

	auto reply = m_networkManager->get(req);

	connect(
		reply,
		&QNetworkReply::finished,
		[this, reply, projectId]
		{
			if (reply->error()) return;

			auto doc = QJsonDocument::fromJson(reply->readAll());
			reply->deleteLater();

			parseBranches(projectId, doc);
		});
}

void GPManager::loadProjectMRs(int projectId)
{
	QNetworkRequest req;
	req.setRawHeader("PRIVATE-TOKEN", m_settings.privateToken.toUtf8());
	req.setUrl(QUrl(QString("%1/api/v4/projects/%2/merge_requests?state=opened").arg(m_settings.gitlabRoot).arg(projectId)));

	auto reply = m_networkManager->get(req);

	connect(
		reply,
		&QNetworkReply::finished,
		[this, reply, projectId]
		{
			if (reply->error()) return;

			auto doc = QJsonDocument::fromJson(reply->readAll());
			reply->deleteLater();

			parseMRs(projectId, doc);
		});
}

void GPManager::loadCurrentUser()
{
	QNetworkRequest req;
	req.setRawHeader("PRIVATE-TOKEN", m_settings.privateToken.toUtf8());
	req.setUrl(QUrl(QString("%1/api/v4/user").arg(m_settings.gitlabRoot)));

	auto reply = m_networkManager->get(req);

	connect(
		reply,
		&QNetworkReply::finished,
		[this, reply]
		{
			auto doc = QJsonDocument::fromJson(reply->readAll());
			reply->deleteLater();

			parseCurrentUser(doc);
		});
}

void GPManager::cancelPipeline(int pipelineId)
{
	if (m_currentProject <= 0 || pipelineId <= 0) return;

	QNetworkRequest req;
	req.setRawHeader("PRIVATE-TOKEN", m_settings.privateToken.toUtf8());
	req.setUrl(QUrl(
		QString("%1/api/v4/projects/%2/pipelines/%3/cancel").arg(m_settings.gitlabRoot).arg(m_currentProject).arg(pipelineId)));

	auto reply = m_networkManager->post(req, QByteArray{});

	connect(
		reply,
		&QNetworkReply::finished,
		[this, reply]
		{
			auto doc = QJsonDocument::fromJson(reply->readAll());
			qDebug() << doc.toJson();

			reply->deleteLater();
		});
}

void GPManager::retryPipeline(int pipelineId)
{
	if (m_currentProject <= 0 || pipelineId <= 0) return;

	QNetworkRequest req;
	req.setRawHeader("PRIVATE-TOKEN", m_settings.privateToken.toUtf8());
	req.setUrl(QUrl(
		QString("%1/api/v4/projects/%2/pipelines/%3/retry").arg(m_settings.gitlabRoot).arg(m_currentProject).arg(pipelineId)));

	auto reply = m_networkManager->post(req, QByteArray{});

	connect(
		reply,
		&QNetworkReply::finished,
		[this, reply]
		{
			auto doc = QJsonDocument::fromJson(reply->readAll());
			qDebug() << doc.toJson();

			reply->deleteLater();
		});
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

void GPManager::readSettings()
{
	if (!QFile::exists(SettingsFile)) return;

	QFile settings{SettingsFile};
	settings.open(QIODevice::ReadOnly);
	auto doc = QJsonDocument::fromJson(settings.readAll());

	m_settings.gitlabRoot = doc.object().value("gitlab-url").toString();
	m_settings.privateToken = doc.object().value("private-token").toString();
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

QJsonArray GPManager::prepareVariables() const
{
	QJsonArray vars;

	for (auto const &variable : m_variableModel->variables() | std::ranges::views::filter(&gpr::Variable::used))
	{
		QJsonObject var;
		var.insert("key", variable.key);
		var.insert("value", variable.value);
		vars.append(var);
	}

	return vars;
}
