import QtCore
import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import "components"
import "js/Helper.js" as Helper

Window {
    id: root

    property alias downloadUrl: urlBox.text

    opacity: 0

    visible: false

    title: "New Download"

    height: 320
    width: 650

    color: "transparent"

    flags: Qt.Window | Qt.FramelessWindowHint

    function resetForm() {
        downloadUrl = ""
        fileNameBox.text = ""
        pathBox.text = Helper.formatFilePaths(StandardPaths.writableLocation(StandardPaths.DownloadLocation))
        sha256Box.text = ""
    }

    FontLoader {
        id: appFont
        source: "qrc:/qml/assets/fonts/Lexend.ttf"
    }

    Timer {
        id: headTimer
        interval: 600
        repeat: false

        onTriggered: {
            if (urlBox.text.length <= 8) {
                return
            }
            if (Helper.looksLikeUrl(urlBox.text))
                backend.getHeadInfo(urlBox.text, pathBox.text)
        }
    }

    Item {
        id: contentWrapper
        anchors.fill: parent
        scale: 0.98

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
                        onClicked: {
                            root.closeAnimated()
                            root.resetForm()
                        }
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

                    text: ""

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

                    Layout.fillWidth: true

                    Layout.leftMargin: 25
                    Layout.rightMargin: 25

                    Connections {
                        target: backend
                        function onFileNameChanged() {
                            fileNameBox.text = backend.fileName
                        }
                    }
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

                        Component.onCompleted: {
                            text = Helper.formatFilePaths(StandardPaths.writableLocation(StandardPaths.DownloadLocation))
                        }
                    }

                    FolderDialog {
                        id: folderDialog

                        title: "Select download location"
                        currentFolder: pathBox.text !== "" ? "file:///" + pathBox.text : StandardPaths.writableLocation(StandardPaths.DownloadLocation)

                        onAccepted: {
                            pathBox.text = Helper.formatFilePaths(selectedFolder)
                        }
                    }

                    UiButton {
                        id: browseButton

                        buttonHeight: 32
                        buttonWidth: 130
                        buttonText: "Browse"
                        buttonIcon: "qrc:/qml/assets/icons/folder.svg"

                        Layout.alignment: Qt.AlignBottom

                        onClicked: folderDialog.open()
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

                    ColumnLayout {

                        Text {
                            text: "CONNECTIONS"

                            color: "#656565"

                            font.pixelSize: 10
                            font.family: appFont.name
                        }

                        DropDown {
                            id: connectionsList

                            buttonHeight: 32
                            buttonWidth: 130

                            model: Helper.getThreadSelectionModel(backend.fileSize)
                            currentIndex: model.length > 2 ? model.indexOf("8") : model.length - 1

                            Layout.alignment: Qt.AlignBottom

                            dropdownEnabled: backend.headReqCompleted

                            onActivated: (index) => {
                                currentIndex = index
                            }
                        }
                    }
                }

                Item {
                    Layout.preferredHeight: 10
                }

                // Window buttons
                RowLayout {

                    spacing: 10

                    Layout.rightMargin: 25
                    Layout.leftMargin: 25

                    Text {
                        id: fileSize

                        Layout.alignment: Qt.AlignVCenter

                        text: {
                            if (urlBox.text === "")
                                return ""

                            if (backend.isHeadReqActive)
                                return "Calculating file size..."

                            if (backend.fileSize > 0)
                                return "File Size: " + Helper.formatFileSize(backend.fileSize)

                            return ""
                        }

                        color: "white"
                        font.family: appFont.name
                        font.pixelSize: 16
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    UiButton {
                        id: cancelButton

                        buttonHeight: 32
                        buttonWidth: 130
                        buttonText: "Cancel"
                        buttonIcon: "qrc:/qml/assets/icons/close.svg"

                        Layout.alignment: Qt.AlignBottom

                        // Close the app
                        onClicked: {
                            root.closeAnimated()
                            root.resetForm()
                        }
                    }

                    UiButton {
                        id: startButton

                        buttonHeight: 32
                        buttonWidth: 130
                        buttonText: "Start"
                        buttonIcon: "qrc:/qml/assets/icons/download.svg"

                        buttonEnabled: backend.headReqCompleted

                        Layout.alignment: Qt.AlignBottom

                        // Pass info to backend class to start download
                        onClicked: {
                            if (backend.downloadRequested(urlBox.text, fileNameBox.text, pathBox.text, Helper.getNumberFromStr(connectionsList.model[connectionsList.currentIndex]), sha256Box.text)) {
                                root.closeAnimated()
                                resetForm()
                            }
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

    function openAnimated() {
        show()
        raise()
        requestActivate()
        openAnim.start()
    }

    ParallelAnimation {
        id: openAnim
        NumberAnimation { target: root; property: "opacity"; to: 1; duration: 100; easing.type: Easing.OutCubic }
        NumberAnimation { target: contentWrapper; property: "scale"; to: 1; duration: 140;}
    }

    function closeAnimated() {
        if (connectionsList.isActivated)
            connectionsList.isActivated = false
        closeAnim.start()
    }

    ParallelAnimation {
        id: closeAnim
        NumberAnimation { target: root; property: "opacity"; to: 0; duration: 100; easing.type: Easing.InCubic }
        NumberAnimation { target: contentWrapper; property: "scale"; to: 0.92; duration: 140 }
        onFinished: root.hide()
    }
}
