#pragma once

#include <functional>
#include <variant>

#include <QJsonDocument>
#include <QJsonArray>
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
		void requestFileDownload(QString const &url, RawCallback callback);

		void runPipeline(int projectId, QString const &ref, std::vector<Variable> const &variables);
		void cancelPipeline(int projectId, int pipelineId);
		void retryPipeline(int projectId, int pipelineId);
		void resolveDiscussion(int projectId, int mrIid, QString const &discussionId);
		void unresolveDiscussion(int projectId, int mrIid, QString const &discussionId);
		void approveMR(int projectId, int mrIid);
		void unapproveMR(int projectId, int mrIid);

	private:
		void makeGetRequest(QNetworkRequest request, Callback callback = {});
		void makePostRequest(QNetworkRequest request, QByteArray data = {}, Callback callback = {});
		void makePutRequest(QNetworkRequest request, QByteArray data = {}, Callback callback = {});

		template<typename ...Ts>
		QNetworkRequest prepareRequest(QString urlSubpath, Ts && ...args) const;
		QJsonArray prepareVariables(std::vector<Variable> const &variables) const;

		void readSettings();

		void connectReplyCallback(QNetworkReply *reply, Callback callback = {});

		QNetworkAccessManager m_networkManager;

		Settings m_settings;
	};
} // namespace gpr