﻿#pragma once

#include "model/classes/Project.h"
#include "model/classes/Pipeline.h"
#include "model/classes/Job.h"
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
	Job::Data parseJobInfo(QJsonObject const &json);
	MR::Data parseMR(QJsonObject const &json);
	Pipeline::Data parseMRDetails(QJsonObject const &json);
	std::pair<Discussion::Data, std::vector<Note::Data>> parseDiscussion(QJsonObject const &json);
	std::vector<QString> parseApprovals(QJsonObject const &json);
	std::vector<Variable> parseVariables(QJsonDocument const &doc);
	QStringList parseBranches(QJsonDocument const &doc);
	gpr::User parseUser(QJsonObject const &json);
	std::vector<gpr::EmojiReaction> parseNoteEmojis(QJsonDocument const &doc, std::map<QString, gpr::Emoji> const &emojiDictionary);

	std::map<QString, gpr::Emoji> parseEmojis(QJsonDocument const &doc);
} // namespace gpr::api
