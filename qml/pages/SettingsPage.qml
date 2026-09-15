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
                title: "Settings Category #1"
                Layout.fillWidth: true

                SettingRow {
                    title: "Setting #1"

                    UiButton {
                        buttonHeight: 32
                        buttonWidth: 170

                        fillColor: fillColor
                        borderColor: borderColor

                        buttonText: "Button #1"
                        buttonIcon: "qrc:/qml/assets/icons/download.svg"
                    }
                }

                SettingRow {
                    title: "Setting #2"

                    UiButton {
                        buttonHeight: 32
                        buttonWidth: 170

                        fillColor: fillColor
                        borderColor: borderColor

                        buttonText: "Button #2"
                        buttonIcon: "qrc:/qml/assets/icons/download.svg"
                    }
                }
            }
        }
    }
}
