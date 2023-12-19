#pragma once

#include <QtCore/QPointer>
#include <QtCore/QAbstractItemModel>

class GPManager;

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
public:
	enum Column
	{
		Note,
		Count
	};

	enum Role
	{
		Author = Qt::ItemDataRole::UserRole,
		DiscussionId,
		NoteId,
		CreatedDate,
		Resolvable,
		Resolved,
		CanEdit,
		CanResolve,
		CanUnresolve,
		NoteCount,
		NoteUrl,
		HasUnreadNotes,
		Reactions,
	};

	DiscussionModel(GPManager &manager);

	void clear();
	void setMR(QPointer<gpr::api::MR> mr);

	int columnCount(QModelIndex const &parent = {}) const override;
	int rowCount(QModelIndex const &parent = {}) const override;
	QVariant data(QModelIndex const &index, int role) const override;
	QModelIndex index(int row, int column, QModelIndex const &parent = {}) const override;
	QModelIndex parent(QModelIndex const &index) const override;
	QHash<int, QByteArray> roleNames() const override;

private:
	QVariant discussionData(QPointer<gpr::api::Discussion> discussion, int role) const;
	QVariant noteData(QPointer<gpr::api::Discussion> discussion, QPointer<gpr::api::Note> note, int role) const;

	void connectMR(QPointer<gpr::api::MR> mr);
	void disconnectMR(QPointer<gpr::api::MR> mr);

	void onDiscussionAdded(QPointer<gpr::api::Discussion> discussion);
	void onDiscussionUpdated(QPointer<gpr::api::Discussion> discussion);
	void onDiscussionRemoved(QPointer<gpr::api::Discussion> discussion);

	void onDiscussionNoteAdded(QPointer<gpr::api::Discussion> discussion, QPointer<gpr::api::Note> note);
	void onDiscussionNoteUpdated(QPointer<gpr::api::Discussion> discussion, QPointer<gpr::api::Note> note);
	void onDiscussionNoteRemoved(QPointer<gpr::api::Discussion> discussion, QPointer<gpr::api::Note> note);

	int getRow(QPointer<gpr::api::Discussion> discussion) const;
	int getRow(QPointer<gpr::api::Discussion> discussion, QPointer<gpr::api::Note> note) const;

	GPManager &m_manager;

	QPointer<gpr::api::MR> m_mr;
};
