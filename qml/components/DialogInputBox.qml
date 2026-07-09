import QtQuick 2.15
import QtQuick.Layouts

Item {
    property string titleText

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

            color: "transparent"

            radius: 6

            border.color: "#480069"
            border.width: 2

            Layout.preferredHeight: 32
            Layout.fillWidth: true
        }
    }
}
