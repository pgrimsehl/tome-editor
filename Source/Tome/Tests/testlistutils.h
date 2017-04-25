#ifndef TESTLISTUTILS_H
#define TESTLISTUTILS_H

#include <QtTest/QtTest>


class TestListUtils : public QObject
{
    Q_OBJECT

    private slots:
        void findInsertionIndexFirst();
        void findInsertionIndexMid();
        void findInsertionIndexLast();
};

#endif // TESTLISTUTILS_H
