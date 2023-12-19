#pragma once

#include <QObject>
#include "model/classes/GPClasses.h"

class GPManager;

namespace gpr::api
{
	class Note : public QObject
	{
		Q_OBJECT

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

		Note(GPManager &manager, Data data, QObject *parent = nullptr);

		void update(Data data);

		int id() const;

		User const &author() const;

		QString const &body() const;

		QDateTime created() const;

		QDateTime updated() const;

		bool isResolvable() const;

		bool isResolved() const;

		std::vector<EmojiReaction> const &reactions() const;
		void setReactions(std::vector<EmojiReaction> reactions);

		bool isRead() const;
		void markRead();

	Q_SIGNALS:
		void modified();

	private:
		Data m_data;
		GPManager &m_manager;
		std::vector<EmojiReaction> m_reactions;
		bool m_wasRead{false};
	};
} // namespace gpr::api
