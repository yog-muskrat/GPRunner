#include <cassert>

#include "model/classes/MR.h"

namespace gpr::api
{
	MR::MR(Data data, QObject *parent)
		: QObject(parent)
		, m_data{std::move(data)}
	{}

	int MR::id() const
	{
		return m_data.id;
	}

	int MR::iid() const
	{
		return m_data.iid;
	}

	QDateTime MR::createdAt() const
	{
		return m_data.created;
	}

	void MR::update(Data data)
	{
		if (data.id != m_data.id)
		{
			assert(false && "Invalid MR data");
			return;
		}

		m_data = std::move(data);
		Q_EMIT modified();
	}

	void MR::setCreatedAt(QDateTime time)
	{
		m_data.created = time;
		Q_EMIT modified();
	}

	QDateTime MR::updatedAt() const
	{
		return m_data.updated;
	}

	void MR::setUpdatedAt(QDateTime time)
	{
		m_data.updated = time;
		Q_EMIT modified();
	}

	QString MR::title() const
	{
		return m_data.title;
	}

	void MR::setTitle(QString title)
	{
		m_data.title = std::move(title);
		Q_EMIT modified();
	}

	QString MR::author() const
	{
		return m_data.author;
	}

	void MR::setAuthor(QString author)
	{
		m_data.author = std::move(author);
		Q_EMIT modified();
	}

	QString MR::assignee() const
	{
		return m_data.assignee;
	}

	void MR::setAssignee(QString assignee)
	{
		m_data.assignee = std::move(assignee);
		Q_EMIT modified();
	}

	QString MR::reviewer() const
	{
		return m_data.reviewer;
	}

	void MR::setReviewer(QString reviewer)
	{
		m_data.reviewer = std::move(reviewer);
		Q_EMIT modified();
	}

	QString MR::sourceBranch() const
	{
		return m_data.sourceBranch;
	}

	void MR::setSourceBranch(QString sourceBranch)
	{
		m_data.sourceBranch = std::move(sourceBranch);
		Q_EMIT modified();
	}

	QString MR::targetBranch() const
	{
		return m_data.targetBranch;
	}

	void MR::setTargetBranch(QString targetBranch)
	{
		m_data.targetBranch = std::move(targetBranch);
		Q_EMIT modified();
	}

	std::vector<Discussion> const &MR::discussions() const
	{
		return m_discussions;
	}

	QString MR::url() const
	{
		return m_data.url;
	}

	void MR::setUrl(QString url)
	{
		m_data.url = std::move(url);
		Q_EMIT modified();
	}

	QString MR::mergeStatus() const
	{
		return m_data.mergeStatus;
	}

	void MR::setMergeStatus(QString mergeStatus)
	{
		m_data.mergeStatus = std::move(mergeStatus);
		Q_EMIT modified();
	}

	bool MR::hasNotes() const
	{
		return m_data.hasNotes;
	}

	void MR::setHasNotes(bool hasNotes)
	{
		m_data.hasNotes = hasNotes;
		Q_EMIT modified();
	}

	QString MR::pipelineStatus() const
	{
		return m_pipelineStatus;
	}

	void MR::setPipelineStatus(QString status)
	{
		m_pipelineStatus = std::move(status);
		Q_EMIT modified();
	}

	std::vector<QString> const &MR::approvedBy() const
	{
		return m_approvedBy;
	}

	void MR::setApprovedBy(std::vector<QString> list)
	{
		m_approvedBy = std::move(list);
		Q_EMIT modified();
	}

	void MR::updateDiscussions(std::vector<Discussion> discussions)
	{
		m_discussions = std::move(discussions);
		Q_EMIT modified();
	}
} // namespace gpr::api
