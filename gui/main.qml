import QtCore
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qt.labs.platform

Window {
    property bool forceQuit: false

    id: mainWindow
    visible: true
    width: 1280
    height: 600
    title: qsTr("GPRunner")
    Universal.theme: Universal.System
    color: palette.window

    onClosing: (close) => {
        if(!forceQuit) {
            close.accepted = false
            hide()
        }
    }

    Component.onCompleted: {
        pipelines.restoreState(settings.value("ui/pipelines"))
        mergeRequests.restoreState(settings.value("ui/mrs"))
    }

    Component.onDestruction: {
        settings.setValue("ui/pipelines", pipelines.saveState())
        settings.setValue("ui/mrs", mergeRequests.saveState())
    }

    Settings {
        property alias x: mainWindow.x
        property alias y: mainWindow.y
        property alias width: mainWindow.width
        property alias height: mainWindow.height

        id: settings

        category: "ui"
        location: "settings.ini"
    }

    SystemTrayIcon {
        id: tray

        visible: true
        icon.source: gpm.hasNewNotes ? "qrc:/icons/gitlab-green-notification.png" : "qrc:/icons/gitlab-green.png"

        onActivated: {
            mainWindow.show()
            mainWindow.raise()
            mainWindow.requestActivate()
        }

        menu: Menu {
            MenuItem {
                text: qsTr("Quit")
                onTriggered: {
                    mainWindow.forceQuit = true
                    Qt.quit()
                }
            }
        }
    }

    Connections {
        target: gpm
        function onNotification(title, message) { tray.showMessage(title, message) }
    }

    Pane {
        anchors.fill: parent
        padding: 0

        RowLayout {
            anchors.fill: parent
            spacing: 4

            Rectangle {
                color: palette.window
                Layout.maximumWidth: 250
                Layout.minimumWidth: 250
                Layout.fillHeight: true

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0

                    ProjectList {
                        id: projects
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                    }

                    Button {
                        Layout.fillWidth: true
                        text: qsTr("Connect")
                        visible: !gpm.currentUser
                        onClicked: gpm.connect()
                    }

                    Item {
                        visible: gpm.currentUser
                        Layout.fillWidth: true
                        height: 64

                        Image {
                            id: avatar

                            anchors.top: parent.top
                            anchors.left: parent.left
                            anchors.bottom: parent.bottom

                            source: gpm.currentUserAvatar
                            fillMode: Image.PreserveAspectFit
                        }

                        Label {
                            anchors.left: avatar.right
                            anchors.verticalCenter: avatar.verticalCenter

                            text: "Logged as\n" + gpm.currentUser
                            padding: 5
                        }
                    }

                    Label {
                        text: "Not connected"
                        padding: 5
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        visible: !gpm.currentUser
                    }
                }
            }

            Rectangle {
                color: palette.window
                Layout.fillWidth: true
                Layout.fillHeight: true

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0

                    TabBar {
                        id: tabBar

                        Layout.fillWidth: true

                        TabButton { text: "MRs" }
                        TabButton { text: "Pipelines" }
                        TabButton { text: "Statistics" }
                    }

                    StackLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        currentIndex: tabBar.currentIndex

                        MergeRequests {
                            id: mergeRequests

                            currentProject: projects.currentProject
                        }

                        Pipelines {
                            id: pipelines

                            currentProject: projects.currentProject
                        }

                        PipelineStatistics {
                            id: statistics

                            currentProject: pipelines.currentProject
                        }
                    }
                }
            }
        }
    }
}
