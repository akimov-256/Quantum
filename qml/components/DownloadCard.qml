import QtQuick 2.15
import QtQuick.Layouts

Rectangle {
    id: root

    Layout.preferredHeight: 90
    Layout.fillWidth: true
    Layout.margins: 15

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
            id: fileName

            text: "Downloaded.txt"

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

                Text {
                    id: remainingTime

                    Layout.alignment: Qt.AlignLeft

                    text: "Remaining: 13 min 12 s"

                    color: "gray"
                    font.family: appFont.name
                    font.pointSize: 10
                }

                Item {
                    Layout.preferredWidth: progressBar.width - fileSize.width - fileName.width
                }

                Text {
                    id: fileSize

                    Layout.alignment: Qt.AlignRight

                    text: "13 MB / 256 MB"

                    color: "gray"
                    font.family: appFont.name
                    font.pixelSize: 10
                }
            }

            ProgressBar {
                id: progressBar

                progress: 80
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
