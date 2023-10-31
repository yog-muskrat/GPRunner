#include <QJsonObject>
#include <QJsonArray>

#include "model/classes/Parser.h"
#include "Parser.h"

namespace gpr::api
{
	Project::Data gpr::api::parseProject(QJsonObject const &json)
	{
		return {.id = json.value("id").toInt(), .name = json.value("name").toString()};
	}

	Pipeline::Data parsePipeline(QJsonObject const &json)
	{
		return {
			.id = json.value("id").toInt(),
			.status = json.value("status").toString(),
			.source = json.value("source").toString(),
			.ref = json.value("ref").toString(),
			.created = QDateTime::fromString(json.value("created_at").toString(), Qt::DateFormat::ISODate),
			.updated = QDateTime::fromString(json.value("updated_at").toString(), Qt::DateFormat::ISODate)};
	}

	MR::Data parseMR(QJsonObject const &json)
	{
		return {
			.id = json.value("id").toInt(),
			.created = QDateTime::fromString(json.value("created_at").toString(), Qt::DateFormat::ISODate),
			.updated = QDateTime::fromString(json.value("updated_at").toString(), Qt::DateFormat::ISODate),
			.title = json.value("title").toString(),
			.status = json.value("state").toString(),
			.author = json.value("author").toObject().value("username").toString(),
			.assignee = json.value("assignee").toObject().value("username").toString(),
			.reviewer = json.value("reviewers").toArray().first().toObject().value("username").toString(),
			.sourceBranch = json.value("source_branch").toString(),
			.targetBranch = json.value("target_branch").toString()};
	}
} // namespace gpr::api