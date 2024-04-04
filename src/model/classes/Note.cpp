#include <QtCore/QRegularExpression>
#include <cassert>

#include "GPManager.h"
#include "model/classes/Note.h"
#include "model/classes/Discussion.h"

namespace gpr::api
{
		void replaceEmojis(QString &text, std::map<QString, gpr::Emoji> const &emojiDict)
		{
			QRegularExpression regex{":([a-zA-Z_]+):"};

			qsizetype offset = 0;

			for(auto iter = regex.globalMatch(text, offset); iter.hasNext(); iter = regex.globalMatch(text, offset))
			{
				auto match = iter.next();
				auto const captured = match.captured(1);
				if(auto const pos = emojiDict.find(captured); pos != emojiDict.end())
				{
					text.replace(match.capturedStart(), match.capturedEnd(), pos->second.moji);
					offset = match.capturedStart() + pos->second.moji.length() + 1;
				}
				else
				{
					offset = match.capturedEnd();
				}
			}
		}

	Note::Note(Data data, Discussion &discussion)
		: QObject(&discussion)
		, m_data{std::move(data)}
		, m_discussion{discussion}
	{}

	int Note::id() const
	{
		return m_data.id;
	}

	void Note::update(Data data)
	{
		assert(data.id == m_data.id);

		if(data.updated > updated())
		{
			m_data = std::move(data);
			replaceEmojis(m_data.body, discussion().mr().project().manager().emojiDict());
			Q_EMIT modified();
		}
	}

	Discussion &Note::discussion()
	{
		return m_discussion;
	}

	Discussion const &Note::discussion() const
	{
		return m_discussion;
	}

	void Note::remove()
	{
		discussion().mr().project().manager().client().removeDiscussionNote(
			discussion().mr().project().id(),
			discussion().mr().iid(),
			discussion().id(),
			id());
	}

	User const &Note::author() const
	{
		return m_data.author;
	}

	QString const &Note::body() const
	{
		return m_data.body;
	}

	void Note::setBody(QString body)
	{
		discussion().mr().project().manager().client().editDiscussionNote(
			discussion().mr().project().id(),
			discussion().mr().iid(),
			discussion().id(),
			id(),
			std::move(body));
	}

	QString Note::url() const
	{
		return m_discussion.mr().noteUrl(*this);
	}

	QDateTime Note::created() const
	{
		return m_data.created;
	}

	QDateTime Note::updated() const
	{
		return m_data.updated;
	}

	bool Note::isResolvable() const
	{
		return m_data.resolvable;
	}

	bool Note::isResolved() const
	{
		return m_data.resolved;
	}

	QList<EmojiReaction> const &Note::reactions() const
	{
		return m_reactions;
	}

	void Note::setReactions(QList<EmojiReaction> reactions)
	{
		//TODO: сранивать умнее
		if(!std::ranges::equal(reactions, m_reactions))
		{
			m_reactions.swap(reactions);
			Q_EMIT modified();
		}
	}

	QList<User> const &Note::mentionedUsers() const
	{
		return m_data.mentions;
	}

	void Note::setMentionedUsers(QList<User> users)
	{
		if(!std::ranges::equal(users, m_data.mentions))
		{
			m_data.mentions.swap(users);
			Q_EMIT modified();
		}
	}

	bool Note::isRead() const
	{
		return m_wasRead;
	}

	void Note::markRead()
	{
		if(!std::exchange(m_wasRead, true))
		{
			Q_EMIT modified();
		}
	}
} // namespace gpr::api
