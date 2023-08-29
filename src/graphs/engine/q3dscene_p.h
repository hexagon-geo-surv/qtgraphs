// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtGraphs API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Q3DSCENE_P_H
#define Q3DSCENE_P_H

#include <private/graphsglobal_p.h>
#include <QtGraphs/q3dscene.h>

QT_BEGIN_NAMESPACE

class Q3DCamera;

struct Q3DSceneChangeBitField {
    bool viewportChanged               : 1;
    bool primarySubViewportChanged     : 1;
    bool secondarySubViewportChanged   : 1;
    bool subViewportOrderChanged       : 1;
    bool cameraChanged                 : 1;
    bool slicingActivatedChanged       : 1;
    bool devicePixelRatioChanged       : 1;
    bool selectionQueryPositionChanged : 1;
    bool graphPositionQueryPositionChanged      : 1;
    bool windowSizeChanged             : 1;

    Q3DSceneChangeBitField()
        : viewportChanged(true),
          primarySubViewportChanged(true),
          secondarySubViewportChanged(true),
          subViewportOrderChanged(true),
          cameraChanged(true),
          slicingActivatedChanged(true),
          devicePixelRatioChanged(true),
          selectionQueryPositionChanged(false),
          graphPositionQueryPositionChanged(false),
          windowSizeChanged(true)
    {
    }
};

class Q_GRAPHS_EXPORT Q3DScenePrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(Q3DScene)

public:
    Q3DScenePrivate(Q3DScene *q);
    ~Q3DScenePrivate();

    void sync(Q3DScenePrivate &other);

    void setViewport(const QRect &viewport);
    void setViewportSize(int width, int height);
    void setWindowSize(const QSize &size);
    QSize windowSize() const;
    void calculateSubViewports();
    void updateGLViewport();
    void updateGLSubViewports();

    QRect glViewport();
    QRect glPrimarySubViewport();
    QRect glSecondarySubViewport();

    void markDirty();

    bool isInArea(const QRect &area, int x, int y) const;

Q_SIGNALS:
    void needRender();

protected:
    Q3DScene *q_ptr;

public:
    Q3DSceneChangeBitField m_changeTracker;

    QRect m_viewport;
    QRect m_primarySubViewport;
    QRect m_secondarySubViewport;
    bool m_isSecondarySubviewOnTop;
    float m_devicePixelRatio;
    Q3DCamera *m_camera;
    bool m_isUnderSideCameraEnabled;
    bool m_isSlicingActive;
    QPoint m_selectionQueryPosition;
    QPoint m_graphPositionQueryPosition;
    QSize m_windowSize;
    QRect m_glViewport;
    QRect m_glPrimarySubViewport;
    QRect m_glSecondarySubViewport;
    bool m_sceneDirty;
    QRect m_defaultSmallViewport;
    QRect m_defaultLargeViewport;
};

QT_END_NAMESPACE

#endif
