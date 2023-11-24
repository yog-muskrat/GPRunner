#pragma once

#include <algorithm>

#include <QDateTime>
#include <QPixmap>
#include <QObject>

namespace gpr
{
	struct User
	{
		Q_GADGET

		Q_PROPERTY(QString username MEMBER username)
		Q_PROPERTY(QString avatarUrl MEMBER avatarUrl)

	public:
		QString username;
		QString avatarUrl;

		friend auto operator<=>(User const &, User const &) = default;
	};

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
		User author;
		QString body;
		QDateTime created;
		QDateTime updated;

		bool resolvable{};
		bool resolved{};

		mutable bool wasShown{false};

		bool operator==(Note const &other) const { return id == other.id; }
	};

	struct Discussion
	{
		QString id;
		std::vector<Note> notes;

		bool isEmpty() const { return notes.empty(); }
		bool isResolvable() const { return std::ranges::any_of(notes, &Note::resolvable); }
		bool isResolved() const { return std::ranges::all_of(notes, &Note::resolved); }

		User author() const
		{
			// NOTE: Автором дискуссии считаем автора первой заметки
			return isEmpty() ? User{} : notes.front().author;
		}

		bool operator==(Discussion const &other) const { return id == other.id; }
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

Q_DECLARE_METATYPE(gpr::User)
