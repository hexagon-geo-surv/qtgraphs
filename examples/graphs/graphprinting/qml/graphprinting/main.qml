// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Fusion
import QtQuick.Dialogs

Item {
    id: mainView
    width: 1280
    height: 720

    //! [0]
    TabBar {
        id: tabBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        TabButton {
            text: "2D Graph"
            implicitHeight: 50
        }

        TabButton {
            text: "3D Graph"
            implicitHeight: 50
        }
    }

    StackLayout {
        id: stackLayout
        anchors.top: tabBar.bottom
        anchors.bottom: parent.bottom
        width: parent.width
        currentIndex: tabBar.currentIndex

        Graph2D {}

        Graph3D {}
    }
    //! [0]

    RowLayout {
        id: rowLayout
        anchors.bottom: parent.bottom
        anchors.left: parent.left

        //! [1.2]
        Button {
            id: setFolderButton
            onClicked: dialog.open()
            text: "Set save location"
            Layout.margins: 5
        }
        //! [1.2]
        //! [3.1]
        Button {
            id: captureButton
            text: "Save to PDF"
            Layout.margins: 5
            property var item: stackLayout.itemAt(stackLayout.currentIndex)

            onPressed: {
                if (stackLayout.currentIndex === 1) {
                    item.width = 7282
                    item.height = 4096
                }
                item.grabToImage(function(result) {
                    graphPrinter.generatePDF(dialog.currentFolder, result.image)
                }, Qt.size(7282, 4096))
            }

            onReleased: {
                if (stackLayout.currentIndex === 1) {
                    item.width = mainView.width
                    item.height = mainView.height
                }
            }
        }
        //! [3.1]

        Button {
            id: printButton
            text: "Send to printer"
            Layout.margins: 5
            onPressed: printerDialog.open()
        }
    }

    //! [1.1]
    FolderDialog {
        id: dialog
        onAccepted: console.log("Saving to " + currentFolder)
    }
    //! [1.1]

    //! [2.1]
    Dialog {
        id: printerDialog
        x: parent.width * 0.5 - width * 0.5
        y: parent.height * 0.5;
        contentHeight: 200
        contentWidth: 200

        title: qsTr("Available printers")
        modal: true

        property var item: stackLayout.itemAt(stackLayout.currentIndex)

        onOpened: {
            printerModel.clear()
            var printers = graphPrinter.getPrinters()
            printers.forEach((x,i) =>
                             printerModel.append({"name": x}))
        }
        //! [2.1]

        //! [3.2]
        onAccepted: {
            var selectedPrinter = printerModel.get(printerListView.currentIndex)
            if (stackLayout.currentIndex === 1) {
                item.width = 7282
                item.height = 4096
            }
            item.grabToImage(function(result) {
                graphPrinter.print(result.image, selectedPrinter.name)
            }, Qt.size(7282, 4096))
        }
        onClosed: {
            if (stackLayout.currentIndex === 1) {
                item.width = mainView.width
                item.height = mainView.height
            }
        }
        //! [3.2]

        Component {
            id : printerDelegate
            Rectangle {
                width: 200; height: 40
                color: "transparent"
                border.color: "black"
                Text {
                    padding: 5
                    text: qsTr("<b>%1</b>").arg(name)
                    color: "white"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: printerListView.currentIndex = index
                }
            }
        }

        //! [2.2]
        contentItem: Item {
            id: printerItem
            height: 200
            width: parent.width
            ListView {
                id: printerListView
                height: 200
                width: 200
                clip: true

                model: printerModel
                delegate: printerDelegate
                highlight: Rectangle {color: "darkgrey"}
            }
        }
        //! [2.2]

        footer: DialogButtonBox {
            Button {
                text: "Print"
                DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
            }
            standardButtons: Dialog.Cancel
        }
    }

    ListModel {
        id: printerModel
    }
}


