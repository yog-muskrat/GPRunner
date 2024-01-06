#ifdef _DEBUG
#include <QAbstractItemModelTester>
#endif

#include "model/VariableModel.h"

VariableModel::VariableModel(QObject *parent)
	: QAbstractTableModel(parent)
{
#ifdef _DEBUG
	new QAbstractItemModelTester(this, this);
#endif
}

void VariableModel::clear()
{
	beginResetModel();
	m_variables.clear();
	endResetModel();
}

void VariableModel::setVariables(std::vector<gpr::Variable> variables)
{
	beginResetModel();
	m_variables = std::move(variables);
	endResetModel();
}

void VariableModel::addVariable(gpr::Variable variable)
{
	auto const row = rowCount();
	beginInsertRows({}, row, row);
	m_variables.push_back(std::move(variable));
	endInsertRows();
}

int VariableModel::rowCount(QModelIndex const &) const
{
	return std::ranges::ssize(m_variables);
}

int VariableModel::columnCount(QModelIndex const &) const
{
	return Column::Count;
}

QVariant VariableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		switch (section)
		{
			case Column::Key: return "Key";
			case Column::Value: return "Value";
			case Column::Used: return "Used";
			default: break;
		}
	}

	return QAbstractTableModel::headerData(section, orientation, role);
}

QVariant VariableModel::data(QModelIndex const &index, int role) const
{
	if (index.row() < 0 || index.row() >= rowCount()) return {};

	switch (role)
	{
		case Qt::DisplayRole: return displayRole(index);
		case Qt::EditRole: return editRole(index);
		default: break;
	}
	return {};
}

bool VariableModel::setData(QModelIndex const &index, QVariant const &value, int role)
{
	if (index.row() < 0 || index.row() >= rowCount() || index.column() < 0 || index.column() >= columnCount()) return false;

	auto &variable = m_variables[index.row()];

	if (role == Qt::EditRole)
	{
		if (index.column() == Column::Key) variable.key = value.toString();
		if (index.column() == Column::Value) variable.value = value.toString();
		if (index.column() == Column::Used) variable.used = value.toBool();

		Q_EMIT dataChanged(index, index);
	}

	return QAbstractTableModel::setData(index, value, role);
}

bool VariableModel::removeRows(int firstRow, int count, QModelIndex const &parent)
{
	beginRemoveRows(parent, firstRow, firstRow + count - 1);

	auto const first = std::next(m_variables.begin(), firstRow);
	auto const last = std::next(first, count);
	m_variables.erase(first, last);

	endRemoveRows();
	return true;
}

Qt::ItemFlags VariableModel::flags(QModelIndex const &) const
{
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QVariant VariableModel::displayRole(QModelIndex const &idx) const
{
	auto const &var = m_variables[idx.row()];

	switch (idx.column())
	{
		case Column::Key: return var.key;
		case Column::Value: return var.value;
		case Column::Used: return var.used ? QString{"✓"} : QString{};
		default: break;
	}
	return {};
}

QVariant VariableModel::editRole(QModelIndex const &idx) const
{
	auto const &var = m_variables[idx.row()];

	switch (idx.column())
	{
		case Column::Key: return var.key;
		case Column::Value: return var.value;
		case Column::Used: return var.used;
		default: break;
	}
	return {};
}
