#include "model/DiscussionModel.h"
#include "model/classes/MR.h"

#include "GPManager.h"

DiscussionModel::DiscussionModel(GPManager &manager)
	: QAbstractItemModel(&manager)
	, m_manager{manager}
{}

void DiscussionModel::clear()
{
	setMR(nullptr);
}

void DiscussionModel::setMR(QPointer<gpr::api::MR> mr)
{
	beginResetModel();
	m_mr = mr;

	// TODO: mark notes read

	endResetModel();
}

int DiscussionModel::columnCount(QModelIndex const &) const
{
	return Column::Count;
}

int DiscussionModel::rowCount(QModelIndex const &parent) const
{
	if (!m_mr) return 0;

	if (!parent.isValid()) return m_mr->discussions().size();

	if (parent.internalPointer()) return 0;

	auto const discussion = m_mr->discussions().at(parent.row());
	return discussion.notes.size();
}

QVariant DiscussionModel::data(QModelIndex const &index, int role) const
{
	assert(index.isValid());

	if (index.parent().isValid())
	{
		auto const discussion = static_cast<gpr::Discussion const *>(index.internalPointer());
		auto const &note = discussion->notes.at(index.row());
		return noteData(*discussion, note, role);
	}

	return discussionData(m_mr->discussions().at(index.row()), role);
}

QModelIndex DiscussionModel::index(int row, int column, QModelIndex const &parent) const
{
	if (!parent.isValid())
	{
		return createIndex(row, column);
	}

	if (parent.internalPointer()) return {};

	auto const &discussion = m_mr->discussions().at(parent.row());
	return createIndex(row, column, &discussion);
}

QModelIndex DiscussionModel::parent(QModelIndex const &index) const
{
	if (!index.isValid()) return {};

	if (index.internalPointer() == nullptr) return {};

	auto const discussion = static_cast<gpr::Discussion const *>(index.internalPointer());

	auto pos = std::ranges::find(m_mr->discussions(), discussion->id, &gpr::Discussion::id);
	assert(pos != m_mr->discussions().end());

	auto const parentRow = std::ranges::distance(m_mr->discussions().begin(), pos);

	return this->index(parentRow, index.column());
}

QHash<int, QByteArray> DiscussionModel::roleNames() const
{
	auto names = QAbstractItemModel::roleNames();
	names.insert(Qt::FontRole, "font");
	names.insert(Role::Author, "author");
	names.insert(Role::Avatar, "avatar");
	names.insert(Role::CreatedDate, "created");
	names.insert(Role::Resolvable, "resolvable");
	names.insert(Role::Resolved, "resolved");
	return names;
}

QVariant DiscussionModel::discussionData(gpr::Discussion const &discussion, int role) const
{
	if (role == Qt::ItemDataRole::DisplayRole)
	{ 
		auto pos = std::ranges::find(m_mr->discussions(), discussion.id, &gpr::Discussion::id);
		assert(pos != m_mr->discussions().end());

		auto const row = std::ranges::distance(m_mr->discussions().begin(), pos);

		return QString("%1 [%2/%3]").arg(discussion.isResolvable() ? "Дискуссия" : "Комментарий" ).arg(row + 1).arg(m_mr->discussions().size());
	}
	if (role == Role::Author && !discussion.isEmpty())      return discussion.notes.front().author;
	if (role == Role::Avatar && !discussion.isEmpty())      return discussion.notes.front().authorAvatar;
	if (role == Role::CreatedDate && !discussion.isEmpty()) return discussion.notes.front().created;
	if (role == Role::Resolvable)                           return discussion.isResolvable();
	if (role == Role::Resolved)                             return discussion.isResolved();

	return QVariant{};
}

QVariant DiscussionModel::noteData(gpr::Discussion const &discussion, gpr::Note const &note, int role) const
{
	// mark node as read
	if (role == Qt::ItemDataRole::DisplayRole) return note.body;
	if (role == Role::Author)                  return note.author;
	if (role == Role::Avatar)                  return note.authorAvatar;
	if (role == Role::CreatedDate)             return note.created;
	if (role == Role::Resolvable)              return note.resolvable;
	if (role == Role::Resolved)                return note.resolved;

	return QVariant{};
}
