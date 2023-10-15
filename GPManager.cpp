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

namespace
{
	QString const SettingsFile{"./settings.json"};
}

GPManager::GPManager(QObject *parent)
	: QObject(parent)
{
	readSettings();

	m_projectModel = new ProjectModel(this);
	m_pipelineModel = new PipelineModel(this);
	m_variableModel = new VariableModel(this);

	m_networkManager = new QNetworkAccessManager;
	m_networkManager->connectToHost(m_settings.gitlabRoot, 443);

	m_pipelineUpdateTimer.setSingleShot(true);
	connect(&m_pipelineUpdateTimer, &QTimer::timeout, [this] { loadPipelines(m_currentProject); });
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
		});
}

void GPManager::loadPipelines(int projectId)
{
	m_pipelineUpdateTimer.stop();

	if (projectId != m_currentProject)
	{
		m_pipelineModel->clear();
		m_variableModel->clear();
	}

	m_currentProject = projectId;

	QNetworkRequest req;
	req.setRawHeader("PRIVATE-TOKEN", m_settings.privateToken.toUtf8());
	req.setUrl(QUrl(QString("%1/api/v4/projects/%2/pipelines").arg(m_settings.gitlabRoot).arg(projectId)));

	auto reply = m_networkManager->get(req);

	connect(
		reply,
		&QNetworkReply::finished,
		[this, reply]
		{
			parsePipelines(QJsonDocument::fromJson(reply->readAll()));
			reply->deleteLater();
			m_pipelineUpdateTimer.start(std::chrono::milliseconds{2000});
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

	auto paramsStr = QJsonDocument(param).toJson(QJsonDocument::Compact);

	qDebug() << "Run pipeline variables: " << paramsStr;

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
	if (auto prj = m_projectModel->findProject(projectId)) return prj->branches;
	return QStringList{"master"};
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
			auto doc = QJsonDocument::fromJson(reply->readAll());
			qDebug() << "Get project branches response: " << doc.toJson();

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
				prj->branches = result;
				m_projectModel->addProject(std::move(*prj));
			}
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

	for (auto const &prj : doc.array())
	{
		m_projectModel->addProject(
			{prj.toObject().value("id").toInt(),
		     prj.toObject().value("name").toString(),
		     prj.toObject().value("avatar_url").toString()});

		loadProjectBranches(prj.toObject().value("id").toInt());
	}
}

void GPManager::parsePipelines(QJsonDocument const &doc)
{
	for (auto const &pipeline : doc.array())
	{
		auto const obj = pipeline.toObject();
		m_pipelineModel->addPipeline(
			{.id = obj.value("id").toInt(),
		     .status = obj.value("status").toString(),
		     .source = obj.value("source").toString(),
		     .ref = obj.value("ref").toString(),
		     .created = QDateTime::fromString(obj.value("created_at").toString(), Qt::DateFormat::ISODate),
		     .updated = QDateTime::fromString(obj.value("updated_at").toString(), Qt::DateFormat::ISODate)});
	}
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
