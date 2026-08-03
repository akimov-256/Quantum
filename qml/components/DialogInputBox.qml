import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Controls

Item {
    property string titleText
    property string placeHolderText
    property alias text: input.text
    signal boxTextChanged()

    height: 51
    width: 600

    ColumnLayout {
        anchors.fill: parent

        Text {
            text: titleText

            color: "#656565"

            font.pixelSize: 10
            font.family: appFont.name
        }

        Rectangle {
            id: urlBox

            color: input.hovered ? "#200025" : "#100019"

            radius: 6

            border.color: input.activeFocus ? "#AE00FF" : "#480069"
            border.width: 2

            Layout.preferredHeight: 32
            Layout.fillWidth: true

            Behavior on color {
                ColorAnimation {
                    duration: 200
                }
            }

            Behavior on border.color {
                ColorAnimation {
                    duration: 80
                }
            }

            TextField {
                id: input

                anchors.fill: parent
                anchors.margins: 1

                hoverEnabled: true

                verticalAlignment: Text.AlignVCenter

                placeholderText: placeHolderText

                color: "white"
                font.family: appFont.name
                font.pixelSize: 16

                background: Rectangle {
                    color: "transparent"
                }

                onTextChanged: boxTextChanged()
            }
        }
    }
}
