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

            Rectangle {
                id: downloadCounterBox
                Layout.preferredHeight: 90
                Layout.fillHeight: false
                Layout.fillWidth: true

                color: fillColor

                border.color: borderColor
                border.width: 2
            }

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

            Rectangle {
                id: settingsBox
                Layout.preferredHeight: 50
                Layout.fillHeight: false
                Layout.fillWidth: true

                color: fillColor

                border.color: borderColor
                border.width: 2

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
                id: downloadsToolbar
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

                // Separator border
                Rectangle {
                    height: parent.height
                    width: 2
                    x: 200

                    color: borderColor
                }
            }

            Rectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true
                color: "transparent"
            }

        }
    }
}
