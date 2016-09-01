#include "searchresultsdockwidget.h"

#include <QHeaderView>

#include "../Model/searchresult.h"


using namespace Tome;


SearchResultsDockWidget::SearchResultsDockWidget(QWidget* parent) :
    QDockWidget(tr("Search Results"), parent)
{
    this->widget = new QWidget(this);
    this->verticalLayout = new QVBoxLayout();

    this->tableWidget = new QTableWidget();
    this->tableWidget->setEditTriggers(QTableWidget::NoEditTriggers);
    this->tableWidget->setSelectionMode(QTableWidget::SingleSelection);
    this->tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
    this->tableWidget->horizontalHeader()->setStretchLastSection(true);
    this->tableWidget->horizontalHeader()->setVisible(false);
    this->tableWidget->verticalHeader()->setVisible(false);
    this->tableWidget->setSortingEnabled(true);

    this->verticalLayout->addWidget(this->tableWidget);

    // Finish layout.
    this->widget->setLayout(this->verticalLayout);
    this->setWidget(this->widget);
}

SearchResultsDockWidget::~SearchResultsDockWidget()
{
    delete this->tableWidget;
    delete this->verticalLayout;
    delete this->widget;
}

void SearchResultsDockWidget::showResults(const QString& title, const SearchResultList& results)
{
    this->setWindowTitle("Search Results - " + title + " - " + QString::number(results.size()) + " matches found");
    this->results = results;

    // Reset output window.
    this->tableWidget->setRowCount(this->results.count());
    this->tableWidget->setColumnCount(1);

    // Show results.
    for (int i = 0; i < this->results.count(); ++i)
    {
        const SearchResult& result = this->results.at(i);
        const QString resultString = result.targetSiteId + ": " + result.content;
        this->tableWidget->setItem(i, 0, new QTableWidgetItem(resultString));
    }
}