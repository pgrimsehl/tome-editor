#ifndef FIELDTYPEDOESNOTEXISTTASK_H
#define FIELDTYPEDOESNOTEXISTTASK_H

#include "../../Tasks/Controller/task.h"

namespace Tome
{
    class TaskContext;

    class FieldTypeDoesNotExistTask : public Task
    {
        public:
            FieldTypeDoesNotExistTask();

            const QString getId() const;
            const MessageList execute(const TaskContext& context) const;
    };
}

#endif // FIELDTYPEDOESNOTEXISTTASK_H
