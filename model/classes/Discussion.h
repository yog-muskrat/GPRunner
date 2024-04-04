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
		Q_PROPERTY (QString id READ id NOTIFY modified)
		Q_PROPERTY (QString url READ url NOTIFY modified)
		Q_PROPERTY (bool isResolved READ isResolved NOTIFY modified)
		Q_PROPERTY (bool isResolvable READ isResolvable NOTIFY modified)
		Q_PROPERTY (bool hasUnreadNotes READ hasUnreadNotes NOTIFY modified)
		Q_PROPERTY (User author READ author NOTIFY modified)
		Q_PROPERTY (int noteCount READ noteCount NOTIFY modified)
		Q_PROPERTY (QDateTime started READ started NOTIFY modified)
		Q_PROPERTY (QDateTime updated READ updated NOTIFY modified)

	public:

		struct Data
		{
			QString id;

			friend auto operator<(Data const &l, Data const &r) { return l.id < r.id; }
		};

		Discussion(Data data, MR &mr);

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
		Q_INVOKABLE bool isUserInvolved(QString const &username) const;

		QString url() const;
		
		std::vector<QPointer<Note>> const &notes() const;
		int noteCount() const;
		QPointer<Note> findNote(int id) const;
		void updateNotes(std::vector<Note::Data> notes);

		bool isEmpty() const;
		bool isResolvable() const;
		bool isResolved() const;

		User const &author() const;

		QDateTime started() const;
		QDateTime updated() const;

		void setLoadFinished();

	Q_SIGNALS:
		void modified();
		void noteAdded(QPointer<Note>);
		void noteRemoved(QPointer<Note>);
		void noteUpdated(QPointer<Note>);

	private:
		void connectNote(QPointer<Note> note);

		Data m_data;
		MR &m_mr;

		bool m_loaded{false};

		std::vector<QPointer<Note>> m_notes;
	};
} // namespace gpr::api
