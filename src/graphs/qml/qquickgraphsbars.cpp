// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "qquickgraphsbars_p.h"
#include "quickgraphstexturedata_p.h"
#include "bars3dcontroller_p.h"
#include "declarativescene_p.h"
#include "qbar3dseries_p.h"
#include "qvalue3daxis_p.h"
#include "qcategory3daxis_p.h"
#include "q3dcamera_p.h"
#include <QtCore/QMutexLocker>
#include <QColor>

#include <QtQuick3D/private/qquick3drepeater_p.h>
#include <QtQuick3D/private/qquick3dprincipledmaterial_p.h>
#include "quickgraphstexturedata_p.h"
#include <QtQuick3D/private/qquick3dcustommaterial_p.h>

QQuickGraphsBars::QQuickGraphsBars(QQuickItem *parent)
    : QQuickGraphsItem(parent),
      m_barsController(0),
      m_cachedRowCount(0),
      m_cachedColumnCount(0),
      m_minRow(0),
      m_maxRow(0),
      m_minCol(0),
      m_maxCol(0),
      m_newRows(0),
      m_newCols(0),
      m_maxSceneSize(40.0f),
      m_rowWidth(0),
      m_columnDepth(0),
      m_maxDimension(0),
      m_scaleFactor(0),
      m_xScaleFactor(1.0f),
      m_zScaleFactor(1.0f),
      m_cachedBarSeriesMargin(0.0f, 0.0f),
      m_hasNegativeValues(false),
      m_noZeroInRange(false),
      m_actualFloorLevel(0.0f),
      m_heightNormalizer(1.0f),
      m_backgroundAdjustment(0.0f),
      m_selectedBarSeries(0),
      m_selectedBarCoord(Bars3DController::invalidSelectionPosition()),
      m_selectedBarPos(0.0f, 0.0f, 0.0f),
      m_keepSeriesUniform(false),
      m_seriesScaleX(0.0f),
      m_seriesScaleZ(0.0f),
      m_seriesStep(0.0f),
      m_seriesStart(0.0f),
      m_zeroPosition(0.0f),
      m_visibleSeriesCount(0)
{
    setAcceptedMouseButtons(Qt::AllButtons);
    setFlags(ItemHasContents);
    // Create the shared component on the main GUI thread.
    m_barsController = new Bars3DController(boundingRect().toRect(), new Declarative3DScene);
    setSharedController(m_barsController);

    QQuick3DSceneEnvironment *scene = environment();
    scene->setBackgroundMode(QQuick3DSceneEnvironment::QQuick3DEnvironmentBackgroundTypes::Color);
    scene->setClearColor(Qt::blue);

    QObject::connect(m_barsController, &Bars3DController::primarySeriesChanged,
                     this, &QQuickGraphsBars::primarySeriesChanged);
    QObject::connect(m_barsController, &Bars3DController::selectedSeriesChanged,
                     this, &QQuickGraphsBars::selectedSeriesChanged);
    QObject::connect(m_barsController, &Abstract3DController::optimizationHintsChanged,
                     this, &QQuickGraphsBars::handleOptimizationHintsChanged);
}

QQuickGraphsBars::~QQuickGraphsBars()
{
    QMutexLocker locker(m_nodeMutex.data());
    const QMutexLocker locker2(mutex());
    delete m_barsController;
    m_barModelsMap.clear();
}

QCategory3DAxis *QQuickGraphsBars::rowAxis() const
{
    return static_cast<QCategory3DAxis *>(m_barsController->axisZ());
}

void QQuickGraphsBars::setRowAxis(QCategory3DAxis *axis)
{
    m_barsController->setAxisZ(axis);
}

QValue3DAxis *QQuickGraphsBars::valueAxis() const
{
    return static_cast<QValue3DAxis *>(m_barsController->axisY());
}

void QQuickGraphsBars::setValueAxis(QValue3DAxis *axis)
{
    m_barsController->setAxisY(axis);
    if (segmentLineRepeaterY()) {
        int segmentCount = 0;
        int subSegmentCount = 0;
        int gridLineCount = 0;
        int subGridLineCount = 0;
        if (axis->type() & QAbstract3DAxis::AxisTypeValue) {
            QValue3DAxis *valueAxis = static_cast<QValue3DAxis *>(axis);
            segmentCount = valueAxis->segmentCount();
            subSegmentCount = valueAxis->subSegmentCount();
            gridLineCount = 2 * (segmentCount + 1);
            subGridLineCount = 2 * (segmentCount * (subSegmentCount - 1));
        } else if (axis->type() & QAbstract3DAxis::AxisTypeCategory) {
            gridLineCount = axis->labels().size();
        }
        segmentLineRepeaterY()->setModel(gridLineCount);
        subsegmentLineRepeaterY()->setModel(subGridLineCount);
        repeaterY()->setModel(2 * axis->labels().size());
    }
}

QCategory3DAxis *QQuickGraphsBars::columnAxis() const
{
    return static_cast<QCategory3DAxis *>(m_barsController->axisX());
}

void QQuickGraphsBars::setColumnAxis(QCategory3DAxis *axis)
{
    m_barsController->setAxisX(axis);
}

void QQuickGraphsBars::setMultiSeriesUniform(bool uniform)
{
    if (uniform != isMultiSeriesUniform()) {
        m_barsController->setMultiSeriesScaling(uniform);
        emit multiSeriesUniformChanged(uniform);
    }
}

bool QQuickGraphsBars::isMultiSeriesUniform() const
{
    return m_barsController->multiSeriesScaling();
}

void QQuickGraphsBars::setBarThickness(float thicknessRatio)
{
    if (thicknessRatio != barThickness()) {
        m_barsController->setBarSpecs(thicknessRatio, barSpacing(),
                                      isBarSpacingRelative());
        emit barThicknessChanged(thicknessRatio);
    }
}

float QQuickGraphsBars::barThickness() const
{
    return m_barsController->barThickness();
}

void QQuickGraphsBars::setBarSpacing(const QSizeF &spacing)
{
    if (spacing != barSpacing()) {
        m_barsController->setBarSpecs(barThickness(), spacing, isBarSpacingRelative());
        emit barSpacingChanged(spacing);
    }
}

QSizeF QQuickGraphsBars::barSpacing() const
{
    return m_barsController->barSpacing();
}

void QQuickGraphsBars::setBarSpacingRelative(bool relative)
{
    if (relative != isBarSpacingRelative()) {
        m_barsController->setBarSpecs(barThickness(), barSpacing(), relative);
        emit barSpacingRelativeChanged(relative);
    }
}

bool QQuickGraphsBars::isBarSpacingRelative() const
{
    return m_barsController->isBarSpecRelative();
}

