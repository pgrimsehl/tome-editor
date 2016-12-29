#ifndef UPDATERECORDFIELDVALUECOMMAND_H
#define UPDATERECORDFIELDVALUECOMMAND_H

#include <QUndoCommand>
#include <QVariant>

namespace Tome
{
    class RecordsController;

    class UpdateRecordFieldValueCommand : public QUndoCommand, public QObject
    {
        public:
            UpdateRecordFieldValueCommand(RecordsController& recordsController,
                                          const QString& recordId,
                                          const QString& fieldId,
                                          const QVariant& newFieldValue);

            virtual void undo();
            virtual void redo();

        private:
            RecordsController& recordsController;

            const QString recordId;
            const QString fieldId;
            const QVariant newFieldValue;

            QVariant oldFieldValue;
    };
}

#endif // UPDATERECORDFIELDVALUECOMMAND_H