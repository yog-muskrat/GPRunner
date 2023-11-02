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

		namespace endpoint
		{
			QString const User                 {"/user"};
			QString const Projects             {"/projects"};
			QString const ProjectPipelines     {"/projects/%1/pipelines"};
			QString const ProjectBranches      {"/projects/%1/repository/branches"};
			QString const ProjectOpenMRs       {"/projects/%1/merge_requests?state=opened"};
			QString const ProjectFile          {"/projects/%1/repository/files/%3?ref=%2"};
			QString const ProjectMRDiscussions {"/projects/%1/merge_requests/%2/discussions"};
			QString const ProjectPipelineRun   {"/projects/%1/pipeline"};
			QString const ProjectPipelineCancel{"/projects/%1/pipelines/%2/cancel"};
			QString const ProjectPipelineRetry {"/projects/%1/pipelines/%2/retry"};
		} // namespace endpoint

		template<typename T>
		QString toArg(T &&value)
		{
			if constexpr (std::integral<std::remove_cvref_t<T>>)
			{
				return QString::number(value);
			}
			else
			{
				return QString(std::forward<T>(value));
			}
		}
	} // namespace

	Client::Client()
	{
		readSettings();
		m_networkManager.connectToHost(m_settings.gitlabRoot, 443);
	}

	void Client::requestCurrentUser(Callback callback)
	{
		makeGetRequest(prepareRequest(endpoint::User), std::move(callback));
	}

	void Client::requestProjects(Callback callback)
	{
		makeGetRequest(prepareRequest(endpoint::Projects), std::move(callback));
	}

	void Client::requestProjectPipelines(int projectId, Callback callback)
	{
		makeGetRequest(prepareRequest(endpoint::ProjectPipelines, projectId), std::move(callback));
	}

	void Client::requestProjectBranches(int projectId, Callback callback)
	{
		makeGetRequest(prepareRequest(endpoint::ProjectBranches, projectId), std::move(callback));
	}

	void Client::requestProjectMRs(int projectId, Callback callback)
	{
		makeGetRequest(prepareRequest(endpoint::ProjectOpenMRs, projectId), std::move(callback));
	}

	void Client::requestPipelineVariables(int projectId, QString const &ref, Callback callback)
	{
		makeGetRequest(prepareRequest(endpoint::ProjectFile, projectId, ref, VariablesPath), std::move(callback));
	}

	void Client::requestMRDiscussions(int projectId, int mrIid, Callback callback)
	{
		// TODO: pagination
		makeGetRequest(prepareRequest(endpoint::ProjectMRDiscussions, projectId, mrIid), std::move(callback));
	}

	void Client::runPipeline(int projectId, QString const &ref, std::vector<Variable> const &variables)
	{
		auto req = prepareRequest(endpoint::ProjectPipelineRun, projectId);
		req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

		QJsonObject param;
		param.insert("ref", ref);
		param.insert("variables", prepareVariables(variables));

		auto reply = m_networkManager.post(req, QJsonDocument(param).toJson(QJsonDocument::Compact));

		connectReplyCallback(reply);
	}

	void Client::cancelPipeline(int projectId, int pipelineId)
	{
		auto req = prepareRequest(endpoint::ProjectPipelineCancel, projectId, pipelineId);
		auto reply = m_networkManager.post(req, QByteArray{});
		connectReplyCallback(reply);
	}

	void Client::retryPipeline(int projectId, int pipelineId)
	{
		auto req = prepareRequest(endpoint::ProjectPipelineRetry, projectId, pipelineId);
		auto reply = m_networkManager.post(req, QByteArray{});
		connectReplyCallback(reply);
	}

	void Client::makeGetRequest(QNetworkRequest request, Callback callback)
	{
		auto reply = m_networkManager.get(std::move(request));
		connectReplyCallback(reply, std::move(callback));
	}

	void Client::makePostRequest(QNetworkRequest request, QByteArray data, Callback callback)
	{
		auto reply = m_networkManager.post(std::move(request),std::move(data));
		connectReplyCallback(reply, std::move(callback));
	}

	template<typename... Ts>
	QNetworkRequest Client::prepareRequest(QString urlSubpath, Ts &&...args) const
	{
		if constexpr (sizeof...(args) > 0)
		{
			urlSubpath = std::move(urlSubpath).arg(toArg(std::forward<Ts>(args))...);
		}

		QNetworkRequest request;
		request.setRawHeader("PRIVATE-TOKEN", m_settings.privateToken.toUtf8());
		request.setUrl(QUrl(QString("%1/api/v4%2").arg(m_settings.gitlabRoot).arg(std::move(urlSubpath))));
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
