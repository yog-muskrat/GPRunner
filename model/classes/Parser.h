#pragma once

#include "model/classes/Project.h"
#include "model/classes/Pipeline.h"
#include "model/classes/MR.h"

class QJsonObject;

namespace gpr::api
{
	Project::Data parseProject(QJsonObject const &json);
	Pipeline::Data parsePipeline(QJsonObject const &json);
	MR::Data parseMR(QJsonObject const &json);
	Discussion parseDiscussion(QJsonObject const &json);
	std::vector<QString> parseApprovals(QJsonObject const &json);
} // namespace gpr::api
