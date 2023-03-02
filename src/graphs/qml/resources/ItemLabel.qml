// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import QtQuick3D

Node {
    property string labelText: "Bar"
    property color backgroundColor: "gray"
    property bool backgroundEnabled: true
    property color labelTextColor: "red"
    property bool borderEnabled : false
    property font labelFont
    property real labelWidth: -1

    Item {
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -25
        width: Math.max(labelWidth / 2, text0.implicitWidth)
        height: text0.implicitHeight
        enabled: false

        Rectangle {
            id: labelBackground
            anchors.fill: parent
            color: backgroundColor
            visible: backgroundEnabled
            border.color: labelTextColor
            border.width: borderEnabled ? 1 : 0
            radius: 3
        }

        Text {
            id: text0
            anchors.centerIn: parent
            color: labelTextColor
            text: labelText
            font: labelFont
            padding: 4
        }
    }
}
