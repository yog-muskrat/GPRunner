import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

SplitView {
    required property var currentProject

    id: root

    orientation: Qt.Vertical

    RowLayout {
        PipelineTable {
            id: pipelinesTable
            currentProject: root.currentProject
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

        StartPipeline {
            id: startPipeline
            Layout.fillHeight: true
            Layout.minimumWidth: 500
            Layout.maximumWidth: 500
            currentProject: root.currentProject
        }
    }

    PipelineJobs {
        pipeline: pipelinesTable.currentPipeline
    }
}
