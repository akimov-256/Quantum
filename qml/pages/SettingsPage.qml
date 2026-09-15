import QtQuick 2.15
import QtQuick.Layouts
import "../components"

Item {
    id: root

    Flickable {
        anchors.fill: parent
        anchors.margins: 10
        contentHeight: column.implicitHeight
        clip: true

        ColumnLayout {
            id: column

            width: parent.width
            spacing: 10

            SettingsBox {
                Layout.fillWidth: true
            }
        }
    }
}
