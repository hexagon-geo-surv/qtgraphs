// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef SCATTERGRAPH_H
#define SCATTERGRAPH_H

#include <QtCore/qobject.h>
#include <QtGraphs/q3dscatter.h>

class ScatterGraph : public QObject
{
    Q_OBJECT
public:
    ScatterGraph();
    ~ScatterGraph();

    void initialize();
    QWidget *scatterWidget() { return m_scatterWidget; }

private:
    Q3DScatter *m_scatterGraph = nullptr;
    QWidget *m_scatterWidget = nullptr;
};

#endif
