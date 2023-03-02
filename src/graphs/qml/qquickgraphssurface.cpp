// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "qquickgraphssurface_p.h"
#include <QtCore/QMutexLocker>

#include "declarativescene_p.h"
#include "surface3dcontroller_p.h"
#include "surfaceselectioninstancing_p.h"
#include "qvalue3daxis_p.h"
#include "qcategory3daxis_p.h"

#include <QtQuick3D/private/qquick3dprincipledmaterial_p.h>
#include <QtQuick3D/private/qquick3ddefaultmaterial_p.h>

QT_BEGIN_NAMESPACE

QQuickGraphsSurface::QQuickGraphsSurface(QQuickItem *parent)
    : QQuickGraphsItem(parent),
      m_surfaceController(0)
{
    setAcceptedMouseButtons(Qt::AllButtons);

    // Create the shared component on the main GUI thread.
    m_surfaceController = new Surface3DController(boundingRect().toRect(), new Declarative3DScene);
    setSharedController(m_surfaceController);

    QObject::connect(m_surfaceController, &Surface3DController::selectedSeriesChanged,
                     this, &QQuickGraphsSurface::selectedSeriesChanged);
    QObject::connect(m_surfaceController, &Surface3DController::flipHorizontalGridChanged,
                     this, &QQuickGraphsSurface::flipHorizontalGridChanged);
}

QQuickGraphsSurface::~QQuickGraphsSurface()
{
    QMutexLocker locker(m_nodeMutex.data());
    const QMutexLocker locker2(mutex());
    delete m_surfaceController;
    for (auto model : m_model)
        delete model;
}

QValue3DAxis *QQuickGraphsSurface::axisX() const
{
    return static_cast<QValue3DAxis *>(m_surfaceController->axisX());
}

void QQuickGraphsSurface::setAxisX(QValue3DAxis *axis)
{
    m_surfaceController->setAxisX(axis);
}

QValue3DAxis *QQuickGraphsSurface::axisY() const
{
    return static_cast<QValue3DAxis *>(m_surfaceController->axisY());
}

void QQuickGraphsSurface::setAxisY(QValue3DAxis *axis)
{
    m_surfaceController->setAxisY(axis);
}

QValue3DAxis *QQuickGraphsSurface::axisZ() const
{
    return static_cast<QValue3DAxis *>(m_surfaceController->axisZ());
}

void QQuickGraphsSurface::setAxisZ(QValue3DAxis *axis)
{
    m_surfaceController->setAxisZ(axis);
}

void QQuickGraphsSurface::handleFlatShadingEnabledChanged()
{
    auto series = static_cast<QSurface3DSeries *>(sender());
    for (auto model : m_model) {
        if (model->series == series) {
            updateModel(model);
            break;
        }
    }
}

void QQuickGraphsSurface::handleWireframeColorChanged()
{
    for (auto model : m_model) {
        QQmlListReference gridMaterialRef(model->gridModel, "materials");
        auto gridMaterial = static_cast<QQuick3DPrincipledMaterial *>(gridMaterialRef.at(0));
        QColor gridColor = model->series->wireframeColor();
        gridMaterial->setBaseColor(gridColor);

        if (sliceView()) {
            QQmlListReference gridMaterialRef(model->sliceGridModel, "materials");
            auto gridMaterial = static_cast<QQuick3DPrincipledMaterial *>(gridMaterialRef.at(0));
            gridMaterial->setBaseColor(gridColor);
        }
    }
}

QSurface3DSeries *QQuickGraphsSurface::selectedSeries() const
{
    return m_surfaceController->selectedSeries();
}

void QQuickGraphsSurface::setFlipHorizontalGrid(bool flip)
{
    m_surfaceController->setFlipHorizontalGrid(flip);
}

bool QQuickGraphsSurface::flipHorizontalGrid() const
{
    return m_surfaceController->flipHorizontalGrid();
}

QQmlListProperty<QSurface3DSeries> QQuickGraphsSurface::seriesList()
{
    return QQmlListProperty<QSurface3DSeries>(this, this,
                                              &QQuickGraphsSurface::appendSeriesFunc,
                                              &QQuickGraphsSurface::countSeriesFunc,
                                              &QQuickGraphsSurface::atSeriesFunc,
                                              &QQuickGraphsSurface::clearSeriesFunc);
}

void QQuickGraphsSurface::appendSeriesFunc(QQmlListProperty<QSurface3DSeries> *list,
                                           QSurface3DSeries *series)
{
    reinterpret_cast<QQuickGraphsSurface *>(list->data)->addSeries(series);
}

qsizetype QQuickGraphsSurface::countSeriesFunc(QQmlListProperty<QSurface3DSeries> *list)
{
    return reinterpret_cast<QQuickGraphsSurface *>(list->data)->m_surfaceController->surfaceSeriesList().size();
}

QSurface3DSeries *QQuickGraphsSurface::atSeriesFunc(QQmlListProperty<QSurface3DSeries> *list,
                                                    qsizetype index)
{
    return reinterpret_cast<QQuickGraphsSurface *>(list->data)->m_surfaceController->surfaceSeriesList().at(index);
}

void QQuickGraphsSurface::clearSeriesFunc(QQmlListProperty<QSurface3DSeries> *list)
{
    QQuickGraphsSurface *declSurface = reinterpret_cast<QQuickGraphsSurface *>(list->data);
    QList<QSurface3DSeries *> realList = declSurface->m_surfaceController->surfaceSeriesList();
    int count = realList.size();
    for (int i = 0; i < count; i++)
        declSurface->removeSeries(realList.at(i));
}

void QQuickGraphsSurface::addSeries(QSurface3DSeries *series)
{
    m_surfaceController->addSeries(series);
    if (isReady())
        addModel(series);
}

void QQuickGraphsSurface::removeSeries(QSurface3DSeries *series)
{
    m_surfaceController->removeSeries(series);
    series->setParent(this); // Reparent as removing will leave series parentless
    for (int i = 0; i < m_model.size();) {
        if (m_model[i]->series == series) {
            m_model[i]->model->deleteLater();
            m_model[i]->gridModel->deleteLater();
            m_model.removeAt(i);
        } else {
            ++i;
        }
    }
}

void QQuickGraphsSurface::handleAxisXChanged(QAbstract3DAxis *axis)
{
    emit axisXChanged(static_cast<QValue3DAxis *>(axis));
}

void QQuickGraphsSurface::handleAxisYChanged(QAbstract3DAxis *axis)
{
    emit axisYChanged(static_cast<QValue3DAxis *>(axis));
}

void QQuickGraphsSurface::handleAxisZChanged(QAbstract3DAxis *axis)
{
    emit axisZChanged(static_cast<QValue3DAxis *>(axis));
}

