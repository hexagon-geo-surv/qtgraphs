// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "qabstract3dseries_p.h"
#include "qabstractdataproxy_p.h"
#include "abstract3dcontroller_p.h"
#include "utils_p.h"

QT_BEGIN_NAMESPACE

/*!
 * \class QAbstract3DSeries
 * \inmodule QtGraphs
 * \brief The QAbstract3DSeries class is a base class for all data series.
 *
 * There are inherited classes for each supported series type: QBar3DSeries,
 * QScatter3DSeries, and QSurface3DSeries.
 *
 * For more information, see \l{Qt Graphs Data Handling}.
 */

/*!
 * \class QAbstract3DSeriesChangeBitField
 * \internal
 */

/*!
 * \class QAbstract3DSeriesThemeOverrideBitField
 * \internal
 */

/*!
 * \qmltype Abstract3DSeries
 * \inqmlmodule QtGraphs
 * \ingroup graphs_qml
 * \instantiates QAbstract3DSeries
 * \brief A base type for all data series.
 *
 * This type is uncreatable, but contains properties that are exposed via the
 * following subtypes: Bar3DSeries, Scatter3DSeries, and Surface3DSeries.
 *
 * For more information, see \l{Qt Graphs Data Handling}.
 */

/*!
 * \enum QAbstract3DSeries::SeriesType
 *
 * Type of the series.
 *
 * \value None
 *        No series type.
 * \value Bar
 *        Series type for Q3DBars.
 * \value Scatter
 *        Series type for Q3DScatter.
 * \value Surface
 *        Series type for Q3DSurface.
 */

/*!
 *  \enum QAbstract3DSeries::Mesh
 *
 *  Predefined mesh types. All styles are not usable with all graphs types.
 *
 *  \value UserDefined
 *         User defined mesh, set via QAbstract3DSeries::userDefinedMesh property.
 *  \value Bar
 *         Basic rectangular bar.
 *  \value Cube
 *         Basic cube.
 *  \value Pyramid
 *         Four-sided pyramid.
 *  \value Cone
 *         Basic cone.
 *  \value Cylinder
 *         Basic cylinder.
 *  \value BevelBar
 *         Slightly beveled (rounded) rectangular bar.
 *  \value BevelCube
 *         Slightly beveled (rounded) cube.
 *  \value Sphere
 *         Sphere.
 *  \value Minimal
 *         The minimal 3D mesh: a triangular pyramid. Usable only with Q3DScatter.
 *  \value Arrow
 *         Arrow pointing upwards.
 *  \value Point
 *         2D point. Usable only with Q3DScatter.
 *         Shadows do not affect this style. Color style Q3DTheme::ColorStyle::ObjectGradient
 *         is not supported by this style.
 */

/*!
 * \qmlproperty Abstract3DSeries.SeriesType Abstract3DSeries::type
 * The type of the series. One of the QAbstract3DSeries::SeriesType values.
 *
 */

/*!
 * \qmlproperty string Abstract3DSeries::itemLabelFormat
 *
 * The label format for data items in this series. This format is used for single item labels,
 * for example, when an item is selected. How the format is interpreted depends
 * on series type: Bar3DSeries, Scatter3DSeries, Surface3DSeries.
 */

/*!
 * \qmlproperty bool Abstract3DSeries::visible
 * Sets the visibility of the series. If \c false, the series is not rendered.
 */

/*!
 * \qmlproperty Abstract3DSeries.Mesh Abstract3DSeries::mesh
 *
 * Sets the mesh of the items in the series, or the selection pointer in case of
 * Surface3DSeries. If the mesh is \l{QAbstract3DSeries::Mesh::UserDefined}{Abstract3DSeries.Mesh.UserDefined},
 * then the userDefinedMesh property must also be set for items to render properly.
 * The default value depends on the graph type.
 *
 * \sa QAbstract3DSeries::Mesh
 */

/*!
 * \qmlproperty bool Abstract3DSeries::meshSmooth
 *
 * If \c true, smooth versions of predefined meshes set via the \l mesh property are used.
 * This property does not affect custom meshes used when the mesh is set to
 * \l{QAbstract3DSeries::Mesh::UserDefined}{Abstract3DSeries.Mesh.UserDefined}.
 * Defaults to \c{false}.
 */

