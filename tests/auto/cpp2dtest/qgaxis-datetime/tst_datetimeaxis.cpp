// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtGraphs/QDateTimeAxis>
#include <QtTest/QtTest>
#include "qtestcase.h"

class tst_datetimeaxis : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void construct();

    void initialProperties();
    void initializeProperties();
    void invalidProperties();

private:
    QDateTimeAxis *m_axis;
};

void tst_datetimeaxis::initTestCase() {}

void tst_datetimeaxis::cleanupTestCase() {}

void tst_datetimeaxis::init()
{
    m_axis = new QDateTimeAxis();
}

void tst_datetimeaxis::cleanup()
{
    delete m_axis;
}

void tst_datetimeaxis::construct()
{
    QDateTimeAxis *axis = new QDateTimeAxis();
    QVERIFY(axis);
    delete axis;
}

void tst_datetimeaxis::initialProperties()
{
    QVERIFY(m_axis);

    QCOMPARE(m_axis->min(), QDateTime(QDate(1970, 1, 1), QTime::fromMSecsSinceStartOfDay(0)));
    QCOMPARE(m_axis->max(),
             QDateTime(QDate(1970, 1, 1), QTime::fromMSecsSinceStartOfDay(0)).addYears(10));
    QCOMPARE(m_axis->labelFormat(), "dd-MMMM-yy");
    QCOMPARE(m_axis->minorTickCount(), 0);
    QCOMPARE(m_axis->tickInterval(), 0.0);
}

void tst_datetimeaxis::initializeProperties()
{
    QVERIFY(m_axis);

    m_axis->setMin(QDateTime(QDate::currentDate(), QTime::fromMSecsSinceStartOfDay(0)));
    m_axis->setMax(QDateTime(QDate::currentDate(), QTime::fromMSecsSinceStartOfDay(0)).addYears(20));
    m_axis->setLabelFormat("yyyy");
    m_axis->setMinorTickCount(2);
    m_axis->setTickInterval(0.5);

    QCOMPARE(m_axis->min(), QDateTime(QDate::currentDate(), QTime::fromMSecsSinceStartOfDay(0)));
    QCOMPARE(m_axis->max(),
             QDateTime(QDate::currentDate(), QTime::fromMSecsSinceStartOfDay(0)).addYears(20));
    QCOMPARE(m_axis->labelFormat(), "yyyy");
    QCOMPARE(m_axis->minorTickCount(), 2);
    QCOMPARE(m_axis->tickInterval(), 0.5);
}

void tst_datetimeaxis::invalidProperties()
{
    QVERIFY(m_axis);

    m_axis->setMinorTickCount(-1);
    m_axis->setTickInterval(-1);

    QCOMPARE(m_axis->minorTickCount(), 0);
    QCOMPARE(m_axis->tickInterval(), 0);
}

QTEST_MAIN(tst_datetimeaxis)
#include "tst_datetimeaxis.moc"