void QQuickGraphsBars::setBarSeriesMargin(const QSizeF &margin)
{
    if (margin != barSeriesMargin()) {
        m_barsController->setBarSeriesMargin(margin);
        emit barSeriesMarginChanged(barSeriesMargin());
    }
}

QSizeF QQuickGraphsBars::barSeriesMargin() const
{
    return m_barsController->barSeriesMargin();
}

QQmlListProperty<QBar3DSeries> QQuickGraphsBars::seriesList()
{
    return QQmlListProperty<QBar3DSeries>(this, this,
                                          &QQuickGraphsBars::appendSeriesFunc,
                                          &QQuickGraphsBars::countSeriesFunc,
                                          &QQuickGraphsBars::atSeriesFunc,
                                          &QQuickGraphsBars::clearSeriesFunc);
}

void QQuickGraphsBars::appendSeriesFunc(QQmlListProperty<QBar3DSeries> *list, QBar3DSeries *series)
{
    reinterpret_cast<QQuickGraphsBars *>(list->data)->addSeries(series);
}

qsizetype QQuickGraphsBars::countSeriesFunc(QQmlListProperty<QBar3DSeries> *list)
{
    return reinterpret_cast<QQuickGraphsBars *>(list->data)->m_barsController->barSeriesList().size();
}

QBar3DSeries *QQuickGraphsBars::atSeriesFunc(QQmlListProperty<QBar3DSeries> *list, qsizetype index)
{
    return reinterpret_cast<QQuickGraphsBars *>(list->data)->m_barsController->barSeriesList().at(index);
}

void QQuickGraphsBars::clearSeriesFunc(QQmlListProperty<QBar3DSeries> *list)
{
    QQuickGraphsBars *declBars = reinterpret_cast<QQuickGraphsBars *>(list->data);
    QList<QBar3DSeries *> realList = declBars->m_barsController->barSeriesList();
    int count = realList.size();
    for (int i = 0; i < count; i++)
        declBars->removeSeries(realList.at(i));
}

void QQuickGraphsBars::addSeries(QBar3DSeries *series)
{
    m_barsController->addSeries(series);
    connectSeries(series);
    if (series->selectedBar() != invalidSelectionPosition())
        updateSelectedBar();
}

void QQuickGraphsBars::removeSeries(QBar3DSeries *series)
{
    m_barsController->removeSeries(series);
    series->setParent(this); // Reparent as removing will leave series parentless
}

void QQuickGraphsBars::insertSeries(int index, QBar3DSeries *series)
{
    m_barsController->insertSeries(index, series);
}

void QQuickGraphsBars::setPrimarySeries(QBar3DSeries *series)
{
    m_barsController->setPrimarySeries(series);
}

QBar3DSeries *QQuickGraphsBars::primarySeries() const
{
    return m_barsController->primarySeries();
}

QBar3DSeries *QQuickGraphsBars::selectedSeries() const
{
    return m_barsController->selectedSeries();
}

void QQuickGraphsBars::setFloorLevel(float level)
{
    if (level != floorLevel()) {
        m_barsController->setFloorLevel(level);
        emit floorLevelChanged(level);
    }
}

float QQuickGraphsBars::floorLevel() const
{
    return m_barsController->floorLevel();
}

void QQuickGraphsBars::componentComplete()
{
    QQuickGraphsItem::componentComplete();

    auto wallBackground = background();
    QUrl wallUrl = QUrl(QStringLiteral("defaultMeshes/backgroundNoFloorMesh"));
    wallBackground->setSource(wallUrl);
    setBackground(wallBackground);

    QUrl floorUrl = QUrl(QStringLiteral(":/defaultMeshes/planeMesh"));
    m_floorBackground = new QQuick3DModel();
    m_floorBackgroundScale = new QQuick3DNode();
    m_floorBackgroundRotation = new QQuick3DNode();

    m_floorBackgroundScale->setParent(rootNode());
    m_floorBackgroundScale->setParentItem(rootNode());

    m_floorBackgroundRotation->setParent(m_floorBackgroundScale);
    m_floorBackgroundRotation->setParentItem(m_floorBackgroundScale);

    m_floorBackground->setObjectName("Floor Background");
    m_floorBackground->setParent(m_floorBackgroundRotation);
    m_floorBackground->setParentItem(m_floorBackgroundRotation);

    m_floorBackground->setSource(floorUrl);

    QValue3DAxis *axisY = static_cast<QValue3DAxis *>(m_barsController->axisY());
    m_helperAxisY.setFormatter(axisY->formatter());

    setFloorGridInRange(true);
    setVerticalSegmentLine(false);
}

void QQuickGraphsBars::synchData()
{
    if (!m_noZeroInRange) {
        m_barsController->m_scene->activeCamera()->d_ptr->setMinYRotation(-90.0f);
        m_barsController->m_scene->activeCamera()->d_ptr->setMaxYRotation(90.0f);
    } else {
        if ((m_hasNegativeValues && !m_helperAxisY.isReversed())
                || (!m_hasNegativeValues && m_helperAxisY.isReversed())) {
            m_barsController->m_scene->activeCamera()->d_ptr->setMinYRotation(-90.0f);
            m_barsController->m_scene->activeCamera()->d_ptr->setMaxYRotation(0.0f);
        } else {
            m_barsController->m_scene->activeCamera()->d_ptr->setMinYRotation(0.0f);
            m_barsController->m_scene->activeCamera()->d_ptr->setMaxYRotation(90.0f);
        }
    }
    if (m_barsController->m_changeTracker.barSpecsChanged || !m_cachedBarThickness.isValid()) {
        updateBarSpecs(m_barsController->m_barThicknessRatio, m_barsController->m_barSpacing,
                       m_barsController->m_isBarSpecRelative);
        m_barsController->m_changeTracker.barSpecsChanged = false;
    }

    // Floor level update requires data update, so do before abstract sync
    if (m_barsController->m_changeTracker.floorLevelChanged) {
        updateFloorLevel(m_barsController->m_floorLevel);
        m_barsController->m_changeTracker.floorLevelChanged = false;
    }

    if (m_barsController->m_changeTracker.barSeriesMarginChanged) {
        updateBarSeriesMargin(barSeriesMargin());
        m_barsController->m_changeTracker.barSeriesMarginChanged = false;
    }

    auto axisY = static_cast<QValue3DAxis *>(m_barsController->axisY());
    axisY->formatter()->d_ptr->recalculate();
    m_helperAxisY.setFormatter(axisY->formatter());

    QQuickGraphsItem::synchData();

    // Needs to be done after data is set, as it needs to know the visual array.
    if (m_barsController->m_changeTracker.selectedBarChanged) {
        if (m_barsController->m_selectedBar != m_selectedBarCoord || m_barsController->m_selectedBarSeries != m_selectedBarSeries)
            setSelectedBar(m_barsController->m_selectedBarSeries, m_barsController->m_selectedBar);
        updateSelectedBar();
        m_barsController->m_changeTracker.selectedBarChanged = false;
    }

    QMatrix4x4 modelMatrix;

    // Draw floor
    m_floorBackground->setPickable(false);
    m_floorBackgroundScale->setScale(scaleWithBackground());
    modelMatrix.scale(scaleWithBackground());
    m_floorBackgroundScale->setPosition(QVector3D(0.0f, -m_backgroundAdjustment, 0.0f));

    QQuaternion m_xRightAngleRotation(QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, 90.0f));
    QQuaternion m_xRightAngleRotationNeg(QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, -90.0f));

    if (isYFlipped()) {
        m_floorBackgroundRotation->setRotation(m_xRightAngleRotation);
        modelMatrix.rotate(m_xRightAngleRotation);
    } else {
        m_floorBackgroundRotation->setRotation(m_xRightAngleRotationNeg);
        modelMatrix.rotate(m_xRightAngleRotationNeg);
    }

    auto bgFloor = m_floorBackground;
    bgFloor->setPickable(false);
    QQmlListReference materialsRefF(bgFloor, "materials");
    QQuick3DPrincipledMaterial * bgMatFloor;

    if (!materialsRefF.size()) {
        bgMatFloor = new QQuick3DPrincipledMaterial();
        bgMatFloor->setParent(this);
        bgMatFloor->setRoughness(.3f);
        bgMatFloor->setEmissiveFactor(QVector3D(.075f, .075f, .075f));
        materialsRefF.append(bgMatFloor);
    } else {
        bgMatFloor = static_cast<QQuick3DPrincipledMaterial *>(materialsRefF.at(0));
    }
    Q3DTheme *theme = m_barsController->activeTheme();
    bgMatFloor->setBaseColor(theme->backgroundColor());

    if (m_axisRangeChanged) {
        updateGrid();
        updateLabels();
        m_axisRangeChanged = false;
    }
}

