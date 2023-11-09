#pragma once

#include <QPointer>
#include <QAbstractItemModel>

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
		CreatedDate,
		Resolvable,
		Resolved
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

	GPManager &m_manager;

	QPointer<gpr::api::MR> m_mr;
};
