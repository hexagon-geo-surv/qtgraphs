// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "q3dscene_p.h"

QT_BEGIN_NAMESPACE

/*!
 * \class Q3DScene
 * \inmodule QtGraphs
 * \brief Q3DScene class provides description of the 3D scene being visualized.
 *
 * The 3D scene contains a single active camera and a single active light
 * source. Visualized data is assumed to be at a fixed location.
 *
 * The 3D scene also keeps track of the viewport in which graph rendering is
 * done, the primary subviewport inside the viewport where the main 3D graphs
 * view resides and the secondary subviewport where the 2D sliced view of the
 * data resides. The subviewports are by default resized by the \a Q3DScene. To
 * override the resize behavior you need to listen to both \l viewportChanged()
 * and \l slicingActiveChanged() signals and recalculate the subviewports
 * accordingly.
 *
 * Also the scene has flag for tracking if the secondary 2D slicing view is
 * currently active or not. \note Not all graphs support the secondary 2D
 * slicing view.
 */

/*!
 * \class Q3DSceneChangeBitField
 * \internal
 */

/*!
 * \qmltype Scene3D
 * \inqmlmodule QtGraphs3D
 * \ingroup graphs_qml
 * \instantiates Q3DScene
 * \brief Scene3D type provides description of the 3D scene being visualized.
 *
 * The 3D scene contains a single active camera and a single active light
 * source. Visualized data is assumed to be at a fixed location.
 *
 * The 3D scene also keeps track of the viewport in which graph rendering is
 * done, the primary subviewport inside the viewport where the main 3D graphs
 * view resides and the secondary subviewport where the 2D sliced view of the
 * data resides.
 *
 * Also the scene has flag for tracking if the secondary 2D slicing view is
 * currently active or not. \note Not all graphs support the secondary 2D
 * slicing view.
 */

/*!
 * \qmlproperty rect Scene3D::viewport
 *
 * The current viewport rectangle where all 3D rendering is targeted.
 */

/*!
 * \qmlproperty rect Scene3D::primarySubViewport
 *
 * The current subviewport rectangle inside the viewport where the
 * primary view of the graphs is targeted.
 *
 * If slicingActive is \c false, the primary sub viewport will be equal to the
 * viewport. If slicingActive is \c true and the primary sub viewport has not
 * been explicitly set, it will be one fifth of the viewport.
 * \note Setting primarySubViewport larger than or outside of viewport resizes
 * viewport accordingly.
 */

/*!
 * \qmlproperty rect Scene3D::secondarySubViewport
 *
 * The secondary viewport rectangle inside the viewport. The secondary viewport
 * is used for drawing the 2D slice view in some graphs. If it has not
 * been explicitly set, it will be null. If slicingActive is \c true, it will
 * be equal to the viewport.
 * \note If the secondary sub viewport is larger than or outside of the
 * viewport, the viewport is resized accordingly.
 */

/*!
 * \qmlproperty point Scene3D::selectionQueryPosition
 *
 * The coordinates for the user input that should be processed
 * by the scene as a selection. If this property is set to a value other than
 * invalidSelectionPoint, the
 * graph tries to select a data item at the given point within the primary
 * viewport. After the rendering pass, the property is returned to its default
 * state of invalidSelectionPoint.
 */

/*!
 * \qmlproperty point Scene3D::graphPositionQuery
 *
 * The coordinates for the user input that should be processed by the scene as a
 * graph position query. If this property is set to value other than
 * invalidSelectionPoint, the graph tries to match a graph position to the given
 * point within the primary viewport. After the rendering pass, this property is
 * returned to its default state of invalidSelectionPoint. The queried graph
 * position can be read from the AbstractGraph3D::queriedGraphPosition property
 * after the next render pass.
 *
 * There is no single correct 3D coordinate to match a particular screen
 * position, so to be consistent, the queries are always done against the inner
 * sides of an invisible box surrounding the graph.
 *
 * \note Bar graphs allow graph position queries only at the graph floor level.
 *
 * \sa AbstractGraph3D::queriedGraphPosition
 */

