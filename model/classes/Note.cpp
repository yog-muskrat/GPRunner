#include <QtCore/QRegularExpression>
#include <cassert>

#include "model/classes/Note.h"
#include "GPManager.h"

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

	Note::Note(GPManager &manager, Data data, QObject *parent)
		: QObject(parent)
		, m_data{std::move(data)}
		, m_manager{manager}
	{}

	void Note::update(Data data)
	{
		assert(data.id == m_data.id);

		if(data.updated > updated())
		{
			m_data = std::move(data);
			replaceEmojis(m_data.body, m_manager.emojiDict());
			Q_EMIT modified();
		}
	}

	int Note::id() const
	{
		return m_data.id;
	}

	User const &Note::author() const
	{
		return m_data.author;
	}

	QString const &Note::body() const
	{
		return m_data.body;
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

	std::vector<EmojiReaction> const &Note::reactions() const
	{
		return m_reactions;
	}

	void Note::setReactions(std::vector<EmojiReaction> reactions)
	{
		//TODO: сранивать умнее
		if(!std::ranges::equal(reactions, m_reactions))
		{
			m_reactions = reactions;
			Q_EMIT modified();
		}
	}

	bool Note::isRead() const
	{
		return m_wasRead;
	}

	void Note::markRead()
	{
		m_wasRead = true;
	}
} // namespace gpr::api