/*!
 * \qmlproperty quaternion Abstract3DSeries::meshRotation
 *
 * Sets the mesh rotation that is applied to all items of the series.
 * The rotation should be a normalized quaternion.
 * For those series types that support item specific rotation, the rotations are
 * multiplied together.
 * Bar3DSeries ignores any rotation that is not around the y-axis.
 * Surface3DSeries applies the rotation only to the selection pointer.
 * Defaults to no rotation.
 */

/*!
 * \qmlproperty string Abstract3DSeries::userDefinedMesh
 *
 * Sets the filename for a user defined custom mesh for objects that is used when \l mesh
 * is \l{QAbstract3DSeries::Mesh::UserDefined}{Abstract3DSeries.Mesh.UserDefined}.
 * \note The file needs to be in the Wavefront OBJ format and include
 * vertices, normals, and UVs. It also needs to be in triangles.
 */

/*!
 * \qmlproperty Theme3D.ColorStyle Abstract3DSeries::colorStyle
 *
 * Sets the color style for the series.
 *
 * \sa {Theme3D::colorStyle}{Theme3D.colorStyle}
 */

/*!
 * \qmlproperty color Abstract3DSeries::baseColor
 *
 * Sets the base color of the series.
 *
 * \sa colorStyle, {Theme3D::baseColors}{Theme3D.baseColors}
 */

/*!
 * \qmlproperty Gradient Abstract3DSeries::baseGradient
 *
 * Sets the base gradient of the series.
 *
 * \sa colorStyle, {Theme3D::baseGradients}{Theme3D.baseGradients}
 */

/*!
 * \qmlproperty color Abstract3DSeries::singleHighlightColor
 *
 * Sets the single item highlight color of the series.
 *
 * \sa colorStyle, {Theme3D::singleHighlightColor}{Theme3D.singleHighlightColor}
 */

/*!
 * \qmlproperty Gradient Abstract3DSeries::singleHighlightGradient
 *
 * Sets the single item highlight gradient of the series.
 *
 * \sa colorStyle, {Theme3D::singleHighlightGradient}{Theme3D.singleHighlightGradient}
 */

/*!
 * \qmlproperty color Abstract3DSeries::multiHighlightColor
 *
 * Sets the multiple item highlight color of the series.
 *
 * \sa colorStyle, {Theme3D::multiHighlightColor}{Theme3D.multiHighlightColor}
 */

/*!
 * \qmlproperty Gradient Abstract3DSeries::multiHighlightGradient
 *
 * Sets the multiple item highlight gradient of the series.
 *
 * \sa colorStyle, {Theme3D::multiHighlightGradient}{Theme3D.multiHighlightGradient}
 */

/*!
 * \qmlproperty string Abstract3DSeries::name
 *
 * The series name.
 * It can be used in item label format with the tag \c{@seriesName}.
 *
 * \sa itemLabelFormat
 */

/*!
 * \qmlproperty string Abstract3DSeries::itemLabel
 *
 * The formatted item label. If there is no selected item or the selected item is not
 * visible, returns an empty string.
 *
 * \sa itemLabelFormat
 */

/*!
 * \qmlproperty bool Abstract3DSeries::itemLabelVisible
 *
 * If \c true, item labels are drawn as floating labels in the graph. Otherwise,
 * item labels are not drawn. To show the item label in an external control,
 * this property is set to \c false. Defaults to \c true.
 *
 * \sa itemLabelFormat, itemLabel
 */

/*!
 * \qmlmethod void Abstract3DSeries::setMeshAxisAndAngle(vector3d axis, real angle)
 *
 * A convenience function to construct a mesh rotation quaternion from \a axis
 * and \a angle.
 *
 * \sa meshRotation
 */

/*!
 * \internal
 */
QAbstract3DSeries::QAbstract3DSeries(QAbstract3DSeriesPrivate *d, QObject *parent) :
    QObject(parent),
    d_ptr(d)
{
}

/*!
 * Deletes the abstract 3D series.
 */
QAbstract3DSeries::~QAbstract3DSeries()
{
}

/*!
 * \property QAbstract3DSeries::type
 *
 * \brief The type of the series.
 */
