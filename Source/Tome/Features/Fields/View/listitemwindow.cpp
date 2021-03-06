#include "listitemwindow.h"
#include "ui_listitemwindow.h"

#include <QPushButton>
#include <QMessageBox>

#include "fieldvaluewidget.h"
#include "../../Facets/Controller/facetscontroller.h"
#include "../../Records/Controller/recordscontroller.h"
#include "../../Types/Controller/typescontroller.h"


using namespace Tome;


ListItemWindow::ListItemWindow(Tome::FacetsController& facetsController,
                               ProjectController& projectController,
                               Tome::RecordsController& recordsController,
                               Tome::TypesController& typesController,
                               QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ListItemWindow),
    facetsController(facetsController),
    projectController(projectController),
    recordsController(recordsController),
    typesController(typesController)
{
    ui->setupUi(this);

    // Add widget for specifying the list item value.
    this->fieldValueWidget = new FieldValueWidget(this->facetsController, this->projectController, this->recordsController, this->typesController, this);
    QFormLayout* layout = static_cast<QFormLayout*>(this->layout());
    layout->insertRow(0, tr("Value:"), this->fieldValueWidget);
}

ListItemWindow::~ListItemWindow()
{
    delete this->ui;

    delete this->fieldValueWidget;
}

QVariant ListItemWindow::getValue() const
{
    return this->fieldValueWidget->getFieldValue();
}

void ListItemWindow::setValue(const QVariant& value)
{
    this->fieldValueWidget->setFieldValue(value);
}

void ListItemWindow::setFieldType(const QString& fieldType) const
{
    this->fieldValueWidget->setFieldType(fieldType);
}

void ListItemWindow::accept()
{
    // Validate data.
    if (this->validate())
    {
        this->done(Accepted);
    }
}

void ListItemWindow::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);

    // Set OK as default button (instead of Cancel).
    QPushButton* okButton = this->ui->buttonBox->button(QDialogButtonBox::Ok);
    okButton->setAutoDefault(true);
    okButton->setDefault(true);
}

bool ListItemWindow::validate()
{
    // Value must be valid.
    QString validationError = this->fieldValueWidget->validate();

    if (!validationError.isEmpty())
    {
        QMessageBox::information(
                    this,
                    tr("Invalid data"),
                    validationError,
                    QMessageBox::Close,
                    QMessageBox::Close);
        return false;
    }

    return true;
}
