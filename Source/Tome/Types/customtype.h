#ifndef CUSTOMFIELDTYPE_H
#define CUSTOMFIELDTYPE_H

#include <QMap>
#include <QString>
#include <QStringList>

namespace Tome
{
    class CustomType
    {
        public:
            CustomType();

            QString name;
            QString baseType;
            QMap<QString, QString> restrictions;

            /**
             * @brief getEnumeration Convenience function for retrieving the Enumeration restriction of this type, if available. Returns an empty list, if not.
             * @return Enumeration of allowed field values, if available, and an empty list otherwise.
             */
            QStringList getEnumeration();

            /**
             * @brief setEnumeration Sets the Enumeration restriction of this type.
             * @param enumeration Allowed field values.
             */
            void setEnumeration(QStringList enumeration);

        private:
            static const QString RestrictionEnumeration;
    };
}

#endif // CUSTOMFIELDTYPE_H