/*!
 * \qmlproperty bool Scene3D::slicingActive
 *
 * Defines whether the 2D slicing view is currently active. If \c true,
 * AbstractGraph3D::selectionMode must have either the
 * \l{QAbstract3DGraph::SelectionRow}{AbstractGraph3D.SelectionRow} or
 * \l{QAbstract3DGraph::SelectionColumn}{AbstractGraph3D.SelectionColumn}
 * set to a valid selection.
 * \note Not all graphs support the 2D slicing view.
 */

/*!
 * \qmlproperty bool Scene3D::secondarySubviewOnTop
 *
 * Defines whether the 2D slicing view or the 3D view is drawn on top.
 */

/*!
 * \qmlproperty float Scene3D::devicePixelRatio
 *
 * The current device pixel ratio that is used when mapping input
 * coordinates to pixel coordinates.
 */

/*!
 * \qmlproperty point Scene3D::invalidSelectionPoint
 * A constant property providing an invalid point for selection.
 */

/*!
 * Constructs a basic scene with one light and one camera in it. An
 * optional \a parent parameter can be given and is then passed to QObject
 * constructor.
 */
Q3DScene::Q3DScene(QObject *parent)
    : QObject(*(new Q3DScenePrivate(this)), parent)
{}

/*!
 * Destroys the 3D scene and all the objects contained within it.
 */
Q3DScene::~Q3DScene() {}

/*!
 * \property Q3DScene::viewport
 *
 * \brief A read only property that contains the current viewport rectangle
 * where all the 3D rendering is targeted.
 */
QRect Q3DScene::viewport() const
{
    const Q_D(Q3DScene);
    return d->m_viewport;
}

/*!
 * \property Q3DScene::primarySubViewport
 *
 * \brief The current subviewport rectangle inside the viewport where the
 * primary view of the graphs is targeted.
 *
 * If isSlicingActive() is \c false, the primary sub viewport is equal to
 * viewport(). If isSlicingActive() is \c true and the primary sub viewport has
 * not been explicitly set, it will be one fifth of viewport().
 *
 * \note Setting primarySubViewport larger than or outside of the viewport
 * resizes the viewport accordingly.
 */
QRect Q3DScene::primarySubViewport() const
{
    const Q_D(Q3DScene);
    QRect primary = d->m_primarySubViewport;
    if (primary.isNull()) {
        if (d->m_isSlicingActive)
            primary = d->m_defaultSmallViewport;
        else
            primary = d->m_defaultLargeViewport;
    }
    return primary;
}

void Q3DScene::setPrimarySubViewport(const QRect &primarySubViewport)
{
    Q_D(Q3DScene);
    if (d->m_primarySubViewport != primarySubViewport) {
        if (!primarySubViewport.isValid() && !primarySubViewport.isNull()) {
            qWarning("Viewport is invalid.");
            return;
        }

        // If viewport is smaller than primarySubViewport, enlarge it
        if ((d->m_viewport.width() < (primarySubViewport.width() + primarySubViewport.x()))
            || (d->m_viewport.height() < (primarySubViewport.height() + primarySubViewport.y()))) {
            d->m_viewport.setWidth(
                qMax(d->m_viewport.width(), primarySubViewport.width() + primarySubViewport.x()));
            d->m_viewport.setHeight(
                qMax(d->m_viewport.height(), primarySubViewport.height() + primarySubViewport.y()));
            d->calculateSubViewports();
        }

        d->m_primarySubViewport = primarySubViewport;
        d->updateGLSubViewports();
        d->m_changeTracker.primarySubViewportChanged = true;
        d->m_sceneDirty = true;

        emit primarySubViewportChanged(primarySubViewport);
        emit needRender();
    }
}

/*!
 * Returns whether the given \a point resides inside the primary subview or not.
 * \return \c true if the point is inside the primary subview.
 * \note If subviews are superimposed, and the given \a point resides inside
 * both, result is \c true only when the primary subview is on top.
 */
bool Q3DScene::isPointInPrimarySubView(const QPoint &point)
{
    Q_D(Q3DScene);
    int x = point.x();
    int y = point.y();
    bool isInSecondary = d->isInArea(secondarySubViewport(), x, y);
    if (!isInSecondary || (isInSecondary && !d->m_isSecondarySubviewOnTop))
        return d->isInArea(primarySubViewport(), x, y);
    else
        return false;
}

