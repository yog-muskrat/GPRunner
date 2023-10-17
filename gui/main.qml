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

            ProjectList {
                id: projects
                onCurrentProjectChanged: ref.model = gpm.getProjectBranches(currentProject)
            }

            Button {
                Layout.fillWidth: true
                text: qsTr("Load projects")
                onClicked: gpm.loadProjects()
            }

            Text { text: "Logged as " + gpm.currentUser }
        }

        PipelineTable{
            SplitView.minimumWidth: 750
            SplitView.fillWidth:    true
        }

        ColumnLayout {
            SplitView.minimumWidth: 250

            PipelineVariables{
                Layout.fillWidth:  true
                Layout.fillHeight: true
                enabled: projects.currentProject > 0
            }

            RowLayout {
                Text { text: "Ref"}

                ComboBox {
                    id: ref

                    Layout.fillWidth: true
                    Layout.horizontalStretchFactor: 1

                    enabled:  projects.currentProject > 0
                    editable: true

                    model: ["master"]
                }
                Button {
                    text: "Get vars"
                    enabled: projects.currentProject > 0
                    onClicked: gpm.loadPipelineVariables(ref.currentText)
                }
                Button {
                    text: "Run pipeline"
                    enabled: projects.currentProject > 0
                    onClicked: gpm.runPipeline(ref.currentText)
                   }
            }
        }
    }
}
