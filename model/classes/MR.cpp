﻿#include <cassert>
#include <ranges>

#include <QtCore/QRegularExpression>

#include "model/classes/MR.h"

namespace gpr::api
{
	namespace
	{
		void replaceEmojis(Note &note, std::map<QString, gpr::Emoji> const &emojiDict)
		{
			QRegularExpression regex{":([a-zA-Z_]+):"};

			qsizetype offset = 0;

			for(auto iter = regex.globalMatch(note.body, offset); iter.hasNext(); iter = regex.globalMatch(note.body, offset))
			{
				auto match = iter.next();
				auto const captured = match.captured(1);
				if(auto const pos = emojiDict.find(captured); pos != emojiDict.end())
				{
					note.body.replace(match.capturedStart(), match.capturedEnd(), pos->second.moji);
					offset = match.capturedStart() + pos->second.moji.length() + 1;
				}
				else
				{
					offset = match.capturedEnd();
				}
			}
		}
	}

	MR::MR(Data data, QObject *parent)
		: QObject(parent)
		, m_data{std::move(data)}
	{}

	int MR::id() const
	{
		return m_data.id;
	}

	int MR::iid() const
	{
		return m_data.iid;
	}

	QDateTime MR::createdAt() const
	{
		return m_data.created;
	}

	void MR::update(Data data)
	{
		if (data.id != m_data.id)
		{
			assert(false && "Invalid MR data");
			return;
		}

		m_data = std::move(data);
		Q_EMIT modified();
	}

	void MR::setCreatedAt(QDateTime time)
	{
		m_data.created = time;
		Q_EMIT modified();
	}

	QDateTime MR::updatedAt() const
	{
		return m_data.updated;
	}

	void MR::setUpdatedAt(QDateTime time)
	{
		m_data.updated = time;
		Q_EMIT modified();
	}

	QString MR::title() const
	{
		return m_data.title;
	}

	void MR::setTitle(QString title)
	{
		m_data.title = std::move(title);
		Q_EMIT modified();
	}

	User const &MR::author() const
	{
		return m_data.author;
	}

	void MR::setAuthor(User author)
	{
		m_data.author = std::move(author);
		Q_EMIT modified();
	}

	User const &MR::assignee() const
	{
		return m_data.assignee;
	}

	void MR::setAssignee(User assignee)
	{
		m_data.assignee = std::move(assignee);
		Q_EMIT modified();
	}

	User const &MR::reviewer() const
	{
		return m_data.reviewer;
	}

	void MR::setReviewer(User reviewer)
	{
		m_data.reviewer = std::move(reviewer);
		Q_EMIT modified();
	}

	QString MR::sourceBranch() const
	{
		return m_data.sourceBranch;
	}

	void MR::setSourceBranch(QString sourceBranch)
	{
		m_data.sourceBranch = std::move(sourceBranch);
		Q_EMIT modified();
	}

	QString MR::targetBranch() const
	{
		return m_data.targetBranch;
	}

	void MR::setTargetBranch(QString targetBranch)
	{
		m_data.targetBranch = std::move(targetBranch);
		Q_EMIT modified();
	}

	std::vector<Discussion> const &MR::discussions() const
	{
		return m_discussions;
	}

	QString MR::url() const
	{
		return m_data.url;
	}

	void MR::setUrl(QString url)
	{
		m_data.url = std::move(url);
		Q_EMIT modified();
	}

	QString MR::mergeStatus() const
	{
		return m_data.mergeStatus;
	}

	void MR::setMergeStatus(QString mergeStatus)
	{
		m_data.mergeStatus = std::move(mergeStatus);
		Q_EMIT modified();
	}

	bool MR::hasNotes() const
	{
		return m_data.hasNotes;
	}

	void MR::setHasNotes(bool hasNotes)
	{
		m_data.hasNotes = hasNotes;
		Q_EMIT modified();
	}

	QString MR::pipelineStatus() const
	{
		return m_pipelineStatus;
	}

	bool MR::hasNewNotes() const
	{
		auto notes = m_discussions | std::views::transform(&Discussion::notes) | std::views::join;
		return !std::ranges::all_of(notes, &Note::wasShown);
	}

	bool MR::discussionsLoaded() const
	{
		return m_discussionsLoaded;
	}

	void MR::setPipelineStatus(QString status)
	{
		m_pipelineStatus = std::move(status);
		Q_EMIT modified();
	}

	std::vector<QString> const &MR::approvedBy() const
	{
		return m_approvedBy;
	}

	bool MR::isApprovedBy(User const &user) const
	{
		return isApprovedBy(user.username);
	}

	bool MR::isApprovedBy(QString const &username) const
	{
		return std::ranges::contains(m_approvedBy, username);
	}

	void MR::setApprovedBy(std::vector<QString> list)
	{
		if(m_approvedBy != list)
		{
			m_approvedBy = std::move(list);
			Q_EMIT modified();
		}
	}

