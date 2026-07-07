import QtQuick 2.15
import QtQuick.Layouts

Item {
    id: root

    property int buttonHeight
    property int buttonWidth
    property string buttonText
    property string buttonIcon
    property color fillColor: "#35003D"
    property color borderColor: "#AC00FB"
    signal clicked()

    height: buttonHeight
    width: buttonWidth

    Rectangle {
        height: parent.height
        width: parent.width
        anchors.fill: parent

        color: fillColor
        radius: 10

        border.color: borderColor
        border.width: 2

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