QAbstract3DSeries::SeriesType QAbstract3DSeries::type() const
{
    const Q_D(QAbstract3DSeries);
    return d->m_type;
}

/*!
 * \property QAbstract3DSeries::itemLabelFormat
 *
 * \brief The label format for data items in this series.
 *
 * This format is used for single item labels,
 * for example, when an item is selected. How the format is interpreted depends
 * on series type: QBar3DSeries, QScatter3DSeries, QSurface3DSeries.
 */
void QAbstract3DSeries::setItemLabelFormat(const QString &format)
{
    Q_D(QAbstract3DSeries);
    if (d->m_itemLabelFormat != format) {
        d->setItemLabelFormat(format);
        emit itemLabelFormatChanged(format);
    }
}

QString QAbstract3DSeries::itemLabelFormat() const
{
    const Q_D(QAbstract3DSeries);
    return d->m_itemLabelFormat;
}

/*!
 * \property QAbstract3DSeries::visible
 *
 * \brief The visibility of the series.
 *
 * If this property is \c false, the series is not rendered.
 * Defaults to \c{true}.
 */
void QAbstract3DSeries::setVisible(bool visible)
{
    Q_D(QAbstract3DSeries);
    if (d->m_visible != visible) {
        d->setVisible(visible);
        emit visibilityChanged(visible);
    }
}

bool QAbstract3DSeries::isVisible() const
{
    const Q_D(QAbstract3DSeries);
    return d->m_visible;
}

/*!
 * \property QAbstract3DSeries::mesh
 *
 * \brief The mesh of the items in the series.
 *
 * For QSurface3DSeries, this property holds the selection pointer.
 *
 * If the mesh is MeshUserDefined, then the userDefinedMesh property
 * must also be set for items to render properly. The default value depends on the graph type.
 */
void QAbstract3DSeries::setMesh(QAbstract3DSeries::Mesh mesh)
{
    Q_D(QAbstract3DSeries);
    if ((mesh == QAbstract3DSeries::Mesh::Point || mesh == QAbstract3DSeries::Mesh::Minimal
         || mesh == QAbstract3DSeries::Mesh::Arrow)
            && type() != QAbstract3DSeries::SeriesType::Scatter) {
        qWarning() << "Specified style is only supported for QScatter3DSeries.";
    } else if (d->m_mesh != mesh) {
        d->setMesh(mesh);
        emit meshChanged(mesh);
    }
}

QAbstract3DSeries::Mesh QAbstract3DSeries::mesh() const
{
    const Q_D(QAbstract3DSeries);
    return d->m_mesh;
}

/*!
 * \property QAbstract3DSeries::meshSmooth
 *
 * \brief Whether smooth versions of predefined meshes are used.
 *
 * If \c true, smooth versions set via the \l mesh property are used.
 * This property does not affect custom meshes used when the mesh is set to
 * MeshUserDefined. Defaults to \c{false}.
 */
void QAbstract3DSeries::setMeshSmooth(bool enable)
{
    Q_D(QAbstract3DSeries);
    if (d->m_meshSmooth != enable) {
        d->setMeshSmooth(enable);
        emit meshSmoothChanged(enable);
    }
}

bool QAbstract3DSeries::isMeshSmooth() const
{
    const Q_D(QAbstract3DSeries);
    return d->m_meshSmooth;
}

/*!
 * \property QAbstract3DSeries::meshRotation
 *
 * \brief The mesh rotation that is applied to all items of the series.
 *
 * The rotation should be a normalized QQuaternion.
 * For those series types that support item specific rotation, the rotations are
 * multiplied together.
 * QBar3DSeries ignores any rotation that is not around the y-axis.
 * QSurface3DSeries applies the rotation only to the selection pointer.
 * Defaults to no rotation.
 */
void QAbstract3DSeries::setMeshRotation(const QQuaternion &rotation)
{
    Q_D(QAbstract3DSeries);
    if (d->m_meshRotation != rotation) {
        d->setMeshRotation(rotation);
        emit meshRotationChanged(rotation);
    }
}

QQuaternion QAbstract3DSeries::meshRotation() const
{
    const Q_D(QAbstract3DSeries);
    return d->m_meshRotation;
}

