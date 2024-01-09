# Copyright (C) 2024 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

qt_feature("graphs-3d" PUBLIC
    LABEL "3D Graphs"
    PURPOSE "Support for 3D graphs"
    CONDITION TARGET Qt6::Quick3D
)