	std::vector<int> MR::updateDiscussions(std::vector<Discussion> discussions, std::map<QString, gpr::Emoji> const &emojiDict)
	{
		std::vector<int> updatedNotes;

		auto const removed = m_discussions
			| std::views::filter([&discussions](auto const &discussion) { return !std::ranges::contains(discussions, discussion.id, &Discussion::id); })
			| std::views::transform(&Discussion::id)
			| std::ranges::to<std::vector>();

		for (auto &discussion : discussions)
		{
			if (auto existingDiscussion = findDiscussion(discussion.id))
			{
				auto updated = updateDiscussionNotes(*existingDiscussion, std::move(discussion.notes), emojiDict);
				std::ranges::move(updated, std::back_inserter(updatedNotes));
				Q_EMIT discussionUpdated(*existingDiscussion);
			}
			else
			{
				for(auto &note : discussion.notes) 
				{
					replaceEmojis(note, emojiDict);
					note.wasShown = !m_discussionsLoaded;
					updatedNotes.push_back(note.id);
				}

				auto const &newDiscussion = m_discussions.emplace_back(std::move(discussion));
				Q_EMIT discussionAdded(newDiscussion);
			}
		}

		for (auto const &discussionId : removed)
		{
			auto existingDiscussion = findDiscussion(discussionId);
			assert(existingDiscussion);
			Q_EMIT discussionRemoved(*existingDiscussion);
			std::erase(m_discussions, *existingDiscussion);
		}

		Q_EMIT modified();

		m_discussionsLoaded = true;

		return updatedNotes;
	}

	void MR::markDiscussionsRead()
	{
		for(auto &discussion : m_discussions)
		{
			discussion.markRead();
			Q_EMIT discussionUpdated(discussion);
		}
		Q_EMIT modified();
	}

	void MR::markDiscussionRead(QString const &discussionId)
	{
		if(auto pos = std::ranges::find(m_discussions, discussionId, &Discussion::id); pos != m_discussions.end())
		{
			pos->markRead();
			Q_EMIT discussionUpdated(*pos);
			Q_EMIT modified();
		}
	}

	QString MR::noteUrl(gpr::Note const &note) const
	{
		return QString{"%1#note_%2"}.arg(url()).arg(note.id);
	}

	bool MR::isUserInvolved(User const &user) const
	{
		return isUserInvolved(user.username);
	}

	bool MR::isUserInvolved(QString const &username) const
	{
		// TODO: Проверять упоминания пользователя в дискуссиях.
		return m_data.author.username == username || m_data.assignee.username == username || m_data.reviewer.username == username;
	}

	std::vector<int> MR::updateDiscussionNotes(Discussion &discussion, std::vector<Note> notes, std::map<QString, gpr::Emoji> const &emojiDict)
	{
		std::vector<int> updatedNotes;

		auto const removed = discussion.notes
			| std::views::filter([&notes](auto const &note) { return !std::ranges::contains(notes, note.id, &Note::id); })
			| std::views::transform(&Note::id)
			| std::ranges::to<std::vector>();

		for (auto &note : notes)
		{
			if(!m_discussionsLoaded) note.wasShown = true;

			if (auto existingNote = findDiscussionNote(discussion, note.id))
			{
				if(note.updated > existingNote->updated)
				{
					replaceEmojis(note, emojiDict);
					updatedNotes.push_back(note.id);
					auto const wasShown = existingNote->wasShown;
					auto reactions = std::move(existingNote->reactions);
					*existingNote = std::move(note);
					existingNote->wasShown = wasShown;
					existingNote->reactions = std::move(reactions);
					Q_EMIT discussionNoteUpdated(discussion, *existingNote);
				}
			}
			else
			{
				replaceEmojis(note, emojiDict);
				updatedNotes.push_back(note.id);
				auto const &newNote = discussion.notes.emplace_back(std::move(note));
				Q_EMIT discussionNoteAdded(discussion, newNote);
			}
		}

		for (auto const &noteId : removed)
		{
			auto existingNote = findDiscussionNote(discussion, noteId);
			assert(existingNote);
			Q_EMIT discussionNoteRemoved(discussion, *existingNote);
			std::erase(discussion.notes, *existingNote);
		}

		Q_EMIT modified();

		m_discussionsLoaded = true;

		return updatedNotes;
	}


	void MR::setNoteReactions(int noteId, std::vector<gpr::EmojiReaction> reactions)
	{
		for(auto &discussion : m_discussions)
		{
			if(auto pos = std::ranges::find(discussion.notes, noteId, &Note::id); pos != discussion.notes.cend())
			{
				pos->reactions = std::move(reactions);
				Q_EMIT discussionNoteUpdated(discussion, *pos);
				return;
			}
		}
	}


	Discussion *MR::findDiscussion(QString const &id)
	{
		if(auto const pos = std::ranges::find(m_discussions, id, &Discussion::id); pos != m_discussions.end())
		{
			return &(*pos);
		}
		return nullptr;
	}

	Note *MR::findDiscussionNote(int noteId)
	{
		auto notes = m_discussions | std::views::transform(&Discussion::notes) | std::views::join;

		if(auto const pos = std::ranges::find(notes, noteId, &Note::id); pos != std::ranges::end(notes))
		{
			return &*pos;
		}
		return nullptr;
	}

	Note *MR::findDiscussionNote(Discussion &discussion, int noteId) const
	{
		if(auto const pos = std::ranges::find(discussion.notes, noteId, &Note::id); pos != discussion.notes.end())
		{
			return &(*pos);
		}
		return nullptr;
	}
} // namespace gpr::api
