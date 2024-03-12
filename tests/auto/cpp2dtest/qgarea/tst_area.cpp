// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtGraphs/QAreaSeries>
#include <QtGraphs/QLineSeries>
#include <QtGraphs/QValueAxis>
#include <QtQml/QQmlComponent>
#include <QtTest/QtTest>

class tst_area : public QObject
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
    QAreaSeries *m_series;
};

void tst_area::initTestCase() {}

void tst_area::cleanupTestCase() {}

void tst_area::init()
{
    m_series = new QAreaSeries();
}

void tst_area::cleanup()
{
    delete m_series;
}

void tst_area::construct()
{
    QAreaSeries *series = new QAreaSeries();
    QVERIFY(series);
    delete series;
}

void tst_area::initialProperties()
{
    QVERIFY(m_series);

    // Properties from QAreaSeries
    QCOMPARE(m_series->axisX(), nullptr);
    QCOMPARE(m_series->axisY(), nullptr);
    QCOMPARE(m_series->color(), QColor(Qt::transparent));
    QCOMPARE(m_series->selectedColor(), QColor(Qt::transparent));
    QCOMPARE(m_series->borderColor(), QColor(Qt::transparent));
    QCOMPARE(m_series->selectedBorderColor(), QColor(Qt::transparent));
    QCOMPARE(m_series->borderWidth(), 1.0);
    QCOMPARE(m_series->selected(), false);
    QCOMPARE(m_series->upperSeries(), nullptr);
    QCOMPARE(m_series->lowerSeries(), nullptr);

    // Properties from QAbstractSeries
    QCOMPARE(m_series->theme(), nullptr);
    QCOMPARE(m_series->name(), "");
    QCOMPARE(m_series->isVisible(), true);
    QCOMPARE(m_series->selectable(), false);
    QCOMPARE(m_series->hoverable(), false);
    QCOMPARE(m_series->opacity(), 1.0);
    QCOMPARE(m_series->valuesMultiplier(), 1.0);
}

void tst_area::initializeProperties()
{
    QVERIFY(m_series);

    auto axisX = new QValueAxis(this);
    auto axisY = new QValueAxis(this);
    auto theme = new QSeriesTheme(this);
    auto upperSeries = new QLineSeries(this);
    auto lowerSeries = new QLineSeries(this);

    m_series->setAxisX(axisX);
    m_series->setAxisY(axisY);

    m_series->setColor("#ff0000");
    m_series->setSelectedColor("#0000ff");
    m_series->setBorderColor("#ff0000");
    m_series->setSelectedBorderColor("#0000ff");
    m_series->setBorderWidth(2.0);
    m_series->setSelected(true);
    m_series->setUpperSeries(upperSeries);
    m_series->setLowerSeries(lowerSeries);

    m_series->setTheme(theme);
    m_series->setName("AreaSeries");
    m_series->setVisible(false);
    m_series->setSelectable(true);
    m_series->setHoverable(true);
    m_series->setOpacity(0.5);
    m_series->setValuesMultiplier(0.5);

    QCOMPARE(m_series->axisX(), axisX);
    QCOMPARE(m_series->axisY(), axisY);

    QCOMPARE(m_series->color(), "#ff0000");
    QCOMPARE(m_series->selectedColor(), "#0000ff");
    QCOMPARE(m_series->borderColor(), "#ff0000");
    QCOMPARE(m_series->selectedBorderColor(), "#0000ff");
    QCOMPARE(m_series->borderWidth(), 2.0);
    QCOMPARE(m_series->selected(), true);
    QCOMPARE(m_series->upperSeries(), upperSeries);
    QCOMPARE(m_series->lowerSeries(), lowerSeries);

    QCOMPARE(m_series->theme(), theme);
    QCOMPARE(m_series->name(), "AreaSeries");
    QCOMPARE(m_series->isVisible(), false);
    QCOMPARE(m_series->selectable(), true);
    QCOMPARE(m_series->hoverable(), true);
    QCOMPARE(m_series->opacity(), 0.5);
    QCOMPARE(m_series->valuesMultiplier(), 0.5);
}

void tst_area::invalidProperties()
{
    QVERIFY(m_series);

    m_series->setValuesMultiplier(2.0); // range 0...1
    QCOMPARE(m_series->valuesMultiplier(), 1.0);

    m_series->setValuesMultiplier(-1.0); // range 0...1
    QCOMPARE(m_series->valuesMultiplier(), 0.0);
}

QTEST_MAIN(tst_area)
#include "tst_area.moc"
