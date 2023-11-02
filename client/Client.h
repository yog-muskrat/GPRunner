#pragma once

#include <functional>

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

		Client();

		void requestCurrentUser(Callback callback);
		void requestProjects(Callback callback);
		void requestProjectPipelines(int projectId, Callback callback);
		void requestProjectBranches(int projectId, Callback callback);
		void requestProjectMRs(int projectId, Callback callback);
		void requestPipelineVariables(int projectId, QString const &ref, Callback callback);

		void runPipeline(int projectId, QString const &ref, std::vector<Variable> const &variables);
		void cancelPipeline(int projectId, int pipelineId);
		void retryPipeline(int projectId, int pipelineId);

	private:
		QNetworkRequest prepareRequest(QString urlSubpath) const;
		QJsonArray prepareVariables(std::vector<Variable> const &variables) const;

		void readSettings();
		void connectReplyCallback(QNetworkReply *reply, Callback callback = {});

		QNetworkAccessManager m_networkManager;

		Settings m_settings;
	};
} // namespace gpr