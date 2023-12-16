#pragma once

#include <functional>
#include <variant>

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkAccessManager>

#include "GPClasses.h"

class QNetworkRequest;

namespace gpr
{
	class Client
	{
	public:
		using Callback = std::function<void(QJsonDocument)>;
		using RawCallback = std::function<void(QByteArray)>;

		Client();

		void requestCurrentUser(Callback callback);
		void requestActiveUsers(Callback callback);

		void requestProjects(Callback callback);
		void requestProjectPipelines(int projectId, Callback callback);
		void requestProjectPipelineTestReport(int projectId, int pipelineId, Callback callback);
		void requestProjectBranches(int projectId, Callback callback);
		void requestProjectMRs(int projectId, Callback callback);

		void requestPipelineInfo(int projectId, int pipelineId, Callback callback);
		void requestPipelineVariables(int projectId, QString const &ref, Callback callback);

		void requestMRDetails(int projectId, int mrIid, Callback callback);
		void requestMRDiscussions(int projectId, int mrIid, Callback callback);
		void requestMRApprovals(int projectId, int mrIid, Callback callback);
		void requestMRNoteEmojis(int projectId, int mrIid, int noteId, Callback callback);
		void setMRReviewer(int projectId, int mrIid, int userId);
		void setMRAssignee(int projectId, int mrIid, int userId);

		void requestFileDownload(QString const &url, RawCallback callback);

		void runPipeline(int projectId, QString const &ref, std::vector<Variable> const &variables);
		void cancelPipeline(int projectId, int pipelineId);
		void retryPipeline(int projectId, int pipelineId);

		void resolveDiscussion(int projectId, int mrIid, QString const &discussionId);
		void unresolveDiscussion(int projectId, int mrIid, QString const &discussionId);
		void addDiscussion(int projectId, int mrIid, QString text);
		void addDiscussionNote(int projectId, int mrIid, QString const &discussionId, QString text);
		void editDiscussionNote(int projectId, int mrIid, QString const &discussionId, int noteId, QString text);
		void removeDiscussionNote(int projectId, int mrIid, QString const &discussionId, int noteId);

		void approveMR(int projectId, int mrIid);
		void unapproveMR(int projectId, int mrIid);

	private:
		using PaginationCallback = std::function<void(QJsonDocument )>;

		void makeGetRequest(QNetworkRequest request, Callback callback, bool noPagination = false);
		void makePaginatedGetRequest(QUrl nextUrl, Callback callback, QJsonArray data);
		void makePostRequest(QNetworkRequest request, QJsonObject const &data = {}, Callback callback = {});
		void makePutRequest(QNetworkRequest request, QJsonObject const &data = {}, Callback callback = {});
		void makeDeleteRequest(QNetworkRequest request, Callback callback = {});

		QNetworkRequest prepareRequest() const;

		template<typename ...Ts>
		QNetworkRequest prepareRequest(QString urlSubpath, Ts && ...args) const;
		QJsonArray prepareVariables(std::vector<Variable> const &variables) const;

		static std::optional<QUrl> getNextPageLink(QByteArray const &linkHeader);

		void readSettings();

		void connectReplyCallback(QNetworkReply *reply, Callback callback = {}, bool disablePagination = false);
		void connectPaginatedReplyCallback(QNetworkReply *reply, Callback callback, QJsonArray data = {});

		QNetworkAccessManager m_networkManager;

		Settings m_settings;
	};
} // namespace gpr