/*!
 * Returns whether the given \a point resides inside the secondary subview or
 * not. \return \c true if the point is inside the secondary subview. \note If
 * subviews are superimposed, and the given \a point resides inside both, result
 * is \c true only when the secondary subview is on top.
 */
bool Q3DScene::isPointInSecondarySubView(const QPoint &point)
{
    Q_D(Q3DScene);
    int x = point.x();
    int y = point.y();
    bool isInPrimary = d->isInArea(primarySubViewport(), x, y);
    if (!isInPrimary || (isInPrimary && d->m_isSecondarySubviewOnTop))
        return d->isInArea(secondarySubViewport(), x, y);
    else
        return false;
}

/*!
 * \property Q3DScene::secondarySubViewport
 *
 * \brief The secondary viewport rectangle inside the viewport.
 *
 * The secondary viewport is used for drawing the 2D slice view in some
 * graphs. If it has not been explicitly set, it will be equal to
 * QRect. If isSlicingActive() is \c true, it will be equal to \l viewport.
 * \note If the secondary sub viewport is larger than or outside of the
 * viewport, the viewport is resized accordingly.
 */
QRect Q3DScene::secondarySubViewport() const
{
    const Q_D(Q3DScene);
    QRect secondary = d->m_secondarySubViewport;
    if (secondary.isNull() && d->m_isSlicingActive)
        secondary = d->m_defaultLargeViewport;
    return secondary;
}

void Q3DScene::setSecondarySubViewport(const QRect &secondarySubViewport)
{
    Q_D(Q3DScene);
    if (d->m_secondarySubViewport != secondarySubViewport) {
        if (!secondarySubViewport.isValid() && !secondarySubViewport.isNull()) {
            qWarning("Viewport is invalid.");
            return;
        }

        // If viewport is smaller than secondarySubViewport, enlarge it
        if ((d->m_viewport.width() < (secondarySubViewport.width() + secondarySubViewport.x()))
            || (d->m_viewport.height()
                < (secondarySubViewport.height() + secondarySubViewport.y()))) {
            d->m_viewport.setWidth(qMax(d->m_viewport.width(),
                                        secondarySubViewport.width() + secondarySubViewport.x()));
            d->m_viewport.setHeight(qMax(d->m_viewport.height(),
                                         secondarySubViewport.height() + secondarySubViewport.y()));
            d->calculateSubViewports();
        }

        d->m_secondarySubViewport = secondarySubViewport;
        d->updateGLSubViewports();
        d->m_changeTracker.secondarySubViewportChanged = true;
        d->m_sceneDirty = true;

        emit secondarySubViewportChanged(secondarySubViewport);
        emit needRender();
    }
}

/*!
 * \property Q3DScene::selectionQueryPosition
 *
 * \brief The coordinates for the user input that should be processed
 * by the scene as a selection.
 *
 * If this property is set to a value other than invalidSelectionPoint(), the
 * graph tries to select a data item, axis label, or a custom item at the
 * specified coordinates within the primary viewport.
 * After the rendering pass, the property is returned to its default state of
 * invalidSelectionPoint().
 *
 * \sa QAbstract3DGraph::selectedElement
 */
void Q3DScene::setSelectionQueryPosition(const QPoint &point)
{
    Q_D(Q3DScene);
    if (point != d->m_selectionQueryPosition) {
        d->m_selectionQueryPosition = point;
        d->m_changeTracker.selectionQueryPositionChanged = true;
        d->m_sceneDirty = true;

        emit selectionQueryPositionChanged(point);
        emit needRender();
    }
}

const QPoint Q3DScene::selectionQueryPosition() const
{
    const Q_D(Q3DScene);
    return d->m_selectionQueryPosition;
}

/*!
 * \return a QPoint signifying an invalid selection position.
 */
QPoint Q3DScene::invalidSelectionPoint() const
{
    static const QPoint invalidSelectionPos(-1, -1);
    return invalidSelectionPos;
}

