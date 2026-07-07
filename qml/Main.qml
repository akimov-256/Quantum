import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: mainWindow

    visible: true
    width: 1200
    height: 700

    // Disable resizing
    minimumWidth: 1200
    maximumWidth: 1200
    minimumHeight: 700
    maximumHeight: 700

    title: "Quantum"

    // Remove the Default title bar
    flags: Qt.Window | Qt.FramelessWindowHint

    // Create the title bar
    Rectangle {
        id: titleBar

        width: parent.width
        height: 26.25

        color: "#000000"

        // Handle window movement and maximizing/minimizing
        MouseArea {
            anchors.fill: parent
            property point clickPos: "0,0"

            onPressed: (mouse) => {
                mainWindow.startSystemMove()
            }

            onDoubleClicked: (mouse) => {
                if (mainWindow.visibility === Window.Maximized)
                    mainWindow.showNormal()
                else
                    mainWindow.showMinimized()
            }
        }

        // Add title bar content
        RowLayout {
            id: titleBarLayout

            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.rightMargin: 10

            // Add the app name
            Text {
                id: appName

                text: "QUANTUM DOWNLOAD MANAGER"
                font.pixelSize: 15
                color: "#ffffff"
                Layout.alignment: Qt.AlignVCenter
            }

            // Add spacer
            Item {
                Layout.fillWidth: true
            }

            // Add window control buttons
            RowLayout {
                Layout.alignment: Qt.AlignVCenter
                spacing: 5

                Button {
                    id: minimize
                    text: "—"
                    onClicked: mainWindow.showMinimized()
                }

                Button {
                    id: maximize
                    text: mainWindow.visibility === Window.Maximized ? "🗗" : "🗖"
                    onClicked:
                    {
                        if (mainWindow.visibility === Window.Maximized)
                            mainWindow.showNormal()
                        else
                            mainWindow.showMaximized()
                    }
                }

                Button {
                    id: close
                    text: "✕"
                    onClicked: mainWindow.close()
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