void QQuickGraphsSurface::componentComplete()
{
    QQuickGraphsItem::componentComplete();

    createSliceView();

    for (auto series : m_surfaceController->surfaceSeriesList())
        addModel(series);

    QQuick3DNode *parent = rootNode();
    QQuick3DNode *sliceParent = sliceView()->scene();

    m_selectionPointer = new QQuick3DModel();
    m_selectionPointer->setParent(parent);
    m_selectionPointer->setParentItem(parent);
    m_selectionPointer->setSource(QUrl(QStringLiteral("#Sphere")));
    auto pointerMaterial = new QQuick3DPrincipledMaterial();
    pointerMaterial->setParent(this);
    pointerMaterial->setBaseColor(m_surfaceController->activeTheme()->singleHighlightColor());
    QQmlListReference materialRef(m_selectionPointer, "materials");
    materialRef.append(pointerMaterial);
    m_instancing = new SurfaceSelectionInstancing();
    m_instancing->setScale(QVector3D(0.001f, 0.001f, 0.001f));
    m_selectionPointer->setInstancing(m_instancing);

    m_sliceSelectionPointer = new QQuick3DModel();
    m_sliceSelectionPointer->setParent(sliceParent);
    m_sliceSelectionPointer->setParentItem(sliceParent);
    m_sliceSelectionPointer->setSource(QUrl(QStringLiteral("#Sphere")));
    pointerMaterial = new QQuick3DPrincipledMaterial();
    pointerMaterial->setParent(m_sliceSelectionPointer);
    pointerMaterial->setBaseColor(m_surfaceController->activeTheme()->singleHighlightColor());
    QQmlListReference sliceMaterialRef(m_sliceSelectionPointer, "materials");
    sliceMaterialRef.append(pointerMaterial);
    m_sliceInstancing = new SurfaceSelectionInstancing();
    m_sliceInstancing->setScale(QVector3D(0.001f, 0.001f, 0.001f));
    m_sliceSelectionPointer->setInstancing(m_sliceInstancing);

    setScaleWithBackground({2.0f, 1.0f, 2.0f});
    setBackgroundScaleMargin({0.1f, 0.1f, 0.1f});
    setScale({2.0f, 1.0f, 2.0f});
}

void QQuickGraphsSurface::synchData()
{
    QQuickGraphsItem::synchData();

    if (m_surfaceController->isSelectedPointChanged()) {
        if (m_surfaceController->selectionMode().testFlag(QAbstract3DGraph::SelectionItem))
            updateSelectedPoint();
        m_surfaceController->setSelectedPointChanged(false);
    }
}

inline static float getDataValue(const QSurfaceDataArray &array, bool searchRow, int index)
{
    if (searchRow)
        return array.at(0)->at(index).x();
    else
        return array.at(index)->at(0).z();
}

inline static int binarySearchArray(const QSurfaceDataArray &array, int maxIndex, float limitValue, bool searchRow, bool lowBound, bool ascending)
{
    int min = 0;
    int max = maxIndex;
    int mid = 0;
    int retVal;

    while (max >= min) {
        mid = (min + max) / 2;
        float arrayValue = getDataValue(array, searchRow, mid);
        if (arrayValue == limitValue)
            return mid;
        if (ascending) {
            if (arrayValue < limitValue)
                min = mid + 1;
            else
                max = mid -1;
        } else {
            if (arrayValue > limitValue)
                min = mid + 1;
            else
                max = mid - 1;
        }
    }

    if (lowBound == ascending) {
        if (mid > max)
            retVal = mid;
        else
            retVal = min;
    } else {
        if (mid > max)
            retVal = max;
        else
            retVal = mid;
    }

    if (retVal < 0 || retVal > maxIndex) {
        retVal = -1;
    } else if (lowBound) {
        if (getDataValue(array, searchRow, retVal) < limitValue)
            retVal = -1;
    } else {
        if (getDataValue(array, searchRow, retVal) > limitValue)
            retVal = -1;
    }
    return retVal;
}

void QQuickGraphsSurface::updateGraph()
{
    if (m_surfaceController->hasChangedSeriesList())
        handleChangedSeries();

    if (m_surfaceController->isSeriesVisibilityDirty()) {
        for (auto model : m_model) {
            bool visible = model->series->isVisible();
            if (visible != model->model->visible() && isSliceEnabled()) {
                setSliceEnabled(false);
                setSliceActivatedChanged(true);
            }
            if (!visible) {
                model->model->setVisible(visible);
                model->gridModel->setVisible(visible);
                if (sliceView()) {
                    model->sliceModel->setVisible(visible);
                    model->sliceGridModel->setVisible(visible);
                }
                continue;
            }
            model->gridModel->setVisible(model->series->drawMode().testFlag(QSurface3DSeries::DrawWireframe));
            model->model->setVisible(model->series->drawMode().testFlag(QSurface3DSeries::DrawSurface));
            if (isSliceEnabled()) {
                model->sliceGridModel->setVisible(model->series->drawMode().testFlag(QSurface3DSeries::DrawWireframe));
                model->sliceModel->setVisible(model->series->drawMode().testFlag(QSurface3DSeries::DrawSurface));
            }
        }

        if (m_surfaceController->selectionMode().testFlag(QAbstract3DGraph::SelectionItem))
            updateSelectedPoint();
    }

    if (m_surfaceController->isDataDirty()) {
        for (auto model : m_model) {
            bool visible = model->series->isVisible();
            if (visible)
                updateModel(model);
        }
    }
}

void QQuickGraphsSurface::handleChangedSeries()
{
    auto changedSeries = m_surfaceController->changedSeriesList();

    for (auto series : changedSeries) {
        for (auto model : m_model) {
            if (model->series == series) {
                updateModel(model);
            }
        }
    }
}