void QQuickGraphsBars::updateParameters() {
    m_minRow = m_barsController->m_axisZ->min();
    m_maxRow = m_barsController->m_axisZ->max();
    m_minCol = m_barsController->m_axisX->min();
    m_maxCol = m_barsController->m_axisX->max();
    m_newRows = m_maxRow - m_minRow + 1;
    m_newCols = m_maxCol - m_minCol + 1;

    if (m_cachedRowCount!= m_newRows || m_cachedColumnCount != m_newCols) {
        // Force update for selection related items
        if (isSliceEnabled() && m_barsController->isSlicingActive()) {
            setSliceEnabled(false);
            setSliceActivatedChanged(true);
        }

        m_cachedColumnCount = m_newCols;
        m_cachedRowCount = m_newRows;

        // Calculate max scene size
        float sceneRatio = qMin(float(m_newCols) / float(m_newRows),
                                float(m_newRows) / float(m_newCols));
        m_maxSceneSize = 2.0f * qSqrt(sceneRatio * m_newCols * m_newRows);

        if (m_cachedBarThickness.isValid())
            calculateSceneScalingFactors();
    }

    m_axisRangeChanged = true;
    createSliceView();
    update();
}

void QQuickGraphsBars::updateFloorLevel(float level)
{
    setFloorLevel(level);
    calculateHeightAdjustment();
}

void QQuickGraphsBars::updateGraph()
{
    QList<QBar3DSeries *> barSeriesList = m_barsController->barSeriesList();
    calculateSceneScalingFactors();

    if (m_barsController->m_changedSeriesList.size()) {
        for (auto series : m_barsController->barSeriesList()) {
            if (m_barModelsMap.contains(series))
                removeDataItems(series);
        }
    }
    generateBars(barSeriesList);
    int visualIndex = 0;
    for (auto barSeries : m_barsController->barSeriesList()) {
        if (barSeries->isVisible()) {
            updateBarVisuality(barSeries, visualIndex);
            updateBarPositions(barSeries);
            updateBarVisuals(barSeries);
            ++visualIndex;
        }
        else
            updateBarVisuality(barSeries, -1);
    }
}

void QQuickGraphsBars::updateAxisRange(float min, float max)
{
    QQuickGraphsItem::updateAxisRange(min, max);

    m_helperAxisY.setMin(min);
    m_helperAxisY.setMax(max);

    calculateHeightAdjustment();
}

void QQuickGraphsBars::updateAxisReversed(bool enable)
{
    m_helperAxisY.setReversed(enable);
    calculateHeightAdjustment();
}

void QQuickGraphsBars::calculateSceneScalingFactors()
{
    m_rowWidth = (m_cachedColumnCount * m_cachedBarSpacing.width()) * 0.5f;
    m_columnDepth = (m_cachedRowCount * m_cachedBarSpacing.height()) * 0.5f;
    m_maxDimension = qMax(m_rowWidth, m_columnDepth);
    m_scaleFactor = qMin((m_cachedColumnCount *(m_maxDimension / m_maxSceneSize)),
                         (m_cachedRowCount * (m_maxDimension / m_maxSceneSize)));

    // Single bar scaling
    m_xScale = m_cachedBarThickness.width() / m_scaleFactor;
    m_zScale = m_cachedBarThickness.height() / m_scaleFactor;

    // Adjust scaling according to margin
    m_xScale = m_xScale - m_xScale * m_cachedBarSeriesMargin.width();
    m_zScale = m_zScale - m_zScale * m_cachedBarSeriesMargin.height();

    // Whole graph scale factors
    m_xScaleFactor = m_rowWidth / m_scaleFactor;
    m_zScaleFactor = m_columnDepth / m_scaleFactor;

    if (m_requestedMargin < 0.0f) {
        m_hBackgroundMargin = 0.0f;
        m_vBackgroundMargin = 0.0f;
    } else {
        m_hBackgroundMargin = m_requestedMargin;
        m_vBackgroundMargin = m_requestedMargin;
    }

    m_scaleXWithBackground = m_xScaleFactor + m_hBackgroundMargin;
    m_scaleYWithBackground = 1.0f + m_vBackgroundMargin;
    m_scaleZWithBackground = m_zScaleFactor + m_hBackgroundMargin;

    auto scale = QVector3D(m_xScaleFactor, 1.0f, m_zScaleFactor);
    setScaleWithBackground(scale);
    setBackgroundScaleMargin({m_hBackgroundMargin, m_vBackgroundMargin, m_hBackgroundMargin});
    setScale(scale);

    m_helperAxisX.setScale(m_scaleXWithBackground * 2);
    m_helperAxisY.setScale(m_yScale);
    m_helperAxisZ.setScale(-m_scaleZWithBackground * 2);
    m_helperAxisX.setTranslate(-m_xScale);
    m_helperAxisY.setTranslate(0.0f);
}

