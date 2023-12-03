#pragma once

#include "model/classes/Project.h"
#include "model/classes/Pipeline.h"
#include "model/classes/MR.h"

class QJsonObject;
class QJsonDocument;

namespace gpr::api
{
	Project::Data parseProject(QJsonObject const &json);
	/**
	 * @brief Сокращенная версия описания пайплайна. В частности, не содержит информации о пользователе
	*/
	Pipeline::Data parseProjectPipeline(QJsonObject const &json);
	Pipeline::Data parsePipelineInfo(QJsonObject const &json);
	MR::Data parseMR(QJsonObject const &json);
	QString parseMRDetails(QJsonObject const &json);
	Discussion parseDiscussion(QJsonObject const &json);
	std::vector<QString> parseApprovals(QJsonObject const &json);
	std::vector<Variable> parseVariables(QJsonDocument const &doc);
	QStringList parseBranches(QJsonDocument const &doc);
	gpr::User parseUser(QJsonObject const &json);
} // namespace gpr::api
