import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

SplitView {
    required property var currentProject

    orientation: Qt.Vertical

    onCurrentProjectChanged: {
        startPipeline.currentProject = currentProject
    }

    RowLayout {
        PipelineTable {
            id: pipelinesTable
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

        StartPipeline {
            id: startPipeline
            Layout.fillHeight: true
            Layout.minimumWidth: 500
            Layout.maximumWidth: 500
            currentProject: currentProject
        }
    }

    PipelineJobs {
        pipeline: pipelinesTable.currentPipeline
    }
}
