﻿#pragma once

#include <QObject>
#include "model/classes/GPClasses.h"

class GPManager;

namespace gpr::api
{
	class Discussion;

	class Note : public QObject
	{
		Q_OBJECT
		Q_PROPERTY(int id READ id)
		Q_PROPERTY(User author READ author NOTIFY modified)
		Q_PROPERTY(QDateTime created READ created NOTIFY modified)
		Q_PROPERTY(QString url READ url NOTIFY modified)
		Q_PROPERTY(QString body READ body WRITE setBody NOTIFY modified)
		Q_PROPERTY(QList<EmojiReaction> reactions READ reactions NOTIFY modified)
		Q_PROPERTY(QList<User> mentionedUsers READ mentionedUsers NOTIFY modified)

	public:
		struct Data
		{
			int id{};
			User author;
			QString body;
			QDateTime created;
			QDateTime updated;

			bool resolvable{};
			bool resolved{};

			QList<User> mentions;
		};

		Note(Data data, Discussion &discussion);

		int id() const;

		void update(Data data);

		Discussion &discussion();
		Discussion const &discussion() const;

		Q_INVOKABLE void remove();

		User const &author() const;

		QString const &body() const;
		void setBody(QString body);

		QString url() const;
		
		QDateTime created() const;

		QDateTime updated() const;

		bool isResolvable() const;

		bool isResolved() const;

		QList<EmojiReaction> const &reactions() const;
		void setReactions(QList<EmojiReaction> reactions);

		QList<User> const &mentionedUsers() const;
		void setMentionedUsers(QList<User> users);

		bool isRead() const;
		void markRead();

	Q_SIGNALS:
		void modified();

	private:
		Data m_data;
		Discussion &m_discussion;
		QList<EmojiReaction> m_reactions;
		bool m_wasRead{false};
	};
} // namespace gpr::api
