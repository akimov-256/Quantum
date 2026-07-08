import QtQuick 2.15
import QtQuick.Layouts

Item {
    id: root

    property int counterHeight
    property int counterWdith
    property color fillColor
    property color borderColor
    property string counterText
    property string titleText

    height: counterHeight
    width: counterWdith

    Rectangle {
        id: background
        anchors.fill: parent

        radius: 10
        color: fillColor
        border.color: borderColor
        border.width: 3

        ColumnLayout {
            id: layout

            anchors.fill: parent

            Item {
                Layout.fillHeight: true
            }

            FontLoader {
                id: appFont
                source: "qrc:/qml/assets/fonts/Lexend.ttf"
            }

            // Count text
            Text {
                Layout.alignment: Qt.AlignHCenter

                text: counterText
                font.family: appFont.name
                font.pixelSize: 20
                color: borderColor
            }

            // Title text
            Text {
                Layout.alignment: Qt.AlignHCenter

                text: titleText
                font.family: appFont.name
                font.pixelSize: 9
                color: borderColor
            }

            Item {
                Layout.fillHeight: true
            }
        }
    }
}
