#pragma once

#include <algorithm>

#include <QDateTime>
#include <QPixmap>
#include <QObject>

namespace gpr
{
	struct Settings
	{
		QString gitlabRoot{};
		QString privateToken{};
	};

	struct Variable
	{
		QString key;
		QString value;
		bool used{true};

		friend auto operator<=>(Variable const &, Variable const &) = default;
	};

	struct Note
	{
		int id {};
		QString author;
		QString authorAvatar;
		QString body;
		QDateTime created;
		QDateTime updated;

		bool resolvable{};
		bool resolved{};
	};

	struct Discussion
	{
		QString id;
		std::vector<Note> notes;

		bool isEmpty() const { return notes.empty(); }
		bool isResolvable() const { return std::ranges::any_of(notes, &Note::resolvable); }
		bool isResolved() const { return std::ranges::all_of(notes, &Note::resolved); }
	};

	struct PipelineTestReport
	{
		QDateTime updated;
		QString suite;
		QString testCase;
		QString className;
		QString status;
	};
} // namespace gpr