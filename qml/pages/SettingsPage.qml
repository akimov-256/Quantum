import QtQuick 2.15
import QtQuick.Layouts
import "../components"

Item {
    id: root

    ColumnLayout {
        anchors.fill: parent

        SettingsBox {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}
