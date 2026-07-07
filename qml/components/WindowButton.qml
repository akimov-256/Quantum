import QtQuick 2.15

Rectangle {
    id: root

    // Declare properties
    property string buttonIcon
    property color hoverColor : "#151515"
    property color clickColor : "#101010"
    signal clicked()

    width: 26.25
    height: 26.25

    color: mouseArea.pressed ? clickColor : mouseArea.containsMouse ? hoverColor : "transparent"

    Behavior on color {

        ColorAnimation {
            duration: 100
        }
    }

    Image {
        id: icon
        source: buttonIcon
        anchors.centerIn: parent

        width: 12
        height: 12
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true

        onClicked: root.clicked()
    }
}
