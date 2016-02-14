#ifndef LISTWINDOW_H
#define LISTWINDOW_H

#include <QDialog>

#include "../Controller/typescontroller.h"


namespace Ui {
    class ListWindow;
}

class ListWindow : public QDialog
{
        Q_OBJECT

    public:
        explicit ListWindow(Tome::TypesController& typesController, QWidget *parent = 0);
        ~ListWindow();

        QString getListName() const;
        QString getListItemType() const;

        void init();

        void setListName(const QString& listName);
        void setListItemType(const QString& itemType);

    public slots:
        void accept();

    private:
        Ui::ListWindow *ui;
        Tome::TypesController& typesController;

        bool validate();
};

#endif // LISTWINDOW_H
