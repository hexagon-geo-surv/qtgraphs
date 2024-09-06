// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtGraphs/qpieseries.h>
#include <QtGraphs/qpieslice.h>
#include <QtQuick/private/qquicktext_p.h>
#include <private/pierenderer_p.h>
#include <private/qabstractseries_p.h>
#include <private/qgraphsview_p.h>
#include <private/qpieseries_p.h>
#include <private/qpieslice_p.h>
#include <private/qquickshape_p.h>
#include <private/qquicksvgparser_p.h>

PieRenderer::PieRenderer(QGraphsView *graph)
    : QQuickItem(graph)
    , m_graph(graph)
    , m_painterPath()
{
    setFlag(QQuickItem::ItemHasContents);
    setClip(true);

    m_shape = new QQuickShape(this);
    m_shape->setParentItem(this);
    m_shape->setPreferredRendererType(QQuickShape::CurveRenderer);
}

PieRenderer::~PieRenderer() {}

void PieRenderer::setSize(QSizeF size)
{
    QQuickItem::setSize(size);
}

void PieRenderer::handlePolish(QPieSeries *series)
{
    for (QPieSlice *slice : series->slices()) {
        QPieSlicePrivate *d = slice->d_func();
        QQuickShapePath *shapePath = d->m_shapePath;
        QQuickShapePath *labelPath = d->m_labelPath;
        auto labelElements = labelPath->pathElements();
        auto pathElements = shapePath->pathElements();
        auto labelItem = d->m_labelItem;

        if (!m_activeSlices.contains(slice)) {
            auto data = m_shape->data();
            data.append(&data, shapePath);
            SliceData sliceData{};
            sliceData.initialized = false;
            m_activeSlices.insert(slice, sliceData);
        }

        QQuickShape *labelShape = d->m_labelShape;
        labelShape->setVisible(series->isVisible() && d->m_isLabelVisible);
        labelItem->setVisible(series->isVisible() && d->m_isLabelVisible);

        if (!series->isVisible()) {
            pathElements.clear(&pathElements);
            labelElements.clear(&labelElements);
            continue;
        }

        if (!shapePath->parent())
            shapePath->setParent(m_shape);

        if (!d->m_labelItem->parent()) {
            d->m_labelItem->setParent(this);
            d->m_labelItem->setParentItem(this);
        }

        if (!labelShape->parent()) {
            labelShape->setParent(this);
            labelShape->setParentItem(this);
        }
    }

    if (!series->isVisible())
        return;

    QPointF center = QPointF(size().width() * series->horizontalPosition(),
                             size().height() * series->verticalPosition());
    qreal radius = size().width() > size().height() ? size().height() : size().width();
    radius *= (.5 * series->pieSize());

    QGraphsTheme *theme = m_graph->theme();

    if (!theme)
        return;

    if (m_colorIndex < 0)
        m_colorIndex = m_graph->graphSeriesCount();
    m_graph->setGraphSeriesCount(m_colorIndex + series->slices().size());

    int sliceIndex = 0;
    QList<QLegendData> legendDataList;
    for (QPieSlice *slice : series->slices()) {
        m_painterPath.clear();

        QPieSlicePrivate *d = slice->d_func();

        // update slice
        QQuickShapePath *shapePath = d->m_shapePath;

        const auto &borderColors = theme->borderColors();
        int index = sliceIndex % borderColors.size();
        QColor borderColor = borderColors.at(index);
        if (d->m_borderColor.isValid())
            borderColor = d->m_borderColor;
        qreal borderWidth = theme->borderWidth();
        if (d->m_borderWidth > 0.0)
            borderWidth = d->m_borderWidth;
        const auto &seriesColors = theme->seriesColors();
        index = sliceIndex % seriesColors.size();
        QColor color = seriesColors.at(index);
        if (d->m_color.isValid())
            color = d->m_color;
        shapePath->setStrokeWidth(borderWidth);
        shapePath->setStrokeColor(borderColor);
        shapePath->setFillColor(color);

        if (!m_activeSlices.contains(slice))
            return;

        qreal radian = qDegreesToRadians(slice->startAngle());
        qreal startBigX = radius * qSin(radian);
        qreal startBigY = radius * qCos(radian);

        qreal startSmallX = startBigX * series->holeSize();
        qreal startSmallY = startBigY * series->holeSize();

        qreal explodeDistance = .0;
        if (slice->isExploded())
            explodeDistance = slice->explodeDistanceFactor() * radius;
        radian = qDegreesToRadians(slice->startAngle() + (slice->angleSpan() * .5));
        qreal xShift = center.x() + (explodeDistance * qSin(radian));
        qreal yShift = center.y() - (explodeDistance * qCos(radian));

        shapePath->setStartX(xShift + startBigX);
        shapePath->setStartY(yShift - startBigY);
        m_painterPath.moveTo(xShift + startBigX, yShift - startBigY);

        radian = qDegreesToRadians(slice->angleSpan());
        qreal pointX = startBigY * qSin(radian) + startBigX * qCos(radian);
        qreal pointY = startBigY * qCos(radian) - startBigX * qSin(radian);

        // Big arc
        auto start = m_painterPath.currentPosition();
        QQuickSvgParser::pathArc(m_painterPath,
                                 radius,
                                 radius,
                                 0,
                                 slice->angleSpan() > 180,
                                 1,
                                 xShift + pointX,
                                 yShift - pointY,
                                 start.x(),
                                 start.y());

        d->m_largeArc = {xShift + pointX, yShift - pointY};

        pointX = startSmallY * qSin(radian) + startSmallX * qCos(radian);
        pointY = startSmallY * qCos(radian) - startSmallX * qSin(radian);

        m_painterPath.lineTo(xShift + pointX, yShift - pointY);

        d->m_centerLine = {xShift + pointX, yShift - pointY};

        // Small arc
        start = m_painterPath.currentPosition();
        QQuickSvgParser::pathArc(m_painterPath,
                                 radius * series->holeSize(),
                                 radius * series->holeSize(),
                                 0,
                                 slice->angleSpan() > 180,
                                 0,
                                 xShift + startSmallX,
                                 yShift - startSmallY,
                                 start.x(),
                                 start.y());

        m_painterPath.lineTo(xShift + startBigX, yShift - startBigY);

        shapePath->setPath(m_painterPath);
        m_painterPath.clear();

        radian = qDegreesToRadians(slice->startAngle() + (slice->angleSpan() * .5));
        startBigX = radius * qSin(radian);
        startBigY = radius * qCos(radian);

        pointX = radius * (1.0 + d->m_labelArmLengthFactor) * qSin(radian);
        pointY = radius * (1.0 + d->m_labelArmLengthFactor) * qCos(radian);

        start = m_painterPath.currentPosition();
        m_painterPath.moveTo(xShift + startBigX, yShift - startBigY);
        m_painterPath.lineTo(xShift + pointX, yShift - pointY);

        d->m_labelArm = {xShift + pointX, yShift - pointY};

        auto labelWidth = radian > M_PI ? -d->m_labelItem->width() : d->m_labelItem->width();
        m_painterPath.lineTo(d->m_labelArm.x() + labelWidth, d->m_labelArm.y());

        d->setLabelPosition(d->m_labelPosition);
        d->m_labelPath->setPath(m_painterPath);

        sliceIndex++;
        legendDataList.push_back({color, borderColor, d->m_labelText});
    }
    series->d_func()->setLegendData(legendDataList);
}

