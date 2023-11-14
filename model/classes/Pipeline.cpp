#include "model/classes/Pipeline.h"
#include "Pipeline.h"

namespace gpr::api
{
	Pipeline::Pipeline(Data data, QObject *parent)
		: QObject(parent)
		, m_data{std::move(data)}
	{}
	int Pipeline::id() const
	{
		return m_data.id;
	}

	void Pipeline::update(Data data)
	{
		if (data.id != m_data.id)
		{
			assert(false && "Invalid pipeline data");
			return;
		}

		m_data = std::move(data);
		Q_EMIT modified();
	}

	QString Pipeline::status() const
	{
		return m_data.status;
	}

	void Pipeline::setStatus(QString status)
	{
		m_data.status = std::move(status);
		Q_EMIT modified();
	}

	QString Pipeline::source() const
	{
		return m_data.source;
	}

	void Pipeline::setSource(QString source)
	{
		m_data.source = std::move(source);
		Q_EMIT modified();
	}

	QString Pipeline::username() const
	{
		return m_data.username;
	}

	void Pipeline::setUsername(QString const &name)
	{
		m_data.username = name;
		Q_EMIT modified();
	}

	QString Pipeline::ref() const
	{
		return m_data.ref;
	}

	void Pipeline::setRef(QString ref)
	{
		m_data.ref = std::move(ref);
		Q_EMIT modified();
	}

	QDateTime Pipeline::createdAt() const
	{
		return m_data.created;
	}

	void Pipeline::setCreatedAt(QDateTime created)
	{
		m_data.created = created;
		Q_EMIT modified();
	}

	QDateTime Pipeline::updatedAt() const
	{
		return m_data.updated;
	}

	void Pipeline::setUpdatedAt(QDateTime updated)
	{
		m_data.updated = updated;
		Q_EMIT modified();
	}
} // namespace gpr::api
