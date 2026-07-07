import QtQuick
import QtQuick.Controls

ApplicationWindow {
    visible: true
    width: 1200
    height: 700

    title: "Quantum"

    Rectangle {
        width: 1200
        height: 700
        anchors.fill: parent
        gradient: Gradient {
            GradientStop{ position: 0.0; color: "#1F0024"}
            GradientStop{ position: 1.0; color: "#0E0010"}
        }
    }

    Label {
        anchors.centerIn: parent
        text: "Hello Quantum!"
    }
}