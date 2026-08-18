import QtQuick 2.15
import QtQuick.Layouts
import "../components"

Item {
    id: root

    property color fillColor: "#100019"
    property color borderColor: "#35003D"

    signal newDownloadRequested()

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

            // Download Counter Box
            Rectangle {
                id: downloadCounterBox
                Layout.preferredHeight: 90
                Layout.fillHeight: false
                Layout.fillWidth: true

                color: fillColor

                border.color: borderColor
                border.width: 2

                RowLayout {
                    anchors.fill: parent
                    spacing: 20

                    Item {
                        Layout.fillWidth: true
                    }

                    // Completed
                    Counter {
                        id: completed

                        counterHeight: 70
                        counterWdith: 90

                        fillColor: "#041500"
                        borderColor: "#0AC300"

                        counterText: backend.completedCount
                        titleText: "COMPLETED"
                    }

                    // Downloading
                    Counter {
                        id: downloading

                        counterHeight: 70
                        counterWdith: 90

                        fillColor: "#150000"
                        borderColor: "#C30003"

                        counterText: backend.downloadCount
                        titleText: "DOWNLOADING"
                    }

                    Item {
                        Layout.fillWidth: true
                    }
                }
            }

            // Categories box
            Rectangle {
                id: categoriesBox
                Layout.fillHeight: true
                Layout.fillWidth: true

                color: fillColor

                // Left border
                Rectangle {
                    width: 2
                    height: parent.height

                    color: borderColor

                    anchors.left: parent.left
                    anchors.top: parent.top
                }

                // Right border
                Rectangle {
                    width: 2
                    height: parent.height

                    color: borderColor

                    anchors.right: parent.right
                    anchors.top: parent.top
                }

                ColumnLayout {
                    anchors.fill: parent

                    spacing: 0

                    Item {
                        Layout.preferredHeight: 3
                    }

                    FontLoader {
                        id: appFont
                        source: "qrc:/qml/assets/fonts/Lexend.ttf"
                    }

                    Text {
                        id: categoriesTitle

                        Layout.leftMargin: 10

                        text: "CATEGORIES"
                        font.family: appFont.name
                        font.pixelSize: 12
                        color: "#595959"
                    }

                    Item {
                        Layout.preferredHeight: 5
                    }

                    CategoryTab {
                        Layout.alignment: Qt.AlignRight

                        tabText: "All Downloads"
                        tabIcon: "qrc:/qml/assets/icons/download.svg"

                        isSelected: true
                    }

                    CategoryTab {
                        Layout.alignment: Qt.AlignRight

                        tabText: "Compressed"
                        tabIcon: "qrc:/qml/assets/icons/compressed.svg"

                        isSelected: false
                    }

                    CategoryTab {
                        Layout.alignment: Qt.AlignRight

                        tabText: "Documents"
                        tabIcon: "qrc:/qml/assets/icons/document.svg"

                        isSelected: false
                    }

                    CategoryTab {
                        Layout.alignment: Qt.AlignRight

                        tabText: "Music"
                        tabIcon: "qrc:/qml/assets/icons/music.svg"

                        isSelected: false
                    }

                    CategoryTab {
                        Layout.alignment: Qt.AlignRight

                        tabText: "Videos"
                        tabIcon: "qrc:/qml/assets/icons/video.svg"

                        isSelected: false
                    }

                    CategoryTab {
                        Layout.alignment: Qt.AlignRight

                        tabText: "Programs"
                        tabIcon: "qrc:/qml/assets/icons/program.svg"

                        isSelected: false
                    }

                    Item {
                        Layout.fillHeight: true
                    }
                }
            }

            // Settings box
            Rectangle {
                id: settingsBox
                Layout.preferredHeight: 50
                Layout.fillHeight: false
                Layout.fillWidth: true

                color: fillColor

                border.color: borderColor
                border.width: 2

                // Settings button
                UiButton {
                    id: settingsButton

                    anchors.centerIn: parent

                    buttonHeight: 32
                    buttonWidth: 215

                    fillColor: fillColor
                    borderColor: borderColor

                    buttonText: "Settings"
                    buttonIcon: "qrc:/qml/assets/icons/setting.svg"
                }
            }
        }

        DownloadsPage {
            Layout.fillHeight: true
            Layout.fillWidth: true

            // Signals
            onNewDownloadRequested: {
                root.newDownloadRequested()
            }
        }
    }
}
