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
		Q_GADGET
		Q_PROPERTY(QString key MEMBER key)
		Q_PROPERTY(QString value MEMBER value)
		Q_PROPERTY(bool used MEMBER used)

	public:
		QString key;
		QString value;
		bool used{true};

		friend auto operator<=>(Variable const &, Variable const &) = default;
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
