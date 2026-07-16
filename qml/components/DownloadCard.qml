import QtQuick 2.15
import QtQuick.Layouts

Rectangle {
    id: root

    property string fileName
    property int downloadProgress
    property string speed
    property string downloaded
    property string fileSize
    property string status
    property string rta

    height: 90

    color: "#35003D"
    border.color: "#AC00FB"
    border.width: 2
    radius: 5

    FontLoader {
        id: appFont

        source: "qrc:/qml/assets/fonts/Lexend.ttf"
    }

    RowLayout {
        anchors.fill: parent

        anchors.margins: 15

        spacing: 15

        Text {
            id: fileNameText

            text: fileName

            color: "white"
            font.family: appFont.name
            font.pixelSize: 20
        }

        Item {
            Layout.fillWidth: true
        }

        ColumnLayout {
            Layout.preferredWidth: progressBar.width

            RowLayout {
                Layout.preferredWidth: progressBar.width

                Text {
                    id: remainingTime

                    Layout.alignment: Qt.AlignLeft

                    text: rta

                    color: "gray"
                    font.family: appFont.name
                    font.pointSize: 10
                }

                Item {
                    Layout.preferredWidth: progressBar.width - fileSizeText.width - fileNameText.width
                }

                Text {
                    id: speedText

                    Layout.alignment: Qt.AlignRight

                    text: speed

                    color: "gray"
                    font.family: appFont.name
                    font.pixelSize: 10
                }
            }

            ProgressBar {
                id: progressBar

                progress: downloadProgress
            }

            RowLayout {
                Layout.preferredWidth: progressBar.width

                Text {
                    id: fileSizeText

                    Layout.alignment: Qt.AlignRight

                    text: fileSize

                    color: "gray"
                    font.family: appFont.name
                    font.pixelSize: 10
                }
            }
        }

        UiButton {
            id: button

            buttonHeight: 32
            buttonWidth: 90

            buttonText: "Pause"
            buttonIcon: "qrc:/qml/assets/icons/pause.png"
        }
    }
}
