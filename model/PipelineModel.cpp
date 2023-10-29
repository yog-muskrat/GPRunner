#include "PipelineModel.h"

void PipelineModel::clear()
{
	beginResetModel();
	m_pipelines.clear();
	endResetModel();
}

void PipelineModel::addPipeline(gpr::Pipeline pipeline)
{
	if (auto const pos = std::ranges::find(m_pipelines, pipeline.id, &gpr::Pipeline::id); pos != m_pipelines.cend())
	{
		if (*pos != pipeline)
		{
			auto const row = std::ranges::distance(m_pipelines.cbegin(), pos);
			*pos = std::move(pipeline);
			Q_EMIT dataChanged(index(row, 0), index(row, Column::Count - 1));
		}

		return;
	}

	beginResetModel();
	m_pipelines.push_back(std::move(pipeline));
	std::ranges::sort(m_pipelines, std::ranges::greater{}, &gpr::Pipeline::updated);
	endResetModel();
}

int PipelineModel::rowCount(QModelIndex const &) const
{
	return m_pipelines.size();
}

int PipelineModel::columnCount(QModelIndex const &) const
{
	return Column::Count;
}

QVariant PipelineModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Orientation::Horizontal || section < 0 || section >= Column::Count)
	{
		return QAbstractTableModel::headerData(section, orientation, role);
	}

	switch (section)
	{
		case Column::Id: return "ID";
		case Column::Ref: return "Ref";
		case Column::Source: return "Source";
		case Column::Status: return "Status";
		case Column::Created: return "Created";
		case Column::Updated: return "Updated";
		default: break;
	}
	return {};
}

QVariant PipelineModel::data(QModelIndex const &index, int role) const
{
	if (!checkIndex(index)) return {};

	auto const &pipeline = m_pipelines[index.row()];

	if (role == Qt::DisplayRole)
	{
		if (index.column() == Column::Id) return pipeline.id;
		if (index.column() == Column::Ref) return pipeline.ref;
		if (index.column() == Column::Status) return pipeline.status;
		if (index.column() == Column::Source) return pipeline.source;
		if (index.column() == Column::Created)
		{
			auto const format = pipeline.created.date() == QDate::currentDate() ? "hh:mm" : "dd.MM.yyyy hh:mm";

			return pipeline.created.toLocalTime().toString(format);
		}
		if (index.column() == Column::Updated)
		{
			auto const format = pipeline.updated.date() == QDate::currentDate() ? "hh:mm" : "dd.MM.yyyy hh:mm";

			return pipeline.updated.toLocalTime().toString(format);
		}
	}

	if (role == Role::PipelineIdRole) return pipeline.id;
	if (role == Role::PipelineStatusRole) return pipeline.status;
	if (role == Role::PipelineSourceRole) return pipeline.source;

	return QVariant();
}

QHash<int, QByteArray> PipelineModel::roleNames() const
{
	auto names = QAbstractTableModel::roleNames();
	names.insert(Role::PipelineIdRole, "pipelineId");
	names.insert(Role::PipelineSourceRole, "pipelineSource");
	names.insert(Role::PipelineStatusRole, "pipelineStatus");

	return names;
}
