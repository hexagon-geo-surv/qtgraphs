# Copyright (C) 2024 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

qt_internal_add_test(tst_qgdatetimeaxis2d
    SOURCES
        tst_datetimeaxis.cpp
    INCLUDE_DIRECTORIES
        ../common
    LIBRARIES
        Qt::Gui
        Qt::GuiPrivate
        Qt::Graphs
)
