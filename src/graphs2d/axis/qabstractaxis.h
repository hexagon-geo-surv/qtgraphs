// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QABSTRACTAXIS_H
#define QABSTRACTAXIS_H

#include <QtCore/qobject.h>
#include <QtCore/qvariant.h>
#include <QtGraphs/qgraphsglobal.h>
#include <QtGui/qcolor.h>
#include <QtGui/qfont.h>
#include <QtQml/qqmlcomponent.h>
#include <QtQml/qqmlengine.h>

QT_BEGIN_NAMESPACE

class QAbstractAxisPrivate;

class Q_GRAPHS_EXPORT QAbstractAxis : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")
    //visibility
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(bool lineVisible READ isLineVisible WRITE setLineVisible NOTIFY lineVisibleChanged)
    //labels
    Q_PROPERTY(bool labelsVisible READ labelsVisible WRITE setLabelsVisible NOTIFY labelsVisibleChanged)
    Q_PROPERTY(qreal labelsAngle READ labelsAngle WRITE setLabelsAngle NOTIFY labelsAngleChanged)
    Q_PROPERTY(QQmlComponent *labelsComponent READ labelsComponent
                       WRITE setLabelsComponent NOTIFY labelsComponentChanged FINAL)
    //grid
    Q_PROPERTY(bool gridVisible READ isGridLineVisible WRITE setGridLineVisible NOTIFY gridVisibleChanged)
    Q_PROPERTY(bool minorGridVisible READ isMinorGridLineVisible WRITE setMinorGridLineVisible NOTIFY minorGridVisibleChanged)
    //title
    Q_PROPERTY(QString titleText READ titleText WRITE setTitleText NOTIFY titleTextChanged)
    Q_PROPERTY(QColor titleColor READ titleColor WRITE setTitleColor NOTIFY titleColorChanged)
    Q_PROPERTY(bool titleVisible READ isTitleVisible WRITE setTitleVisible NOTIFY titleVisibleChanged)
    Q_PROPERTY(QFont titleFont READ titleFont WRITE setTitleFont NOTIFY titleFontChanged)
    //orientation
    Q_PROPERTY(Qt::Orientation orientation READ orientation CONSTANT)
    //alignment
    Q_PROPERTY(Qt::Alignment alignment READ alignment CONSTANT)
    QML_FOREIGN(QAbstractAxis)
    QML_UNCREATABLE("Trying to create uncreatable: AbstractAxis.")
    QML_NAMED_ELEMENT(AbstractAxis)
    Q_DECLARE_PRIVATE(QAbstractAxis)

public:
    enum class AxisType { Value, BarCategory, DateTime };
    Q_ENUM(AxisType)

protected:
    explicit QAbstractAxis(QAbstractAxisPrivate &dd, QObject *parent = nullptr);

public:
    ~QAbstractAxis() override;

    virtual AxisType type() const = 0;

    //visibility handling
    bool isVisible() const;
    void setVisible(bool visible = true);
    void show();
    void hide();

    //arrow handling
    bool isLineVisible() const;
    void setLineVisible(bool visible = true);

    //grid handling
    bool isGridLineVisible() const;
    void setGridLineVisible(bool visible = true);
    bool isMinorGridLineVisible() const;
    void setMinorGridLineVisible(bool visible = true);

    //labels handling
    bool labelsVisible() const;
    void setLabelsVisible(bool visible = true);
    void setLabelsAngle(qreal angle);
    qreal labelsAngle() const;
    QQmlComponent *labelsComponent() const;
    void setLabelsComponent(QQmlComponent *newLabelsComponent);

    //title handling
    bool isTitleVisible() const;
    void setTitleVisible(bool visible = true);
    void setTitleColor(const QColor &color);
    QColor titleColor() const;
    void setTitleFont(const QFont &font);
    QFont titleFont() const;
    void setTitleText(const QString &title);
    QString titleText() const;

    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation orientation);
    Qt::Alignment alignment() const;

    //range handling
    void setMin(const QVariant &min);
    void setMax(const QVariant &max);
    void setRange(const QVariant &min, const QVariant &max);

Q_SIGNALS:
    void visibleChanged(bool visible);
    void lineVisibleChanged(bool visible);
    void labelsVisibleChanged(bool visible);
    void labelsAngleChanged(qreal angle);
    void labelsComponentChanged();
    void gridVisibleChanged(bool visible);
    void minorGridVisibleChanged(bool visible);
    void titleTextChanged(const QString &title);
    void titleColorChanged(const QColor &color);
    void titleVisibleChanged(bool visible);
    void titleFontChanged(const QFont &font);
    void update();
    void rangeChanged(qreal min, qreal max);

private:
    Q_DISABLE_COPY(QAbstractAxis)
};

QT_END_NAMESPACE

#endif // QABSTRACTAXIS_H
