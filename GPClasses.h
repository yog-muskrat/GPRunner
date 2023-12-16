#pragma once

#include <algorithm>
#include <ranges>

#include <QDateTime>
#include <QPixmap>
#include <QObject>

namespace gpr
{
	struct User
	{
		Q_GADGET

		Q_PROPERTY(int id MEMBER id)
		Q_PROPERTY(QString username MEMBER username)
		Q_PROPERTY(QString avatarUrl MEMBER avatarUrl)

	public:
		int id{};
		QString username;
		QString avatarUrl;

		friend auto operator<=>(User const &, User const &) = default;
		friend bool operator==(User const &l, User const &r) { return l.id == r.id; }
	};

	struct Emoji
	{
		Q_GADGET
		Q_PROPERTY(QString id MEMBER id)
		Q_PROPERTY(QString name MEMBER name)
		Q_PROPERTY(QString shortname MEMBER shortname)
		Q_PROPERTY(QString category MEMBER category)
		Q_PROPERTY(QStringList aliases MEMBER aliases)
		Q_PROPERTY(QString moji MEMBER moji)

	public:
		QString id;
		QString name;
		QString shortname;
		QString category;
		QStringList aliases;
		QString moji;

		friend auto operator<=>(Emoji const &, Emoji const &) = default;
		friend bool operator==(Emoji const &l, Emoji const &r) { return l.id == r.id; }
	};

	struct EmojiReaction
	{
		Q_GADGET
		Q_PROPERTY(int id MEMBER id)
		Q_PROPERTY(int awardableId MEMBER awardableId)
		Q_PROPERTY(QString awardableType MEMBER awardableType)
		Q_PROPERTY(Emoji reaction MEMBER reaction)
		Q_PROPERTY(User user MEMBER user)
		Q_PROPERTY(QDateTime created MEMBER created)

	public:
		int id{};
		int awardableId{};
		QString awardableType;
		Emoji reaction;
		User user;
		QDateTime created;

		friend auto operator<=>(EmojiReaction const &, EmojiReaction const &) = default;
		friend bool operator==(EmojiReaction const &l, EmojiReaction const &r) { return l.id== r.id; }
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

		std::vector<EmojiReaction> reactions;

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

		void markRead() { std::ranges::fill(notes | std::views::transform(&Note::wasShown), true); }

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
Q_DECLARE_METATYPE(gpr::Emoji)
