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
		Avatar,
		DiscussionId,
		CreatedDate,
		Resolvable,
		Resolved,
		CanResolve,
		CanUnresolve
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
	QVariant discussionData(gpr::Discussion const &discussion, int role) const;
	QVariant noteData(gpr::Discussion const &discussion, gpr::Note const &note, int role) const;

	void connectMR(QPointer<gpr::api::MR> mr);
	void disconnectMR(QPointer<gpr::api::MR> mr);

	void onDiscussionAdded(gpr::Discussion const &discussion);
	void onDiscussionUpdated(gpr::Discussion const &discussion);
	void onDiscussionRemoved(gpr::Discussion const &discussion);

	void onDiscussionNoteAdded(gpr::Discussion const &discussion, gpr::Note const &note);
	void onDiscussionNoteUpdated(gpr::Discussion const &discussion, gpr::Note const &note);
	void onDiscussionNoteRemoved(gpr::Discussion const &discussion, gpr::Note const &note);

	int getRow(gpr::Discussion const &discussion) const;
	int getRow(gpr::Discussion const &discussion, gpr::Note const &note) const;

	GPManager &m_manager;

	QPointer<gpr::api::MR> m_mr;
};
