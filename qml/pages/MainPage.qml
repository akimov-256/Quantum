import QtQuick 2.15
import QtQuick.Layouts

Item {
    id: root

    property color fillColor: "#100019"
    property color borderColor: "#35003D"

    anchors.fill: parent

    // The main layout
    RowLayout {
        anchors.fill: parent

        spacing: 0

        // The sidebar
        ColumnLayout {
            id: sideBar

            spacing: 0

            Layout.preferredWidth: 240
            Layout.fillWidth: false
            Layout.fillHeight: true

            Rectangle {
                id: downloadCounterBox
                Layout.preferredHeight: 90
                Layout.fillHeight: false
                Layout.fillWidth: true

                color: fillColor

                border.color: borderColor
                border.width: 2
            }

            Rectangle {
                id: categoriesBox
                Layout.fillHeight: true
                Layout.fillWidth: true

                color: fillColor

                border.color: borderColor
                border.width: 2
            }

            Rectangle {
                id: settingsBox
                Layout.preferredHeight: 50
                Layout.fillHeight: false
                Layout.fillWidth: true

                color: fillColor

                border.color: borderColor
                border.width: 2
            }
        }

        // The downloads layout
        ColumnLayout {
            id: downloadsLayout

            spacing: 0

            Layout.fillHeight: true
            Layout.fillWidth: true

            Rectangle {
                id: downloadsToolbar
                Layout.preferredHeight: 50
                Layout.fillHeight: false
                Layout.fillWidth: true

                color: fillColor

                border.color: borderColor
                border.width: 2
            }

            Rectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true
                color: "transparent"
            }

        }
    }
}
