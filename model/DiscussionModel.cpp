#include <ranges>

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
	disconnectMR(m_mr);
	m_mr = mr;
	connectMR(m_mr);
	endResetModel();
}

int DiscussionModel::columnCount(QModelIndex const &) const
{
	return Column::Count;
}

int DiscussionModel::rowCount(QModelIndex const &parent) const
{
	if (!m_mr) return 0;

	if (!parent.isValid()) return std::ranges::ssize(m_mr->discussions());

	if (parent.internalPointer()) return 0;

	auto const discussion = m_mr->discussions().at(parent.row());
	return std::ranges::ssize(discussion.notes);
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
	names.insert(Role::CanResolve, "canResolve");
	names.insert(Role::CanUnresolve, "canUnresolve");
	names.insert(Role::DiscussionId, "discussionId");
	names.insert(Role::NoteCount, "noteCount");
	names.insert(Role::HasUnreadNotes, "hasUnreadNotes");
	return names;
}

QVariant DiscussionModel::discussionData(gpr::Discussion const &discussion, int role) const
{
	if (role == Qt::ItemDataRole::DisplayRole)
	{
		auto resolvables = m_mr->discussions() | std::views::filter(&gpr::Discussion::isResolvable);

		if(auto pos = std::ranges::find(resolvables, discussion.id, &gpr::Discussion::id); pos == std::ranges::cend(resolvables))
		{
			return QString("Комментарий от %1").arg(discussion.notes.front().author);
		}
		else
		{
			auto const row = std::ranges::distance(resolvables.cbegin(), pos);
			auto const count = std::ranges::count_if(m_mr->discussions(), &gpr::Discussion::isResolvable);

			return QString("Дискуссия [%1/%2] от %3").arg(row).arg(count).arg(discussion.notes.front().author);
		}
	}
	if (role == Role::NoteCount)                            return discussion.notes.size();
	if (role == Role::HasUnreadNotes)                       return std::ranges::any_of(discussion.notes, std::not_fn(&gpr::Note::wasShown));
	if (role == Role::Author && !discussion.isEmpty())      return discussion.notes.front().author;
	if (role == Role::Avatar && !discussion.isEmpty())      return discussion.notes.front().authorAvatar;
	if (role == Role::CreatedDate && !discussion.isEmpty()) return discussion.notes.front().created;
	if (role == Role::Resolvable)                           return discussion.isResolvable();
	if (role == Role::Resolved)                             return discussion.isResolved();
	if (role == Role::DiscussionId)                         return discussion.id;
	if (role == Role::CanResolve)
	{
		return !discussion.isEmpty() && discussion.notes.front().author == m_manager.getCurrentUser() && discussion.isResolvable()
		    && !discussion.isResolved();
	}
	if (role == Role::CanUnresolve)
	{
		return !discussion.isEmpty() && discussion.notes.front().author == m_manager.getCurrentUser() && discussion.isResolved();
	}

	return QVariant{};
}

QVariant DiscussionModel::noteData(gpr::Discussion const &, gpr::Note const &note, int role) const
{
	if (role == Qt::ItemDataRole::DisplayRole)
	{
		// NOTE: Считаем, что если у модели была запрошена displayRole, то текст заметки был отрисован и увиден пользователем.
		// TODO: Подумать, как сделать умнее, чтобы не использовать mutable поле.
		note.wasShown = true;
		return note.body;
	}
	if (role == Role::Author)         return note.author;
	if (role == Role::Avatar)         return note.authorAvatar;
	if (role == Role::CreatedDate)    return note.created;
	if (role == Role::Resolvable)     return note.resolvable;
	if (role == Role::Resolved)       return note.resolved;
	if (role == Role::CanResolve)     return false;
	if (role == Role::CanUnresolve)   return false;
	if (role == Role::NoteCount)      return 0;
	if (role == Role::HasUnreadNotes) return false;
	return QVariant{};
}