void QQuickGraphsSurface::updateModel(SurfaceModel *model)
{
    const QSurfaceDataArray &array = *(model->series->dataProxy())->array();

    // calculateSampleRect
    QRect sampleSpace;
    if (array.size() > 0) {
        if (array.size() >= 2 && array.at(0)->size() >= 2) {
            const int maxRow = array.size() - 1;
            const int maxColumn = array.at(0)->size() - 1;

            const bool ascendingX = array.at(0)->at(0).x() < array.at(0)->at(maxColumn).x();
            const bool ascendingZ = array.at(0)->at(0).z() < array.at(maxRow)->at(0).z();

            int idx = binarySearchArray(array, maxColumn, m_surfaceController->axisX()->min(), true, true, ascendingX);
            if (idx != -1) {
                if (ascendingX)
                    sampleSpace.setLeft(idx);
                else
                    sampleSpace.setRight(idx);
            } else {
                sampleSpace.setWidth(-1);
            }

            idx = binarySearchArray(array, maxColumn, m_surfaceController->axisX()->max(), true, false, ascendingX);
            if (idx != -1) {
                if (ascendingX)
                    sampleSpace.setRight(idx);
                else
                    sampleSpace.setLeft(idx);
            } else {
                sampleSpace.setWidth(-1); // to indicate nothing needs to be shown
            }

            idx = binarySearchArray(array, maxRow, m_surfaceController->axisZ()->min(), false, true, ascendingZ);
            if (idx != -1) {
                if (ascendingZ)
                    sampleSpace.setTop(idx);
                else
                    sampleSpace.setBottom(idx);
            } else {
                sampleSpace.setWidth(-1); // to indicate nothing needs to be shown
            }

            idx = binarySearchArray(array, maxRow, m_surfaceController->axisZ()->max(), false, false, ascendingZ);
            if (idx != -1) {
                if (ascendingZ)
                    sampleSpace.setBottom(idx);
                else
                    sampleSpace.setTop(idx);
            } else {
                sampleSpace.setWidth(-1); // to indicate nothing needs to be shown
            }
        }

        int rowCount = sampleSpace.height();
        int columnCount = sampleSpace.width();
        model->rowCount = rowCount;
        model->columnCount = columnCount;

        int totalSize = rowCount * columnCount * 2;
        float uvX = 1.0f / float(columnCount - 1);
        float uvY = 1.0f / float(rowCount - 1);

        // checkDirection
        int dataDimensions = Surface3DController::BothAscending;
        if (array.at(0)->at(0).x() > array.at(0)->at(array.at(0)->size() - 1).x())
            dataDimensions |= Surface3DController::XDescending;
        if (static_cast<QValue3DAxis *>(m_surfaceController->axisX())->reversed())
            dataDimensions ^= Surface3DController::XDescending;

        if (array.at(0)->at(0).z() > array.at(array.size() - 1)-> at(0).z())
            dataDimensions |= Surface3DController::ZDescending;
        if (static_cast<QValue3DAxis *>(m_surfaceController->axisZ())->reversed())
            dataDimensions ^= Surface3DController::ZDescending;

        m_surfaceController->setDataDimensions(static_cast<Surface3DController::DataDimensions>(dataDimensions));

        model->vertices.reserve(totalSize);

        bool isFlatShadingEnabled = model->series->isFlatShadingEnabled();

        QVector3D boundsMin(0.0f, 0.0f, 0.0f);
        QVector3D boundsMax(0.0f, 0.0f, 0.0f);

        model->vertices.clear();
        model->height.clear();
        for (int i = 0 ; i < rowCount ; i++) {
            const QSurfaceDataRow &row = *array.at(i);
            for (int j = 0 ; j < columnCount ; j++) {
                // getNormalizedVertex
                SurfaceVertex vertex;
                QVector3D pos = getNormalizedVertex(model, row.at(j), false, false);
                vertex.position = pos;
                vertex.normal = QVector3D(0, 0, 0);
                vertex.uv = QVector2D(j * uvX, i * uvY);
                vertex.coord = QPoint(i, j);
                model->vertices.push_back(vertex);
                if (boundsMin.isNull())
                    boundsMin = pos;
                else
                    boundsMin = QVector3D(qMin(boundsMin.x(), pos.x()), qMin(boundsMin.y(), pos.y()), qMin(boundsMin.z(), pos.z()));
                if (boundsMax.isNull())
                    boundsMax = pos;
                else
                    boundsMax = QVector3D(qMax(boundsMax.x(), pos.x()), qMax(boundsMax.y(), pos.y()), qMax(boundsMax.z(), pos.z()));
            }
        }

        //create normals
        int rowLimit = rowCount - 1;
        int colLimit = columnCount - 1;

        int totalIndex = 0;

        model->indices.clear();

        if (isFlatShadingEnabled) {
            model->coarceVertices.clear();

            createCoarseVertices(model, 0, 0, colLimit, rowLimit);
        } else {
            if (dataDimensions == Surface3DController::BothAscending || dataDimensions == Surface3DController::XDescending) {
                for (int row = 0 ; row < rowLimit ; row++)
                    createSmoothNormalBodyLine(model, totalIndex, row * columnCount);
                createSmoothNormalUpperLine(model, totalIndex);
            }
            else {
                createSmoothNormalUpperLine(model, totalIndex);
                for (int row = 1 ; row < rowCount ; row++)
                    createSmoothNormalBodyLine(model, totalIndex, row * columnCount);
            }

            createSmoothIndices(model, 0, 0, colLimit, rowLimit);
        }

        auto geometry = model->model->geometry();
        QByteArray vertexBuffer;
        if (isFlatShadingEnabled)
            vertexBuffer.setRawData(reinterpret_cast<char *>(model->coarceVertices.data()),
                                    model->coarceVertices.size() * sizeof(SurfaceVertex));
        else
            vertexBuffer.setRawData(reinterpret_cast<char *>(model->vertices.data()),
                                    model->vertices.size() * sizeof(SurfaceVertex));
        geometry->setVertexData(vertexBuffer);
        QByteArray indexBuffer(reinterpret_cast<char *>(model->indices.data()),
                               model->indices.size() * sizeof(quint32));
        geometry->setIndexData(indexBuffer);
        geometry->setBounds(boundsMin, boundsMax);
        geometry->update();

        updateMaterial(model);

        createGridlineIndices(model, 0, 0, colLimit, rowLimit);

        auto gridGeometry = model->gridModel->geometry();

        if (isFlatShadingEnabled)
            vertexBuffer.setRawData(reinterpret_cast<char *>(model->vertices.data()),
                                    model->vertices.size() * sizeof(SurfaceVertex));
        gridGeometry->setVertexData(vertexBuffer);
        QByteArray gridIndexBuffer(reinterpret_cast<char *>(model->gridIndices.data()),
                                   model->gridIndices.size() * sizeof(quint32));
        gridGeometry->setIndexData(gridIndexBuffer);
        gridGeometry->setBounds(boundsMin, boundsMax);
        gridGeometry->update();

        QQmlListReference gridMaterialRef(model->gridModel, "materials");
        auto gridMaterial = static_cast<QQuick3DPrincipledMaterial *>(gridMaterialRef.at(0));
        QColor gridColor = model->series->wireframeColor();
        gridMaterial->setBaseColor(gridColor);
    }
}

