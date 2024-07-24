// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only
#ifndef QSPLINE3DSERIES_H
#define QSPLINE3DSERIES_H

#include <QtGraphs/qscatter3dseries.h>
QT_BEGIN_NAMESPACE

class QSpline3DSeriesPrivate;

class Q_GRAPHS_EXPORT QSpline3DSeries : public QScatter3DSeries
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QSpline3DSeries)
    Q_PROPERTY(bool splineVisible READ isSplineVisible WRITE setSplineVisible NOTIFY
                   splineVisibilityChanged FINAL)
    Q_PROPERTY(float splineTension READ splineTension WRITE setSplineTension NOTIFY
                   splineTensionChanged FINAL)
    Q_PROPERTY(float splineKnotting READ splineKnotting WRITE setSplineKnotting NOTIFY
                   splineKnottingChanged FINAL)
    Q_PROPERTY(bool splineLooping READ isSplineLooping WRITE setSplineLooping NOTIFY
                   splineLoopingChanged FINAL)
    Q_PROPERTY(
        QColor splineColor READ splineColor WRITE setSplineColor NOTIFY splineColorChanged FINAL)
    Q_PROPERTY(int splineResolution READ splineResolution WRITE setSplineResolution NOTIFY
                   splineResolutionChanged FINAL)
public:
    explicit QSpline3DSeries(QObject *parent = nullptr);
    explicit QSpline3DSeries(QScatterDataProxy *dataProxy, QObject *parent = nullptr);
    ~QSpline3DSeries() override;

    void setSplineVisible(bool draw);
    bool isSplineVisible() const;

    void setSplineTension(float tension);
    float splineTension() const;

    void setSplineKnotting(float knotting);
    float splineKnotting() const;

    void setSplineLooping(bool looping);
    bool isSplineLooping() const;

    void setSplineColor(QColor color);
    QColor splineColor() const;

    void setSplineResolution(int resolution);
    int splineResolution() const;

Q_SIGNALS:
    void splineVisibilityChanged(bool visible);
    void splineTensionChanged(float tension);
    void splineKnottingChanged(float knotting);
    void splineLoopingChanged(bool looping);
    void splineColorChanged(QColor color);
    void splineResolutionChanged(int resolution);

protected:
    explicit QSpline3DSeries(QSpline3DSeriesPrivate &d, QObject *parent = nullptr);

private:
    Q_DISABLE_COPY_MOVE(QSpline3DSeries)

    friend class QQuickGraphsScatter;
};

QT_END_NAMESPACE

#endif // QSPLINE3DSERIES_H
