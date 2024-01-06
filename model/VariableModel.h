#pragma once

#include <QAbstractTableModel>

#include "model/classes/GPClasses.h"

class VariableModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	enum Column
	{
		Key,
		Value,
		Used,
		Count
	};

	VariableModel(QObject *parent = nullptr);

	void clear();
	std::vector<gpr::Variable> variables() const { return m_variables; }
	void setVariables(std::vector<gpr::Variable> variables);
	void addVariable(gpr::Variable variable);

	int rowCount(QModelIndex const & = {}) const override;
	int columnCount(QModelIndex const & = {}) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	QVariant data(QModelIndex const &index, int role = Qt::ItemDataRole::DisplayRole) const override;
	bool setData(QModelIndex const &index, QVariant const &value, int role) override;
	bool removeRows(int firstRow, int count, QModelIndex const & = {}) override;
	Qt::ItemFlags flags(QModelIndex const &idx) const override;

private:
	QVariant displayRole(QModelIndex const &idx) const;
	QVariant editRole(QModelIndex const &idx) const;

	std::vector<gpr::Variable> m_variables;
};
