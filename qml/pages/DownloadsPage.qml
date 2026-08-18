import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Dialogs
import "../components"
import "../js/Helper.js" as Helper

Item {
    id: root

    signal newDownloadRequested();

    MessageDialog {
        id: cancelWarning

        property int targetIndex: -1

        title: "Cancel download"
        text: "This download will be canceled.\nAre you sure?"

        buttons: MessageDialog.No | MessageDialog.Yes

        onAccepted: {
            backend.cancelClicked(targetIndex)
            targetIndex = -1
        }

        onRejected: {
            targetIndex = -1
        }
    }

    MessageDialog {
        id: removeWarning

        property int targetIndex: -1

        title: "Remove download"
        text: "This download will be removed.\nAre you sure?"

        buttons: MessageDialog.Yes | MessageDialog.No

        onAccepted: {
            backend.removeRequested(targetIndex)
            targetIndex = -1
        }

        onRejected: {
            targetIndex = -1
        }
    }

    ColumnLayout {
        id: downloadsLayout

        spacing: 0

        anchors.fill: parent

        Rectangle {
            Layout.preferredHeight: 50
            Layout.fillHeight: false
            Layout.fillWidth: true

            color: fillColor

            // Top border
            Rectangle {
                height: 2
                width: parent.width

                color: borderColor

                anchors.top: parent.top
                anchors.left: parent.left
            }

            // Bottom border
            Rectangle {
                height: 2
                width: parent.width

                color: borderColor

                anchors.bottom: parent.bottom
                anchors.left: parent.left
            }

            // Right border
            Rectangle {
                height: parent.height
                width: 2

                color: borderColor

                anchors.right: parent.right
                anchors.top: parent.top
            }

            RowLayout {
                id: downloadsToolbarBox

                anchors.fill: parent
                anchors.leftMargin: 10
                spacing: 10

                Layout.fillHeight: true
                Layout.fillWidth: true

                // New download
                UiButton {
                    id: newDownloadButton

                    Layout.alignment: Qt.AlignVCenter

                    buttonHeight: 32
                    buttonWidth: 170

                    fillColor: fillColor
                    borderColor: borderColor

                    buttonText: "New Download"
                    buttonIcon: "qrc:/qml/assets/icons/download.svg"

                    onClicked: root.newDownloadRequested()
                }

                // Separator border
                Rectangle {
                    Layout.fillHeight: true
                    Layout.preferredWidth: 2
                    Layout.fillWidth: false

                    color: borderColor
                }

                // Pause all
                UiButton {
                    id: pauseAllButton

                    Layout.alignment: Qt.AlignVCenter

                    buttonEnabled: backend.activeCount > 0

                    buttonHeight: 32
                    buttonWidth: 120

                    fillColor: fillColor
                    borderColor: borderColor

                    buttonText: "Pause All"
                    buttonIcon: "qrc:/qml/assets/icons/pause.svg"

                    onClicked: backend.pauseAll()
                }

                // Resume all
                UiButton {
                    id: resumeAllButton

                    Layout.alignment: Qt.AlignVCenter

                    buttonEnabled: backend.pausedCount > 0

                    buttonHeight: 32
                    buttonWidth: 120

                    fillColor: fillColor
                    borderColor: borderColor

                    buttonText: "Resume All"
                    buttonIcon: "qrc:/qml/assets/icons/play.svg"

                    onClicked: backend.resumeAll()
                }

                Item {
                    Layout.fillWidth: true
                }
            }
        }

        // Download cards box
        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            color: "transparent"

            ColumnLayout {
                anchors.fill: parent

                ListView {
                    Layout.margins: 15

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    spacing: 15

                    model: backend.downloadModel
                    delegate: DownloadCard {
                        width: ListView.view.width

                        fileName: model.fileName
                        downloadProgress: model.progress
                        speed: Helper.formatSpeed(model.speed)
                        rta: Helper.handleRTA(model.speed, model.downloaded, model.fileSize)
                        fileSize: Helper.formatFileSize(model.downloaded) + "/" + Helper.formatFileSize(model.fileSize)
                        status: model.status

                        isCompleted: status === "Completed"
                        isPaused: status === "Paused"

                        onButtonClicked: {
                            if (isCompleted)
                                backend.openRequested(index)
                            else
                                backend.buttonClicked(index)
                        }

                        onCancelClicked: {
                            if (isCompleted) {
                                removeWarning.targetIndex = index
                                removeWarning.open()
                            }
                            else
                            {
                                cancelWarning.targetIndex = index
                                cancelWarning.open()
                            }
                        }
                    }
                }

                Item {
                    Layout.fillHeight: true
                }
            }
        }
    }
}
