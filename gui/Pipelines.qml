import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

SplitView {
    required property int currentProject

    orientation: Qt.Vertical

    PipelineTable {
        id: pipelinesTable
        currentProject: currentProject
    }

    PipelineJobs {
        pipeline: pipelinesTable.currentPipeline
    }
}