void QQuickGraphsBars::calculateHeightAdjustment()
{
    m_minHeight = m_helperAxisY.min();
    m_maxHeight = m_helperAxisY.max();
    float newAdjustment = 1.0f;
    m_actualFloorLevel = qBound(m_minHeight, floorLevel(), m_maxHeight);
    float maxAbs = qFabs(m_maxHeight - m_actualFloorLevel);

    // Check if we have negative values
    if (m_minHeight < m_actualFloorLevel)
        m_hasNegativeValues = true;
    else if (m_minHeight >= m_actualFloorLevel)
        m_hasNegativeValues = false;

    if (m_maxHeight < m_actualFloorLevel) {
        m_heightNormalizer = float(qFabs(m_minHeight) - qFabs(m_maxHeight));
        maxAbs = qFabs(m_maxHeight) - qFabs(m_minHeight);
    } else {
        m_heightNormalizer = float(m_maxHeight - m_minHeight);
    }

    // Height fractions are used in gradient calculations and are therefore doubled
    // Note that if max or min is exactly zero, we still consider it outside the range
    if (m_maxHeight <= m_actualFloorLevel || m_minHeight >= m_actualFloorLevel) {
        m_noZeroInRange = true;
        m_gradientFraction = 2.0f;
    } else {
        m_noZeroInRange = false;
        float minAbs = qFabs(m_minHeight - m_actualFloorLevel);
        m_gradientFraction = qMax(minAbs, maxAbs) / m_heightNormalizer * 2.0f;
    }

    // Calculate translation adjustment for background floor
    newAdjustment = (qBound(0.0f, (maxAbs / m_heightNormalizer), 1.0f) - 0.5f) * 2.0f;
    if (m_helperAxisY.isReversed())
        newAdjustment = -newAdjustment;

    if (newAdjustment != m_backgroundAdjustment)
        m_backgroundAdjustment = newAdjustment;
}

void QQuickGraphsBars::calculateSeriesStartPosition()
{
    m_seriesStart = -((float(m_visibleSeriesCount) - 1.0f) * 0.5f)
            * (m_seriesStep - (m_seriesStep * m_cachedBarSeriesMargin.width()));
}

QVector3D QQuickGraphsBars::calculateCategoryLabelPosition(QAbstract3DAxis *axis,
                                                           QVector3D labelPosition, int index)
{
    QVector3D ret = labelPosition;
    if (axis->orientation() == QAbstract3DAxis::AxisOrientationX) {
        float xPos = (index + 0.5f) * m_cachedBarSpacing.width();
        ret.setX((xPos - m_rowWidth) / m_scaleFactor);
    }
    if (axis->orientation() == QAbstract3DAxis::AxisOrientationZ) {
        float zPos = (index + 0.5f) * m_cachedBarSpacing.height();
        ret.setZ((m_columnDepth - zPos) / m_scaleFactor);
    }
    ret.setY(-m_backgroundAdjustment);
    return ret;
}

float QQuickGraphsBars::calculateCategoryGridLinePosition(QAbstract3DAxis *axis, int index)
{
    float ret = 0.0f;
    if (axis->orientation() == QAbstract3DAxis::AxisOrientationZ) {
        float colPos = index * -(m_cachedBarSpacing.height() / m_scaleFactor);
        ret = colPos + scale().z();
    }
    if (axis->orientation() == QAbstract3DAxis::AxisOrientationX) {
        float rowPos = index * (m_cachedBarSpacing.width() / m_scaleFactor);
        ret = rowPos - scale().x();
    }
    if (axis->orientation() == QAbstract3DAxis::AxisOrientationY)
        ret = -m_backgroundAdjustment;
    return ret;
}

void QQuickGraphsBars::handleAxisXChanged(QAbstract3DAxis *axis)
{
    emit columnAxisChanged(static_cast<QCategory3DAxis *>(axis));
}

void QQuickGraphsBars::handleAxisYChanged(QAbstract3DAxis *axis)
{
    emit valueAxisChanged(static_cast<QValue3DAxis *>(axis));
}

void QQuickGraphsBars::handleAxisZChanged(QAbstract3DAxis *axis)
{
    emit rowAxisChanged(static_cast<QCategory3DAxis *>(axis));
}

void QQuickGraphsBars::handleSeriesMeshChanged(QAbstract3DSeries::Mesh mesh)
{
    QList<QBar3DSeries *> barSeriesList = m_barsController->barSeriesList();
    m_meshType = mesh;
    if (m_barsController->optimizationHints() == QAbstract3DGraph::OptimizationDefault) {
        for (auto series : m_barsController->barSeriesList()) {
            if (m_barModelsMap.contains(series))
                removeDataItems(series);
        }
        generateBars(barSeriesList);
    } else if (m_barsController->optimizationHints() == QAbstract3DGraph::OptimizationStatic) {
        resetClickedStatus();
//        m_instancingRootItem->setSource(QUrl(getMeshFileName()));
        m_selectionIndicator->setSource(QUrl(getMeshFileName()));
        m_barsController->markDataDirty();
        m_barsController->markSeriesVisualsDirty();
        generateBars(barSeriesList);
    }
}

void QQuickGraphsBars::handleOptimizationHintsChanged(QAbstract3DGraph::OptimizationHints hints)
{
    Q_UNUSED(hints);
//    setup();
}

void QQuickGraphsBars::handleMeshSmoothChanged(bool enable)
{
    QList<QBar3DSeries *> barSeriesList = m_barsController->barSeriesList();
    m_smooth = enable;

    if (m_barsController->optimizationHints() == QAbstract3DGraph::OptimizationDefault) {
        for (auto series : m_barsController->barSeriesList()) {
            if (m_barModelsMap.contains(series))
                removeDataItems(series);
        }
        generateBars(barSeriesList);
    } else if (m_barsController->optimizationHints() == QAbstract3DGraph::OptimizationStatic) {
        resetClickedStatus();
//        m_instancingRootItem->setSource(QUrl(getMeshFileName()));
        m_selectionIndicator->setSource(QUrl(getMeshFileName()));
        m_barsController->markDataDirty();
        m_barsController->markSeriesVisualsDirty();
        generateBars(barSeriesList);
    }
}

void QQuickGraphsBars::handleRowCountChanged()
{
    QCategory3DAxis *categoryAxisZ = static_cast<QCategory3DAxis *>(m_barsController->axisZ());
    segmentLineRepeaterZ()->setModel(categoryAxisZ->labels().size());
    repeaterZ()->setModel(categoryAxisZ->labels().size());
    updateParameters();
}

