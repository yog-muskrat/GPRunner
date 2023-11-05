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

            Text {
                text: gpm.currentUser ? "Logged as " + gpm.currentUser : "Not connected"
                color: palette.text
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
                Layout.fillWidth: true
                Layout.fillHeight: true
                currentProject: projects.currentProject
            }
        }
    }
}