/*!
 * \property Q3DScene::graphPositionQuery
 *
 * \brief The coordinates for the user input that should be processed
 * by the scene as a graph position query.
 *
 * If this property is set to a value other than invalidSelectionPoint(), the
 * graph tries to match a graph position to the specified coordinates
 * within the primary viewport.
 * After the rendering pass, this property is returned to its default state of
 * invalidSelectionPoint(). The queried graph position can be read from the
 * QAbstract3DGraph::queriedGraphPosition property after the next render pass.
 *
 * There is no single correct 3D coordinate to match a particular screen
 * position, so to be consistent, the queries are always done against the inner
 * sides of an invisible box surrounding the graph.
 *
 * \note Bar graphs allow graph position queries only at the graph floor level.
 *
 * \sa QAbstract3DGraph::queriedGraphPosition
 */
void Q3DScene::setGraphPositionQuery(const QPoint &point)
{
    Q_D(Q3DScene);
    if (point != d->m_graphPositionQueryPosition) {
        d->m_graphPositionQueryPosition = point;
        d->m_changeTracker.graphPositionQueryPositionChanged = true;
        d->m_sceneDirty = true;

        emit graphPositionQueryChanged(point);
        emit needRender();
    }
}

QPoint Q3DScene::graphPositionQuery() const
{
    const Q_D(Q3DScene);
    return d->m_graphPositionQueryPosition;
}

/*!
 * \property Q3DScene::slicingActive
 *
 * \brief Whether the 2D slicing view is currently active.
 *
 * If \c true, QAbstract3DGraph::selectionMode must have either
 * QAbstract3DGraph::SelectionRow or QAbstract3DGraph::SelectionColumn set
 * to a valid selection.
 * \note Not all graphs support the 2D slicing view.
 */
bool Q3DScene::isSlicingActive() const
{
    const Q_D(Q3DScene);
    return d->m_isSlicingActive;
}

void Q3DScene::setSlicingActive(bool isSlicing)
{
    Q_D(Q3DScene);
    if (d->m_isSlicingActive != isSlicing) {
        d->m_isSlicingActive = isSlicing;
        d->m_changeTracker.slicingActivatedChanged = true;
        d->m_sceneDirty = true;

        // Set secondary subview behind primary to achieve default functionality (=
        // clicking on primary disables slice)
        setSecondarySubviewOnTop(!isSlicing);

        d->calculateSubViewports();
        emit slicingActiveChanged(isSlicing);
        emit needRender();
    }
}

/*!
 * \property Q3DScene::secondarySubviewOnTop
 *
 * \brief Whether the 2D slicing view or the 3D view is drawn on top.
 */
bool Q3DScene::isSecondarySubviewOnTop() const
{
    const Q_D(Q3DScene);
    return d->m_isSecondarySubviewOnTop;
}

void Q3DScene::setSecondarySubviewOnTop(bool isSecondaryOnTop)
{
    Q_D(Q3DScene);
    if (d->m_isSecondarySubviewOnTop != isSecondaryOnTop) {
        d->m_isSecondarySubviewOnTop = isSecondaryOnTop;
        d->m_changeTracker.subViewportOrderChanged = true;
        d->m_sceneDirty = true;

        emit secondarySubviewOnTopChanged(isSecondaryOnTop);
        emit needRender();
    }
}

/*!
 * \property Q3DScene::devicePixelRatio
 *
 * \brief The device pixel ratio that is used when mapping input
 * coordinates to pixel coordinates.
 */
float Q3DScene::devicePixelRatio() const
{
    const Q_D(Q3DScene);
    return d->m_devicePixelRatio;
}

void Q3DScene::setDevicePixelRatio(float pixelRatio)
{
    Q_D(Q3DScene);
    if (d->m_devicePixelRatio != pixelRatio) {
        d->m_devicePixelRatio = pixelRatio;
        d->m_changeTracker.devicePixelRatioChanged = true;
        d->m_sceneDirty = true;

        emit devicePixelRatioChanged(pixelRatio);
        d->updateGLViewport();
        emit needRender();
    }
}