void QQuickGraphsBars::handleColCountChanged()
{
    QCategory3DAxis *categoryAxisX = static_cast<QCategory3DAxis *>(m_barsController->axisX());
    segmentLineRepeaterX()->setModel(categoryAxisX->labels().size());
    repeaterX()->setModel(categoryAxisX->labels().size());
    updateParameters();
}

void QQuickGraphsBars::connectSeries(QBar3DSeries *series)
{
    m_meshType = series->mesh();
    m_smooth = series->isMeshSmooth();

    QObject::connect(series, &QBar3DSeries::meshChanged, this,
                     &QQuickGraphsBars::handleSeriesMeshChanged);
    QObject::connect(series, &QBar3DSeries::meshSmoothChanged, this,
                     &QQuickGraphsBars::handleMeshSmoothChanged);
    QObject::connect(series->dataProxy(), &QBarDataProxy::rowCountChanged, this,
                     &QQuickGraphsBars::handleRowCountChanged);
    QObject::connect(series->dataProxy(), &QBarDataProxy::colCountChanged, this,
                     &QQuickGraphsBars::handleColCountChanged);
}

void QQuickGraphsBars::disconnectSeries(QBar3DSeries *series)
{
    QObject::disconnect(series, 0, this, 0);
}

void QQuickGraphsBars::generateBars(QList<QBar3DSeries *> &barSeriesList)
{
    int seriesCount = barSeriesList.size();
    m_visibleSeriesCount = 0;
    for (int i = 0; i < seriesCount; i++) {
        QBar3DSeries *barSeries = static_cast<QBar3DSeries *>(barSeriesList[i]);
        QVector<BarModel *> *barList = m_barModelsMap.value(barSeries);
        if (!barList) {
            barList = new QVector<BarModel *>;
            m_barModelsMap[barSeries] = barList;
        }
        if (barList->isEmpty()) {
            QQuick3DTexture *texture = createTexture();
            texture->setParent(this);
            auto gradient = barSeries->baseGradient();
            auto textureData = static_cast<QuickGraphsTextureData *>(texture->textureData());
            textureData->createGradient(gradient);

            bool visible = barSeries->isVisible();
            int minRow = m_barsController->m_axisZ->min();
            int dataRowCount = 0;
            int dataColCount = 0;

            const QBarDataArray *array = barSeries->dataProxy()->array();
            QBarDataProxy *dataProxy = barSeries->dataProxy();
            dataRowCount = dataProxy->rowCount();
            dataColCount = dataProxy->colCount();
            int dataRowIndex = minRow;

            while (dataRowIndex < dataRowCount) {
                const QBarDataRow *dataRow = array->at(dataRowIndex);
                Q_ASSERT(dataRow->size() == dataColCount);
                for (int i = 0; i < dataColCount; i++) {
                    QBarDataItem *dataItem = const_cast <QBarDataItem *> (&(dataRow->at(i)));
                    auto scene = QQuick3DViewport::scene();
                    QQuick3DModel *model = createDataItem(scene);
                    model->setVisible(visible);

                    BarModel *barModel = new BarModel();
                    barModel->model = model;
                    barModel->barItem = dataItem;
                    barModel->coord = QPoint(dataRowIndex, i);
                    barModel->texture = texture;

                    if (!barList->contains(barModel))
                        barList->append(barModel);
                }
                ++dataRowIndex;
            }
        }
        if (barSeries->isVisible())
            m_visibleSeriesCount++;
    }
}

QQuick3DModel *QQuickGraphsBars::createDataItem(QQuick3DNode *scene)
{
    auto model = new QQuick3DModel();
    model->setParent(scene);
    model->setParentItem(scene);
    model->setObjectName(QStringLiteral("BarModel"));
    QString fileName = getMeshFileName();
    model->setSource(QUrl(fileName));
    return model;
}

QString QQuickGraphsBars::getMeshFileName()
{
    QString fileName;
    QString smoothString = QStringLiteral("Smooth");
    switch (m_meshType) {
    case QAbstract3DSeries::MeshSphere:
        fileName = QStringLiteral("defaultMeshes/sphereMesh");
        break;
    case QAbstract3DSeries::MeshBar:
    case QAbstract3DSeries::MeshCube:
        fileName = QStringLiteral("defaultMeshes/barMesh");
        break;
    case QAbstract3DSeries::MeshPyramid:
        fileName = QStringLiteral("defaultMeshes/pyramidMesh");
        break;
    case QAbstract3DSeries::MeshCone:
        fileName = QStringLiteral("defaultMeshes/coneMesh");
        break;
    case QAbstract3DSeries::MeshCylinder:
        fileName = QStringLiteral("defaultMeshes/cylinderMesh");
        break;
    case QAbstract3DSeries::MeshBevelBar:
    case QAbstract3DSeries::MeshBevelCube:
        fileName = QStringLiteral("defaultMeshes/bevelBarMesh");
        break;
    default:
        fileName = QStringLiteral("defaultMeshes/sphereMesh");
    }
    if (m_smooth && m_meshType != QAbstract3DSeries::MeshPoint)
        fileName += smoothString;

    fixMeshFileName(fileName, m_meshType);

    return fileName;
}

void QQuickGraphsBars::fixMeshFileName(QString &fileName, QAbstract3DSeries::Mesh meshType)
{
    if (!m_barsController->activeTheme()->isBackgroundEnabled()
            && meshType != QAbstract3DSeries::MeshSphere) {
        fileName.append(QStringLiteral("Full"));
    }
}

void QQuickGraphsBars::updateBarVisuality(QBar3DSeries *series, int visualIndex)
{
    QVector<BarModel *> barList = *m_barModelsMap.value(series);
    for (int i = 0; i < barList.count(); i++) {
        if (barList.at(i)->model->visible() != series->isVisible() && isSliceEnabled()) {
            setSliceEnabled(false);
            setSliceActivatedChanged(true);
        }
        barList.at(i)->visualIndex = visualIndex;
        barList.at(i)->model->setVisible(series->isVisible());
    }
    setSelectedBar(m_selectedBarSeries, m_selectedBarCoord);
    itemLabel()->setVisible(false);
}