void QQuickGraphsSurface::updateMaterial(SurfaceModel *model)
{
    auto axisY = m_surfaceController->axisY();
    float maxY = axisY->max();
    float minY = axisY->min();
    QQmlListReference materialRef(model->model, "materials");
    auto material = static_cast<QQuick3DDefaultMaterial *>(materialRef.at(0));
    auto textureData = material->diffuseMap()->textureData();
    textureData->setSize(QSize(model->rowCount, model->columnCount));
    textureData->setFormat(QQuick3DTextureData::RGBA8);
    QByteArray imageData;
    imageData.resize(model->height.size() * 4);
    QLinearGradient gradient = model->series->baseGradient();
    auto stops = gradient.stops();
    for (int i = 0; i < model->height.size(); i++) {
        float height = model->height.at(i);
        float normalizedHeight = (height - minY) / (maxY - minY);
        for (int j = 0; j < stops.size(); j++) {
            if (normalizedHeight < stops.at(j).first ||
                    (normalizedHeight >= (float)stops.at(j).first && j == stops.size() - 1)) {
                QColor color;
                if (j == 0 || normalizedHeight >= (float)stops.at(j).first) {
                    color = stops.at(j).second;
                } else {
                    float normalLowerBound = stops.at(j - 1).first;
                    float normalUpperBound = stops.at(j).first;
                    normalizedHeight = (normalizedHeight - normalLowerBound) / (normalUpperBound - normalLowerBound);
                    QColor start = stops.at(j - 1).second;
                    QColor end = stops.at(j).second;
                    float red = start.redF() + ((end.redF() - start.redF()) * normalizedHeight);
                    float green = start.greenF() + ((end.greenF() - start.greenF()) * normalizedHeight);
                    float blue = start.blueF() + ((end.blueF() - start.blueF()) * normalizedHeight);
                    color.setRedF(red);
                    color.setGreenF(green);
                    color.setBlueF(blue);
                }
                imageData.data()[i * 4 + 0] = char(color.red());
                imageData.data()[i * 4 + 1] = char(color.green());
                imageData.data()[i * 4 + 2] = char(color.blue());
                imageData.data()[i * 4 + 3] = char(color.alpha());
                break;
            }
        }
    }
    textureData->setTextureData(imageData);

    QQmlListReference sliceMaterialRef(model->sliceModel, "materials");
    material = static_cast<QQuick3DDefaultMaterial *>(sliceMaterialRef.at(0));
    textureData = material->diffuseMap()->textureData();
    textureData->setSize(QSize(model->rowCount, model->columnCount));
    textureData->setFormat(QQuick3DTextureData::RGBA8);
    textureData->setTextureData(imageData);
}

QVector3D QQuickGraphsSurface::getNormalizedVertex(SurfaceModel *model, const QSurfaceDataItem &data, bool polar, bool flipXZ)
{
    Q_UNUSED(polar);
    Q_UNUSED(flipXZ);

    float normalizedX;
    float normalizedY;
    float normalizedZ;
    QValue3DAxis* axisX = static_cast<QValue3DAxis *>(m_surfaceController->axisX());
    QValue3DAxis* axisY = static_cast<QValue3DAxis *>(m_surfaceController->axisY());
    QValue3DAxis* axisZ = static_cast<QValue3DAxis *>(m_surfaceController->axisZ());
    // TODO : Need to handle polar, flipXZ
    float scale, translate;
    scale = translate = this->scale().x();
    normalizedX = axisX->positionAt(data.x()) * scale * 2.0f - translate;
    scale = translate = this->scale().y();
    model->height.push_back(data.y());
    normalizedY = axisY->positionAt(data.y()) * scale * 2.0f - translate;
    scale = translate = this->scale().z();
    normalizedZ = axisZ->positionAt(data.z()) * -scale * 2.0f + translate;
    return QVector3D(normalizedX, normalizedY, normalizedZ);
}

inline static QVector3D normal(const QVector3D &a, const QVector3D &b, const QVector3D &c)
{
    QVector3D v1 = b - a;
    QVector3D v2 = c - a;
    QVector3D normal = QVector3D::crossProduct(v1, v2);

    return normal;
}

void QQuickGraphsSurface::updateSliceGraph()
{
    QQuickGraphsItem::updateSliceGraph();

    if (!sliceView()->isVisible())
        return;

    auto selectionMode = m_surfaceController->selectionMode();

    for (auto model : m_model) {
        if (!model->series->isVisible())
            continue;

        QVector<SurfaceVertex> selectedSeries;

        if (selectionMode.testFlag(QAbstract3DGraph::SelectionRow)) {
            int selectedRow = model->selectedVertex.coord.x() * model->columnCount;
            selectedSeries.reserve(model->rowCount * 2);
            QVector<SurfaceVertex> list;
            for (int i = 0; i < model->rowCount; i++) {
                SurfaceVertex vertex = model->vertices.at(selectedRow + i);
                vertex.normal = QVector3D(.0f, .0f, 1.f);
                vertex.position.setY(vertex.position.y() - .025f);
                vertex.position.setZ(.0f);
                selectedSeries.append(vertex);
                vertex.position.setY(vertex.position.y() + .05f);
                list.append(vertex);
            }
            selectedSeries.append(list);
        }

        if (selectionMode.testFlag(QAbstract3DGraph::SelectionColumn)) {
            int selectedColumn = model->selectedVertex.coord.y();
            selectedSeries.reserve(model->columnCount * 2);
            QVector<SurfaceVertex> list;
            for (int i = 0; i < model->columnCount; i++) {
                SurfaceVertex vertex = model->vertices.at((i * model->rowCount) + selectedColumn);
                vertex.normal = QVector3D(.0f, .0f, -1.0f);
                vertex.position.setX(vertex.position.z());
                vertex.position.setY(vertex.position.y() - .025f);
                vertex.position.setZ(0);
                selectedSeries.append(vertex);
                vertex.position.setY(vertex.position.y() + .05f);
                list.append(vertex);
            }
            selectedSeries.append(list);
        }

        int cnt = model->rowCount - 1;
        QVector<quint32> indices;
        indices.reserve(cnt * 6);
        for (int i = 0; i < cnt; i++) {
            indices.push_back(i + 1);
            indices.push_back(i + cnt + 1);
            indices.push_back(i);

            indices.push_back(i + cnt + 2);
            indices.push_back(i + cnt + 1);
            indices.push_back(i + 1);
        }

        auto geometry = model->sliceModel->geometry();
        QByteArray vertexBuffer(reinterpret_cast<char *>(selectedSeries.data()),
                                selectedSeries.size() * sizeof(SurfaceVertex));
        geometry->setVertexData(vertexBuffer);
        QByteArray indexBuffer(reinterpret_cast<char *>(indices.data()),
                               indices.size() * sizeof(quint32));
        geometry->setIndexData(indexBuffer);
        geometry->update();

        geometry = model->sliceGridModel->geometry();
        geometry->setVertexData(vertexBuffer);

        QVector<quint32> gridIndices;
        gridIndices.reserve(cnt * 4);
        for (int i = 0; i < cnt; i++) {
            gridIndices.push_back(i);
            gridIndices.push_back(i + cnt + 1);

            gridIndices.push_back(i);
            gridIndices.push_back(i + 1);
        }
        QByteArray gridIndexBuffer(reinterpret_cast<char *>(gridIndices.data()),
                                   gridIndices.size() * sizeof(quint32));
        geometry->setIndexData(gridIndexBuffer);
        geometry->update();

        QQmlListReference gridMaterialRef(model->sliceGridModel, "materials");
        auto gridMaterial = static_cast<QQuick3DPrincipledMaterial *>(gridMaterialRef.at(0));
        QColor gridColor = model->series->wireframeColor();
        gridMaterial->setBaseColor(gridColor);
    }
}

