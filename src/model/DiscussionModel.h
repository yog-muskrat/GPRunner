#pragma once

#include <QQmlEngine>
#include <QtCore/QPointer>
#include <QtCore/QAbstractItemModel>

#include "GPManager.h"

namespace gpr
{
	struct Discussion;
	struct Note;

	namespace api
	{
		class MR;
		class Discussion;
		class Note;
	}
} // namespace gpr

class DiscussionModel : public QAbstractItemModel
{
	Q_OBJECT
	QML_ELEMENT
	Q_PROPERTY(GPManager* manager WRITE setGPManager READ gpManager)

public:
	enum Column
	{
		Note,
		Count
	};

	enum Role
	{
		DiscussionRole = Qt::ItemDataRole::UserRole,
		NoteRole,
		IsNoteRole,
	};
	Q_ENUMS(Role)

	DiscussionModel(QObject *parent = nullptr);

	Q_INVOKABLE void clear();
	Q_INVOKABLE void setMR(gpr::api::MR* mr);
	
	void setGPManager(GPManager *manager);
	GPManager *gpManager() const { return m_manager; }

	int columnCount(QModelIndex const &parent = {}) const override;
	int rowCount(QModelIndex const &parent = {}) const override;
	QVariant data(QModelIndex const &index, int role) const override;
	QModelIndex index(int row, int column, QModelIndex const &parent = {}) const override;
	QModelIndex parent(QModelIndex const &index) const override;
	QHash<int, QByteArray> roleNames() const override;

private:
	QVariant discussionData(QPointer<gpr::api::Discussion> discussion, int role) const;
	QVariant noteData(QPointer<gpr::api::Note> note, int role) const;

	void connectMR(QPointer<gpr::api::MR> mr);
	void disconnectMR(QPointer<gpr::api::MR> mr);

	void onMRRemoved();

	void onDiscussionAdded(QPointer<gpr::api::Discussion> discussion);
	void onDiscussionUpdated(QPointer<gpr::api::Discussion> discussion);
	void onDiscussionRemoved(QPointer<gpr::api::Discussion> discussion);

	void onDiscussionNoteAdded(QPointer<gpr::api::Note> note);
	void onDiscussionNoteUpdated(QPointer<gpr::api::Note> note);
	void onDiscussionNoteRemoved(QPointer<gpr::api::Note> note);

	int getRow(QPointer<gpr::api::Discussion> discussion) const;
	int getRow(QPointer<gpr::api::Note> note) const;

	GPManager *m_manager;

	QPointer<gpr::api::MR> m_mr;
};
