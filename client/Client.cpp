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
			QString const User{"/user"};
			QString const Users{"/users?active=true"};

			QString const Projects{"/projects"};
			QString const ProjectPipelines{"/projects/%1/pipelines"};
			QString const ProjectPipelineInfo{"/projects/%1/pipelines/%2"};
			QString const ProjectBranches{"/projects/%1/repository/branches"};
			QString const ProjectFile{"/projects/%1/repository/files/%3?ref=%2"};

			QString const ProjectOpenMRs{"/projects/%1/merge_requests?state=opened"};
			QString const ProjectMRDetails{"/projects/%1/merge_requests/%2/"};
			QString const ProjectMRDiscussions{"/projects/%1/merge_requests/%2/discussions"};
			QString const ProjectMRDiscussionAdd{"/projects/%1/merge_requests/%2/discussions?body=%3"};
			QString const ProjectMRDiscussionNoteAdd{"/projects/%1/merge_requests/%2/discussions/%3/notes"};
			QString const ProjectMRDiscussionNoteEmojis{"/projects/%1/merge_requests/%2/notes/%3/award_emoji"};
			QString const ProjectMRDiscussionNoteEdit{"/projects/%1/merge_requests/%2/discussions/%3/notes/%4"};
			QString const ProjectMRDiscussionNoteRemove{"/projects/%1/merge_requests/%2/discussions/%3/notes/%4"};
			QString const ProjectMRDiscussionResolved{"/projects/%1/merge_requests/%2/discussions/%3?resolved=%4"};
			QString const ProjectMRApprovals{"/projects/%1/merge_requests/%2/approvals"};

			QString const ProjectMRApprove{"/projects/%1/merge_requests/%2/approve"};
			QString const ProjectMRUnapprove{"/projects/%1/merge_requests/%2/unapprove"};

			QString const ProjectPipelineRun{"/projects/%1/pipeline"};
			QString const ProjectPipelineCancel{"/projects/%1/pipelines/%2/cancel"};
			QString const ProjectPipelineRetry{"/projects/%1/pipelines/%2/retry"};
			QString const ProjectPipelineTestReport{"/projects/%1/pipelines/%2/test_report"};
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
	}

	void Client::requestCurrentUser(Callback callback)
	{
		makeGetRequest(prepareRequest(endpoint::User), std::move(callback));
	}

	void Client::requestActiveUsers(Callback callback)
	{
		makeGetRequest(prepareRequest(endpoint::Users), std::move(callback));
	}

	void Client::requestProjects(Callback callback)
	{
		makeGetRequest(prepareRequest(endpoint::Projects), std::move(callback));
	}

	void Client::requestProjectPipelines(int projectId, Callback callback)
	{
		makeGetRequest(prepareRequest(endpoint::ProjectPipelines, projectId), std::move(callback), true);
	}

	void Client::requestProjectPipelineTestReport(int projectId, int pipelineId, Callback callback)
	{
		makeGetRequest(prepareRequest(endpoint::ProjectPipelineTestReport, projectId, pipelineId), std::move(callback));
	}

	void Client::requestProjectBranches(int projectId, Callback callback)
	{
		makeGetRequest(prepareRequest(endpoint::ProjectBranches, projectId), std::move(callback));
	}

	void Client::requestProjectMRs(int projectId, Callback callback)
	{
		makeGetRequest(prepareRequest(endpoint::ProjectOpenMRs, projectId), std::move(callback));
	}

	void Client::requestPipelineInfo(int projectId, int pipelineId, Callback callback)
	{
		makeGetRequest(prepareRequest(endpoint::ProjectPipelineInfo, projectId, pipelineId), std::move(callback));
	}

	void Client::requestPipelineVariables(int projectId, QString const &ref, Callback callback)
	{
		makeGetRequest(prepareRequest(endpoint::ProjectFile, projectId, ref, VariablesPath), std::move(callback));
	}

	void Client::requestMRDetails(int projectId, int mrIid, Callback callback)
	{
		makeGetRequest(prepareRequest(endpoint::ProjectMRDetails, projectId, mrIid), std::move(callback));
	}

	void Client::requestMRDiscussions(int projectId, int mrIid, Callback callback)
	{
		makeGetRequest(prepareRequest(endpoint::ProjectMRDiscussions, projectId, mrIid), std::move(callback));
	}

	void Client::requestMRApprovals(int projectId, int mrIid, Callback callback)
	{
		makeGetRequest(prepareRequest(endpoint::ProjectMRApprovals, projectId, mrIid), std::move(callback));
	}

	void Client::requestMRNoteEmojis(int projectId, int mrIid, int noteId, Callback callback)
	{
		makeGetRequest(prepareRequest(endpoint::ProjectMRDiscussionNoteEmojis, projectId, mrIid, noteId), std::move(callback));
	}

	void Client::setMRReviewer(int projectId, int mrIid, int userId)
	{
		makePutRequest(prepareRequest(endpoint::ProjectMRDetails, projectId, mrIid), {{"reviewer_ids", QJsonArray{{userId}}}});
	}

	void Client::setMRAssignee(int projectId, int mrIid, int userId)
	{
		makePutRequest(prepareRequest(endpoint::ProjectMRDetails, projectId, mrIid), {{"assignee_ids", QJsonArray{{userId}}}});
	}

	void Client::requestFileDownload(QString const &url, RawCallback callback)
	{
		QNetworkRequest request;
		request.setUrl(url);

		qDebug() << "Request to download " << url;

		auto reply = m_networkManager.get(std::move(request));

		QObject::connect(
			reply,
			&QNetworkReply::finished,
			[reply, callback = std::move(callback)]
			{
				reply->deleteLater();
				if (reply->error())
				{
					qDebug() << "API request error:" << reply->error() << "\n" << reply->errorString();
					return;
				}

				if (callback)
				{
					std::invoke(callback, reply->readAll());
				}
			});
	}

	void Client::runPipeline(int projectId, QString const &ref, std::vector<Variable> const &variables)
	{
		makePostRequest(
			prepareRequest(endpoint::ProjectPipelineRun, projectId),
			{{"ref", ref}, {"variables", prepareVariables(variables)}});
	}

	void Client::cancelPipeline(int projectId, int pipelineId)
	{
		makePostRequest(prepareRequest(endpoint::ProjectPipelineCancel, projectId, pipelineId));
	}

	void Client::retryPipeline(int projectId, int pipelineId)
	{
		makePostRequest(prepareRequest(endpoint::ProjectPipelineRetry, projectId, pipelineId));
	}

	void Client::resolveDiscussion(int projectId, int mrIid, QString const &discussionId)
	{
		makePutRequest(prepareRequest(endpoint::ProjectMRDiscussionResolved, projectId, mrIid, discussionId, "true"));
	}

	void Client::unresolveDiscussion(int projectId, int mrIid, QString const &discussionId)
	{
		makePutRequest(prepareRequest(endpoint::ProjectMRDiscussionResolved, projectId, mrIid, discussionId, "false"));
	}

	void Client::addDiscussion(int projectId, int mrIid, QString text)
	{
		makePostRequest(prepareRequest(endpoint::ProjectMRDiscussionAdd, projectId, mrIid), {{"body", std::move(text)}});
	}

	void Client::addDiscussionNote(int projectId, int mrIid, QString const &discussionId, QString text)
	{
		makePostRequest(
			prepareRequest(endpoint::ProjectMRDiscussionNoteAdd, projectId, mrIid, discussionId),
			{{"body", std::move(text)}});
	}

	void Client::editDiscussionNote(int projectId, int mrIid, QString const &discussionId, int noteId, QString text)
	{
		makePutRequest(
			prepareRequest(endpoint::ProjectMRDiscussionNoteEdit, projectId, mrIid, discussionId, noteId),
			{{"body", std::move(text)}});
	}

	void Client::removeDiscussionNote(int projectId, int mrIid, QString const &discussionId, int noteId)
	{
		makeDeleteRequest(prepareRequest(endpoint::ProjectMRDiscussionNoteRemove, projectId, mrIid, discussionId, noteId));
	}

	void Client::approveMR(int projectId, int mrIid)
	{
		makePostRequest(prepareRequest(endpoint::ProjectMRApprove, projectId, mrIid));
	}

	void Client::unapproveMR(int projectId, int mrIid)
	{
		makePostRequest(prepareRequest(endpoint::ProjectMRUnapprove, projectId, mrIid));
	}

	void Client::makeGetRequest(QNetworkRequest request, Callback callback, bool noPagination)
	{
		auto reply = m_networkManager.get(std::move(request));
		connectReplyCallback(reply, std::move(callback), noPagination);
	}

	void Client::makePaginatedGetRequest(QUrl nextUrl, Callback callback, QJsonArray currentData)
	{
		auto request = prepareRequest();
		request.setUrl(nextUrl);
		auto reply = m_networkManager.get(std::move(request));
		connectPaginatedReplyCallback(reply, std::move(callback), std::move(currentData));
	}

	void Client::makePostRequest(QNetworkRequest request, QJsonObject const &data, Callback callback)
	{
		QByteArray ba;
		if(!data.isEmpty())
		{
			request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
			ba = QJsonDocument{data}.toJson(QJsonDocument::Compact);
		}

		auto reply = m_networkManager.post(std::move(request), std::move(ba));
		connectReplyCallback(reply, std::move(callback));
	}

	void Client::makePutRequest(QNetworkRequest request, QJsonObject const &data, Callback callback)
	{
		QByteArray ba;
		if(!data.isEmpty())
		{
			request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
			ba = QJsonDocument{data}.toJson(QJsonDocument::Compact);
		}

		auto reply = m_networkManager.put(std::move(request), std::move(ba));
		connectReplyCallback(reply, std::move(callback));
	}

	void Client::makeDeleteRequest(QNetworkRequest request, Callback callback)
	{
		auto reply = m_networkManager.deleteResource(std::move(request));
		connectReplyCallback(reply, std::move(callback));
	}

	QNetworkRequest Client::prepareRequest() const
	{
		QNetworkRequest request;
		request.setRawHeader("PRIVATE-TOKEN", m_settings.privateToken.toUtf8());

		return request;
	}

	template<typename... Ts>
	QNetworkRequest Client::prepareRequest(QString urlSubpath, Ts &&...args) const
	{
		if constexpr (sizeof...(args) > 0)
		{
			urlSubpath = std::move(urlSubpath).arg(toArg(std::forward<Ts>(args))...);
		}

		auto request = prepareRequest();
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

	void Client::connectReplyCallback(QNetworkReply *reply, Callback callback, bool noPagination)
	{
		assert(reply);
		QObject::connect(
			reply,
			&QNetworkReply::finished,
			[this, reply, callback = std::move(callback), noPagination]
			{
				reply->deleteLater();
				if (reply->error())
				{
					qDebug() << "API request error:" << reply->errorString();
					qDebug() << "API request error:" << reply->error();
					return;
				}

				if (callback)
				{
					auto data = QJsonDocument::fromJson(reply->readAll());

					if (auto next = getNextPageLink(reply->rawHeader("link")); next && !noPagination)
					{
						makePaginatedGetRequest(*next, std::move(callback), std::move(data).array());
					}
					else
					{
						std::invoke(callback, std::move(data));
					}
				}
			});
	}

	void Client::connectPaginatedReplyCallback(QNetworkReply * reply, Callback callback, QJsonArray data)
	{
		assert(reply);
		QObject::connect(
			reply,
			&QNetworkReply::finished,
			[this, reply, data = std::move(data), callback = std::move(callback)] () mutable
			{
				reply->deleteLater();
				if (reply->error())
				{
					qDebug() << "API request error:" << reply->errorString();
					qDebug() << "API request error:" << reply->error();
					return;
				}

				for(auto newData : QJsonDocument::fromJson(reply->readAll()).array()) data.push_back(std::move(newData));

				if (auto next = getNextPageLink(reply->rawHeader("link")))
				{
					makePaginatedGetRequest(*next, std::move(callback), std::move(data));
				}
				else
				{
					std::invoke(callback, QJsonDocument{std::move(data)});
				}
			});
	}

	std::optional<QUrl> Client::getNextPageLink(QByteArray const & linkHeader)
	{
		for(auto const &linkInfo : QString(linkHeader).split(",", Qt::SplitBehaviorFlags::SkipEmptyParts))
		{
			auto const linkParts = linkInfo.split(";", Qt::SplitBehaviorFlags::SkipEmptyParts);
			assert(linkParts.size() == 2);

			if(linkParts.at(1).contains("next"))
			{
				auto link = linkParts.first().trimmed();
				link.removeFirst();
				link.removeLast();

				return link;
			}
		}
		return std::optional<QUrl>();
	}
} // namespace gpr
