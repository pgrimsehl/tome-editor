#ifndef REMOVEDFILESUFFIXFACET_H
#define REMOVEDFILESUFFIXFACET_H

#include <QObject>

#include "facet.h"

namespace Tome
{
    class FacetContext;

    /**
     * @brief Removes a suffix from all values of a custom File type (e.g. file extension).
     */
    class RemovedFileSuffixFacet : public QObject, public Facet
    {
            Q_OBJECT

        public:
            RemovedFileSuffixFacet();

            QWidget* createWidget(const FacetContext& context) const Q_DECL_OVERRIDE;
            const QVariant getDefaultValue() const Q_DECL_OVERRIDE;
            const QString getDescriptionForValue(const QVariant facetValue) const Q_DECL_OVERRIDE;
            const QString getDisplayName() const Q_DECL_OVERRIDE;
            const QString getKey() const Q_DECL_OVERRIDE;
            const QString getTargetType() const Q_DECL_OVERRIDE;
            const QVariant getWidgetValue(QWidget* widget) const Q_DECL_OVERRIDE;
            void setWidgetValue(QWidget* widget, const QVariant value) const Q_DECL_OVERRIDE;
            QString validateValue(const FacetContext& context, const QVariant value, const QVariant facetValue) const Q_DECL_OVERRIDE;

            /**
             * @brief Key of the restriction of the domain of the custom type used for serialization.
             */
            static const QString FacetKey;
    };
}

#endif // REMOVEDFILESUFFIXFACET_H