/*!
 * A convenience function to construct a mesh rotation quaternion from
 * \a axis and \a angle.
 *
 * \sa meshRotation
 */
void QAbstract3DSeries::setMeshAxisAndAngle(const QVector3D &axis, float angle)
{
    setMeshRotation(QQuaternion::fromAxisAndAngle(axis, angle));
}

/*!
 * \property QAbstract3DSeries::userDefinedMesh
 *
 * \brief The filename for a user defined custom mesh for objects.
 *
 * The custom mesh is used when \l mesh is MeshUserDefined.
 * \note The file needs to be in the Wavefront OBJ format and include
 * vertices, normals, and UVs. It also needs to be in triangles.
 */
void QAbstract3DSeries::setUserDefinedMesh(const QString &fileName)
{
    Q_D(QAbstract3DSeries);
    if (d->m_userDefinedMesh != fileName) {
        d->setUserDefinedMesh(fileName);
        emit userDefinedMeshChanged(fileName);
    }
}

QString QAbstract3DSeries::userDefinedMesh() const
{
    const Q_D(QAbstract3DSeries);
    return d->m_userDefinedMesh;
}

/*!
 * \property QAbstract3DSeries::colorStyle
 *
 * \brief The color style for the series.
 *
 * \sa Q3DTheme::ColorStyle
 */
void QAbstract3DSeries::setColorStyle(Q3DTheme::ColorStyle style)
{
    Q_D(QAbstract3DSeries);
    if (d->m_colorStyle != style) {
        d->setColorStyle(style);
        emit colorStyleChanged(style);
    }
    d->m_themeTracker.colorStyleOverride = true;
}

Q3DTheme::ColorStyle QAbstract3DSeries::colorStyle() const
{
    const Q_D(QAbstract3DSeries);
    return d->m_colorStyle;
}

/*!
 * \property QAbstract3DSeries::baseColor
 *
 * \brief The base color of the series.
 *
 * \sa colorStyle, Q3DTheme::baseColors
 */
void QAbstract3DSeries::setBaseColor(const QColor &color)
{
    Q_D(QAbstract3DSeries);
    if (d->m_baseColor != color) {
        d->setBaseColor(color);
        emit baseColorChanged(color);
    }
    d->m_themeTracker.baseColorOverride = true;
}

QColor QAbstract3DSeries::baseColor() const
{
    const Q_D(QAbstract3DSeries);
    return d->m_baseColor;
}

/*!
 * \property QAbstract3DSeries::baseGradient
 *
 * \brief The base gradient of the series.
 *
 * \sa colorStyle, Q3DTheme::baseGradients
 */
void QAbstract3DSeries::setBaseGradient(const QLinearGradient &gradient)
{
    Q_D(QAbstract3DSeries);
    if (d->m_baseGradient != gradient) {
        d->setBaseGradient(gradient);
        emit baseGradientChanged(gradient);
    }
    d->m_themeTracker.baseGradientOverride = true;
}

QLinearGradient QAbstract3DSeries::baseGradient() const
{
    const Q_D(QAbstract3DSeries);
    return d->m_baseGradient;
}

/*!
 * \property QAbstract3DSeries::singleHighlightColor
 *
 * \brief The single item highlight color of the series.
 *
 * \sa colorStyle, Q3DTheme::singleHighlightColor
 */
void QAbstract3DSeries::setSingleHighlightColor(const QColor &color)
{
    Q_D(QAbstract3DSeries);
    if (d->m_singleHighlightColor != color) {
        d->setSingleHighlightColor(color);
        emit singleHighlightColorChanged(color);
    }
    d->m_themeTracker.singleHighlightColorOverride = true;
}

QColor QAbstract3DSeries::singleHighlightColor() const
{
    const Q_D(QAbstract3DSeries);
    return d->m_singleHighlightColor;
}

/*!
 * \property QAbstract3DSeries::singleHighlightGradient
 *
 * \brief The single item highlight gradient of the series.
 *
 * \sa colorStyle, Q3DTheme::singleHighlightGradient
 */
