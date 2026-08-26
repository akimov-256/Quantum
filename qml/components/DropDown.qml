import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Window 2.15

Item {
    id: root

    property int buttonHeight
    property int buttonWidth
    property string buttonIcon
    property color fillColor: "#35003D"
    property color hoverColor: "#4A0055"
    property color pressedColor: "#2A0030"
    property color borderColor: "#AC00FB"
    property color popupColor: "#1B002B"
    property color itemHoverColor: "#2A0040"

    property var model: []
    property int currentIndex: 0
    readonly property string currentText: model.length > 0 ? model[currentIndex] : ""
    readonly property int itemHeight: 32

    property bool dropdownEnabled: false
    property bool openDownward: true
    property bool isActivated: false

    signal activated(int index)

    height: buttonHeight
    width: buttonWidth

    Rectangle {
        id: background
        anchors.fill: parent

        color: dropdownEnabled ? mouseArea.pressed ? pressedColor : mouseArea.containsMouse ? hoverColor : fillColor : fillColor
        scale: dropdownEnabled ? mouseArea.pressed ? 0.98 : 1.0 : 1.0
        opacity: dropdownEnabled ? 1.0 : 0.5

        radius: 10

        Behavior on color { ColorAnimation { duration: 80 } }
        Behavior on scale { NumberAnimation { duration: 80 } }

        border.color: borderColor
        border.width: 2

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true

            onClicked: {
                if (!dropdownEnabled)
                    return

                if (isActivated) {
                    isActivated = false
                    return
                }

                var globalPos = root.mapToGlobal(0, 0)
                var screenHeight = root.Window.window ? root.Window.window.screen.height : Screen.height
                var spaceBelow = screenHeight - (globalPos.y + root.height)

                root.openDownward = spaceBelow >= popupWindow.height + 4

                popupWindow.x = globalPos.x
                popupWindow.y = root.openDownward
                    ? globalPos.y + root.height + 4
                    : globalPos.y - popupWindow.height - 4

                isActivated = true
            }
        }

        RowLayout {
            anchors.fill: parent

            Item { Layout.fillWidth: true }

            Image {
                id: icon
                source: buttonIcon
                visible: buttonIcon !== ""
                Layout.preferredHeight: 16.25
                Layout.preferredWidth: 16.25
                Layout.fillHeight: false
                Layout.fillWidth: false
            }

            FontLoader {
                id: appFont
                source: "qrc:/qml/assets/fonts/Lexend.ttf"
            }

            Text {
                text: root.currentText
                font.family: appFont.name
                font.pixelSize: 15
                color: "#ffffff"
            }

            Text {
                text: isActivated ? "▲" : "▼"
                font.pixelSize: 10
                color: "#ffffff"
            }

            Item { Layout.fillWidth: true }
        }
    }

    // Separate top-level window so the list can render outside UrlWindow's bounds
    Window {
        id: popupWindow

        flags: Qt.Popup | Qt.FramelessWindowHint
        color: "transparent"

        width: root.width
        height: root.model.length * root.itemHeight

        visible: isActivated

        onActiveChanged: {
            if (!active)
                isActivated = false
        }

        Rectangle {
            anchors.fill: parent
            color: popupColor
            border.color: borderColor
            border.width: 2
            radius: 8

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                Repeater {
                    model: root.model

                    delegate: Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: root.itemHeight

                        color: itemMouseArea.containsMouse ? itemHoverColor : "transparent"
                        Behavior on color { ColorAnimation { duration: 80 } }

                        Text {
                            anchors.fill: parent
                            anchors.leftMargin: 10
                            verticalAlignment: Text.AlignVCenter
                            text: modelData
                            color: "#ffffff"
                            font.family: appFont.name
                            font.pixelSize: 14
                        }

                        MouseArea {
                            id: itemMouseArea
                            anchors.fill: parent
                            hoverEnabled: true

                            onClicked: {
                                root.currentIndex = index
                                root.activated(index)
                                isActivated = false
                            }
                        }
                    }
                }
            }
        }
    }
}