Q3DScenePrivate::Q3DScenePrivate(Q3DScene *q)
    : q_ptr(q)
    , m_isSecondarySubviewOnTop(true)
    , m_devicePixelRatio(1.f)
    , m_isUnderSideCameraEnabled(false)
    , m_isSlicingActive(false)
    , m_selectionQueryPosition(q->invalidSelectionPoint())
    , m_graphPositionQueryPosition(q->invalidSelectionPoint())
    , m_windowSize(QSize(0, 0))
    , m_sceneDirty(true)
{}

Q3DScenePrivate::~Q3DScenePrivate() {}

// Copies changed values from this scene to the other scene. If the other scene
// had same changes, those changes are discarded.
void Q3DScenePrivate::sync(Q3DScenePrivate &other)
{
    Q_Q(Q3DScene);
    if (m_changeTracker.windowSizeChanged) {
        other.setWindowSize(windowSize());
        m_changeTracker.windowSizeChanged = false;
        other.m_changeTracker.windowSizeChanged = false;
    }
    if (m_changeTracker.viewportChanged) {
        other.setViewport(m_viewport);
        m_changeTracker.viewportChanged = false;
        other.m_changeTracker.viewportChanged = false;
    }
    if (m_changeTracker.subViewportOrderChanged) {
        other.q_func()->setSecondarySubviewOnTop(q->isSecondarySubviewOnTop());
        m_changeTracker.subViewportOrderChanged = false;
        other.m_changeTracker.subViewportOrderChanged = false;
    }
    if (m_changeTracker.primarySubViewportChanged) {
        other.q_func()->setPrimarySubViewport(q->primarySubViewport());
        m_changeTracker.primarySubViewportChanged = false;
        other.m_changeTracker.primarySubViewportChanged = false;
    }
    if (m_changeTracker.secondarySubViewportChanged) {
        other.q_func()->setSecondarySubViewport(q->secondarySubViewport());
        m_changeTracker.secondarySubViewportChanged = false;
        other.m_changeTracker.secondarySubViewportChanged = false;
    }
    if (m_changeTracker.selectionQueryPositionChanged) {
        other.q_func()->setSelectionQueryPosition(q->selectionQueryPosition());
        m_changeTracker.selectionQueryPositionChanged = false;
        other.m_changeTracker.selectionQueryPositionChanged = false;
    }
    if (m_changeTracker.graphPositionQueryPositionChanged) {
        other.q_func()->setGraphPositionQuery(q->graphPositionQuery());
        m_changeTracker.graphPositionQueryPositionChanged = false;
        other.m_changeTracker.graphPositionQueryPositionChanged = false;
    }

    if (m_changeTracker.slicingActivatedChanged) {
        other.q_func()->setSlicingActive(q->isSlicingActive());
        m_changeTracker.slicingActivatedChanged = false;
        other.m_changeTracker.slicingActivatedChanged = false;
    }

    if (m_changeTracker.devicePixelRatioChanged) {
        other.q_func()->setDevicePixelRatio(q->devicePixelRatio());
        m_changeTracker.devicePixelRatioChanged = false;
        other.m_changeTracker.devicePixelRatioChanged = false;
    }

    m_sceneDirty = false;
    other.m_sceneDirty = false;
}

void Q3DScenePrivate::setViewport(const QRect &viewport)
{
    Q_Q(Q3DScene);
    if (m_viewport != viewport && viewport.isValid()) {
        m_viewport = viewport;
        calculateSubViewports();
        emit q->needRender();
    }
}

void Q3DScenePrivate::setViewportSize(int width, int height)
{
    Q_Q(Q3DScene);
    if (m_viewport.width() != width || m_viewport.height() != height) {
        m_viewport.setWidth(width);
        m_viewport.setHeight(height);
        calculateSubViewports();
        emit q->needRender();
    }
}

/*!
 * \internal
 * Sets the size of the window being rendered to. With widget based graphs, this
 * is equal to the size of the QWindow and is same as the bounding rectangle.
 * With declarative graphs this is equal to the size of the QQuickWindow and
 * can be different from the bounding rectangle.
 */
void Q3DScenePrivate::setWindowSize(const QSize &size)
{
    Q_Q(Q3DScene);
    if (m_windowSize != size) {
        m_windowSize = size;
        updateGLViewport();
        m_changeTracker.windowSizeChanged = true;
        emit q->needRender();
    }
}

