import QtQuick 2.15
import QtQuick.Layouts

Rectangle {
    id: root

    Layout.preferredHeight: column.implicitHeight + 20

    default property alias content: column.data

    color: "#35003D"
    radius: 10

    FontLoader {
        id: appFont
        source: "qrc:/qml/assets/fonts/Lexend.ttf"
    }

    ColumnLayout {
        id: column

        anchors.fill: parent
        anchors.margins: 10

        Text {
            text: "THIS IS A TITLE"
            font.family: appFont.name
            font.pixelSize: 16
            color: "#FFFFFF"
        }
    }
}