void QAbstract3DSeries::setSingleHighlightGradient(const QLinearGradient &gradient)
{
    Q_D(QAbstract3DSeries);
    if (d->m_singleHighlightGradient != gradient) {
        d->setSingleHighlightGradient(gradient);
        emit singleHighlightGradientChanged(gradient);
    }
    d->m_themeTracker.singleHighlightGradientOverride = true;
}

QLinearGradient QAbstract3DSeries::singleHighlightGradient() const
{
    const Q_D(QAbstract3DSeries);
    return d->m_singleHighlightGradient;
}

/*!
 * \property QAbstract3DSeries::multiHighlightColor
 *
 * \brief The multiple item highlight color of the series.
 *
 * \sa colorStyle, Q3DTheme::multiHighlightColor
 */
void QAbstract3DSeries::setMultiHighlightColor(const QColor &color)
{
    Q_D(QAbstract3DSeries);
    if (d->m_multiHighlightColor != color) {
        d->setMultiHighlightColor(color);
        emit multiHighlightColorChanged(color);
    }
    d->m_themeTracker.multiHighlightColorOverride = true;
}

QColor QAbstract3DSeries::multiHighlightColor() const
{
    const Q_D(QAbstract3DSeries);
    return d->m_multiHighlightColor;
}

/*!
 * \property QAbstract3DSeries::multiHighlightGradient
 *
 * \brief The multiple item highlight gradient of the series.
 *
 * \sa colorStyle, Q3DTheme::multiHighlightGradient
 */
void QAbstract3DSeries::setMultiHighlightGradient(const QLinearGradient &gradient)
{
    Q_D(QAbstract3DSeries);
    if (d->m_multiHighlightGradient != gradient) {
        d->setMultiHighlightGradient(gradient);
        emit multiHighlightGradientChanged(gradient);
    }
    d->m_themeTracker.multiHighlightGradientOverride = true;
}

QLinearGradient QAbstract3DSeries::multiHighlightGradient() const
{
    const Q_D(QAbstract3DSeries);
    return d->m_multiHighlightGradient;
}

/*!
 * \property QAbstract3DSeries::name
 *
 * \brief The series name.
 *
 * The series name can be used in item label format with the tag \c{@seriesName}.
 *
 * \sa itemLabelFormat
 */
void QAbstract3DSeries::setName(const QString &name)
{
    Q_D(QAbstract3DSeries);
    if (d->m_name != name) {
        d->setName(name);
        emit nameChanged(name);
    }
}

QString QAbstract3DSeries::name() const
{
    const Q_D(QAbstract3DSeries);
    return d->m_name;
}

/*!
 * \property QAbstract3DSeries::itemLabel
 *
 * \brief The formatted item label.
 *
 * If there is no selected item or the selected item is not
 * visible, returns an empty string.
 *
 * \sa itemLabelFormat
 */
QString QAbstract3DSeries::itemLabel()
{
    Q_D(QAbstract3DSeries);
    return d->itemLabel();
}

/*!
 * \property QAbstract3DSeries::itemLabelVisible
 *
 * \brief The visibility of item labels in the graph.
 *
 * If \c true, item labels are drawn as floating labels in the graph. Otherwise,
 * item labels are not drawn. To show the item label in an external control,
 * this property is set to \c false. Defaults to \c true.
 *
 * \sa itemLabelFormat, itemLabel
 */
void QAbstract3DSeries::setItemLabelVisible(bool visible)
{
    Q_D(QAbstract3DSeries);
    if (d->m_itemLabelVisible != visible) {
        d->setItemLabelVisible(visible);
        emit itemLabelVisibilityChanged(visible);
    }
}

bool QAbstract3DSeries::isItemLabelVisible() const
{
    const Q_D(QAbstract3DSeries);
    return d->m_itemLabelVisible;
}

// QAbstract3DSeriesPrivate

QAbstract3DSeriesPrivate::QAbstract3DSeriesPrivate(QAbstract3DSeries *q,
                                                   QAbstract3DSeries::SeriesType type)
    : q_ptr(q),
      m_type(type),
      m_dataProxy(0),
      m_visible(true),
      m_controller(0),
      m_mesh(QAbstract3DSeries::Mesh::Cube),
      m_meshSmooth(false),
      m_colorStyle(Q3DTheme::ColorStyle::Uniform),
      m_baseColor(Qt::black),
      m_singleHighlightColor(Qt::black),
      m_multiHighlightColor(Qt::black),
      m_itemLabelDirty(true),
      m_itemLabelVisible(true)
{
}

