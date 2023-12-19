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
	return std::ranges::ssize(discussion->notes());
}

QVariant DiscussionModel::data(QModelIndex const &index, int role) const
{
	if(!index.isValid()) return {};

	if (index.parent().isValid())
	{
		auto const discussion = static_cast<gpr::api::Discussion *>(index.internalPointer());
		auto const &note = discussion->notes().at(index.row());
		return noteData(discussion, note, role);
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

	auto const discussion = m_mr->discussions().at(parent.row());
	return createIndex(row, column, &*discussion);
}

QModelIndex DiscussionModel::parent(QModelIndex const &index) const
{
	if (!index.isValid()) return {};

	if (index.internalPointer() == nullptr) return {};

	auto const discussion = static_cast<gpr::api::Discussion *>(index.internalPointer());

	auto pos = std::ranges::find(m_mr->discussions(), discussion->id(), &gpr::api::Discussion::id);
	assert(pos != m_mr->discussions().end());

	auto const parentRow = std::ranges::distance(m_mr->discussions().begin(), pos);

	return this->index(parentRow, index.column());
}

QHash<int, QByteArray> DiscussionModel::roleNames() const
{
	auto names = QAbstractItemModel::roleNames();
	names.insert(Qt::FontRole,         "font");
	names.insert(Role::Author,         "author");
	names.insert(Role::CreatedDate,    "created");
	names.insert(Role::Resolvable,     "resolvable");
	names.insert(Role::Resolved,       "resolved");
	names.insert(Role::CanResolve,     "canResolve");
	names.insert(Role::CanUnresolve,   "canUnresolve");
	names.insert(Role::CanEdit,        "canEdit");
	names.insert(Role::DiscussionId,   "discussionId");
	names.insert(Role::NoteId,         "noteId");
	names.insert(Role::NoteCount,      "noteCount");
	names.insert(Role::NoteUrl,        "noteUrl");
	names.insert(Role::HasUnreadNotes, "hasUnreadNotes");
	names.insert(Role::Reactions,      "reactions");
	return names;
}

QVariant DiscussionModel::discussionData(QPointer<gpr::api::Discussion> discussion, int role) const
{
	if (role == Qt::ItemDataRole::DisplayRole)
	{
		auto resolvables = m_mr->discussions() | std::views::filter(&gpr::api::Discussion::isResolvable);

		if(auto pos = std::ranges::find(resolvables, discussion->id(), &gpr::api::Discussion::id); pos == std::ranges::cend(resolvables))
		{
			return QString("Комментарий от %1").arg(discussion->notes().front()->author().username);
		}
		else
		{
			auto const row = std::ranges::distance(resolvables.cbegin(), pos);
			auto const count = std::ranges::count_if(m_mr->discussions(), &gpr::api::Discussion::isResolvable);

			return QString("Дискуссия [%1/%2] от %3").arg(row + 1).arg(count).arg(discussion->notes().front()->author().username);
		}
	}
	if (role == Role::HasUnreadNotes)
	{
		return m_mr->isUserInvolved(m_manager.getCurrentUser())
		    && std::ranges::any_of(discussion->notes(), std::not_fn(&gpr::api::Note::isRead));
	}
	if (role == Role::NoteCount)                             return discussion->notes().size();
	if (role == Role::NoteUrl)                               return m_mr->noteUrl(*(discussion->notes().front()));
	if (role == Role::Author && !discussion->isEmpty())      return QVariant::fromValue(discussion->notes().front()->author());
	if (role == Role::CreatedDate && !discussion->isEmpty()) return discussion->notes().front()->created();
	if (role == Role::Resolvable)                            return discussion->isResolvable();
	if (role == Role::Resolved)                              return discussion->isResolved();
	if (role == Role::DiscussionId)                          return discussion->id();
	if (role == Role::CanEdit)                               return false;
	if (role == Role::CanResolve)
	{
		return !discussion->isEmpty() && discussion->notes().front()->author() == m_manager.getCurrentUser() && discussion->isResolvable()
		    && !discussion->isResolved();
	}
	if (role == Role::CanUnresolve)
	{
		return !discussion->isEmpty() && discussion->notes().front()->author() == m_manager.getCurrentUser() && discussion->isResolved();
	}

	return QVariant{};
}

QVariant DiscussionModel::noteData(QPointer<gpr::api::Discussion> discussion, QPointer<gpr::api::Note> note, int role) const
{
	if (role == Qt::ItemDataRole::DisplayRole)
	{
		// NOTE: Считаем, что если у модели была запрошена displayRole, то текст заметки был отрисован и увиден пользователем.
		note->markRead();
		return note->body();
	}
	if (role == Role::Author)         return QVariant::fromValue(note->author());
	if (role == Role::CreatedDate)    return note->created();
	if (role == Role::Resolvable)     return note->isResolvable();
	if (role == Role::Resolved)       return note->isResolved();
	if (role == Role::CanResolve)     return note->author() == m_manager.getCurrentUser() && note->isResolvable() && !note->isResolved();
	if (role == Role::CanUnresolve)   return note->author() == m_manager.getCurrentUser() && note->isResolvable() && note->isResolved();
	if (role == Role::CanEdit)        return note->author() == m_manager.getCurrentUser();
	if (role == Role::NoteCount)      return 0;
	if (role == Role::HasUnreadNotes) return false;
	if (role == Role::DiscussionId)   return discussion->id();
	if (role == Role::NoteId)         return note->id();
	if (role == Role::NoteUrl)        return m_mr->noteUrl(*note);
	if (role == Role::Reactions)
	{
		QVariantList reactions;
		for(auto const &reaction: note->reactions()) reactions.push_back(QVariant::fromValue(reaction));
		return reactions;
	}
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

void DiscussionModel::onDiscussionAdded(QPointer<gpr::api::Discussion> discussion)
{
	auto const row = getRow(discussion);
	assert(row >= 0);
	beginInsertRows({}, row, row);
	endInsertRows();
}

void DiscussionModel::onDiscussionUpdated(QPointer<gpr::api::Discussion> discussion)
{
	auto const row = getRow(discussion);
	assert(row >= 0);
	Q_EMIT dataChanged(index(row, 0), index(row, columnCount() - 1));
}

void DiscussionModel::onDiscussionRemoved(QPointer<gpr::api::Discussion> discussion)
{
	auto const row = getRow(discussion);
	assert(row >= 0);
	beginRemoveRows({}, row, row);
	endRemoveRows();
}

void DiscussionModel::onDiscussionNoteAdded(QPointer<gpr::api::Discussion> discussion, QPointer<gpr::api::Note> note)
{
	auto const parentRow = getRow(discussion);
	assert(parentRow >= 0);

	auto const row = getRow(discussion, note);
	assert(row >= 0);

	auto const parentIndex = index(parentRow, 0);

	beginInsertRows(parentIndex, row, row);
	endInsertRows();
}

void DiscussionModel::onDiscussionNoteUpdated(QPointer<gpr::api::Discussion> discussion, QPointer<gpr::api::Note> note)
{
	auto const parentRow = getRow(discussion);
	assert(parentRow >= 0);

	auto const row = getRow(discussion, note);
	assert(row >= 0);

	auto const parentIndex = index(parentRow, 0);

	Q_EMIT dataChanged(index(row, 0, parentIndex), index(row, columnCount() - 1, parentIndex));
}

void DiscussionModel::onDiscussionNoteRemoved(QPointer<gpr::api::Discussion> discussion, QPointer<gpr::api::Note> note)
{
	auto const parentRow = getRow(discussion);
	assert(parentRow >= 0);

	auto const row = getRow(discussion, note);
	assert(row >= 0);

	auto const parentIndex = index(parentRow, 0);

	beginRemoveRows(parentIndex, row, row);
	endRemoveRows();
}

int DiscussionModel::getRow(QPointer<gpr::api::Discussion> discussion) const
{
	if(auto const pos = std::ranges::find(m_mr->discussions(), discussion); pos != m_mr->discussions().cend())
	{
		return static_cast<int>(std::ranges::distance(m_mr->discussions().cbegin(), pos));
	}
	return -1;
}

int DiscussionModel::getRow(QPointer<gpr::api::Discussion> discussion, QPointer<gpr::api::Note> note) const
{
	if(auto const pos = std::ranges::find(discussion->notes(), note); pos != discussion->notes().cend())
	{
		return static_cast<int>(std::ranges::distance(discussion->notes().cbegin(), pos));
	}
	return -1;
}
