import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Controls

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

    signal activated(int index)

    height: buttonHeight
    width: buttonWidth

    Rectangle {
        id: background
        anchors.fill: parent

        color: mouseArea.pressed ? pressedColor : mouseArea.containsMouse ? hoverColor : fillColor
        scale: mouseArea.pressed ? 0.98 : 1.0

        radius: 10

        Behavior on color {
            ColorAnimation { duration: 80 }
        }

        Behavior on scale {
            NumberAnimation { duration: 80 }
        }

        border.color: borderColor
        border.width: 2

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true

            onClicked: popup.visible = !popup.visible
        }

        RowLayout {
            anchors.fill: parent

            Item {
                Layout.fillWidth: true
            }

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
                id: text
                text: root.currentText
                font.family: appFont.name
                font.pixelSize: 15
                color: "#ffffff"
            }

            // Arrow indicator
            Text {
                text: popup.visible ? "▲" : "▼"
                font.pixelSize: 10
                color: "#ffffff"
            }

            Item {
                Layout.fillWidth: true
            }
        }
    }

    // Popup list
    Popup {
        id: popup
        parent: root

        y: -popup.height - 4
        width: root.width
        padding: 0

        background: Rectangle {
            color: popupColor
            border.color: borderColor
            border.width: 2
            radius: 8
        }

        contentItem: ColumnLayout {
            spacing: 0

            Repeater {
                model: root.model

                delegate: Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 32

                    color: itemMouseArea.containsMouse ? itemHoverColor : "transparent"

                    Behavior on color {
                        ColorAnimation { duration: 80 }
                    }

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
                            popup.close()
                        }
                    }
                }
            }
        }
    }
}