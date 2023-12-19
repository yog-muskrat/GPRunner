#include "model/classes/Discussion.h"

#include <ranges>

#include "GPManager.h"
#include "model/classes/GPClasses.h"

namespace gpr::api
{
	Discussion::Discussion(GPManager &manager, Data data, QObject *parent)
		: QObject(parent)
		, m_data{std::move(data)}
		, m_manager{manager}
	{}

	QString const &Discussion::id() const
	{
		return m_data.id;
	}

	void Discussion::update(Data data)
	{
		assert(data.id == m_data.id);

		m_data = std::move(data);
		Q_EMIT modified();
	}

	std::vector<QPointer<Note>> const &Discussion::notes() const
	{
		return m_notes;
	}

	QPointer<Note> Discussion::findNote(int id) const
	{
		if(auto const pos = std::ranges::find(m_notes, id, &Note::id); pos != m_notes.cend())
		{
			return *pos;
		}

		return nullptr;
	}

	void Discussion::updateNotes(std::vector<Note::Data> notes)
	{
		auto const removed = m_notes
			| std::views::filter([&notes](auto const &note) { return !std::ranges::contains(notes, note->id(), &Note::Data::id); })
			| std::views::transform(&Note::id)
			| std::ranges::to<std::vector>();

		for (auto &note : notes)
		{
			if (auto pos = std::ranges::find(m_notes, note.id, &Note::id); pos != m_notes.end())
			{
				(*pos)->update(std::move(note));
			}
			else
			{
				auto const &newNote = m_notes.emplace_back(new Note(m_manager, std::move(note), this));
				if(!m_loaded) newNote->markRead();
				connectNote(newNote);
				Q_EMIT noteAdded(newNote);
			}
		}

		for (auto const &noteId : removed)
		{
			auto pos = std::ranges::find(m_notes, noteId, &Note::id);
			assert(pos != m_notes.cend());
			Q_EMIT noteRemoved(*pos);
			m_notes.erase(pos);
		}

		Q_EMIT modified();
	}

	bool Discussion::isEmpty() const
	{
		return m_notes.empty();
	}

	bool Discussion::isResolvable() const
	{
		return std::ranges::any_of(m_notes, &Note::isResolvable);
	}

	bool Discussion::isResolved() const
	{
		return std::ranges::all_of(m_notes, &Note::isResolved);
	}

	User const &Discussion::author() const
	{
		static User const empty;
		
		// NOTE: Автором дискуссии считаем автора первой заметки
		return isEmpty() ? empty : m_notes.front()->author();
	}

	void Discussion::markRead()
	{
		std::ranges::for_each(m_notes, &Note::markRead);
	}

	void Discussion::setLoadFinished()
	{
		m_loaded = true;
	}

	void Discussion::connectNote(QPointer<Note> note)
	{
		connect(note, &Note::modified, [this, note] { Q_EMIT noteUpdated(note); });
	}

} // namespace gpr::api
