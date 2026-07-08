import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts
import "components"

ApplicationWindow {
    id: root
    visible: true

    title: "New Download"

    height: 320
    width: 650

    flags: Qt.Window | Qt.FramelessWindowHint

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
                text: "NEW DOWNLOAD"
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
                    id: close
                    buttonIcon: "qrc:/qml/assets/icons/close.png"
                    hoverColor: "#ff0000"
                    clickColor: "#700000"
                    onClicked: root.close()
                }
            }
        }
    }

    Rectangle {
        anchors {
            top: titleBar.bottom
            right: parent.right
            left: parent.left
            bottom: parent.bottom
        }

        color: "#100019"
    }
}
