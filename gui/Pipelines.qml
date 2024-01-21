import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

SplitView {
    required property var currentProject

    id: root

    orientation: Qt.Vertical

    RowLayout {
        spacing: 0

        PipelineTable {
            id: pipelinesTable
            currentProject: root.currentProject
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

        RowLayout {
            spacing: 0

            Button {
                Layout.fillHeight: true
                Layout.minimumWidth: startPipeline.visible ? 5 : 15
                Layout.maximumWidth: startPipeline.visible ? 5 : 15

                text: startPipeline.visible ? "" : "◀"
                onClicked: startPipeline.visible = !startPipeline.visible
            }

            StartPipeline {
                id: startPipeline
                Layout.fillHeight: true
                Layout.minimumWidth: 500
                Layout.maximumWidth: 500
                currentProject: root.currentProject
            }
        }
    }

    PipelineJobs {
        pipeline: pipelinesTable.currentPipeline
    }
}
