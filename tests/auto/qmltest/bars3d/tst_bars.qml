// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 2.0
import QtGraphs
import QtTest 1.0

Item {
    id: top
    height: 150
    width: 150

    property var bars3d: null

    function constructBars() {
        bars3d = Qt.createQmlObject("
       import QtQuick 2.2
       import QtGraphs
       Bars3D {
           anchors.fill: parent
       }", top)
        bars3d.anchors.fill = top
    }

    TestCase {
        name: "Bars3D Series"
        when: windowShown

        Bar3DSeries { id: series1
            dataProxy: ItemModelBarDataProxy {
                itemModel: ListModel {
                    ListElement{ year: "2012"; city: "Oulu"; expenses: "4200"; }
                    ListElement{ year: "2012"; city: "Rauma"; expenses: "2100"; }
                }
                rowRole: "city"
                columnRole: "year"
                valueRole: "expenses"
            }
        }
        Bar3DSeries { id: series2 }

        function test_1_add_series() {
            constructBars()

            bars3d.seriesList = [series1, series2]
            compare(bars3d.seriesList.length, 2)
            console.log("top:", top)
            waitForRendering(top)
        }

        function test_2_remove_series() {
            bars3d.seriesList = [series1]
            compare(bars3d.seriesList.length, 1)
            waitForRendering(top)
        }

        function test_3_remove_series() {
            bars3d.seriesList = []
            compare(bars3d.seriesList.length, 0)
            waitForRendering(top)
        }

        function test_4_primary_series() {
            bars3d.seriesList = [series1, series2]
            compare(bars3d.primarySeries, series1)
            bars3d.primarySeries = series2
            compare(bars3d.primarySeries, series2)
            waitForRendering(top)
        }

        function test_5_selected_series() {
            bars3d.seriesList[0].selectedBar = Qt.point(0, 0)
            compare(bars3d.selectedSeries, series1)

            waitForRendering(top)
        }

        function test_6_has_series() {
            bars3d.seriesList = [series1]
            compare(bars3d.hasSeries(series1), true)
            compare(bars3d.hasSeries(series2), false)

            waitForRendering(top)
            bars3d.destroy()
            waitForRendering(top)
        }
    }

    Custom3DItem { id: item1; meshFile: ":/customitem.mesh" }
    Custom3DItem { id: item2; meshFile: ":/customitem.mesh" }
    Custom3DItem { id: item3; meshFile: ":/customitem.mesh" }
    Custom3DItem { id: item4; meshFile: ":/customitem.mesh"; position: Qt.vector3d(0.0, 1.0, 0.0) }

    function constructBarsWithCustomItemList() {
        bars3d = Qt.createQmlObject("
       import QtQuick 2.2
       import QtGraphs
       Bars3D {
           anchors.fill: parent
           customItemList: [item1, item2]
       }", top)
        bars3d.anchors.fill = top
    }

    TestCase {
        name: "Bars3D Theme"
        when: windowShown

        GraphsTheme { id: newTheme }

        function test_1_add_theme() {
            constructBars()

            bars3d.theme = newTheme
            compare(bars3d.theme, newTheme)
            waitForRendering(top)
        }

        function test_2_change_theme() {
            newTheme.theme = GraphsTheme.Theme.QtGreenNeon
            compare(bars3d.theme.theme, GraphsTheme.Theme.QtGreenNeon)

            waitForRendering(top)
            bars3d.destroy()
            waitForRendering(top)
        }
    }

    TestCase {
        name: "Bars3D Custom"
        when: windowShown

        function test_1_custom_items() {
            constructBarsWithCustomItemList()
            compare(bars3d.customItemList.length, 2)
            waitForRendering(top)
        }

        function test_2_add_custom_items() {
            bars3d.addCustomItem(item3)
            compare(bars3d.customItemList.length, 3)
            bars3d.addCustomItem(item4)
            compare(bars3d.customItemList.length, 4)
            waitForRendering(top)
        }

        function test_3_change_custom_items() {
            item1.position = Qt.vector3d(1.0, 1.0, 1.0)
            compare(bars3d.customItemList[0].position, Qt.vector3d(1.0, 1.0, 1.0))
            waitForRendering(top)
        }

        function test_4_remove_custom_items() {
            bars3d.removeCustomItemAt(Qt.vector3d(0.0, 1.0, 0.0))
            compare(bars3d.customItemList.length, 3)
            bars3d.releaseCustomItem(item1)
            compare(bars3d.customItemList[0], item2)
            bars3d.releaseCustomItem(item2)
            compare(bars3d.customItemList.length, 1)
            bars3d.removeCustomItems()
            compare(bars3d.customItemList.length, 0)

            waitForRendering(top)
            bars3d.destroy()
            waitForRendering(top)
        }
    }
}
