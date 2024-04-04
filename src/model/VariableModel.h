#pragma once

#include <QAbstractTableModel>
#include <QQmlEngine>

#include "model/classes/GPClasses.h"

class VariableModel : public QAbstractTableModel
{
	Q_OBJECT
	QML_ELEMENT
public:
	enum Column
	{
		Key,
		Value,
		Used,
		Count
	};
	Q_ENUMS(Column)

	VariableModel(QObject *parent = nullptr);

	Q_INVOKABLE void clear();
	Q_INVOKABLE void addVariable(QString key, QString value, bool used);
	Q_INVOKABLE QList<gpr::Variable> variables() const { return m_variables; }
	Q_INVOKABLE void setVariables(QList<gpr::Variable> variables);

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

	QList<gpr::Variable> m_variables;
};