void QQuickGraphsBars::updateBarPositions(QBar3DSeries *series)
{
    QBarDataProxy *dataProxy = series->dataProxy();
    int dataRowCount = 0;
    int dataColCount = 0;


    m_seriesScaleX = 1.0f / float(m_visibleSeriesCount);
    m_seriesStep = 1.0f / float(m_visibleSeriesCount);
    m_seriesStart = -((float(m_visibleSeriesCount) - 1.0f) * 0.5f)
            * (m_seriesStep - (m_seriesStep * m_cachedBarSeriesMargin.width()));

    if (m_keepSeriesUniform)
        m_seriesScaleZ = m_seriesScaleX;
    else
        m_seriesScaleZ = 1.0f;

    m_meshRotation = dataProxy->series()->meshRotation();
    m_zeroPosition = m_helperAxisY.itemPositionAt(m_actualFloorLevel);

    QVector<BarModel *> barList = *m_barModelsMap.value(series);
    if (m_barsController->optimizationHints() == QAbstract3DGraph::OptimizationDefault) {
        for (int i = 0; i < barList.count(); i++) {
            QBarDataItem *item = const_cast<QBarDataItem *>((barList.at(i)->barItem));
            QQuick3DModel *model = barList.at(i)->model;
            float value = item->value();
            float heightValue = m_helperAxisY.itemPositionAt(value);

            if (m_noZeroInRange) {
                if (m_hasNegativeValues) {
                    heightValue = -1.0f + heightValue;
                    if (heightValue > 0.0f)
                        heightValue = 0.0f;
                } else {
                    if (heightValue < 0.0f)
                        heightValue = 0.0f;
                }
            } else {
                heightValue -= m_zeroPosition;
            }

            if (m_helperAxisY.isReversed())
                heightValue = -heightValue;

            float angle = item->rotation();

            if (angle) {
                model->setRotation(
                            QQuaternion::fromAxisAndAngle(
                                upVector, angle));
            } else {
                model->setRotation(QQuaternion());
            }

            if (heightValue < 0.f) {
                const QVector3D rot = model->eulerRotation();
                model->setEulerRotation(QVector3D(-180.f, rot.y(), rot.z()));
            }

            float seriesPos = m_seriesStart + m_seriesStep
                    * (barList.at(i)->visualIndex - (barList.at(i)->visualIndex
                                                     * m_cachedBarSeriesMargin.width())) + 0.5f;


            float colPos = (dataColCount + seriesPos) * m_cachedBarSpacing.width();
            float xPos = (colPos - m_rowWidth) / m_scaleFactor;
            float rowPos = (dataRowCount + 0.5f) * (m_cachedBarSpacing.height());
            float zPos = (m_columnDepth - rowPos) / m_scaleFactor;

            barList.at(i)->heightValue = heightValue;
            model->setPosition(QVector3D(xPos, heightValue - m_backgroundAdjustment, zPos));
            model->setScale(QVector3D(m_xScale * m_seriesScaleX, qAbs(heightValue),
                                      m_zScale * m_seriesScaleZ));

            if (heightValue == 0) {
                model->setPickable(false);
                model->setVisible(false);
            } else {
                model->setPickable(true);
            }

            if (dataColCount < dataProxy->colCount() - 1) {
                ++dataColCount;
            } else {
                dataColCount = 0;
                if (dataRowCount < dataProxy->rowCount() - 1)
                    ++dataRowCount;
                else
                    dataRowCount = 0;
            }
        }
    }
}

void QQuickGraphsBars::updateBarVisuals(QBar3DSeries *series)
{
    QVector<BarModel *> barList = *m_barModelsMap.value(series);
    bool useGradient = series->d_ptr->isUsingGradient();

    if (useGradient) {
        if (!m_hasHighlightTexture) {
            m_highlightTexture = createTexture();
            m_highlightTexture->setParent(this);
            m_hasHighlightTexture = true;
        }
        auto highlightGradient = series->singleHighlightGradient();
        auto highlightTextureData = static_cast<QuickGraphsTextureData *>(m_highlightTexture->textureData());
        highlightTextureData->createGradient(highlightGradient);
    } else {
        if (m_hasHighlightTexture) {
            m_highlightTexture->deleteLater();
            m_hasHighlightTexture = false;
        }
    }

    bool rangeGradient = (useGradient && series->d_ptr->m_colorStyle == Q3DTheme::ColorStyleRangeGradient)
            ? true : false;

    if (m_barsController->optimizationHints() == QAbstract3DGraph::OptimizationDefault) {
        if (!rangeGradient) {
            for (int i = 0; i < barList.count(); i++) {
                QQuick3DModel *model = barList.at(i)->model;
                updateItemMaterial(model, useGradient, rangeGradient);
                updatePrincipledMaterial(model, series->baseColor(), useGradient, false,
                                         barList.at(i)->texture);
            }
        } else {
            for (int i = 0; i < barList.count(); i++) {
                QQuick3DModel *model = barList.at(i)->model;
                updateItemMaterial(model, useGradient, rangeGradient);
                updateCustomMaterial(model, barList.at(i)->texture);
            }
        }
    }
}

void QQuickGraphsBars::updateItemMaterial(QQuick3DModel *item, bool useGradient, bool rangeGradient)
{
    Q_UNUSED(useGradient);
    QQmlListReference materialsRef(item, "materials");
    if (!rangeGradient) {
        if (materialsRef.size()) {
            if (!qobject_cast<QQuick3DPrincipledMaterial *>(materialsRef.at(0))) {
                auto principledMaterial = new QQuick3DPrincipledMaterial();
                principledMaterial->setParent(this);
                auto oldCustomMaterial = materialsRef.at(0);
                materialsRef.replace(0, principledMaterial);
                delete oldCustomMaterial;
            }
        } else {
            auto principledMaterial = new QQuick3DPrincipledMaterial();
            principledMaterial->setParent(this);
            materialsRef.append(principledMaterial);
        }
    } else {
        if (materialsRef.size()) {
            if (!qobject_cast<QQuick3DCustomMaterial *>(materialsRef.at(0))) {
                auto customMaterial = createQmlCustomMaterial(QStringLiteral(":/materials/RangeGradientMaterial"));
                auto oldPrincipledMaterial = materialsRef.at(0);
                materialsRef.replace(0, customMaterial);
                delete oldPrincipledMaterial;
            }
        } else {
            auto customMaterial = createQmlCustomMaterial(QStringLiteral(":/materials/RangeGradientMaterial"));
            materialsRef.append(customMaterial);
        }
    }
}

void QQuickGraphsBars::updateCustomMaterial(QQuick3DModel *item, bool isHighlight,
                                            QQuick3DTexture *texture)
{
    QQmlListReference materialsRef(item, "materials");
    auto customMaterial = static_cast<QQuick3DCustomMaterial *>(materialsRef.at(0));
    QVariant textureInputAsVariant = customMaterial->property("custex");
    QQuick3DShaderUtilsTextureInput *textureInput = textureInputAsVariant.value<QQuick3DShaderUtilsTextureInput *>();

    if (!isHighlight)
        textureInput->setTexture(texture);
    else
        textureInput->setTexture(m_highlightTexture);

    float rangeGradientYScaler = 0.5f / m_yScale;
    float value = (item->y() + m_yScale) * rangeGradientYScaler;
    customMaterial->setProperty("gradientPos", value);
}