void PieRenderer::afterPolish(QList<QAbstractSeries *> &cleanupSeries)
{
    for (auto series : cleanupSeries) {
        auto pieSeries = qobject_cast<QPieSeries *>(series);
        if (pieSeries) {
            for (QPieSlice *slice : pieSeries->slices()) {
                QPieSlicePrivate *d = slice->d_func();
                auto labelElements = d->m_labelPath->pathElements();
                auto shapeElements = d->m_shapePath->pathElements();

                labelElements.clear(&labelElements);
                shapeElements.clear(&shapeElements);

                slice->deleteLater();
                d->m_labelItem->deleteLater();

                m_activeSlices.remove(slice);
            }
        }
    }
}

void PieRenderer::updateSeries(QPieSeries *series)
{
    auto needPolish = false;

    for (auto &sliceData : m_activeSlices) {
        if (!sliceData.initialized) {
            sliceData.initialized = true;
            needPolish = true;
        }
    }

    if (needPolish)
        handlePolish(series);
}

void PieRenderer::afterUpdate(QList<QAbstractSeries *> &cleanupSeries)
{
    Q_UNUSED(cleanupSeries);
}

void PieRenderer::markedDeleted(QList<QPieSlice *> deleted)
{
    auto emptyPath = QPainterPath{};

    for (auto slice : deleted) {
        auto d = slice->d_func();
        d->m_shapePath->setPath(emptyPath);
        d->m_labelPath->setPath(emptyPath);
        d->m_labelItem->deleteLater();
        m_activeSlices.remove(slice);
    }
}