void QQuickGraphsSurface::createSmoothNormalBodyLine(SurfaceModel *model, int &totalIndex, int column)
{
    int columnCount = model->columnCount;
    int colLimit = columnCount - 1;
    Surface3DController::DataDimensions dataDimensions = m_surfaceController->dataDimensions();
    if (dataDimensions == Surface3DController::BothAscending) {
        int end = colLimit + column;
        for (int j = column ; j < end ; j++) {
            SurfaceVertex vertex = model->vertices.at(totalIndex);
            vertex.normal = normal(model->vertices.at(j).position,
                                   model->vertices.at(j + 1).position,
                                   model->vertices.at(j + columnCount).position);
            model->vertices.replace(totalIndex++, vertex);
        }
        SurfaceVertex vertex = model->vertices.at(totalIndex);
        vertex.normal = normal(model->vertices.at(end).position,
                               model->vertices.at(end + columnCount).position,
                               model->vertices.at(end - 1).position);
        model->vertices.replace(totalIndex++, vertex);
    } else if (dataDimensions == Surface3DController::XDescending) {
        SurfaceVertex vertex = model->vertices.at(totalIndex);
        vertex.normal = normal(model->vertices.at(column).position,
                               model->vertices.at(column + columnCount).position,
                               model->vertices.at(column + 1).position);
        model->vertices.replace(totalIndex++, vertex);
        int end = column + columnCount;
        for (int j = column + 1 ; j < end ; j++) {
            SurfaceVertex vertex = model->vertices.at(totalIndex);
            vertex.normal = normal(model->vertices.at(j).position,
                                   model->vertices.at(j - 1).position,
                                   model->vertices.at(j + columnCount).position);
            model->vertices.replace(totalIndex++, vertex);
        }
    } else if (dataDimensions == Surface3DController::ZDescending) {
        int end = colLimit + column;
        for (int j = column; j < end ; j++) {
            SurfaceVertex vertex = model->vertices.at(totalIndex);
            vertex.normal = normal(model->vertices.at(j).position,
                                   model->vertices.at(j + 1).position,
                                   model->vertices.at(j - columnCount).position);
            model->vertices.replace(totalIndex++, vertex);
        }
        SurfaceVertex vertex = model->vertices.at(totalIndex);
        vertex.normal = normal(model->vertices.at(end).position,
                               model->vertices.at(end - columnCount).position,
                               model->vertices.at(end - 1).position);
        model->vertices.replace(totalIndex++, vertex);
    } else {
        SurfaceVertex vertex = model->vertices.at(totalIndex);
        vertex.normal = normal(model->vertices.at(column).position,
                               model->vertices.at(column - columnCount).position,
                               model->vertices.at(column + 1).position);
        model->vertices.replace(totalIndex++, vertex);
        int end = column + columnCount;
        for (int j = 0 ; j < end ; j++) {
            SurfaceVertex vertex = model->vertices.at(totalIndex);
            vertex.normal = normal(model->vertices.at(j).position,
                                   model->vertices.at(j-1).position,
                                   model->vertices.at(j - columnCount).position);
            model->vertices.replace(totalIndex++, vertex);
        }
    }
}

void QQuickGraphsSurface::createSmoothNormalUpperLine(SurfaceModel *model, int &totalIndex)
{
    int columnCount = model->columnCount;
    int rowCount = model->rowCount;
    Surface3DController::DataDimensions dataDimensions = m_surfaceController->dataDimensions();

    if (dataDimensions == Surface3DController::BothAscending) {
        int lineEnd = rowCount * columnCount - 1;
        for (int j = (rowCount - 1) * columnCount; j < lineEnd; j++) {
            SurfaceVertex vertex = model->vertices.at(totalIndex);
            vertex.normal = normal(model->vertices.at(j).position,
                                   model->vertices.at(j - columnCount).position,
                                   model->vertices.at(j + 1).position);
            model->vertices.replace(totalIndex++, vertex);
        }
        SurfaceVertex vertex = model->vertices.at(totalIndex);
        vertex.normal = normal(model->vertices.at(lineEnd).position,
                               model->vertices.at(lineEnd - 1).position,
                               model->vertices.at(lineEnd - columnCount).position);
        model->vertices.replace(totalIndex++, vertex);
    } else if (dataDimensions == Surface3DController::XDescending) {
        int lineStart = (rowCount - 1) * columnCount;
        int lineEnd = rowCount * columnCount;
        SurfaceVertex vertex = model->vertices.at(totalIndex);
        vertex.normal = normal(model->vertices.at(lineStart).position,
                               model->vertices.at(lineStart + 1).position,
                               model->vertices.at(lineStart - columnCount).position);
        model->vertices.replace(totalIndex++, vertex);
        for (int j = lineStart + 1; j < lineEnd; j++) {
            SurfaceVertex vertex = model->vertices.at(totalIndex);
            vertex.normal = normal(model->vertices.at(j).position,
                                   model->vertices.at(j - columnCount).position,
                                   model->vertices.at(j - 1).position);
            model->vertices.replace(totalIndex++, vertex);
        }
    } else if (dataDimensions == Surface3DController::ZDescending) {
        int colLimit = columnCount - 1;
        for (int j = 0; j < colLimit; j++) {
            SurfaceVertex vertex = model->vertices.at(totalIndex);
            vertex.normal = normal(model->vertices.at(j).position,
                                   model->vertices.at(j + columnCount).position,
                                   model->vertices.at(j + 1).position);
            model->vertices.replace(totalIndex++, vertex);
        }
        SurfaceVertex vertex = model->vertices.at(totalIndex);
        vertex.normal = normal(model->vertices.at(colLimit).position,
                               model->vertices.at(colLimit - 1).position,
                               model->vertices.at(colLimit + columnCount).position);
        model->vertices.replace(totalIndex++, vertex);
    } else { // BothDescending
        SurfaceVertex vertex = model->vertices.at(totalIndex);
        vertex.normal = normal(model->vertices.at(0).position,
                               model->vertices.at(1).position,
                               model->vertices.at(columnCount).position);
        model->vertices.replace(totalIndex++, vertex);
        for (int j = 1; j < columnCount; j++) {
            SurfaceVertex vertex = model->vertices.at(totalIndex);
            vertex.normal = normal(model->vertices.at(j).position,
                                   model->vertices.at(j + columnCount).position,
                                   model->vertices.at(j - 1).position);
            model->vertices.replace(totalIndex++, vertex);
        }
    }
}