QAbstract3DSeriesPrivate::~QAbstract3DSeriesPrivate()
{
}

QAbstractDataProxy *QAbstract3DSeriesPrivate::dataProxy() const
{
    return m_dataProxy;
}

void QAbstract3DSeriesPrivate::setDataProxy(QAbstractDataProxy *proxy)
{
    Q_Q(QAbstract3DSeries);
    Q_ASSERT(proxy && proxy != m_dataProxy && !proxy->d_func()->series());

    delete m_dataProxy;
    m_dataProxy = proxy;

    proxy->d_func()->setSeries(q); // also sets parent

    if (m_controller) {
        connectControllerAndProxy(m_controller);
        m_controller->markDataDirty();
    }
}

void QAbstract3DSeriesPrivate::setController(Abstract3DController *controller)
{
    Q_Q(QAbstract3DSeries);
    connectControllerAndProxy(controller);
    m_controller = controller;
    q->setParent(controller);
    markItemLabelDirty();
}

void QAbstract3DSeriesPrivate::setItemLabelFormat(const QString &format)
{
    m_itemLabelFormat = format;
    markItemLabelDirty();
}

void QAbstract3DSeriesPrivate::setVisible(bool visible)
{
    m_visible = visible;
    markItemLabelDirty();
}

void QAbstract3DSeriesPrivate::setMesh(QAbstract3DSeries::Mesh mesh)
{
    m_mesh = mesh;
    m_changeTracker.meshChanged = true;
    if (m_controller) {
        m_controller->markSeriesVisualsDirty();

        if (m_controller->optimizationHint() == QAbstract3DGraph::OptimizationHint::Default)
            m_controller->markDataDirty();
    }
}

void QAbstract3DSeriesPrivate::setMeshSmooth(bool enable)
{
    m_meshSmooth = enable;
    m_changeTracker.meshSmoothChanged = true;
    if (m_controller) {
        m_controller->markSeriesVisualsDirty();

        if (m_controller->optimizationHint() == QAbstract3DGraph::OptimizationHint::Default)
            m_controller->markDataDirty();
    }
}

void QAbstract3DSeriesPrivate::setMeshRotation(const QQuaternion &rotation)
{
    m_meshRotation = rotation;
    m_changeTracker.meshRotationChanged = true;
    if (m_controller) {
        m_controller->markSeriesVisualsDirty();

        if (m_controller->optimizationHint() == QAbstract3DGraph::OptimizationHint::Default)
            m_controller->markDataDirty();
    }
}

void QAbstract3DSeriesPrivate::setUserDefinedMesh(const QString &meshFile)
{
    m_userDefinedMesh = meshFile;
    m_changeTracker.userDefinedMeshChanged = true;
    if (m_controller) {
        m_controller->markSeriesVisualsDirty();

        if (m_controller->optimizationHint() == QAbstract3DGraph::OptimizationHint::Default)
            m_controller->markDataDirty();
    }
}

void QAbstract3DSeriesPrivate::setColorStyle(Q3DTheme::ColorStyle style)
{
    m_colorStyle = style;
    m_changeTracker.colorStyleChanged = true;
    if (m_controller)
        m_controller->markSeriesVisualsDirty();
}

void QAbstract3DSeriesPrivate::setBaseColor(const QColor &color)
{
    m_baseColor = color;
    m_changeTracker.baseColorChanged = true;
    if (m_controller)
        m_controller->markSeriesVisualsDirty();
}

void QAbstract3DSeriesPrivate::setBaseGradient(const QLinearGradient &gradient)
{
    m_baseGradient = gradient;
    Utils::verifyGradientCompleteness(m_baseGradient);
    m_changeTracker.baseGradientChanged = true;
    if (m_controller)
        m_controller->markSeriesVisualsDirty();
}

void QAbstract3DSeriesPrivate::setSingleHighlightColor(const QColor &color)
{
    m_singleHighlightColor = color;
    m_changeTracker.singleHighlightColorChanged = true;
    if (m_controller)
        m_controller->markSeriesVisualsDirty();
}

