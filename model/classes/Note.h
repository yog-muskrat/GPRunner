#pragma once

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
		Q_PROPERTY(QList<EmojiReaction> reactions READ reactions NOTIFY modified)

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
		};

		Note(GPManager &manager, Data data, Discussion &discussion);

		int id() const;

		void update(Data data);

		Discussion &discussion();
		Discussion const &discussion() const;

		User const &author() const;

		QString const &body() const;

		QString url() const;
		
		QDateTime created() const;

		QDateTime updated() const;

		bool isResolvable() const;

		bool isResolved() const;

		QList<EmojiReaction> const &reactions() const;
		void setReactions(QList<EmojiReaction> reactions);

		bool isRead() const;
		void markRead();

	Q_SIGNALS:
		void modified();

	private:
		Data m_data;
		GPManager &m_manager;
		Discussion &m_discussion;
		QList<EmojiReaction> m_reactions;
		bool m_wasRead{false};
	};
} // namespace gpr::api