void QQuickGraphsSurface::createSmoothIndices(SurfaceModel *model, int x, int y, int endX, int endY)
{
    int columnCount = model->columnCount;
    int rowCount = model->rowCount;
    Surface3DController::DataDimensions dataDimensions = m_surfaceController->dataDimensions();

    if (endX >= columnCount)
        endX = columnCount - 1;
    if (endY >= rowCount)
        endY = rowCount - 1;
    if (x > endX)
        x = endX - 1;
    if (y > endY)
        y = endY - 1;

    int indexCount = 6 * (endX - x) * (endY - y);

    QVector<quint32> *indices = &model->indices;

    indices->clear();
    indices->resize(indexCount);

    int rowEnd = endY * columnCount;
    for (int row = y * columnCount ; row < rowEnd ; row += columnCount) {
        for (int j = x ; j < endX ; j++) {
            if (dataDimensions == Surface3DController::BothAscending
                    || dataDimensions == Surface3DController::BothDescending) {
                indices->push_back(row + j + 1);
                indices->push_back(row + columnCount + j);
                indices->push_back(row + j);

                indices->push_back(row + columnCount + j + 1);
                indices->push_back(row + columnCount + j);
                indices->push_back(row + j + 1);
            } else if (dataDimensions == Surface3DController::XDescending) {
                indices->push_back(row + columnCount + j);
                indices->push_back(row + columnCount + j + 1);
                indices->push_back(row + j);

                indices->push_back(row + j);
                indices->push_back(row + columnCount + j + 1);
                indices->push_back(row + j + 1);
            } else {
                indices->push_back(row + columnCount + j);
                indices->push_back(row + columnCount + j + 1);
                indices->push_back(row + j + 1);

                indices->push_back(row + j);
                indices->push_back(row + columnCount + j + 1);
                indices->push_back(row + j + 1);
            }
        }
    }
}

void QQuickGraphsSurface::createCoarseVertices(SurfaceModel *model, int x, int y, int endX, int endY)
{
    int columnCount = model->columnCount;
    int rowCount = model->rowCount;
    Surface3DController::DataDimensions dataDimensions = m_surfaceController->dataDimensions();

    if (endX >= columnCount)
        endX = columnCount - 1;
    if (endY >= rowCount)
        endY = rowCount - 1;
    if (x > endX)
        x = endX - 1;
    if (y > endY)
        y = endY - 1;

    int indexCount = 6 * (endX - x) * (endY - y);
    model->indices.clear();
    model->indices.resize(indexCount);

    int index = 0;
    int rowEnd = endY * columnCount;

    int i1, i2, i3;
    SurfaceVertex v1, v2, v3;
    QVector3D normalVector;

    for (int row = y * columnCount; row < rowEnd; row += columnCount) {
        for (int j = x; j < endX; j++) {
            if (dataDimensions == Surface3DController::BothAscending
                    || dataDimensions == Surface3DController::BothDescending) {
                i1 = row + j + 1, i2 = row + columnCount + j, i3 = row + j;
                v1 = model->vertices.at(i1);
                v2 = model->vertices.at(i2);
                v3 = model->vertices.at(i3);
                normalVector = normal(v1.position, v2.position, v3.position);
                v1.normal = normalVector;
                v2.normal = normalVector;
                v3.normal = normalVector;
                model->coarceVertices.push_back(v1);
                model->coarceVertices.push_back(v2);
                model->coarceVertices.push_back(v3);
                model->indices.push_back(index++);
                model->indices.push_back(index++);
                model->indices.push_back(index++);

                i1 = row + columnCount + j + 1, i2 = row + columnCount + j, i3 = row + j + 1;
                v1 = model->vertices.at(i1);
                v2 = model->vertices.at(i2);
                v3 = model->vertices.at(i3);
                normalVector = normal(v1.position, v2.position, v3.position);
                v1.normal = normalVector;
                v2.normal = normalVector;
                v3.normal = normalVector;
                model->coarceVertices.push_back(v1);
                model->coarceVertices.push_back(v2);
                model->coarceVertices.push_back(v3);
                model->indices.push_back(index++);
                model->indices.push_back(index++);
                model->indices.push_back(index++);
            } else if (dataDimensions == Surface3DController::XDescending) {
                i1 = row + columnCount + j, i2 = row + columnCount + j + 1, i3 = row + j;
                v1 = model->vertices.at(i1);
                v2 = model->vertices.at(i2);
                v3 = model->vertices.at(i3);
                normalVector = normal(v1.position, v2.position, v3.position);
                v1.normal = normalVector;
                v2.normal = normalVector;
                v3.normal = normalVector;
                model->coarceVertices.push_back(v1);
                model->coarceVertices.push_back(v2);
                model->coarceVertices.push_back(v3);
                model->indices.push_back(index++);
                model->indices.push_back(index++);
                model->indices.push_back(index++);

                i1 = row + j, i2 = row + columnCount + j + 1, i3 = row + j + 1;
                v1 = model->vertices.at(i1);
                v2 = model->vertices.at(i2);
                v3 = model->vertices.at(i3);
                normalVector = normal(v1.position, v2.position, v3.position);
                v1.normal = normalVector;
                v2.normal = normalVector;
                v3.normal = normalVector;
                model->coarceVertices.push_back(v1);
                model->coarceVertices.push_back(v2);
                model->coarceVertices.push_back(v3);
                model->indices.push_back(index++);
                model->indices.push_back(index++);
                model->indices.push_back(index++);
            } else {
                i1 = row + columnCount + j, i2 = row + columnCount + j + 1, i3 = row + j + 1;
                v1 = model->vertices.at(i1);
                v2 = model->vertices.at(i2);
                v3 = model->vertices.at(i3);
                normalVector = normal(v1.position, v2.position, v3.position);
                v1.normal = normalVector;
                v2.normal = normalVector;
                v3.normal = normalVector;
                model->coarceVertices.push_back(v1);
                model->coarceVertices.push_back(v2);
                model->coarceVertices.push_back(v3);
                model->indices.push_back(index++);
                model->indices.push_back(index++);
                model->indices.push_back(index++);

                i1 = row + j, i2 = row + columnCount + j + 1, i3 = row + j + 1;
                v1 = model->vertices.at(i1);
                v2 = model->vertices.at(i2);
                v3 = model->vertices.at(i3);
                normalVector = normal(v1.position, v2.position, v3.position);
                v1.normal = normalVector;
                v2.normal = normalVector;
                v3.normal = normalVector;
                model->coarceVertices.push_back(v1);
                model->coarceVertices.push_back(v2);
                model->coarceVertices.push_back(v3);
                model->indices.push_back(index++);
                model->indices.push_back(index++);
                model->indices.push_back(index++);
            }
        }
    }
}