void QQuickGraphsBars::updatePrincipledMaterial(QQuick3DModel *model, const QColor &color,
                                                bool useGradient, bool isHighlight,
                                                QQuick3DTexture *texture)
{
    QQmlListReference materialsRef(model, "materials");
    auto principledMaterial = static_cast<QQuick3DPrincipledMaterial *>(materialsRef.at(0));
    principledMaterial->setParent(this);

    if (useGradient) {
        principledMaterial->setBaseColor(QColor(Qt::white));
        if (!isHighlight)
            principledMaterial->setBaseColorMap(texture);
        else
            principledMaterial->setBaseColorMap(m_highlightTexture);
    } else {
        principledMaterial->setBaseColor(color);
    }
}

void QQuickGraphsBars::removeDataItems(QBar3DSeries *series)
{
    if (m_barModelsMap.value(series)->isEmpty())
        return;
    QVector<BarModel *> barList = *m_barModelsMap.value(series);
    for (int i = 0; i < barList.count(); i++) {
        barList.at(i)->model->setPickable(false);
        barList.at(i)->model->setVisible(false);
        QQmlListReference materialsRef(barList.at(i)->model, "materials");
        if (materialsRef.size()) {
            auto material = materialsRef.at(0);
            delete material;
        }
        delete barList.at(i)->model;
    }
    m_barModelsMap.remove(series);
    setSelectedBar(m_selectedBarSeries, m_selectedBarCoord);
    itemLabel()->setVisible(false);
}

QQuick3DTexture *QQuickGraphsBars::createTexture()
{
    QQuick3DTexture *texture = new QQuick3DTexture();
    texture->setParent(this);
    texture->setRotationUV(-90.0f);
    texture->setHorizontalTiling(QQuick3DTexture::ClampToEdge);
    texture->setVerticalTiling(QQuick3DTexture::ClampToEdge);
    QuickGraphsTextureData *textureData = new QuickGraphsTextureData();
    textureData->setParent(texture);
    textureData->setParentItem(texture);
    texture->setTextureData(textureData);

    return texture;
}

bool QQuickGraphsBars::handleMousePressedEvent(QMouseEvent *event)
{
    QQuickGraphsItem::handleMousePressedEvent(event);

    if (Qt::LeftButton == event->button()) {
        auto mousePos = event->pos();
        QList<QQuick3DPickResult> pickResults = pickAll(mousePos.x(), mousePos.y());
        auto selectionMode = m_barsController->selectionMode();
        QQuick3DModel *selectedModel = nullptr;
        if (!selectionMode.testFlag(QAbstract3DGraph::SelectionNone)) {
            for (const auto &picked : std::as_const(pickResults)) {
                if (picked.objectHit()->visible()) {
                    if (picked.objectHit() == backgroundBB() || picked.objectHit() == background()) {
                        resetClickedStatus();
                        continue;
                    } else if (picked.objectHit()->objectName().contains(QStringLiteral("BarModel"))) {
                        selectedModel = picked.objectHit();
                        break;
                    }
                }
            }

            if (selectedModel) {
                QBar3DSeries *series = 0;
                QPoint coord = m_barsController->invalidSelectionPosition();
                for (auto it = m_barModelsMap.begin(); it != m_barModelsMap.end(); it++) {
                    if (!it.key()->isVisible())
                        continue;
                    for (int i = 0; i < it.value()->count(); i++) {
                        QQuick3DModel *model = it.value()->at(i)->model;
                        if (model == selectedModel) {
                            series = it.key();
                            coord = it.value()->at(i)->coord;
                        }
                    }
                }
                setSelectedBar(series, coord);
            } else {
                resetClickedStatus();
            }
        }
    }

    return true;
}

void QQuickGraphsBars::setSelectedBar(QBar3DSeries *series, const QPoint &coord)
{
    if (!m_barModelsMap.contains(series))
        series = 0;

    if (coord != m_selectedBarCoord || series != m_selectedBarSeries) {
        m_selectedBarSeries = series;
        m_selectedBarCoord = coord;
        if (isSliceEnabled()) {
            m_barsController->setSlicingActive(true);
            setSliceActivatedChanged(true);
        }

        // Clear selection from other series and finally set new selection to the specified series
        for (auto it = m_barModelsMap.begin(); it != m_barModelsMap.end(); it++) {
            if (it.key() != m_selectedBarSeries)
                it.key()->dptr()->setSelectedBar(invalidSelectionPosition());
        }
        if (m_selectedBarSeries) {
            m_selectedBarSeries->dptr()->setSelectedBar(m_selectedBarCoord);
            m_barsController->setSelectedBar(m_selectedBarCoord, m_selectedBarSeries, false);
        }
    }
}

void QQuickGraphsBars::updateSelectedBar()
{
    bool visible = false;
    if (m_selectedBarSeries) {
        for (auto it = m_barModelsMap.begin(); it != m_barModelsMap.end(); it++) {
            QVector<BarModel *> barList = *it.value();
            for (int i = 0; i < barList.count(); i++) {
                Bars3DController::SelectionType selectionType =
                        isSelected(barList.at(i)->coord.x(), barList.at(i)->coord.y(), it.key());
                switch (selectionType) {
                case Bars3DController::SelectionItem: {
                    updatePrincipledMaterial(barList.at(i)->model,
                                             m_selectedBarSeries->singleHighlightColor(),
                                             m_selectedBarSeries->d_ptr->isUsingGradient(), true,
                                             barList.at(i)->texture);

                    m_selectedBarPos = barList.at(i)->model->position();
                    visible = m_selectedBarSeries->isVisible() && !m_selectedBarPos.isNull();
                    QString label = (m_selectedBarSeries->dptr()->itemLabel());

                    if (barList.at(i)->heightValue >= 0.0f) {
                        m_selectedBarPos.setY(m_selectedBarPos.y() + barList.at(i)->heightValue
                                              + 0.2f);
                    } else {
                        m_selectedBarPos.setY(m_selectedBarPos.y() + barList.at(i)->heightValue
                                              - 0.2f);
                    }

                    itemLabel()->setPosition(m_selectedBarPos);
                    itemLabel()->setProperty("labelText", label);
                    itemLabel()->setEulerRotation(
                                QVector3D(-m_barsController->scene()->activeCamera()->yRotation(),
                                          -m_barsController->scene()->activeCamera()->xRotation(),
                                          0));

                    if (isSliceEnabled()) {
                        sliceItemLabel()->setPosition(QVector3D((m_selectedBarPos.x() + .05f),
                                                                (m_selectedBarPos.y() + .5f), 0.0f));
                        sliceItemLabel()->setScale(sliceItemLabel()->scale() / 1.5f);
                        sliceItemLabel()->setProperty("labelText", label);
                        sliceItemLabel()->setEulerRotation(QVector3D(0.0f, 0.0f, 90.0f));
                        sliceItemLabel()->setVisible(true);
                    }

                    break;
                }
                case Bars3DController::SelectionRow: {
                    updatePrincipledMaterial(barList.at(i)->model,
                                             m_selectedBarSeries->multiHighlightColor(),
                                             m_selectedBarSeries->d_ptr->isUsingGradient(), true,
                                             barList.at(i)->texture);
                    break;
                }
                case Bars3DController::SelectionColumn: {
                    updatePrincipledMaterial(barList.at(i)->model,
                                             m_selectedBarSeries->multiHighlightColor(),
                                             m_selectedBarSeries->d_ptr->isUsingGradient(), true,
                                             barList.at(i)->texture);
                }
                default:
                    break;
                }
            }
        }
    }
    itemLabel()->setVisible(visible);
}