void QAbstract3DSeriesPrivate::setSingleHighlightGradient(const QLinearGradient &gradient)
{
    m_singleHighlightGradient = gradient;
    Utils::verifyGradientCompleteness(m_singleHighlightGradient);
    m_changeTracker.singleHighlightGradientChanged = true;
    if (m_controller)
        m_controller->markSeriesVisualsDirty();
}

void QAbstract3DSeriesPrivate::setMultiHighlightColor(const QColor &color)
{
    m_multiHighlightColor = color;
    m_changeTracker.multiHighlightColorChanged = true;
    if (m_controller)
        m_controller->markSeriesVisualsDirty();
}

void QAbstract3DSeriesPrivate::setMultiHighlightGradient(const QLinearGradient &gradient)
{
    m_multiHighlightGradient = gradient;
    Utils::verifyGradientCompleteness(m_multiHighlightGradient);
    m_changeTracker.multiHighlightGradientChanged = true;
    if (m_controller)
        m_controller->markSeriesVisualsDirty();
}

void QAbstract3DSeriesPrivate::setName(const QString &name)
{
    m_name = name;
    markItemLabelDirty();
    m_changeTracker.nameChanged = true;
}

void QAbstract3DSeriesPrivate::resetToTheme(const Q3DTheme &theme, int seriesIndex, bool force)
{
    Q_Q(QAbstract3DSeries);
    int themeIndex = seriesIndex;
    if (force || !m_themeTracker.colorStyleOverride) {
        q->setColorStyle(theme.colorStyle());
        m_themeTracker.colorStyleOverride = false;
    }
    if (force || !m_themeTracker.baseColorOverride) {
        if (theme.baseColors().size() <= seriesIndex)
            themeIndex = seriesIndex % theme.baseColors().size();
        q->setBaseColor(theme.baseColors().at(themeIndex));
        m_themeTracker.baseColorOverride = false;
    }
    if (force || !m_themeTracker.baseGradientOverride) {
        if (theme.baseGradients().size() <= seriesIndex)
            themeIndex = seriesIndex % theme.baseGradients().size();
        q->setBaseGradient(theme.baseGradients().at(themeIndex));
        m_themeTracker.baseGradientOverride = false;
    }
    if (force || !m_themeTracker.singleHighlightColorOverride) {
        q->setSingleHighlightColor(theme.singleHighlightColor());
        m_themeTracker.singleHighlightColorOverride = false;
    }
    if (force || !m_themeTracker.singleHighlightGradientOverride) {
        q->setSingleHighlightGradient(theme.singleHighlightGradient());
        m_themeTracker.singleHighlightGradientOverride = false;
    }
    if (force || !m_themeTracker.multiHighlightColorOverride) {
        q->setMultiHighlightColor(theme.multiHighlightColor());
        m_themeTracker.multiHighlightColorOverride = false;
    }
    if (force || !m_themeTracker.multiHighlightGradientOverride) {
        q->setMultiHighlightGradient(theme.multiHighlightGradient());
        m_themeTracker.multiHighlightGradientOverride = false;
    }
}

QString QAbstract3DSeriesPrivate::itemLabel()
{
    Q_Q(QAbstract3DSeries);
    if (m_itemLabelDirty) {
        QString oldLabel = m_itemLabel;
        if (m_controller && m_visible)
            createItemLabel();
        else
            m_itemLabel = QString();
        m_itemLabelDirty = false;

        if (oldLabel != m_itemLabel)
            emit q->itemLabelChanged(m_itemLabel);
    }

    return m_itemLabel;
}

void QAbstract3DSeriesPrivate::markItemLabelDirty()
{
    m_itemLabelDirty = true;
    m_changeTracker.itemLabelChanged = true;
}

void QAbstract3DSeriesPrivate::setItemLabelVisible(bool visible)
{
    m_itemLabelVisible = visible;
    markItemLabelDirty();
    m_changeTracker.itemLabelVisibilityChanged = true;
}

bool QAbstract3DSeriesPrivate::isUsingGradient()
{
    return m_colorStyle != Q3DTheme::ColorStyle::Uniform ? true : false;
}

QT_END_NAMESPACE
