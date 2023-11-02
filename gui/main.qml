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
        anchors.margins: 5

        ColumnLayout {
            SplitView.minimumWidth: 200

            ProjectList { id: projects }

            Button {
                Layout.fillWidth: true
                text: qsTr("Connect")
                onClicked: gpm.connect()
            }

            Text { text: "Logged as " + gpm.currentUser }
        }

        ColumnLayout {
            Pipelines {
                Layout.fillWidth:  true
                Layout.fillHeight: true
                currentProject: projects.currentProject
            }

            MRTable {
                Layout.fillWidth:  true
                Layout.fillHeight: true
            }
        }
    }
}
