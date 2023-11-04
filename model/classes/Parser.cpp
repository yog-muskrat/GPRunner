#include <QJsonObject>
#include <QJsonArray>

#include <ranges>

#include "model/classes/Parser.h"
#include "Parser.h"

namespace gpr::api
{
	namespace
	{
		QDateTime toDateTime(QJsonValue const &json)
		{
			return QDateTime::fromString(json.toString(), Qt::DateFormat::ISODate);
		}
	}

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
			.created = toDateTime(json.value("created_at")),
			.updated = toDateTime(json.value("updated_at"))};
	}

	MR::Data parseMR(QJsonObject const &json)
	{
		return {
			.id = json.value("id").toInt(),
			.iid = json.value("iid").toInt(),
			.created = toDateTime(json.value("created_at")),
			.updated = toDateTime(json.value("updated_at")),
			.title = json.value("title").toString(),
			.status = json.value("state").toString(),
			.author = json.value("author").toObject().value("username").toString(),
			.assignee = json.value("assignee").toObject().value("username").toString(),
			.reviewer = json.value("reviewers").toArray().first().toObject().value("username").toString(),
			.sourceBranch = json.value("source_branch").toString(),
			.targetBranch = json.value("target_branch").toString(),
			.url = json.value("web_url").toString(),
			.mergeStatus = json.value("detailed_merge_status").toString(),
			.hasNotes = json.value("user_notes_count").toInt() > 0};
	}

	Discussion parseDiscussion(QJsonObject const &json)
	{
		Discussion discussion;
		discussion.id = json["id"].toString();

		for (auto const &note : json["notes"].toArray() | std::views::transform(&QJsonValueRef::toObject)
		                            | std::views::filter([](auto const &obj) { return !obj["system"].toBool(); }))
		{
			discussion.notes.push_back(Note{
				.id = note["id"].toInt(),
				.author = note["author"]["username"].toString(),
				.body = note["body"].toString(),
				.created = toDateTime(note["created_at"]),
				.updated = toDateTime(note["updated_at"]),
				.resolvable = note["resolvable"].toBool(),
				.resolved = note["resolved"].toBool()});
		}

		return discussion;
	}

	std::vector<QString> parseApprovals(QJsonObject const &json)
	{
		return json["approved_by"].toArray()
			| std::views::transform(&QJsonValueRef::toObject)
			| std::views::transform([](auto const &obj) { return obj["user"]["username"].toString();})
			| std::ranges::to<std::vector>();
	}
} // namespace gpr::api
