import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "components"

ApplicationWindow {
    id: root

    property bool isMaximized: false

    visible: true
    width: 1200
    height: 700

    title: "Quantum"

    // Remove the Default title bar
    flags: Qt.Window | Qt.FramelessWindowHint

    onWindowStateChanged: {
        isMaximized = (visibility === Window.Maximized)
        console.log(isMaximized)
    }

    // Create the title bar
    Rectangle {
        id: titleBar

        width: parent.width
        height: 26.25

        color: "#000000"

        // Add title bar content
        RowLayout {
            id: titleBarLayout

            anchors.fill: parent
            anchors.leftMargin: 10

            FontLoader {
                id: appFont
                source: "qrc:/qml/assets/fonts/Lexend.ttf"
            }

            // Add the app name
            Text {
                id: appName
                text: "QUANTUM DOWNLOAD MANAGER"
                font.family: appFont.name
                font.pixelSize: 15
                color: "#ffffff"
                Layout.alignment: Qt.AlignVCenter
            }

            // Add spacer
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                // Handle window movement and maximizing/minimizing
                MouseArea {
                    anchors.fill: parent
                    property point clickPos: "0,0"

                    onPressed: (mouse) => {
                        root.startSystemMove()
                    }

                    onDoubleClicked: (mouse) => {
                        if (root.isMaximized)
                            root.showNormal()
                        else
                            root.showMaximized()
                    }
                }
            }

            // Add window control buttons
            RowLayout {
                Layout.alignment: Qt.AlignVCenter

                WindowButton {
                    id: minimize
                    buttonIcon: "qrc:/qml/assets/icons/minimize.png"
                    onClicked: root.showMinimized()
                }

                WindowButton {
                    id: maximize
                    buttonIcon: root.isMaximized ? "qrc:/qml/assets/icons/restore.png" : "qrc:/qml/assets/icons/maximize.png"
                    onClicked:
                    {
                        if (root.isMaximized)
                            root.showNormal()
                        else
                            root.showMaximized()
                    }
                }

                WindowButton {
                    id: close
                    buttonIcon: "qrc:/qml/assets/icons/close.png"
                    hoverColor: "#ff0000"
                    clickColor: "#700000"
                    onClicked: root.close()
                }
            }
        }
    }

    // Create the background
    Rectangle {
        id: background

        // Set the position
        anchors {
            top: titleBar.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        width: parent.width
        height: parent.height - titleBar.height

        gradient: Gradient {
            GradientStop{ position: 0.0; color: "#1F0024"}
            GradientStop{ position: 1.0; color: "#0E0010"}
        }
    }
}