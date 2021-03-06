#include "removefielddefinitioncommand.h"

#include "../fielddefinitionscontroller.h"
#include "../../../Records/Controller/recordscontroller.h"

using namespace Tome;


RemoveFieldDefinitionCommand::RemoveFieldDefinitionCommand(FieldDefinitionsController& fieldDefinitionsController,
                                                           RecordsController& recordsController,
                                                           const QString& id)
    : fieldDefinitionsController(fieldDefinitionsController),
      recordsController(recordsController),
      id(id)
{
    this->setText(tr("Remove Field - %1").arg(id));
}

void RemoveFieldDefinitionCommand::undo()
{
    qInfo(qUtf8Printable(QString("Undo remove field %1.").arg(this->id)));

    // Add field definition again.
    this->fieldDefinitionsController.addFieldDefinition(this->id,
                                                        this->displayName,
                                                        this->fieldType,
                                                        this->defaultValue,
                                                        this->component,
                                                        this->description,
                                                        this->fieldDefinitionSetName);

    // Add record field values again.
    for (QList<QPair<QVariant, QVariant>>::iterator it = this->removedRecordFieldValues.begin();
         it != this->removedRecordFieldValues.end();
         ++it)
    {
        const QVariant recordId = (*it).first;
        const QVariant recordFieldValue = (*it).second;

        this->recordsController.updateRecordFieldValue(recordId, this->id, recordFieldValue);
    }
}

void RemoveFieldDefinitionCommand::redo()
{
    // Store current data.
    const FieldDefinition& fieldDefinition = this->fieldDefinitionsController.getFieldDefinition(this->id);

    this->displayName = fieldDefinition.displayName;
    this->fieldType = fieldDefinition.fieldType;
    this->defaultValue = fieldDefinition.defaultValue;
    this->component = fieldDefinition.component;
    this->description = fieldDefinition.description;
    this->fieldDefinitionSetName = fieldDefinition.fieldDefinitionSetName;

    // Store record field values.
    const RecordList records = this->recordsController.getRecords();

    for (int i = 0; i < records.size(); ++i)
    {
        const Record& record = records[i];
        if (record.fieldValues.contains(this->id))
        {
            const QVariant recordFieldValue = record.fieldValues[this->id];
            this->removedRecordFieldValues << QPair<QVariant, QVariant>(record.id, recordFieldValue);
        }
    }

    // Remove field definition.
    this->fieldDefinitionsController.removeFieldDefinition(this->id);
}
