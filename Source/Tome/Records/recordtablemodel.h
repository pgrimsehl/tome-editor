#ifndef RECORDTABLEMODEL_H
#define RECORDTABLEMODEL_H

#include <QAbstractTableModel>

#include "../Fields/fieldtype.h"
#include "../Projects/project.h"


namespace Tome
{
    class RecordTableModel : public QAbstractTableModel
    {
            Q_OBJECT

        public:
            RecordTableModel(QObject *parent);

            int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE ;
            int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
            QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
            QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

            bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex()) Q_DECL_OVERRIDE;
            bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex()) Q_DECL_OVERRIDE;

            void addRecordField(const QString& id, const QString& value);
            void removeRecordField(const QString& id);

            QString getFieldId(const QModelIndex &index) const;
            QString getFieldValue(const QModelIndex &index) const;

            void clearRecord();
            void setFieldValue(const QString& fieldId, const QString& fieldValue);
            void setProject(QSharedPointer<Tome::Project> project);
            void setRecord(QSharedPointer<Tome::Record> record);

        private:
            QSharedPointer<Project> project;
            QSharedPointer<Record> record;
    };
}

#endif // RECORDTABLEMODEL_H