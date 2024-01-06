#pragma once

#include <QObject>

#include "model/classes/Note.h"

class GPManager;

namespace gpr::api
{
	class MR;

	class Discussion : public QObject
	{
		Q_OBJECT
		Q_PROPERTY (QString id READ id)
		Q_PROPERTY (bool isResolved READ isResolved NOTIFY modified)
		Q_PROPERTY (bool isResolvable READ isResolvable NOTIFY modified)
		Q_PROPERTY (User author READ author NOTIFY modified)

	public:

		struct Data
		{
			QString id;

			friend auto operator<(Data const &l, Data const &r) { return l.id < r.id; }
		};

		Discussion(GPManager &manager, Data data, MR &mr);

		QString const &id() const;

		void update(Data data);

		MR &mr();
		MR const &mr() const;

		Q_INVOKABLE void resolve();
		Q_INVOKABLE void unresolve();

		Q_INVOKABLE void addNote(QString noteBody);

		Q_INVOKABLE void markRead();
		Q_INVOKABLE bool hasUnreadNotes() const;
		Q_INVOKABLE bool userCanResolve(User const &user) const;

		std::vector<QPointer<Note>> const &notes() const;
		QPointer<Note> findNote(int id) const;
		void updateNotes(std::vector<Note::Data> notes);

		bool isEmpty() const;
		bool isResolvable() const;
		bool isResolved() const;

		User const &author() const;

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
		MR &m_mr;

		bool m_loaded{false};

		std::vector<QPointer<Note>> m_notes;
	};
} // namespace gpr::api
