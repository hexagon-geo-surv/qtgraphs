// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtGraphsWidgets/q3dbarswidgetitem.h>
#include <QtGraphsWidgets/q3dsurfacewidgetitem.h>
#include <QtGraphs/qcategory3daxis.h>
#include <QtGraphs/qitemmodelbardataproxy.h>
#include <QtGraphs/qitemmodelsurfacedataproxy.h>
#include <QtGraphs/qvalue3daxis.h>
#include <QtGraphs/q3dscene.h>
#include <QtGraphs/qbar3dseries.h>

#include <QtWidgets/QApplication>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QTableWidget>
#include <QtGui/QScreen>
#include <QtCore/QTimer>
#include <QtCore/QRandomGenerator>
#include <QtCore/QDebug>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>

#define USE_STATIC_DATA

class GraphDataGenerator : public QObject
{
public:
    explicit GraphDataGenerator(Q3DBarsWidgetItem *bargraph, Q3DSurfaceWidgetItem *surfaceGraph,
                                QTableWidget *tableWidget);
    ~GraphDataGenerator();

    void setupModel();
    void addRow();
    void start();
    void selectFromTable(const QPoint &selection);
    void selectedFromTable(int currentRow, int currentColumn, int previousRow, int previousColumn);
    void fixTableSize();
    void changeSelectedButtonClicked();

private:
    Q3DBarsWidgetItem *m_barGraph;
    Q3DSurfaceWidgetItem *m_surfaceGraph;
    QTimer *m_dataTimer;
    int m_columnCount;
    int m_rowCount;
    QTableWidget *m_tableWidget; // not owned
};

GraphDataGenerator::GraphDataGenerator(Q3DBarsWidgetItem *bargraph,
                                       Q3DSurfaceWidgetItem *surfaceGraph,
                                       QTableWidget *tableWidget)
    : m_barGraph(bargraph)
    , m_surfaceGraph(surfaceGraph)
    , m_dataTimer(0)
    , m_columnCount(100)
    , m_rowCount(50)
    , m_tableWidget(tableWidget)
{
    // Set up bar specifications; make the bars as wide as they are deep,
    // and add a small space between them
    m_barGraph->setBarThickness(1.0f);
    m_barGraph->setBarSpacing(QSizeF(0.2, 0.2));

#ifndef USE_STATIC_DATA
    // Set up sample space; make it as deep as it's wide
    m_barGraph->rowAxis()->setRange(0, m_rowCount);
    m_barGraph->columnAxis()->setRange(0, m_columnCount);
    m_tableWidget->setColumnCount(m_columnCount);

    // Set selection mode to full
    m_barGraph->setSelectionMode(QGraphs3DNamespace::SelectionFlag::ItemRowAndColumn);

    // Hide axis labels by explicitly setting one empty string as label list
    m_barGraph->rowAxis()->setLabels(QStringList(QString()));
    m_barGraph->columnAxis()->setLabels(QStringList(QString()));

    m_barGraph->seriesList().at(0)->setItemLabelFormat(QStringLiteral("@valueLabel"));
#else
    // Set selection mode to slice row
    m_barGraph->setSelectionMode(QtGraphs3D::SelectionFlag::ItemAndRow
                                 | QtGraphs3D::SelectionFlag::Slice);
    m_surfaceGraph->setSelectionMode(QtGraphs3D::SelectionFlag::ItemAndRow
                                     | QtGraphs3D::SelectionFlag::Slice);
#endif
}

GraphDataGenerator::~GraphDataGenerator()
{
    if (m_dataTimer) {
        m_dataTimer->stop();
        delete m_dataTimer;
    }
    delete m_barGraph;
    delete m_surfaceGraph;
}

void GraphDataGenerator::start()
{
#ifndef USE_STATIC_DATA
    m_dataTimer = new QTimer();
    m_dataTimer->setTimerType(Qt::CoarseTimer);
    QObject::connect(m_dataTimer, &QTimer::timeout, this, &GraphDataGenerator::addRow);
    m_dataTimer->start(0);
    m_tableWidget->setFixedWidth(m_graph->width());
#else
    setupModel();

    // Table needs to be shown before the size of its headers can be accurately obtained,
    // so we postpone it a bit
    m_dataTimer = new QTimer();
    m_dataTimer->setSingleShot(true);
    QObject::connect(m_dataTimer, &QTimer::timeout, this, &GraphDataGenerator::fixTableSize);
    m_dataTimer->start(0);
#endif
}

