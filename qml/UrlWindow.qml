import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts
import "components"
import "js/UrlHelper.js" as UrlHelper

Window {
    id: root

    visible: false

    title: "New Download"

    height: 320
    width: 650

    flags: Qt.Window | Qt.FramelessWindowHint

    FontLoader {
        id: appFont
        source: "qrc:/qml/assets/fonts/Lexend.ttf"
    }

    Timer {
        id: headTimer
        interval: 600
        repeat: false

        onTriggered: {
            if (urlBox.text.length > 8) {
                if (UrlHelper.looksLikeUrl(urlBox.text))
                    backend.GetHeadInfo(urlBox.text)
            }
        }
    }

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
        id: body

        anchors {
            top: titleBar.bottom
            right: parent.right
            left: parent.left
            bottom: parent.bottom
        }

        ColumnLayout {
            anchors.fill: parent

            // Download url box
            DialogInputBox {
                id: urlBox

                titleText: "DOWNLOAD URL"
                placeHolderText: "Paste download URL..."

                Layout.fillWidth: true

                Layout.topMargin: 5
                Layout.leftMargin: 25
                Layout.rightMargin: 25

                onBoxTextChanged: headTimer.restart()
            }

            // File name box
            DialogInputBox {
                id: fileNameBox

                titleText: "FILE NAME"
                placeHolderText: "File name (optional)"

                text: backend.fileName

                Layout.fillWidth: true

                Layout.leftMargin: 25
                Layout.rightMargin: 25
            }

            // Save to box
            RowLayout {

                spacing: 10

                Layout.leftMargin: 25
                Layout.rightMargin: 25

                DialogInputBox {
                    id: pathBox

                    titleText: "SAVE TO"
                    placeHolderText: "Select download location..."

                    Layout.fillWidth: true
                }

                UiButton {
                    id: browseButton

                    buttonHeight: 32
                    buttonWidth: 130
                    buttonText: "Browse"
                    buttonIcon: "qrc:/qml/assets/icons/folder.png"

                    Layout.alignment: Qt.AlignBottom
                }
            }

            // SHA256 hash box
            RowLayout {

                spacing: 10

                Layout.leftMargin: 25
                Layout.rightMargin: 25

                DialogInputBox {
                    id: sha256Box

                    titleText: "SHA256 HASH"
                    placeHolderText: "Paste SHA256 for verification (optional)"

                    Layout.fillWidth: true
                }

                UiButton {
                    id: connectionsList

                    buttonHeight: 32
                    buttonWidth: 130
                    buttonText: "8"
                    buttonIcon: "qrc:/qml/assets/icons/left-arrow.png"

                    Layout.alignment: Qt.AlignBottom
                }
            }

            Item {
                Layout.preferredHeight: 10
            }

            // Window buttons
            RowLayout {

                spacing: 10

                Layout.rightMargin: 25

                Item {
                    Layout.fillWidth: true
                }

                UiButton {
                    id: cancelButton

                    buttonHeight: 32
                    buttonWidth: 130
                    buttonText: "Cancel"
                    buttonIcon: "qrc:/qml/assets/icons/close.png"

                    Layout.alignment: Qt.AlignBottom

                    // Close the app
                    onClicked: root.close()
                }

                UiButton {
                    id: startButton

                    buttonHeight: 32
                    buttonWidth: 130
                    buttonText: "Start"
                    buttonIcon: "qrc:/qml/assets/icons/download.png"

                    Layout.alignment: Qt.AlignBottom

                    // Pass info to backend class to start download
                    onClicked: {
                        backend.CreateDownload(urlBox.text, fileNameBox.text, pathBox.text, sha256Box.text)
                        root.close()
                    }
                }
            }

            Item {
                Layout.fillHeight: true
            }
        }

        color: "#100019"
    }
}