void QQuickGraphsSurface::createGridlineIndices(SurfaceModel *model, int x, int y, int endX, int endY)
{
    int columnCount = model->columnCount;
    int rowCount = model->rowCount;

    if (endX >= columnCount)
        endX = columnCount - 1;
    if (endY >= rowCount)
        endY = rowCount - 1;
    if (x > endX)
        x = endX - 1;
    if (y > endY)
        y = endY - 1;

    int nColumns = endX - x + 1;
    int nRows = endY - y + 1;

    int gridIndexCount = 2 * nColumns * (nRows - 1) + 2 * nRows * (nColumns - 1);
    model->gridIndices.resize(gridIndexCount);
    model->gridIndices.clear();

    for (int i = y, row = columnCount * y ; i <= endY ; i++, row += columnCount) {
        for (int j = x ; j < endX ; j++) {
            model->gridIndices.push_back(row + j);
            model->gridIndices.push_back(row + j + 1);
        }
    }
    for (int i = y, row = columnCount * y ; i < endY ; i++, row += columnCount) {
        for (int j = x ; j <= endX ; j++) {
            model->gridIndices.push_back(row + j);
            model->gridIndices.push_back(row + j + columnCount);
        }
    }
}

bool QQuickGraphsSurface::handleMousePressedEvent(QMouseEvent *event)
{
    if (!QQuickGraphsItem::handleMousePressedEvent(event))
        return true;

    if (Qt::LeftButton == event->button()) {
        auto mousePos = event->pos();
        auto pickResult = pickAll(mousePos.x(), mousePos.y());
        QVector3D pickedPos(0.0f, 0.0f, 0.0f);
        QQuick3DModel *pickedModel = nullptr;

        auto selectionMode = m_surfaceController->selectionMode();
        if (!selectionMode.testFlag(QAbstract3DGraph::SelectionNone)) {
            for (auto picked : pickResult) {
                if (picked.objectHit()->objectName().contains(QStringLiteral("SurfaceModel"))) {
                    pickedPos = picked.position();
                    pickedModel = picked.objectHit();
                    break;
                }
            }

            if (!pickedPos.isNull()) {
                float min = -1.0f;
                SurfaceVertex selectedVertex;

                for (auto model : m_model) {
                    if (model->model == pickedModel) {
                        model->picked = true;
                        for (auto vertex : model->vertices) {
                            QVector3D pos = vertex.position;
                            float dist = pickedPos.distanceToPoint(pos);
                            if (selectedVertex.position.isNull() || dist < min) {
                                min = dist;
                                selectedVertex = vertex;
                            }
                        }
                    } else {
                        model->picked = false;
                    }
                }

                for (auto model : m_model) {
                    if (model->picked)
                        model->selectedVertex = selectedVertex;
                    else
                        model->selectedVertex = SurfaceVertex();

                    if (selectionMode.testFlag(QAbstract3DGraph::SelectionMultiSeries)) {
                        if (model->picked) {
                            model->selectedVertex = selectedVertex;
                        } else {
                            QPoint coord = selectedVertex.coord;
                            int index = coord.x() * model->rowCount + coord.y();
                            auto vertex = model->vertices.at(index);
                            model->selectedVertex = vertex;
                        }
                    }

                    if (!selectedVertex.position.isNull()
                            && model->picked) {
                        model->series->setSelectedPoint(selectedVertex.coord);
                        if (isSliceEnabled()) {
                            m_surfaceController->setSlicingActive(true);
                            setSliceActivatedChanged(true);
                        }
                    }
                }
            }
        }
    }

    return true;
}

void QQuickGraphsSurface::updateSelectedPoint()
{
    bool labelVisible = false;
    m_instancing->resetPositions();
    if (isSliceEnabled())
        m_sliceInstancing->resetPositions();
    for (auto model : m_model) {
        SurfaceVertex selectedVertex = model->selectedVertex;
        if (model->series->isVisible() &&
                !selectedVertex.position.isNull()) {
            m_instancing->addPosition(selectedVertex.position);
            if (isSliceEnabled()) {
                QVector3D slicePosition = selectedVertex.position;
                if (m_surfaceController->selectionMode().testFlag(QAbstract3DGraph::SelectionColumn))
                    slicePosition.setX(slicePosition.z());
                slicePosition.setZ(.0f);
                m_sliceInstancing->addPosition(slicePosition);
            }
            if (model->picked) {
                const QSurfaceDataArray &array = *(model->series->dataProxy())->array();
                const QSurfaceDataRow &rowArray = *array.at(selectedVertex.coord.x());
                QVector3D value = rowArray.at(selectedVertex.coord.y()).position();
                QVector3D labelPosition = selectedVertex.position;
                QString x = static_cast<QValue3DAxis *>(m_surfaceController->axisX())->stringForValue(value.x());
                QString y = static_cast<QValue3DAxis *>(m_surfaceController->axisY())->stringForValue(value.y());
                QString z = static_cast<QValue3DAxis *>(m_surfaceController->axisZ())->stringForValue(value.z());
                QString label = x + QStringLiteral(", ") +
                        y + QStringLiteral(", ") +
                        z;
                itemLabel()->setPosition(labelPosition);
                itemLabel()->setProperty("labelText", label);
                itemLabel()->setEulerRotation(QVector3D(
                                                  -m_surfaceController->scene()->activeCamera()->yRotation(),
                                                  -m_surfaceController->scene()->activeCamera()->xRotation(),
                                                  0));
                labelVisible = true;

                if (isSliceEnabled()) {
                    labelPosition.setZ(.1f);
                    labelPosition.setY(labelPosition.y() + .05f);
                    sliceItemLabel()->setPosition(labelPosition);
                    sliceItemLabel()->setProperty("labelText", label);
                }
            }
        }
    }
    itemLabel()->setVisible(labelVisible);
    if (isSliceEnabled())
        sliceItemLabel()->setVisible(labelVisible);
}