void GraphDataGenerator::setupModel()
{
    // Set up row and column names
    QStringList days;
    days << "Monday" << "Tuesday" << "Wednesday" << "Thursday" << "Friday" << "Saturday" << "Sunday";
    QStringList weeks;
    weeks << "week 1" << "week 2" << "week 3" << "week 4" << "week 5";

    // Set up data
    const char *hours[5][7] =
            //    Mon            Tue            Wed            Thu            Fri            Sat            Sun
    {{"9/10/2.0/30", "9/11/1.0/30", "9/12/3.0/30", "9/13/0.2/30", "9/14/1.0/30", "9/15/5.0/30", "9/16/10.0/30"},     // week 1
    {"8/10/0.5/45", "8/11/1.0/45", "8/12/3.0/45", "8/13/1.0/45", "8/14/2.0/45", "8/15/2.0/45", "8/16/3.0/45"},      // week 2
    {"7/10/1.0/60", "7/11/1.0/60", "7/12/2.0/60", "7/13/1.0/60", "7/14/4.0/60", "7/15/4.0/60", "7/16/4.0/60"},      // week 3
    {"6/10/0.0/75", "6/11/1.0/75", "6/12/0.0/75", "6/13/0.0/75", "6/14/2.0/75", "6/15/2.0/75", "6/16/0.3/75"},      // week 4
    {"5/10/3.0/90", "5/11/3.0/90", "5/12/6.0/90", "5/13/2.0/90", "5/14/2.0/90", "5/15/1.0/90", "5/16/1.0/90"}};     // week 5

    // Add labels
    m_barGraph->rowAxis()->setTitle("Week of year");
    m_barGraph->columnAxis()->setTitle("Day of week");
    m_barGraph->valueAxis()->setTitle("Hours spent on the Internet");
    m_barGraph->valueAxis()->setLabelFormat("%.1f h");

    m_surfaceGraph->axisZ()->setTitle("Week of year");
    m_surfaceGraph->axisX()->setTitle("Day of week");
    m_surfaceGraph->axisY()->setTitle("Hours spent on the Internet");
    m_surfaceGraph->axisY()->setLabelFormat("%.1f h");

    m_tableWidget->setRowCount(5);
    m_tableWidget->setColumnCount(7);
    m_tableWidget->setHorizontalHeaderLabels(days);
    m_tableWidget->setVerticalHeaderLabels(weeks);
    m_tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_tableWidget->setCurrentCell(-1, -1);

    for (int week = 0; week < weeks.size(); week++) {
        for (int day = 0; day < days.size(); day++) {
            QModelIndex index = m_tableWidget->model()->index(week, day);
            m_tableWidget->model()->setData(index, hours[week][day]);
        }
    }
}

void GraphDataGenerator::addRow()
{
    m_tableWidget->model()->insertRow(0);
    if (m_tableWidget->model()->rowCount() > m_rowCount)
        m_tableWidget->model()->removeRow(m_rowCount);
    for (int i = 0; i < m_columnCount; i++) {
        QModelIndex index = m_tableWidget->model()->index(0, i);
        m_tableWidget->model()->setData(index,
                                        ((float)i / (float)m_columnCount) / 2.0f
                                        + QRandomGenerator::global()->bounded(30.0 / 100.0f));
    }
    m_tableWidget->resizeColumnsToContents();
}

void GraphDataGenerator::selectFromTable(const QPoint &selection)
{
    m_tableWidget->setFocus();
    m_tableWidget->setCurrentCell(selection.x(), selection.y());
}

void GraphDataGenerator::selectedFromTable(int currentRow, int currentColumn,
                                           int previousRow, int previousColumn)
{
    Q_UNUSED(previousRow);
    Q_UNUSED(previousColumn);
    m_barGraph->seriesList().at(0)->setSelectedBar(QPoint(currentRow, currentColumn));
    m_surfaceGraph->seriesList().at(0)->setSelectedPoint(QPoint(currentRow, currentColumn));
}

void GraphDataGenerator::fixTableSize()
{
    int width = m_tableWidget->horizontalHeader()->length();
    width += m_tableWidget->verticalHeader()->width();
    m_tableWidget->setFixedWidth(width + 2);
    int height = m_tableWidget->verticalHeader()->length();
    height += m_tableWidget->horizontalHeader()->height();
    m_tableWidget->setFixedHeight(height + 2);
}

