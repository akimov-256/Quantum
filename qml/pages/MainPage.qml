import QtQuick 2.15
import QtQuick.Layouts
import "../components"

Item {
    id: root

    property color fillColor: "#100019"
    property color borderColor: "#35003D"

    anchors.fill: parent

    // The main layout
    RowLayout {
        anchors.fill: parent

        spacing: 0

        // The sidebar
        ColumnLayout {
            id: sideBar

            spacing: 0

            Layout.preferredWidth: 240
            Layout.fillWidth: false
            Layout.fillHeight: true

            // Download Counter Box
            Rectangle {
                id: downloadCounterBox
                Layout.preferredHeight: 90
                Layout.fillHeight: false
                Layout.fillWidth: true

                color: fillColor

                border.color: borderColor
                border.width: 2

                RowLayout {
                    anchors.fill: parent
                    spacing: 20

                    Item {
                        Layout.fillWidth: true
                    }

                    // Completed
                    Counter {
                        id: completed

                        counterHeight: 70
                        counterWdith: 90

                        fillColor: "#041500"
                        borderColor: "#0AC300"

                        counterText: "10"
                        titleText: "COMPLETED"
                    }

                    // Downloading
                    Counter {
                        id: downloading

                        counterHeight: 70
                        counterWdith: 90

                        fillColor: "#150000"
                        borderColor: "#C30003"

                        counterText: "3"
                        titleText: "DOWNLOADING"
                    }

                    Item {
                        Layout.fillWidth: true
                    }
                }
            }

            // Categories box
            Rectangle {
                id: categoriesBox
                Layout.fillHeight: true
                Layout.fillWidth: true

                color: fillColor

                // Left border
                Rectangle {
                    width: 2
                    height: parent.height

                    color: borderColor

                    anchors.left: parent.left
                    anchors.top: parent.top
                }

                // Right border
                Rectangle {
                    width: 2
                    height: parent.height

                    color: borderColor

                    anchors.right: parent.right
                    anchors.top: parent.top
                }
            }

            // Settings box
            Rectangle {
                id: settingsBox
                Layout.preferredHeight: 50
                Layout.fillHeight: false
                Layout.fillWidth: true

                color: fillColor

                border.color: borderColor
                border.width: 2

                // Settings button
                UiButton {
                    id: settingsButton

                    anchors.centerIn: parent

                    buttonHeight: 32
                    buttonWidth: 215

                    fillColor: fillColor
                    borderColor: borderColor

                    buttonText: "Settings"
                    buttonIcon: "qrc:/qml/assets/icons/setting.png"
                }
            }
        }

        // The downloads layout
        ColumnLayout {
            id: downloadsLayout

            spacing: 0

            Layout.fillHeight: true
            Layout.fillWidth: true


            Rectangle {
                Layout.preferredHeight: 50
                Layout.fillHeight: false
                Layout.fillWidth: true

                color: fillColor

                // Top border
                Rectangle {
                    height: 2
                    width: parent.width

                    color: borderColor

                    anchors.top: parent.top
                    anchors.left: parent.left
                }

                // Bottom border
                Rectangle {
                    height: 2
                    width: parent.width

                    color: borderColor

                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                }

                // Right border
                Rectangle {
                    height: parent.height
                    width: 2

                    color: borderColor

                    anchors.right: parent.right
                    anchors.top: parent.top
                }

                RowLayout {
                    id: downloadsToolbarBox

                    anchors.fill: parent
                    anchors.leftMargin: 10
                    spacing: 10

                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    // New download
                    UiButton {
                        id: newDownloadButton

                        Layout.alignment: Qt.AlignVCenter

                        buttonHeight: 32
                        buttonWidth: 170

                        fillColor: fillColor
                        borderColor: borderColor

                        buttonText: "New Download"
                        buttonIcon: "qrc:/qml/assets/icons/download.png"
                    }

                    // Separator border
                    Rectangle {
                        Layout.fillHeight: true
                        Layout.preferredWidth: 2
                        Layout.fillWidth: false

                        color: borderColor
                    }

                    // Pause all
                    UiButton {
                        id: pauseAllButton

                        Layout.alignment: Qt.AlignVCenter

                        buttonHeight: 32
                        buttonWidth: 120

                        fillColor: fillColor
                        borderColor: borderColor

                        buttonText: "Pause All"
                        buttonIcon: "qrc:/qml/assets/icons/pause.png"
                    }

                    // Resume all
                    UiButton {
                        id: resumeAllButton

                        Layout.alignment: Qt.AlignVCenter

                        buttonHeight: 32
                        buttonWidth: 120

                        fillColor: fillColor
                        borderColor: borderColor

                        buttonText: "Resume All"
                        buttonIcon: "qrc:/qml/assets/icons/play.png"
                    }

                    Item {
                        Layout.fillWidth: true
                    }
                }
            }

            // Download cards box
            Rectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true
                color: "transparent"
            }

        }
    }
}
