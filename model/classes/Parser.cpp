#include <QJsonDocument>
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
	} // namespace

	Project::Data gpr::api::parseProject(QJsonObject const &json)
	{
		return {
			.id = json.value("id").toInt(),
			.name = json.value("name").toString(),
			.url = json.value("web_url").toString(),
			.avatarUrl = json.value("avatar_url").toString()
		};
	}

	Pipeline::Data parseProjectPipeline(QJsonObject const &json)
	{
		return {
			.id = json.value("id").toInt(),
			.status = json.value("status").toString(),
			.source = json.value("source").toString(),
			.ref = json.value("ref").toString(),
			.url = json.value("web_url").toString(),
			.created = toDateTime(json.value("created_at")),
			.updated = toDateTime(json.value("updated_at"))};
	}

	Pipeline::Data parsePipelineInfo(QJsonObject const &json)
	{
		auto data = parseProjectPipeline(json);
		data.user = parseUser(json["user"].toObject());
		return data;
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
			.author = parseUser(json["author"].toObject()),
			.assignee = parseUser(json["assignee"].toObject()),
			.reviewer = parseUser(json.value("reviewers").toArray().first().toObject()),
			.sourceBranch = json.value("source_branch").toString(),
			.targetBranch = json.value("target_branch").toString(),
			.url = json.value("web_url").toString(),
			.mergeStatus = json.value("detailed_merge_status").toString(),
			.hasNotes = json.value("user_notes_count").toInt() > 0};
	}

	QString parseMRDetails(QJsonObject const &json)
	{
		return json.value("pipeline")["status"].toString();
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
				.author = parseUser(note["author"].toObject()),
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
		return json["approved_by"].toArray() | std::views::transform(&QJsonValueRef::toObject)
		     | std::views::transform([](auto const &obj) { return obj["user"]["username"].toString(); })
		     | std::ranges::to<std::vector>();
	}

	std::vector<Variable> parseVariables(QJsonDocument const &doc)
	{
		if (!doc.isArray())
		{
			qDebug() << "Invalid variables configuration format";
			return {};
		}

		std::vector<gpr::Variable> vars;
		for (auto const var : doc.array() | std::views::transform(&QJsonValueRef::toObject))
		{
			vars.push_back(
				{.key = var.value("key").toString(),
			     .value = var.value("value").toString(),
			     .used = var.value("required").toBool()});
		}

		return vars;
	}

	QStringList parseBranches(QJsonDocument const &doc)
	{
		QStringList result;
		for (auto const &branch : doc.array() | std::views::transform(&QJsonValueRef::toObject))
		{
			auto name = branch.value("name").toString();

			if (branch.value("default").toBool())
			{
				result.prepend(std::move(name));
			}
			else
			{
				result << std::move(name);
			}
		}
		return result;
	}
	gpr::User parseUser(QJsonObject const &json)
	{
		return {.id = json["id"].toInt(), .username = json["username"].toString(), .avatarUrl = json["avatar_url"].toString()};
	}
} // namespace gpr::api
