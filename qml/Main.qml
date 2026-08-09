import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import "components"
import "pages"

ApplicationWindow 
{
    id: root
    width: 1200
    height: 700

    title: "Quantum"

    opacity: 0

    visible: true
    Component.onCompleted: openAnimated()

    property bool isMaximized: false
    property rect normalGeometry: Qt.rect(x, y, width, height)

    // Remove the Default title bar
    flags: Qt.Window | Qt.FramelessWindowHint

    onWindowStateChanged: {
        isMaximized = (visibility === Window.Maximized)
    }

    // wrap existing content
    Item {
        id: contentWrapper
        anchors.fill: parent
        scale: 0.98

        // Add the child windows
        UrlWindow {
            id: urlWindow
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
                            root.toggleMaximize()
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
                            root.toggleMaximize()
                        }
                    }

                    WindowButton {
                        id: close
                        buttonIcon: "qrc:/qml/assets/icons/close.png"
                        hoverColor: "#ff0000"
                        clickColor: "#700000"
                        onClicked: root.closeAnimated()
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

            MainPage {
                anchors.fill: parent

                onNewDownloadRequested: {
                    if(!urlWindow.visible)
                        urlWindow.openAnimated()

                    urlWindow.raise()
                    urlWindow.requestActivate()
                }
            }
        }
    }

    function openAnimated() {
        show()
        raise()
        requestActivate()
        openAnim.start()
    }

    ParallelAnimation {
        id: openAnim
        NumberAnimation { target: root; property: "opacity"; to: 1; duration: 120; easing.type: Easing.OutCubic }
        NumberAnimation { target: contentWrapper; property: "scale"; to: 1; duration: 120;}
    }

    function closeAnimated() {
        closeAnim.start()
    }

    ParallelAnimation {
        id: closeAnim
        NumberAnimation { target: root; property: "opacity"; to: 0; duration: 120 }
        NumberAnimation { target: contentWrapper; property: "scale"; to: 0.92; duration: 120 }
        onFinished: root.hide()
    }

    Behavior on width { NumberAnimation { duration: 180; easing.type: Easing.OutCubic } }
    Behavior on height { NumberAnimation { duration: 180; easing.type: Easing.OutCubic } }
    Behavior on x { NumberAnimation { duration: 180; easing.type: Easing.OutCubic } }
    Behavior on y { NumberAnimation { duration: 180; easing.type: Easing.OutCubic } }

    function toggleMaximize() {
        if (!root.isMaximized) {
            normalGeometry = Qt.rect(x, y, width, height)
            var screenGeo = backend.availableScreenGeometry()
            x = screenGeo.x; y = screenGeo.y
            width = screenGeo.width; height = screenGeo.height
            isMaximized = true
        } else {
            x = normalGeometry.x; y = normalGeometry.y
            width = normalGeometry.width; height = normalGeometry.height
            isMaximized = false
        }
    }
}