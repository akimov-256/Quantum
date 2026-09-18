import QtQuick 2.15
import QtQuick.Layouts

Item {
    id: root

    Layout.preferredHeight: row.height
    Layout.fillWidth: true

    default property alias content: row.data
    property string title: "Setting 1"

    FontLoader {
        id: appFont
        source: "qrc:/qml/assets/fonts/Lexend.ttf"
    }

    RowLayout {
        id: row

        width: parent.width

        Text {
            text: title
            font.family: appFont.name
            font.pixelSize: 12
            color: "#FFFFFF"
        }

        Item {
            Layout.fillWidth: true
        }


    }
}
