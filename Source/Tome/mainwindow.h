#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSharedPointer>

#include "Fields/fielddefinitionswindow.h"
#include "Projects/newprojectwindow.h"
#include "Projects/project.h"
#include "Help/aboutwindow.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

    private slots:
        void on_actionAbout_triggered();
        void on_actionExit_triggered();
        void on_actionField_Definions_triggered();
        void on_actionNew_Project_triggered();
        void on_actionOpen_Project_triggered();
        void on_actionSave_Project_triggered();

    private:
        static const QString FieldDefinitionFileExtension;
        static const QString ProjectFileExtension;
        static const QString RecordFileExtension;

        Ui::MainWindow *ui;

        QSharedPointer<Tome::Project> project;

        AboutWindow *aboutWindow;
        FieldDefinitionsWindow* fieldDefinitionsWindow;
        NewProjectWindow *newProjectWindow;

        void createNewProject(const QString& name, const QString& path);
        void saveProject(QSharedPointer<Tome::Project> project);
        void setProject(QSharedPointer<Tome::Project> project);
        void showWindow(QWidget* widget);
};

#endif // MAINWINDOW_H
