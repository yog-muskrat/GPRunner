#pragma once

#include <QObject>

#include "model/classes/Note.h"

class GPManager;

namespace gpr::api
{
	class Discussion : public QObject
	{
		Q_OBJECT
	public:

		struct Data
		{
			QString id;

			friend auto operator<(Data const &l, Data const &r) { return l.id < r.id; }
		};

		Discussion(GPManager &manager, Data data, QObject *parent = nullptr);

		QString const &id() const;

		void update(Data data);

		std::vector<QPointer<Note>> const &notes() const;
		QPointer<Note> findNote(int id) const;
		void updateNotes(std::vector<Note::Data> notes);

		bool isEmpty() const;
		bool isResolvable() const;
		bool isResolved() const;

		User const &author() const;

		void markRead();

		void setLoadFinished();

	Q_SIGNALS:
		void modified();
		void noteAdded(QPointer<Note>);
		void noteRemoved(QPointer<Note>);
		void noteUpdated(QPointer<Note>);

	private:
		void connectNote(QPointer<Note> note);

		Data m_data;
		GPManager &m_manager;

		bool m_loaded{false};

		std::vector<QPointer<Note>> m_notes;
	};
} // namespace gpr::api
