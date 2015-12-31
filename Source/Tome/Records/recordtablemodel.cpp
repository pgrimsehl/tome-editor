#include "recordtablemodel.h"

#include <QColor>

using namespace Tome;


RecordTableModel::RecordTableModel(QObject* parent)
    : QAbstractTableModel(parent),
      record(0)
{
}

int RecordTableModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return this->record != 0 ? this->record->fieldValues.count() : 0;
}

int RecordTableModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 2;
}

QVariant RecordTableModel::data(const QModelIndex& index, int role) const
{
    // Validate index.
    if (!index.isValid())
    {
        return QVariant();
    }

    if (index.row() >= this->rowCount())
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
            case 0:
                return this->getFieldId(index);
            case 1:
                return this->getFieldValue(index);
        }

        return QVariant();
    }
    else if (role == Qt::ToolTipRole)
    {
        // Show field description as tooltip.
        QString fieldId = this->getFieldId(index);
        QSharedPointer<FieldDefinition> fieldDefinition = this->project->getFieldDefinition(fieldId);
        return fieldDefinition->description;
    }
    else if (role == Qt::DecorationRole && index.column() == 1)
    {
        QString fieldId = this->getFieldId(index);
        QSharedPointer<FieldDefinition> fieldDefinition = this->project->getFieldDefinition(fieldId);

        if (fieldDefinition->fieldType == FieldType::Color)
        {
            // Show color preview.
            QColor color;
            color.setNamedColor(this->getFieldValue(index));
            return color;
        }
    }

    return QVariant();
}

QVariant RecordTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return QString("Field");
            case 1:
                return QString("Value");
            }
        }
    }
    return QVariant();
}

bool RecordTableModel::insertRows(int position, int rows, const QModelIndex& index)
{
    Q_UNUSED(index);

    if (rows <= 0)
    {
        return true;
    }

    beginInsertRows(QModelIndex(), position, position + rows - 1);
    endInsertRows();
    return true;
}

bool RecordTableModel::removeRows(int position, int rows, const QModelIndex& index)
{
    Q_UNUSED(index);

    if (rows <= 0)
    {
        return true;
    }

    beginRemoveRows(QModelIndex(), position, position + rows - 1);
    endRemoveRows();
    return true;
}

void RecordTableModel::addRecordField(const QString& id, const QString& value)
{
    // Update view.
    this->insertRows(this->rowCount(), 1);

    // Update model.
    this->record->fieldValues.insert(id, value);
}

void RecordTableModel::removeRecordField(const QString& id)
{
    // Update view.
    this->removeRows(this->rowCount() - 1, 1);

    // Update model.
    this->record->fieldValues.remove(id);
}

QString RecordTableModel::getFieldId(const QModelIndex& index) const
{
    return this->record->fieldValues.keys()[index.row()];
}

QString RecordTableModel::getFieldValue(const QModelIndex& index) const
{
    return this->record->fieldValues.values()[index.row()];
}

void RecordTableModel::clearRecord()
{
    this->setRecord(QSharedPointer<Record>(0));
}

void RecordTableModel::setFieldValue(const QString& fieldId, const QString& fieldValue)
{
    // Update model.
    this->record->fieldValues[fieldId] = fieldValue;

    // Update view.
    int row = this->record->fieldValues.keys().indexOf(fieldId);
    QModelIndex cell = this->index(row, 1, QModelIndex());

    emit(dataChanged(cell, cell));
}

void RecordTableModel::setProject(QSharedPointer<Project> project)
{
    this->project = project;
}

void RecordTableModel::setRecord(QSharedPointer<Record> record)
{
    this->removeRows(0, this->rowCount());
    this->record = record;
    this->insertRows(0, this->rowCount());
}