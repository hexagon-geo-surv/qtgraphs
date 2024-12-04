// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtGraphs

ColumnLayout {
    anchors.fill: parent

    TabBar {
        id: tab
        Layout.fillWidth: true
        TabButton {
            text: "Bar"
        }
        TabButton {
            text: "Area"
        }
        TabButton {
            text: "Pie"
        }
    }

    StackLayout {
        currentIndex: tab.currentIndex

        // Bar Graph
        RowLayout {
            GraphsView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                axisX: BarCategoryAxis {
                    categories: ["2023", "2024", "2025", "2026"]
                    subGridVisible: false
                }
                axisY: ValueAxis {
                    max: 10
                    subTickCount: 9
                }
                BarSeries {
                    id: barSeries
                    selectable: true
                    BarSet { id: set1; label: "Axel"; values: [1, 2, 3, 4]; selectedColor: "red" }
                    BarSet { id: set2; label: "Frank"; values: [8, 2, 6, 0] }
                    BarSet { id: set3; label: "James"; values: [4+3*Math.sin(fA.elapsedTime), 5+3*Math.sin(fA.elapsedTime), 2, 3] }
                    FrameAnimation {
                        id: fA
                        running: true
                    }
                    onPressed: (index, barset) => {
                                   barPressedIndex.text = "Index : " + index
                                   barPressedLabel.text = "Label : " + barset.label
                               }
                    onReleased: (index, barset) => {
                                    barReleasedIndex.text = "Index : " + index
                                    barReleasedLabel.text = "Label : " + barset.label
                                }
                    onClicked: (index, barset) => {
                                   barClickedIndex.text = "Index : " + index
                                   barClickedLabel.text = "Label : " + barset.label
                               }
                    onDoubleClicked: (index, barset) => {
                                         barDoubleClickedIndex.text = "Index : " + index
                                         barDoubleClickedLabel.text = "Label : " + barset.label
                                     }
                    onHoverEnter: (seriesName, position, value) => {
                                      barHoverEnteredSeries.text = "Series : " + seriesName
                                      barHoverEnteredPosition.text = "Position : " + position
                                      barHoverEnteredValue.text = "Value : " + value
                                  }
                    onHover: (seriesName, position, value) => {
                                 barHoverSeries.text = "Series : " + seriesName
                                 barHoverPosition.text = "Position : " + position
                                 barHoverValue.text = "Value : " + value
                             }
                    onHoverExit: (seriesName, position) => {
                                     barHoverExitSeries.text = "Series : " + seriesName
                                     barHoverExitPosition.text = "Position : " + position
                                 }
                }
            }
            Column {
                Layout.minimumWidth: 250
                spacing: 5

                Text {
                    text: "Pressed"
                    color: "white"
                    font.pointSize: 20
                }

                Text {
                    id: barPressedIndex
                    text: "Index :"
                    color: "white"
                }

                Text {
                    id: barPressedLabel
                    text: "Label :"
                    color: "white"
                }

                Text {
                    text: "Released"
                    color: "white"
                    font.pointSize: 20
                }

                Text {
                    id: barReleasedIndex
                    text: "Index :"
                    color: "white"
                }

                Text {
                    id: barReleasedLabel
                    text: "Label :"
                    color: "white"
                }

                Text {
                    text: "Clicked"
                    color: "white"
                    font.pointSize: 20
                }

                Text {
                    id: barClickedIndex
                    text: "Index :"
                    color: "white"
                }

                Text {
                    id: barClickedLabel
                    text: "Label :"
                    color: "white"
                }

                Text {
                    text: "DoubleClicked"
                    color: "white"
                    font.pointSize: 20
                }

                Text {
                    id: barDoubleClickedIndex
                    text: "Index :"
                    color: "white"
                }

                Text {
                    id: barDoubleClickedLabel
                    text: "Label :"
                    color: "white"
                }

                Text {
                    text: "HoverEnterd"
                    color: "white"
                    font.pointSize: 20
                }

                Text {
                    id: barHoverEnteredSeries
                    text: "Series :"
                    color: "white"
                }

                Text {
                    id: barHoverEnteredPosition
                    text: "Position :"
                    color: "white"
                }

                Text {
                    id: barHoverEnteredValue
                    text: "Value :"
                    color: "white"
                }

                Text {
                    text: "Hover"
                    color: "white"
                    font.pointSize: 20
                }

                Text {
                    id: barHoverSeries
                    text: "Series :"
                    color: "white"
                }

                Text {
                    id: barHoverPosition
                    text: "Position :"
                    color: "white"
                }

                Text {
                    id: barHoverValue
                    text: "Value :"
                    color: "white"
                }

                Text {
                    text: "HoverExit"
                    color: "white"
                    font.pointSize: 20
                }

                Text {
                    id: barHoverExitSeries
                    text: "Series :"
                    color: "white"
                }

                Text {
                    id: barHoverExitPosition
                    text: "Position :"
                    color: "white"
                }
            }
        }

        // Area Graph
        RowLayout {
            GraphsView {
                Layout.fillWidth: true
                Layout.fillHeight: true

                axisX: ValueAxis {
                    id: xAxis
                    max: 8
                }
                axisY: ValueAxis {
                    id: yAxis
                    max: 4
                }

                AreaSeries {
                    selectable: true
                    hoverable: true

                    upperSeries: SplineSeries {
                        XYPoint { x: 3; y: 1 }
                        XYPoint { x: 4; y: 2.5 }
                        XYPoint { x: 5; y: 2.8 }
                    }

                    lowerSeries: SplineSeries {
                        XYPoint { x: 3.4; y: 0.5 }
                        XYPoint { x: 4; y: 1.5 }
                        XYPoint { x: 5; y: 2 }
                    }

                    onPressed: (point) => {
                                   areaPressedPoint.text = "Point : " + point
                               }

                    onReleased: (point) => {
                                    areaReleasedPoint.text = "Point : " + point
                                }

                    onClicked: (point) => {
                                   areaClickedPoint.text = "Point : " + point
                               }

                    onDoubleClicked: (point) => {
                                         areaDoubleClickedPoint.text = "Point : " + point
                                     }

                    onHoverEnter: (seriesName, position, value) => {
                                      areaHoverEnteredSeries.text = "Series : " + seriesName
                                      areaHoverEnteredPosition.text = "Position : " + position
                                      areaHoverEnteredValue.text = "Value : " + value
                                  }
                    onHover: (seriesName, position, value) => {
                                 areaHoverSeries.text = "Series : " + seriesName
                                 areaHoverPosition.text = "Position : " + position
                                 areaHoverValue.text = "Value : " + value
                             }
                    onHoverExit: (seriesName, position) => {
                                     areaHoverExitSeries.text = "Series : " + seriesName
                                     areaHoverExitPosition.text = "Position : " + position
                                 }
                }
            }
            Column {
                Layout.minimumWidth: 250
                spacing: 5

                Text {
                    text: "Pressed"
                    color: "white"
                    font.pointSize: 20
                }

                Text {
                    id: areaPressedPoint
                    text: "Point :"
                    color: "white"
                }

                Text {
                    text: "Released"
                    color: "white"
                    font.pointSize: 20
                }

                Text {
                    id: areaReleasedPoint
                    text: "Point :"
                    color: "white"
                }

                Text {
                    text: "Clicked"
                    color: "white"
                    font.pointSize: 20
                }

                Text {
                    id: areaClickedPoint
                    text: "Point :"
                    color: "white"
                }

                Text {
                    text: "DoubleClicked"
                    color: "white"
                    font.pointSize: 20
                }

                Text {
                    id: areaDoubleClickedPoint
                    text: "Point :"
                    color: "white"
                }

                Text {
                    text: "HoverEnterd"
                    color: "white"
                    font.pointSize: 20
                }

                Text {
                    id: areaHoverEnteredSeries
                    text: "Series :"
                    color: "white"
                }

                Text {
                    id: areaHoverEnteredPosition
                    text: "Position :"
                    color: "white"
                }

                Text {
                    id: areaHoverEnteredValue
                    text: "Value :"
                    color: "white"
                }

                Text {
                    text: "Hover"
                    color: "white"
                    font.pointSize: 20
                }

                Text {
                    id: areaHoverSeries
                    text: "Series :"
                    color: "white"
                }

                Text {
                    id: areaHoverPosition
                    text: "Position :"
                    color: "white"
                }

                Text {
                    id: areaHoverValue
                    text: "Value :"
                    color: "white"
                }

                Text {
                    text: "HoverExit"
                    color: "white"
                    font.pointSize: 20
                }

                Text {
                    id: areaHoverExitSeries
                    text: "Series :"
                    color: "white"
                }

                Text {
                    id: areaHoverExitPosition
                    text: "Position :"
                    color: "white"
                }
            }
        }

        // Pie Graph
        RowLayout {
            GraphsView {
                Layout.fillWidth: true
                Layout.fillHeight: true

                theme: GraphsTheme {
                    colorScheme: GraphsTheme.ColorScheme.Dark
                    theme: GraphsTheme.Theme.QtGreen
                }

                PieSeries {
                    selectable: true
                    hoverable: true
                    PieSlice {
                        label: "Volkswagen"
                        labelVisible: true
                        value: 13.5
                        exploded: true
                    }
                    PieSlice {
                        label: "Toyota"
                        labelVisible: true
                        labelPosition: PieSlice.LabelPosition.InsideHorizontal
                        labelColor: 'black'
                        value: 10.9
                    }
                    PieSlice {
                        label: "Ford"
                        labelVisible: true
                        labelPosition: PieSlice.LabelPosition.InsideNormal
                        labelColor: 'black'
                        value: 8.6
                    }
                    PieSlice {
                        label: "Skoda"
                        labelVisible: true
                        labelPosition: PieSlice.LabelPosition.InsideTangential
                        labelColor: 'black'
                        value: 8.2
                    }
                    PieSlice {
                        label: "Volvo"
                        labelVisible: true
                        value: 6.8
                    }
                    PieSlice {
                        label: "Others"
                        labelVisible: true
                        value: 52.0
                    }

                    onPressed: (slice) => {
                                   piePressedSlice.text = "Slice Label : " + slice.label
                               }

                    onReleased: (slice) => {
                                    pieReleasedSlice.text = "Slice Label : " + slice.label
                                }

                    onClicked: (slice) => {
                                   pieClickedSlice.text = "Slice Label : " + slice.label
                               }

                    onDoubleClicked: (slice) => {
                                         pieDoubleClickedSlice.text = "Slice Label : " + slice.label
                                     }

                    onHoverEnter: (seriesName, position, value) => {
                                      pieHoverEnteredSeries.text = "Series : " + seriesName
                                      pieHoverEnteredPosition.text = "Position : " + position
                                      pieHoverEnteredValue.text = "Value : " + value
                                  }
                    onHover: (seriesName, position, value) => {
                                 pieHoverSeries.text = "Series : " + seriesName
                                 pieHoverPosition.text = "Position : " + position
                                 pieHoverValue.text = "Value : " + value
                             }
                    onHoverExit: (seriesName, position) => {
                                     pieHoverExitSeries.text = "Series : " + seriesName
                                     pieHoverExitPosition.text = "Position : " + position
                                 }
                }
            }
            Column {
                Layout.minimumWidth: 250
                spacing: 5

                Text {
                    text: "Pressed"
                    color: "white"
                    font.pointSize: 20
                }

                Text {
                    id: piePressedSlice
                    text: "Slice Label :"
                    color: "white"
                }

                Text {
                    text: "Released"
                    color: "white"
                    font.pointSize: 20
                }

                Text {
                    id: pieReleasedSlice
                    text: "Slice Label :"
                    color: "white"
                }

                Text {
                    text: "Clicked"
                    color: "white"
                    font.pointSize: 20
                }

                Text {
                    id: pieClickedSlice
                    text: "Slice Label :"
                    color: "white"
                }

                Text {
                    text: "DoubleClicked"
                    color: "white"
                    font.pointSize: 20
                }

                Text {
                    id: pieDoubleClickedSlice
                    text: "Slice Label :"
                    color: "white"
                }

                Text {
                    text: "HoverEnterd"
                    color: "white"
                    font.pointSize: 20
                }

                Text {
                    id: pieHoverEnteredSeries
                    text: "Series :"
                    color: "white"
                }

                Text {
                    id: pieHoverEnteredPosition
                    text: "Position :"
                    color: "white"
                }

                Text {
                    id: pieHoverEnteredValue
                    text: "Value :"
                    color: "white"
                }

                Text {
                    text: "Hover"
                    color: "white"
                    font.pointSize: 20
                }

                Text {
                    id: pieHoverSeries
                    text: "Series :"
                    color: "white"
                }

                Text {
                    id: pieHoverPosition
                    text: "Position :"
                    color: "white"
                }

                Text {
                    id: pieHoverValue
                    text: "Value :"
                    color: "white"
                }

                Text {
                    text: "HoverExit"
                    color: "white"
                    font.pointSize: 20
                }

                Text {
                    id: pieHoverExitSeries
                    text: "Series :"
                    color: "white"
                }

                Text {
                    id: pieHoverExitPosition
                    text: "Position :"
                    color: "white"
                }
            }
        }
    }
}