Abstract3DController::SelectionType QQuickGraphsBars::isSelected(int row, int bar,
                                                                 QBar3DSeries *series)
{
    Bars3DController::SelectionType isSelectedType = Bars3DController::SelectionNone;
    auto selectionMode = m_barsController->selectionMode();
    if ((selectionMode.testFlag(QAbstract3DGraph::SelectionMultiSeries)
         && m_selectedBarSeries) || series == m_selectedBarSeries) {
        if (row == m_selectedBarCoord.x() && bar == m_selectedBarCoord.y()
                && (selectionMode.testFlag(QAbstract3DGraph::SelectionItem))) {
            isSelectedType = Bars3DController::SelectionItem;
        } else if (row == m_selectedBarCoord.x()
                   && (selectionMode.testFlag(QAbstract3DGraph::SelectionRow))) {
            isSelectedType = Bars3DController::SelectionRow;
        } else if (bar == m_selectedBarCoord.y()
                   && (selectionMode.testFlag(QAbstract3DGraph::SelectionColumn))) {
            isSelectedType = Bars3DController::SelectionColumn;
        }
    }

    return isSelectedType;
}

void QQuickGraphsBars::resetClickedStatus()
{
    m_barsController->m_isSeriesVisualsDirty = true;
    m_selectedBarPos = QVector3D(0.0f, 0.0f, 0.0f);
    m_selectedBarCoord = Bars3DController::invalidSelectionPosition();
    m_selectedBarSeries = 0;
    m_barsController->clearSelection();
}

void QQuickGraphsBars::updateSliceGraph()
{
    QQuickGraphsItem::updateSliceGraph();

    if (!sliceView()->isVisible()) {
        if (!m_sliceViewBars.isEmpty()) {
            for (int i = 0; i < m_sliceViewBars.count(); i++) {
                m_sliceViewBars.at(i)->model->setPickable(false);
                m_sliceViewBars.at(i)->model->setVisible(false);
                QQmlListReference materialsRef(m_sliceViewBars.at(i)->model, "materials");
                if (materialsRef.size()) {
                    auto material = materialsRef.at(0);
                    delete material;
                }
                delete m_sliceViewBars.at(i)->model;
            }
            m_sliceViewBars.clear();
        }
        return;
    }

    auto selectionMode = m_barsController->selectionMode();
    QVector<BarModel *> barList = *m_barModelsMap.value(m_selectedBarSeries);
    if (selectionMode.testFlag(QAbstract3DGraph::SelectionRow)) {
        for (int col = 0; col < m_selectedBarSeries->dataProxy()->colCount(); col++) {
            auto index = (m_selectedBarCoord.x() * m_selectedBarSeries->dataProxy()->colCount()) + col;

            QQuick3DViewport *sliceParent = sliceView();
            QQuick3DModel *model = createDataItem(sliceParent->scene());

            BarModel *barModel = new BarModel();
            barModel->model = model;
            barModel->model->setVisible(sliceView()->isVisible());

            QQuick3DTexture *texture = createTexture();
            texture->setParent(barModel->model);
            texture->setParentItem(barModel->model);
            auto gradient = m_selectedBarSeries->baseGradient();
            auto textureData = static_cast<QuickGraphsTextureData *>(texture->textureData());
            textureData->createGradient(gradient);

            barModel->texture = texture;
            barModel->barItem = barList.at(index)->barItem;
            barModel->coord = barList.at(index)->coord;
            barModel->visualIndex = barList.at(index)->visualIndex;
            barModel->heightValue = barList.at(index)->heightValue;

            barModel->model->setPosition(QVector3D(barList.at(index)->model->x(),
                                                   barList.at(index)->model->y(), 0.0f));
            barModel->model->setScale(barList.at(index)->model->scale());

            bool useGradient = m_selectedBarSeries->d_ptr->isUsingGradient();
            bool rangeGradient = (useGradient && m_selectedBarSeries->d_ptr->m_colorStyle ==
                                  Q3DTheme::ColorStyleRangeGradient) ? true : false;

            updateItemMaterial(barModel->model, useGradient, rangeGradient);
            updatePrincipledMaterial(barModel->model,
                                     m_selectedBarSeries->baseColor(),
                                     m_selectedBarSeries->d_ptr->isUsingGradient(), false,
                                     barModel->texture);

            m_sliceViewBars.append(barModel);
        }
    }
}

void QQuickGraphsBars::updateBarSpecs(float thicknessRatio, const QSizeF &spacing, bool relative)
{
    // Convert ratio to QSizeF, as we need it in that format for autoscaling calculations
    m_cachedBarThickness.setWidth(1.0);
    m_cachedBarThickness.setHeight(1.0f / thicknessRatio);

    if (relative) {
        m_cachedBarSpacing.setWidth((m_cachedBarThickness.width() * 2)
                                    * (spacing.width() + 1.0f));
        m_cachedBarSpacing.setHeight((m_cachedBarThickness.height() * 2)
                                     * (spacing.height() + 1.0f));
    } else {
        m_cachedBarSpacing = m_cachedBarThickness * 2 + spacing * 2;
    }

    m_axisRangeChanged = true;
    // Slice mode doesn't update correctly without this
    if (isSliceEnabled() && m_barsController->isSlicingActive()) {
        setSliceEnabled(false);
        setSliceActivatedChanged(true);
    }

    // Calculate here and at setting sample space
    calculateSceneScalingFactors();
}

void QQuickGraphsBars::updateBarSeriesMargin(const QSizeF &margin)
{
    m_cachedBarSeriesMargin = margin;
    calculateSeriesStartPosition();
    calculateSceneScalingFactors();
    m_barsController->m_isSeriesVisualsDirty = true;
}
