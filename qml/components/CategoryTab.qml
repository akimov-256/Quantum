import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Effects

Item {
    id: root

    property int tabHeight: 40
    property int tabWidth: 238
    property string tabText
    property string tabIcon
    property color hoverFillColor: "#1B002B"
    property color pressedFillColor: "#2A0040"
    property color normalFillColor: "#100019"
    property color selectedFillColor: "#1E0030"
    property color normalBorderColor: "#35003D"
    property color selectedBorderColor: "#AC00FB"
    property color textSelectedColor: "#FFFFFF"
    property color textNormalColor: "#656565"
    property bool isSelected: false
    signal pressed()

    height: tabHeight
    width: tabWidth

    Rectangle {
        id: backgoround

        anchors.fill: parent

        color: mouseArea.pressed ? pressedFillColor : mouseArea.containsMouse ? hoverFillColor : isSelected ? selectedFillColor : normalFillColor

        Behavior on color {
            ColorAnimation {
                duration: 80
            }
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true

            onPressed: root.pressed()
        }

        // Right border
        Rectangle {
            id: border

            anchors.right: parent.right
            anchors.top: parent.top

            height: parent.height
            width: isSelected ? 4 : 2

            color: isSelected ? selectedBorderColor : normalBorderColor
        }

        // Content
        RowLayout {
            anchors.fill: parent

            spacing: 0

            // TabIcon
            Image {
                id: icon
                source: tabIcon

                visible: false
            }

            MultiEffect {
                Layout.preferredHeight: 22
                Layout.preferredWidth: 22

                Layout.leftMargin: 10

                source: icon

                brightness: isSelected ? 0 : -0.604
            }

            Item {
                Layout.preferredWidth: 5
            }

            FontLoader {
                id: appFont
                source: "qrc:/qml/assets/fonts/Lexend.ttf"
            }

            Text {
                id: text
                text: tabText

                font.family: appFont.name
                font.pixelSize: 20
                color: isSelected ? textSelectedColor : textNormalColor
            }

            Item {
                Layout.fillWidth: true
            }
        }
    }
}

