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
                title: "Download History"
                Layout.fillWidth: true

                SettingRow {
                    title: "Clear database"

                    UiButton {
                        buttonHeight: 32
                        buttonWidth: 100

                        fillColor: fillColor
                        borderColor: borderColor

                        buttonText: "Clear"
                        buttonIcon: "qrc:/qml/assets/icons/reset.svg"

                        onClicked: {
                            backend.clearDatabase()
                        }
                    }
                }
            }
        }
    }
}