QSize Q3DScenePrivate::windowSize() const
{
    return m_windowSize;
}

void Q3DScenePrivate::calculateSubViewports()
{
    // Calculates the default subviewport layout, used when slicing
    const float smallerViewPortRatio = 0.2f;
    m_defaultSmallViewport = QRect(0,
                                   0,
                                   m_viewport.width() * smallerViewPortRatio,
                                   m_viewport.height() * smallerViewPortRatio);
    m_defaultLargeViewport = QRect(0, 0, m_viewport.width(), m_viewport.height());

    updateGLViewport();
}

void Q3DScenePrivate::updateGLViewport()
{
    Q_Q(Q3DScene);
    // Update GL viewport
    m_glViewport.setX(m_viewport.x() * m_devicePixelRatio);
    m_glViewport.setY((m_windowSize.height() - (m_viewport.y() + m_viewport.height()))
                      * m_devicePixelRatio);
    m_glViewport.setWidth(m_viewport.width() * m_devicePixelRatio);
    m_glViewport.setHeight(m_viewport.height() * m_devicePixelRatio);

    m_changeTracker.viewportChanged = true;
    m_sceneDirty = true;

    // Do default subviewport changes first, then allow signal listeners to
    // override.
    updateGLSubViewports();
    emit q->viewportChanged(m_viewport);
}

void Q3DScenePrivate::updateGLSubViewports()
{
    if (m_isSlicingActive) {
        QRect primary = m_primarySubViewport;
        QRect secondary = m_secondarySubViewport;
        if (primary.isNull())
            primary = m_defaultSmallViewport;
        if (secondary.isNull())
            secondary = m_defaultLargeViewport;

        m_glPrimarySubViewport.setX((primary.x() + m_viewport.x()) * m_devicePixelRatio);
        m_glPrimarySubViewport.setY(
            (m_windowSize.height() - (primary.y() + primary.height() + m_viewport.y()))
            * m_devicePixelRatio);
        m_glPrimarySubViewport.setWidth(primary.width() * m_devicePixelRatio);
        m_glPrimarySubViewport.setHeight(primary.height() * m_devicePixelRatio);

        m_glSecondarySubViewport.setX((secondary.x() + m_viewport.x()) * m_devicePixelRatio);
        m_glSecondarySubViewport.setY(
            (m_windowSize.height() - (secondary.y() + secondary.height() + m_viewport.y()))
            * m_devicePixelRatio);
        m_glSecondarySubViewport.setWidth(secondary.width() * m_devicePixelRatio);
        m_glSecondarySubViewport.setHeight(secondary.height() * m_devicePixelRatio);
    } else {
        m_glPrimarySubViewport.setX(m_viewport.x() * m_devicePixelRatio);
        m_glPrimarySubViewport.setY((m_windowSize.height() - (m_viewport.y() + m_viewport.height()))
                                    * m_devicePixelRatio);
        m_glPrimarySubViewport.setWidth(m_viewport.width() * m_devicePixelRatio);
        m_glPrimarySubViewport.setHeight(m_viewport.height() * m_devicePixelRatio);

        m_glSecondarySubViewport = QRect();
    }
}

QRect Q3DScenePrivate::glViewport()
{
    return m_glViewport;
}

QRect Q3DScenePrivate::glPrimarySubViewport()
{
    return m_glPrimarySubViewport;
}

QRect Q3DScenePrivate::glSecondarySubViewport()
{
    return m_glSecondarySubViewport;
}

void Q3DScenePrivate::markDirty()
{
    Q_Q(Q3DScene);
    m_sceneDirty = true;
    emit q->needRender();
}

bool Q3DScenePrivate::isInArea(const QRect &area, int x, int y) const
{
    int areaMinX = area.x();
    int areaMaxX = area.x() + area.width();
    int areaMinY = area.y();
    int areaMaxY = area.y() + area.height();
    return (x >= areaMinX && x <= areaMaxX && y >= areaMinY && y <= areaMaxY);
}

QT_END_NAMESPACE
