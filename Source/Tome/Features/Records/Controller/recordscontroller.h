#ifndef RECORDSCONTROLLER_H
#define RECORDSCONTROLLER_H

#include <QStringList>

#include "../Model/recordsetlist.h"


namespace Tome
{
    class FieldDefinitionsController;
    class TypesController;

    class RecordsController : public QObject
    {
        Q_OBJECT

        public:
            RecordsController(const FieldDefinitionsController& fieldDefinitionsController, const TypesController& typesController);

            const Record addRecord(const QString& id, const QString& displayName, const QStringList& fieldIds, const QString& recordSetName);
            void addRecordSet(const RecordSet& recordSet);

            const Record duplicateRecord(const QString& existingRecordId, const QString& newRecordId);

            /**
             * @brief getAncestors Gets the list of all ancestors of the record with the specified id, direct parent first.
             * @param id Id of the record to get the ancestors of.
             * @return List of all ancestors of the record with the specified id, direct parent first.
             */
            const RecordList getAncestors(const QString& id) const;

            const RecordList getChildren(const QString& id) const;
            const RecordList getDescendents(const QString& id) const;
            const QVariant getInheritedFieldValue(const QString& id, const QString& fieldId) const;
            const RecordFieldValueMap getInheritedFieldValues(const QString& id) const;
            const RecordSetList& getRecordSets() const;

            /**
             * @brief getRecord Returns the record with the specified id.
             * @param id Id of the record to get.
             * @return Record with the specified id.
             */
            const Record& getRecord(const QString& id) const;

            const RecordList getRecords() const;

            const QStringList getRecordIds() const;

            /**
             * @brief getRecordNames Returns the list of the names of all records of this project.
             * @return List containing the names of all records of this project.
             */
            const QStringList getRecordNames() const;

            const QStringList getRecordSetNames() const;

            /**
             * @brief getRecordFieldValues Returns the map of actual field values of the record with the specified id, including all inherited values.
             * @param id Id of the record the get the map of field values of.
             * @return Map of actual field values of the record with the specified id.
             */
            const RecordFieldValueMap getRecordFieldValues(const QString& id) const;

            bool hasRecord(const QString& id) const;
            int indexOf(const Record& record) const;

            /**
             * @brief isAncestorOf Checks whether the first record is an ancestor of the second one.
             * @param possibleAncestor Record to check if it is an ancestor.
             * @param recordId Record to check the ancestors of.
             * @return true, if the first record is an ancestor of the second one, and false otherwise.
             */
            bool isAncestorOf(const QString& possibleAncestor, const QString& recordId) const;

            void moveFieldToComponent(const QString& fieldId, const QString& oldComponent, const QString& newComponent);
            void moveRecordToSet(const QString& recordId, const QString& recordSetName);

            void removeRecord(const QString& recordId);
            void removeRecordField(const QString fieldId);
            void removeRecordField(const QString& recordId, const QString& fieldId);
            void removeRecordSet(const QString& name);

            void renameRecordField(const QString oldFieldId, const QString newFieldId);

            QVariant revertFieldValue(const QString& recordId, const QString& fieldId);
            void revertRecord(const QString& recordId);

            void reparentRecord(const QString& recordId, const QString& newParentId);
            void setRecordSets(RecordSetList& model);
            void updateRecord(const QString oldId, const QString newId, const QString newDisplayName, const QStringList& fieldIds, const QString& recordSetName);
            void updateRecordFieldValue(const QString& recordId, const QString& fieldId, const QVariant& fieldValue);

        signals:
            void progressChanged(const QString title, const QString text, const int currentValue, const int maximumValue);
            void recordAdded(const QString& recordId, const QString& recordDisplayName, const QString& parentId);
            void recordFieldsChanged(const QString& recordId);
            void recordRemoved(const QString& recordId);
            void recordUpdated(const QString& oldId, const QString& oldDisplayName, const QString& newId, const QString& newDisplayName);
            void recordSetsChanged();

        private:
            RecordSetList* model;

            const FieldDefinitionsController& fieldDefinitionsController;
            const TypesController& typesController;

            void addRecordField(const QString& recordId, const QString& fieldId);
            Record* getRecordById(const QString& id) const;
            void updateRecordReferences(const QString oldReference, const QString newReference);
    };
}

#endif // RECORDSCONTROLLER_H
