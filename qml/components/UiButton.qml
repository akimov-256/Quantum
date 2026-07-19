import QtQuick 2.15
import QtQuick.Layouts

Item {
    id: root

    property int buttonHeight
    property int buttonWidth
    property string buttonText
    property string buttonIcon
    property color fillColor: "#35003D"
    property color hoverColor: "#4A0055"
    property color pressedColor: "#2A0030"
    property color borderColor: "#AC00FB"
    property bool buttonEnabled: true
    signal clicked()

    height: buttonHeight
    width: buttonWidth

    Rectangle {
        height: parent.height
        width: parent.width
        anchors.fill: parent

        color: (!buttonEnabled) ? fillColor : mouseArea.pressed ? pressedColor : mouseArea.containsMouse ? hoverColor : fillColor
        scale: (!buttonEnabled) ? 1.0 : mouseArea.pressed ? 0.98 : 1.0

        radius: 10

        Behavior on color {
            ColorAnimation {
                duration: 80
            }
        }

        Behavior on scale {
            NumberAnimation {
                duration: 80
            }
        }

        border.color: borderColor
        border.width: 2

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true

            onClicked: {
                if (buttonEnabled)
                    root.clicked()
            }
        }

        RowLayout {
            anchors.fill: parent

            // Left spacer
            Item {
                Layout.fillWidth: true
            }

            Image {
                id: icon
                source: buttonIcon

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
                text: buttonText
                font.family: appFont.name
                font.pixelSize: 15
                color: "#ffffff"
            }

            // Right spacer
            Item {
                Layout.fillWidth: true
            }
        }
    }
}
