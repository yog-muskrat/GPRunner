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
        mainSplit.restoreState(settings.value("ui/mainsplitview"))
        pipelines.restoreState(settings.value("ui/pipelines"))
    }

    Component.onDestruction: {
        settings.setValue("ui/mainsplitview", mainSplit.saveState())
        settings.setValue("ui/pipelines", pipelines.saveState())
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
        visible: true
        icon.source: "qrc:/icons/gitlab-green.png"

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

    SplitView {
        id: mainSplit
        anchors.fill: parent

        ColumnLayout {
            SplitView.minimumWidth: 200
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

            Label {
                text: gpm.currentUser ? "Logged as " + gpm.currentUser : "Not connected"
                padding: 5
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }
        }

        ColumnLayout {
            spacing: 0

            MRTable {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Pipelines {
                id: pipelines
                Layout.fillWidth: true
                Layout.fillHeight: true
                currentProject: projects.currentProject
            }
        }
    }
}