void DiscussionModel::connectMR(QPointer<gpr::api::MR> mr)
{
	if(!mr) return;

	connect(mr, &gpr::api::MR::discussionAdded, this, &DiscussionModel::onDiscussionAdded);
	connect(mr, &gpr::api::MR::discussionUpdated, this, &DiscussionModel::onDiscussionUpdated);
	connect(mr, &gpr::api::MR::discussionRemoved, this, &DiscussionModel::onDiscussionRemoved);

	connect(mr, &gpr::api::MR::discussionNoteAdded, this, &DiscussionModel::onDiscussionNoteAdded);
	connect(mr, &gpr::api::MR::discussionNoteUpdated, this, &DiscussionModel::onDiscussionNoteUpdated);
	connect(mr, &gpr::api::MR::discussionNoteRemoved, this, &DiscussionModel::onDiscussionNoteRemoved);
}

void DiscussionModel::disconnectMR(QPointer<gpr::api::MR> mr)
{
	if(!mr) return;

	disconnect(mr, &gpr::api::MR::discussionAdded, this, &DiscussionModel::onDiscussionAdded);
	disconnect(mr, &gpr::api::MR::discussionUpdated, this, &DiscussionModel::onDiscussionUpdated);
	disconnect(mr, &gpr::api::MR::discussionRemoved, this, &DiscussionModel::onDiscussionRemoved);

	disconnect(mr, &gpr::api::MR::discussionNoteAdded, this, &DiscussionModel::onDiscussionNoteAdded);
	disconnect(mr, &gpr::api::MR::discussionNoteUpdated, this, &DiscussionModel::onDiscussionNoteUpdated);
	disconnect(mr, &gpr::api::MR::discussionNoteRemoved, this, &DiscussionModel::onDiscussionNoteRemoved);
}

void DiscussionModel::onDiscussionAdded(gpr::Discussion const &discussion)
{
	auto const row = getRow(discussion);
	assert(row >= 0);
	beginInsertRows({}, row, row);
	endInsertRows();
}

void DiscussionModel::onDiscussionUpdated(gpr::Discussion const &discussion)
{
	auto const row = getRow(discussion);
	assert(row >= 0);
	Q_EMIT dataChanged(index(row, 0), index(row, columnCount() - 1));
}

void DiscussionModel::onDiscussionRemoved(gpr::Discussion const &discussion)
{
	auto const row = getRow(discussion);
	assert(row >= 0);
	beginRemoveRows({}, row, row);
	endRemoveRows();
}

void DiscussionModel::onDiscussionNoteAdded(gpr::Discussion const &discussion, gpr::Note const &note)
{
	auto const parentRow = getRow(discussion);
	assert(parentRow >= 0);

	auto const row = getRow(discussion, note);
	assert(row >= 0);

	auto const parentIndex = index(parentRow, 0);

	beginInsertRows(parentIndex, row, row);
	endInsertRows();
}

void DiscussionModel::onDiscussionNoteUpdated(gpr::Discussion const &discussion, gpr::Note const &note)
{
	auto const parentRow = getRow(discussion);
	assert(parentRow >= 0);

	auto const row = getRow(discussion, note);
	assert(row >= 0);

	auto const parentIndex = index(parentRow, 0);

	Q_EMIT dataChanged(index(row, 0, parentIndex), index(row, columnCount() - 1, parentIndex));
}

void DiscussionModel::onDiscussionNoteRemoved(gpr::Discussion const &discussion, gpr::Note const &note)
{
	auto const parentRow = getRow(discussion);
	assert(parentRow >= 0);

	auto const row = getRow(discussion, note);
	assert(row >= 0);

	auto const parentIndex = index(parentRow, 0);

	beginRemoveRows(parentIndex, row, row);
	endRemoveRows();
}

int DiscussionModel::getRow(gpr::Discussion const &discussion) const
{
	if(auto const pos = std::ranges::find(m_mr->discussions(), discussion); pos != m_mr->discussions().cend())
	{
		return static_cast<int>(std::ranges::distance(m_mr->discussions().cbegin(), pos));
	}
	return -1;
}

int DiscussionModel::getRow(gpr::Discussion const &discussion, gpr::Note const &note) const
{
	if(auto const pos = std::ranges::find(discussion.notes, note); pos != discussion.notes.cend())
	{
		return static_cast<int>(std::ranges::distance(discussion.notes.cbegin(), pos));
	}
	return -1;
}