void QQuickGraphsSurface::addModel(QSurface3DSeries *series)
{
    auto scene = QQuick3DViewport::scene();
    QQuick3DViewport *sliceParent = sliceView();
    bool visible = series->isVisible();

    auto model = new QQuick3DModel();
    model->setParent(scene);
    model->setParentItem(scene);
    model->setObjectName(QStringLiteral("SurfaceModel"));
    model->setVisible(visible);
    if (m_surfaceController->selectionMode().testFlag(QAbstract3DGraph::SelectionNone))
        model->setPickable(false);
    else
        model->setPickable(true);

    auto geometry = new QQuick3DGeometry();
    geometry->setParent(this);
    geometry->setStride(sizeof(SurfaceVertex));
    geometry->setPrimitiveType(QQuick3DGeometry::PrimitiveType::Triangles);
    geometry->addAttribute(QQuick3DGeometry::Attribute::PositionSemantic,
                           0,
                           QQuick3DGeometry::Attribute::F32Type);
    geometry->addAttribute(QQuick3DGeometry::Attribute::TexCoord0Semantic,
                           sizeof(QVector3D) * 2,
                           QQuick3DGeometry::Attribute::F32Type);
    geometry->addAttribute(QQuick3DGeometry::Attribute::NormalSemantic,
                           sizeof(QVector3D),
                           QQuick3DGeometry::Attribute::F32Type);
    geometry->addAttribute(QQuick3DGeometry::Attribute::IndexSemantic,
                           0,
                           QQuick3DGeometry::Attribute::U32Type);
    model->setGeometry(geometry);

    QQmlListReference materialRef(model, "materials");
    auto material = new QQuick3DDefaultMaterial();
    material->setParent(this);
    QQuick3DTexture *texture = new QQuick3DTexture();
    texture->setParent(this);
    QQuick3DTextureData *textureData = new QQuick3DTextureData();
    textureData->setParent(this);
    texture->setTextureData(textureData);
    material->setDiffuseMap(texture);
    material->setSpecularAmount(7.0f);
    material->setSpecularRoughness(0.025f);
    material->setCullMode(QQuick3DMaterial::NoCulling);
    materialRef.append(material);

    auto gridModel = new QQuick3DModel();
    gridModel->setParent(scene);
    gridModel->setParentItem(scene);
    gridModel->setVisible(visible);
    gridModel->setDepthBias(1.0f);
    auto gridGeometry = new QQuick3DGeometry();
    gridGeometry->setParent(this);
    gridGeometry->setStride(sizeof(SurfaceVertex));
    gridGeometry->setPrimitiveType(QQuick3DGeometry::PrimitiveType::Lines);
    gridGeometry->addAttribute(QQuick3DGeometry::Attribute::PositionSemantic,
                               0,
                               QQuick3DGeometry::Attribute::F32Type);
    gridGeometry->addAttribute(QQuick3DGeometry::Attribute::IndexSemantic,
                               0,
                               QQuick3DGeometry::Attribute::U32Type);
    gridModel->setGeometry(gridGeometry);
    QQmlListReference gridMaterialRef(gridModel, "materials");
    auto gridMaterial = new QQuick3DPrincipledMaterial();
    gridMaterial->setParent(this);
    gridMaterial->setLighting(QQuick3DPrincipledMaterial::NoLighting);
    gridMaterial->setParent(this);
    gridMaterialRef.append(gridMaterial);

    SurfaceModel *surfaceModel = new SurfaceModel();
    surfaceModel->model = model;
    surfaceModel->gridModel = gridModel;
    surfaceModel->series = series;

    model = new QQuick3DModel();
    model->setParent(sliceParent->scene());
    model->setParentItem(sliceParent->scene());
    model->setVisible(visible);
    model->setDepthBias(1.f);

    geometry = new QQuick3DGeometry();
    geometry->setParent(model);
    geometry->setParentItem(model);
    geometry->setStride(sizeof(SurfaceVertex));
    geometry->setPrimitiveType(QQuick3DGeometry::PrimitiveType::Triangles);
    geometry->addAttribute(QQuick3DGeometry::Attribute::PositionSemantic,
                           0,
                           QQuick3DGeometry::Attribute::F32Type);
    geometry->addAttribute(QQuick3DGeometry::Attribute::TexCoord0Semantic,
                           sizeof(QVector3D) * 2,
                           QQuick3DGeometry::Attribute::F32Type);
    geometry->addAttribute(QQuick3DGeometry::Attribute::NormalSemantic,
                           sizeof(QVector3D),
                           QQuick3DGeometry::Attribute::F32Type);
    geometry->addAttribute(QQuick3DGeometry::Attribute::IndexSemantic,
                           0,
                           QQuick3DGeometry::Attribute::U32Type);
    model->setGeometry(geometry);

    materialRef = QQmlListReference(model, "materials");
    material = new QQuick3DDefaultMaterial();
    material->setParent(model);
    material->setParentItem(model);
    texture = new QQuick3DTexture();
    texture->setParent(model);
    texture->setParent(model);
    textureData = new QQuick3DTextureData();
    textureData->setParent(model);
    textureData->setParentItem(model);
    texture->setTextureData(textureData);
    material->setDiffuseMap(texture);
    material->setSpecularAmount(.1f);
    material->setSpecularRoughness(0.025f);
    material->setCullMode(QQuick3DMaterial::NoCulling);
    materialRef.append(material);

    surfaceModel->sliceModel = model;

    gridModel = new QQuick3DModel();
    gridModel->setParent(sliceParent->scene());
    gridModel->setParentItem(sliceParent->scene());
    gridModel->setVisible(visible);
    gridModel->setDepthBias(1.0f);
    gridGeometry = new QQuick3DGeometry();
    gridGeometry->setParent(gridModel);
    gridGeometry->setStride(sizeof(SurfaceVertex));
    gridGeometry->setPrimitiveType(QQuick3DGeometry::PrimitiveType::Lines);
    gridGeometry->addAttribute(QQuick3DGeometry::Attribute::PositionSemantic,
                               0,
                               QQuick3DGeometry::Attribute::F32Type);
    gridGeometry->addAttribute(QQuick3DGeometry::Attribute::IndexSemantic,
                               0,
                               QQuick3DGeometry::Attribute::U32Type);
    gridModel->setGeometry(gridGeometry);
    gridMaterialRef = QQmlListReference(gridModel, "materials");
    gridMaterial = new QQuick3DPrincipledMaterial();
    gridMaterial->setParent(gridModel);
    gridMaterial->setLighting(QQuick3DPrincipledMaterial::NoLighting);
    gridMaterial->setParent(gridModel);
    gridMaterialRef.append(gridMaterial);

    surfaceModel->sliceGridModel = gridModel;

    m_model.push_back(surfaceModel);

    connect(series,
            &QSurface3DSeries::flatShadingEnabledChanged,
            this,
            &QQuickGraphsSurface::handleFlatShadingEnabledChanged);
    connect(series,
            &QSurface3DSeries::wireframeColorChanged,
            this,
            &QQuickGraphsSurface::handleWireframeColorChanged);
}

void QQuickGraphsSurface::updateSingleHighlightColor()
{
    m_instancing->setColor(m_surfaceController->activeTheme()->singleHighlightColor());
    if (sliceView())
        m_sliceInstancing->setColor(m_surfaceController->activeTheme()->singleHighlightColor());
}

void QQuickGraphsSurface::handleThemeTypeChange()
{
    for (auto model : m_model)
        updateMaterial(model);
}

QT_END_NAMESPACE
