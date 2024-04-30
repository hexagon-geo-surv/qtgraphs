// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Fusion
import QtGraphs
//import QtDataVisualization
import "."

Item {
    id: mainview
    width: 1280
    height: 820

    property var customTheme: customSurfaceTheme
    property var currentGraph: surface

    Gradient {
        id: customGradient
        GradientStop { id: redstop; position: 0.0; color: "red" }
        GradientStop { id: greenstop; position: 1.0; color: "green" }
    }

    Gradient {
        id: singleGradient
        GradientStop { position: 0.0; color: "white" }
        GradientStop { position: 1.0; color: "yellow" }
    }

    Gradient {
        id: multiGradient
        GradientStop { position: 0.0; color: "white" }
        GradientStop { position: 1.0; color: "blue" }
    }

    Color {
        id: barColor
        color: "blue"
    }

    Color {
        id: surfaceColor
        color: "white"
    }

    GraphsTheme {
        id: customSurfaceTheme
        theme: GraphsTheme.Theme.QtGreen
        colorStyle: GraphsTheme.ObjectGradient
        backgroundColor: "gray"
        gridMainColor: "lightGray"
        multiHighlightColor: "orange"
        singleHighlightColor: "yellow"
        multiHighlightGradient: multiGradient
        singleHighlightGradient: singleGradient
    }

    GraphsTheme {
        id: customBarsTheme
        theme: GraphsTheme.Theme.UserDefined
        colorStyle: GraphsTheme.ObjectGradient
        baseColors: [barColor]
        baseGradients: [customGradient]
        backgroundColor: "gray"
        gridMainColor: "lightGray"
        multiHighlightColor: "orange"
        singleHighlightColor: "yellow"
        multiHighlightGradient: multiGradient
        singleHighlightGradient: singleGradient
    }

    Item {
        id: graphView
        width: mainview.width - settings.width
        height: mainview.height
        anchors.right: mainview.right;

        Surface3D {
            id: surface
            anchors.fill: graphView
            theme: customSurfaceTheme
            shadowQuality: AbstractGraph3D.ShadowQuality.None
            selectionMode: AbstractGraph3D.SelectionNone
            cameraPreset: AbstractGraph3D.CameraPreset.IsometricLeft
            msaaSamples: 4
            aspectRatio: 3.0
            visible: !graphMod.barsVisible
            shaderGridEnabled: backMod.testgridChange.checked

            Surface3DSeries {
                id: surfaceSeries
                // If you want to check with series colors, uncomment these
                // baseGradient: customGradient
                // baseColor: surfaceColor.color
                HeightMapSurfaceDataProxy {
                    heightMapFile: ":/layer_1.png"
                    autoScaleY: true
                }
                flatShadingEnabled: false
                drawMode: Surface3DSeries.DrawSurface
            }
        }

        Bars3D {
            id: bars
            anchors.fill: graphView
            theme: customBarsTheme
            shadowQuality: AbstractGraph3D.ShadowQuality.None
            selectionMode: AbstractGraph3D.SelectionItemAndRow
            cameraPreset: AbstractGraph3D.CameraPreset.IsometricLeft
            //optimizationHint: AbstractGraph3D.OptimizationHint.Legacy
            msaaSamples: 4
            aspectRatio: 3.0
            visible: graphMod.barsVisible
            shaderGridEnabled: backMod.testgridChange.checked

            Bar3DSeries {
                id: barsSeries
                // If you want to check with series colors, uncomment these
                // baseGradient: customGradient
                // baseColor: barColor.color
                ItemModelBarDataProxy {
                    id: barProxy
                    itemModel: ListModel {
                        ListElement{ coords: "0,0"; data: "20.0/10.0/4.75"; }
                        ListElement{ coords: "1,0"; data: "21.1/10.3/3.00"; }
                        ListElement{ coords: "2,0"; data: "22.5/10.7/1.24"; }
                        ListElement{ coords: "3,0"; data: "24.0/10.5/2.53"; }
                        ListElement{ coords: "0,1"; data: "20.2/11.2/3.55"; }
                        ListElement{ coords: "1,1"; data: "21.3/11.5/3.03"; }
                        ListElement{ coords: "2,1"; data: "22.6/11.7/3.46"; }
                        ListElement{ coords: "3,1"; data: "23.4/11.5/4.12"; }
                        ListElement{ coords: "0,2"; data: "20.2/12.3/3.37"; }
                        ListElement{ coords: "1,2"; data: "21.1/12.4/2.98"; }
                        ListElement{ coords: "2,2"; data: "22.5/12.1/3.33"; }
                        ListElement{ coords: "3,2"; data: "23.3/12.7/3.23"; }
                        ListElement{ coords: "0,3"; data: "20.7/13.3/5.34"; }
                        ListElement{ coords: "1,3"; data: "21.5/13.2/4.54"; }
                        ListElement{ coords: "2,3"; data: "22.4/13.6/4.65"; }
                        ListElement{ coords: "3,3"; data: "23.2/13.4/6.67"; }
                        ListElement{ coords: "0,4"; data: "20.6/15.0/6.01"; }
                        ListElement{ coords: "1,4"; data: "21.3/14.6/5.83"; }
                        ListElement{ coords: "2,4"; data: "22.5/14.8/7.32"; }
                        ListElement{ coords: "3,4"; data: "23.7/14.3/6.90"; }
                    }
                    rowRole: "coords"
                    columnRole: "coords"
                    valueRole: "data"
                    rowRolePattern: /(\d),\d/
                    columnRolePattern: /(\d),(\d)/
                    valueRolePattern: /^([^\/]*)\/([^\/]*)\/(.*)$/
                    rowRoleReplace: "\\1"
                    columnRoleReplace: "\\2"
                    valueRoleReplace: "\\3"
                }
            }
        }
    }

    TabBar {
        id: bar
        anchors.top: parent.top
        anchors.left: parent.left
        width: 200

        TabButton {
            text: qsTr("Graph")
        }
        TabButton {
            text: qsTr("Background")
        }
    }

    StackLayout {
        id: settings
        anchors.top: bar.bottom
        anchors.left: parent.left
        anchors.topMargin: 20
        anchors.leftMargin: 20
        width: 200
        currentIndex: bar.currentIndex

        GraphModifiers {id : graphMod}
        BackgroundModifiers {id: backMod}
    }
}
