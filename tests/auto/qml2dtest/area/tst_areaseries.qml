// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import QtGraphs
import QtTest

Item {
    id: top
    height: 150
    width: 150

    AreaSeries {
        id: initial
    }

    AreaSeries {
        id: initialized

        axisX: ValueAxis { max: 4 }
        axisY: ValueAxis { max: 8 }

        color: "#ff00ff"
        selectedColor: "#00ff00"
        borderColor: "#ff00ff"
        selectedBorderColor: "#00ff00"
        borderWidth: 2.0
        selected: true
        upperSeries: upperSeries
        lowerSeries: lowerSeries

        name: "AreaSeries"
        visible: false
        selectable: true
        hoverable: true
        opacity: 0.75
        valuesMultiplier: 0.75
    }

    // Values used for changing the properties
    ValueAxis { id: axisx; max: 10 }
    ValueAxis { id: axisy; max: 10 }

    LineSeries {
        id: upperSeries
        XYPoint { x: 0; y: 1 }
        XYPoint { x: 1; y: 2 }
        XYPoint { x: 2; y: 3 }
    }

    LineSeries {
        id: lowerSeries
        XYPoint { x: 0; y: 0 }
        XYPoint { x: 1; y: 1 }
        XYPoint { x: 2; y: 2 }
    }

    LineSeries {
        id: upperSeries2
        XYPoint { x: 0; y: 1 }
        XYPoint { x: 1; y: 2 }
        XYPoint { x: 2; y: 3 }
    }

    LineSeries {
        id: lowerSeries2
        XYPoint { x: 0; y: 0 }
        XYPoint { x: 1; y: 1 }
        XYPoint { x: 2; y: 2 }
    }

    TestCase {
        name: "AreaSeries Initial"

        function test_1_initial() {
            compare(initial.axisX, null)
            compare(initial.axisY, null)
            compare(initial.color, "#00000000")
            compare(initial.selectedColor, "#00000000")
            compare(initial.borderColor, "#00000000")
            compare(initial.selectedBorderColor, "#00000000")
            compare(initial.borderWidth, 1.0)
            compare(initial.selected, false)
            compare(initial.upperSeries, null)
            compare(initial.lowerSeries, null)
        }

        function test_2_initial_common() {
            // Properties from QAbstractSeries
            verify(initial.theme)
            compare(initial.name, "")
            compare(initial.visible, true)
            compare(initial.selectable, false)
            compare(initial.hoverable, false)
            compare(initial.opacity, 1.0)
            compare(initial.valuesMultiplier, 1.0)
        }

        function test_3_initial_change() {
            initial.axisX = axisx
            initial.axisY = axisy

            initial.color = "#ff00ff"
            initial.selectedColor = "#00ff00"
            initial.borderColor = "#ff00ff"
            initial.selectedBorderColor = "#00ff00"
            initial.borderWidth = 2.0
            initial.selected = true
            initial.upperSeries = upperSeries;
            initial.lowerSeries = lowerSeries;

            initial.name = "Area"
            initial.visible = false
            initial.selectable = true
            initial.hoverable = true
            initial.opacity = 0.5
            initial.valuesMultiplier = 0.5

            compare(initial.axisX, axisx)
            compare(initial.axisY, axisy)
            compare(initial.axisX.max, 10)
            compare(initial.axisY.max, 10)

            compare(initial.color, "#ff00ff")
            compare(initial.selectedColor, "#00ff00")
            compare(initial.borderColor, "#ff00ff")
            compare(initial.selectedBorderColor, "#00ff00")
            compare(initial.borderWidth, 2.0);
            compare(initial.selected, true);
            compare(initial.upperSeries, upperSeries);
            compare(initial.lowerSeries, lowerSeries);

            compare(initial.name, "Area")
            compare(initial.visible, false)
            compare(initial.selectable, true)
            compare(initial.hoverable, true)
            compare(initial.opacity, 0.5)
            compare(initial.valuesMultiplier, 0.5)
        }
    }

    TestCase {
        name: "AreaSeries Initialized"

        function test_1_initialized() {
            compare(initialized.axisX.max, 4)
            compare(initialized.axisY.max, 8)

            compare(initialized.color, "#ff00ff")
            compare(initialized.selectedColor, "#00ff00")
            compare(initialized.borderColor, "#ff00ff")
            compare(initialized.selectedBorderColor, "#00ff00")
            compare(initialized.borderWidth, 2.0)
            compare(initialized.selected, true)
            compare(initialized.upperSeries, upperSeries);
            compare(initialized.lowerSeries, lowerSeries);

            compare(initialized.name, "AreaSeries")
            compare(initialized.visible, false)
            compare(initialized.selectable, true)
            compare(initialized.hoverable, true)
            compare(initialized.opacity, 0.75)
            compare(initialized.valuesMultiplier, 0.75)
        }

        function test_2_initialized_change() {
            initialized.axisX = axisx
            initialized.axisY = axisy

            initialized.color = "#0000ff"
            initialized.selectedColor = "#ff0000"
            initialized.borderColor = "#0000ff"
            initialized.selectedBorderColor = "#ff0000"
            initialized.borderWidth = 3.0;
            initialized.selected = false;
            initialized.upperSeries = upperSeries2;
            initialized.lowerSeries = lowerSeries2;

            initialized.name = "Area"
            initialized.visible = true
            initialized.selectable = false
            initialized.hoverable = false
            initialized.opacity = 0.5
            initialized.valuesMultiplier = 0.25

            compare(initialized.axisX.max, 10)
            compare(initialized.axisY.max, 10)

            compare(initialized.color, "#0000ff")
            compare(initialized.selectedColor, "#ff0000")
            compare(initialized.borderColor, "#0000ff")
            compare(initialized.selectedBorderColor, "#ff0000")
            compare(initialized.borderWidth, 3.0)
            compare(initialized.selected, false)
            compare(initialized.upperSeries, upperSeries2);
            compare(initialized.lowerSeries, lowerSeries2);

            compare(initialized.name, "Area")
            compare(initialized.visible, true)
            compare(initialized.selectable, false)
            compare(initialized.hoverable, false)
            compare(initialized.opacity, 0.5)
            compare(initialized.valuesMultiplier, 0.25)
        }

        function test_3_initialized_change_to_null() {
            initialized.axisX = null
            initialized.axisY = null
            initialized.upperSeries = null
            initialized.lowerSeries = null

            verify(!initialized.axisY)
            verify(!initialized.axisX)
            verify(!initialized.upperSeries)
            verify(!initialized.lowerSeries)
        }

        function test_4_initialized_change_to_invalid() {
            initialized.valuesMultiplier = 2.0 // range 0...1
            compare(initialized.valuesMultiplier, 1.0)

            initialized.valuesMultiplier = -1.0 // range 0...1
            compare(initialized.valuesMultiplier, 0.0)
        }
    }
}
