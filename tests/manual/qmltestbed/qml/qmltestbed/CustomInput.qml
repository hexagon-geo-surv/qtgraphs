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
            text: "Bars"
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
                                   pressedIndex.text = "Index : " + index
                                   pressedLabel.text = "Label : " + barset.label
                               }
                    onReleased: (index, barset) => {
                                   releasedIndex.text = "Index : " + index
                                   releasedLabel.text = "Label : " + barset.label
                               }
                    onClicked: (index, barset) => {
                                   clickedIndex.text = "Index : " + index
                                   clickedLabel.text = "Label : " + barset.label
                               }
                    onDoubleClicked: (index, barset) => {
                                         doubleClickedIndex.text = "Index : " + index
                                         doubleClickedLabel.text = "Label : " + barset.label
                                     }
                    onHoverEnter: (seriesName, position, value) => {
                                      hoverEnteredSeries.text = "Series : " + seriesName
                                      hoverEnteredPosition.text = "Position : " + position
                                      hoverEnteredValue.text = "Value : " + value
                                  }
                    onHover: (seriesName, position) => {
                                      hoverSeries.text = "Series : " + seriesName
                                      hoverPosition.text = "Position : " + position
                                  }
                    onHoverExit: (seriesName, position, value) => {
                                      hoverExitSeries.text = "Series : " + seriesName
                                      hoverExitPosition.text = "Position : " + position
                                      hoverExitValue.text = "Value : " + value
                                  }
                }
            }
            Column {
                id: status
                Layout.minimumWidth: 250
                spacing: 5

                Text {
                    text: "Pressed"
                    color: "white"
                    font.pointSize: 20
                }

                Text {
                    id: pressedIndex
                    text: "Index :"
                    color: "white"
                }

                Text {
                    id: pressedLabel
                    text: "Label :"
                    color: "white"
                }

                Text {
                    text: "Released"
                    color: "white"
                    font.pointSize: 20
                }

                Text {
                    id: releasedIndex
                    text: "Index :"
                    color: "white"
                }

                Text {
                    id: releasedLabel
                    text: "Label :"
                    color: "white"
                }

                Text {
                    text: "Clicked"
                    color: "white"
                    font.pointSize: 20
                }

                Text {
                    id: clickedIndex
                    text: "Index :"
                    color: "white"
                }

                Text {
                    id: clickedLabel
                    text: "Label :"
                    color: "white"
                }

                Text {
                    text: "DoubleClicked"
                    color: "white"
                    font.pointSize: 20
                }

                Text {
                    id: doubleClickedIndex
                    text: "Index :"
                    color: "white"
                }

                Text {
                    id: doubleClickedLabel
                    text: "Label :"
                    color: "white"
                }

                Text {
                    text: "HoverEnterd"
                    color: "white"
                    font.pointSize: 20
                }

                Text {
                    id: hoverEnteredSeries
                    text: "Series :"
                    color: "white"
                }

                Text {
                    id: hoverEnteredPosition
                    text: "Position :"
                    color: "white"
                }

                Text {
                    id: hoverEnteredValue
                    text: "Value :"
                    color: "white"
                }

                Text {
                    text: "Hover"
                    color: "white"
                    font.pointSize: 20
                }

                Text {
                    id: hoverSeries
                    text: "Series :"
                    color: "white"
                }

                Text {
                    id: hoverPosition
                    text: "Position :"
                    color: "white"
                }

                Text {
                    text: "HoverExit"
                    color: "white"
                    font.pointSize: 20
                }

                Text {
                    id: hoverExitSeries
                    text: "Series :"
                    color: "white"
                }

                Text {
                    id: hoverExitPosition
                    text: "Position :"
                    color: "white"
                }

                Text {
                    id: hoverExitValue
                    text: "Value :"
                    color: "white"
                }
            }
        }
    }
}
