#include "client/Client.h"

#include <ranges>

#include <QFile>
#include <QObject>
#include <QUrlQuery>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>

namespace gpr
{
	namespace
	{
		QString const SettingsFile{"./settings.json"};
		QString const VariablesPath{"%2Egitlab%2Fci%2Fpipeline_variables%2Ejson"};
	} // namespace

	Client::Client()
	{
		readSettings();
		m_networkManager.connectToHost(m_settings.gitlabRoot, 443);
	}

	void Client::requestCurrentUser(Callback callback)
	{
		auto req = prepareRequest("user");
		auto reply = m_networkManager.get(req);
		connectReplyCallback(reply, std::move(callback));
	}

	void Client::requestProjects(Callback callback)
	{
		auto req = prepareRequest("projects");
		auto reply = m_networkManager.get(req);

		connectReplyCallback(reply, std::move(callback));
	}

	void Client::requestProjectPipelines(int projectId, Callback callback)
	{
		auto req = prepareRequest(QString("projects/%1/pipelines").arg(projectId));
		auto reply = m_networkManager.get(req);
		connectReplyCallback(reply, std::move(callback));
	}

	void Client::requestProjectBranches(int projectId, Callback callback)
	{
		auto req = prepareRequest(QString("projects/%1/repository/branches").arg(projectId));
		auto reply = m_networkManager.get(req);
		connectReplyCallback(reply, std::move(callback));
	}

	void Client::requestProjectMRs(int projectId, Callback callback)
	{
		auto req = prepareRequest(QString("projects/%1/merge_requests?state=opened").arg(projectId));
		auto reply = m_networkManager.get(req);
		connectReplyCallback(reply, std::move(callback));
	}

	void Client::requestPipelineVariables(int projectId, QString const &ref, Callback callback)
	{
		auto str =  QString("projects/%1/repository/files/%3?ref=%2").arg(projectId).arg(ref).arg(VariablesPath);

		auto req = prepareRequest(str);
		auto reply = m_networkManager.get(req);
		connectReplyCallback(reply, std::move(callback));
	}

	void Client::runPipeline(int projectId, QString const &ref, std::vector<Variable> const &variables)
	{
		auto req = prepareRequest(QString("projects/%1/pipeline").arg(projectId));
		req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

		QJsonObject param;
		param.insert("ref", ref);
		param.insert("variables", prepareVariables(variables));

		auto reply = m_networkManager.post(req, QJsonDocument(param).toJson(QJsonDocument::Compact));

		connectReplyCallback(reply);
	}

	void Client::cancelPipeline(int projectId, int pipelineId)
	{
		auto req = prepareRequest(QString("projects/%1/pipelines/%3/cancel").arg(projectId).arg(pipelineId));
		auto reply = m_networkManager.post(req, QByteArray{});
		connectReplyCallback(reply);
	}

	void Client::retryPipeline(int projectId, int pipelineId)
	{
		auto req = prepareRequest(QString("projects/%1/pipelines/%2/retry").arg(projectId).arg(pipelineId));
		auto reply = m_networkManager.post(req, QByteArray{});
		connectReplyCallback(reply);
	}

	QNetworkRequest Client::prepareRequest(QString urlSubpath) const
	{
		QNetworkRequest request;
		request.setRawHeader("PRIVATE-TOKEN", m_settings.privateToken.toUtf8());
		request.setUrl(QUrl(QString("%1/api/v4/%2").arg(m_settings.gitlabRoot).arg(std::move(urlSubpath))));
		return request;
	}

	QJsonArray Client::prepareVariables(std::vector<Variable> const &variables) const
	{
		QJsonArray vars;

		for (auto const &variable : variables | std::ranges::views::filter(&gpr::Variable::used))
		{
			QJsonObject var;
			var.insert("key", variable.key);
			var.insert("value", variable.value);
			vars.append(var);
		}

		return vars;
	}

	void Client::readSettings()
	{
		if (!QFile::exists(SettingsFile)) return;

		QFile settings{SettingsFile};
		settings.open(QIODevice::ReadOnly);
		auto doc = QJsonDocument::fromJson(settings.readAll());

		m_settings.gitlabRoot = doc.object().value("gitlab-url").toString();
		m_settings.privateToken = doc.object().value("private-token").toString();
	}

	void Client::connectReplyCallback(QNetworkReply *reply, Callback callback)
	{
		assert(reply);
		QObject::connect(
			reply,
			&QNetworkReply::finished,
			[reply, callback = std::move(callback)]
			{
				reply->deleteLater();
				if (reply->error())
				{
					qDebug() << "API request error:" << reply->errorString();
					return;
				}

				if (callback)
				{
					std::invoke(callback, QJsonDocument::fromJson(reply->readAll()));
				}
			});
	}
} // namespace gpr