void GraphDataGenerator::changeSelectedButtonClicked()
{
    // Change all selected cells to a random value 1-10
    QVariant value = QVariant::fromValue(QRandomGenerator::global()->bounded(10.0) + 1);
    const auto selectedItems = m_tableWidget->selectedItems();
    for (const auto &item : selectedItems) {
        QString oldData = item->data(Qt::DisplayRole).toString();
        item->setData(Qt::DisplayRole,
                      oldData.left(5)
                      .append(QString::number(value.toReal()))
                      .append("/")
                      .append(QString::number(value.toReal() * 10)));
    }
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QQuickWidget *barQuickWidget = new QQuickWidget;
    QQuickWidget *surfaceQuickWidget = new QQuickWidget;
    Q3DBarsWidgetItem *barGraph = new Q3DBarsWidgetItem();
    Q3DSurfaceWidgetItem *surfaceGraph = new Q3DSurfaceWidgetItem();
    barGraph->setWidget(barQuickWidget);
    surfaceGraph->setWidget(surfaceQuickWidget);
    QSize screenSize = barGraph->widget()->screen()->size();

    barGraph->widget()->setMinimumSize(QSize(screenSize.width() / 4, screenSize.height() / 4));
    barGraph->widget()->setMaximumSize(screenSize);
    barGraph->widget()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    barGraph->widget()->setFocusPolicy(Qt::StrongFocus);
    barGraph->activeTheme()->setTheme(QGraphsTheme::Theme::QtGreen);
    surfaceGraph->widget()->setMinimumSize(QSize(screenSize.width() / 4, screenSize.height() / 4));
    surfaceGraph->widget()->setMaximumSize(screenSize);
    surfaceGraph->widget()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    surfaceGraph->widget()->setFocusPolicy(Qt::StrongFocus);
    surfaceGraph->activeTheme()->setTheme(QGraphsTheme::Theme::QtGreenNeon);

    QWidget widget;
    QVBoxLayout *mainLayout = new QVBoxLayout(&widget);
    QHBoxLayout *graphLayout = new QHBoxLayout();
    QVBoxLayout *buttonLayout = new QVBoxLayout();
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    QTableWidget *tableWidget = new QTableWidget(&widget);
    QPushButton *changeSelectedButton = new QPushButton(&widget);
    changeSelectedButton->setText(QStringLiteral("Change Selected"));

    buttonLayout->addWidget(changeSelectedButton);
    graphLayout->addWidget(barGraph->widget(), 1);
    graphLayout->addWidget(surfaceGraph->widget(), 1);
    bottomLayout->addLayout(buttonLayout);
    bottomLayout->addWidget(tableWidget);
    mainLayout->addLayout(graphLayout);
    mainLayout->addLayout(bottomLayout);

    tableWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    tableWidget->setAlternatingRowColors(true);
    widget.setWindowTitle(QStringLiteral("Hours spent on the Internet"));

    // Since we are dealing with QTableWidget, the model will already have data sorted properly
    // into rows and columns, so we simply set useModelCategories property to true to utilize this.
    QItemModelBarDataProxy *barProxy = new QItemModelBarDataProxy(tableWidget->model());
    QItemModelSurfaceDataProxy *surfaceProxy = new QItemModelSurfaceDataProxy(tableWidget->model());
    barProxy->setUseModelCategories(true);
    surfaceProxy->setUseModelCategories(true);
    barProxy->setRotationRole(tableWidget->model()->roleNames().value(Qt::DisplayRole));
    barProxy->setValueRolePattern(QRegularExpression(QStringLiteral("^(\\d*)(\\/)(\\d*)\\/(\\d*[\\.\\,]?\\d*)\\/\\d*[\\.\\,]?\\d*$")));
    barProxy->setRotationRolePattern(QRegularExpression(QStringLiteral("^(\\d*)(\\/)\\d*\\/\\d*([\\.\\,]?)\\d*(\\/)(\\d*[\\.\\,]?\\d*)$")));
    barProxy->setValueRoleReplace(QStringLiteral("\\4"));
    barProxy->setRotationRoleReplace(QStringLiteral("\\5"));
    surfaceProxy->setXPosRole(tableWidget->model()->roleNames().value(Qt::DisplayRole));
    surfaceProxy->setZPosRole(tableWidget->model()->roleNames().value(Qt::DisplayRole));
    surfaceProxy->setXPosRolePattern(QRegularExpression(QStringLiteral("^(\\d*)\\/(\\d*)\\/\\d*[\\.\\,]?\\d*\\/\\d*[\\.\\,]?\\d*$")));
    surfaceProxy->setXPosRoleReplace(QStringLiteral("\\2"));
    surfaceProxy->setYPosRolePattern(QRegularExpression(QStringLiteral("^\\d*(\\/)(\\d*)\\/(\\d*[\\.\\,]?\\d*)\\/\\d*[\\.\\,]?\\d*$")));
    surfaceProxy->setYPosRoleReplace(QStringLiteral("\\3"));
    surfaceProxy->setZPosRolePattern(QRegularExpression(QStringLiteral("^(\\d*)(\\/)(\\d*)\\/\\d*[\\.\\,]?\\d*\\/\\d*[\\.\\,]?\\d*$")));
    surfaceProxy->setZPosRoleReplace(QStringLiteral("\\1"));
    QBar3DSeries *barSeries = new QBar3DSeries(barProxy);
    QSurface3DSeries *surfaceSeries = new QSurface3DSeries(surfaceProxy);
    barSeries->setMesh(QAbstract3DSeries::Mesh::Pyramid);
    barGraph->addSeries(barSeries);
    surfaceGraph->addSeries(surfaceSeries);

    barGraph->setCameraPreset(QtGraphs3D::CameraPreset::Behind);
    surfaceGraph->setCameraPreset(QtGraphs3D::CameraPreset::Front);

    GraphDataGenerator generator(barGraph, surfaceGraph, tableWidget);
    QObject::connect(barSeries, &QBar3DSeries::selectedBarChanged, &generator,
                     &GraphDataGenerator::selectFromTable);
    QObject::connect(surfaceSeries, &QSurface3DSeries::selectedPointChanged, &generator,
                     &GraphDataGenerator::selectFromTable);
    QObject::connect(tableWidget, &QTableWidget::currentCellChanged, &generator,
                     &GraphDataGenerator::selectedFromTable);
    QObject::connect(changeSelectedButton, &QPushButton::clicked, &generator,
                     &GraphDataGenerator::changeSelectedButtonClicked);

    widget.show();
    generator.start();
    return app.exec();
}
