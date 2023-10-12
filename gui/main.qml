import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Window {
    visible: true
    width: 1280
    height: 600
    title: qsTr("GPRunner")

    SplitView {
        anchors.fill: parent
        anchors.margins: 5

        ColumnLayout {
            SplitView.minimumWidth: 200

            ProjectList {
                id: projects
            }

            Button {
                id: loadProjects
                Layout.fillWidth: true
                text: qsTr("Load projects")
                onClicked: gpm.loadProjects()
            }
        }

        PipelineTable{
            SplitView.minimumWidth: 750
        }

        ColumnLayout {
            SplitView.minimumWidth: 250

            PipelineVariables{}
            Button {
                id: runPipeline
                //enabled: projects.projects.currentProject > 0
                Layout.fillWidth: true
                text: "Run pipeline"
                onClicked: gpm.runPipeline()
               }
        }
    }
}
