import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

SplitView {
    required property int currentProject

    id: pipelines

    onCurrentProjectChanged: ref.model = gpm.getProjectBranches(currentProject)

    StackLayout {
        currentIndex: statisticsButton.checked ? statistics.StackLayout.index : pipelinesTable.StackLayout.index

        SplitView.minimumWidth: 500
        implicitWidth: 750

        PipelineTable {
            id: pipelinesTable

            currentProject: pipelines.currentProject
        }

        PipelineStatistics {
            id: statistics

            currentProject: pipelines.currentProject
        }
    }

    ColumnLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        implicitWidth: 250

        PipelineVariablesTable {
            currentProject: pipelines.currentProject

            Layout.fillWidth: true
            Layout.fillHeight: true
            enabled: currentProject > 0
        }

        RowLayout {
            ComboBox {
                id: ref

                enabled: currentProject > 0
                editable: true

                model: ["master"]
                implicitContentWidthPolicy: ComboBox.WidestText
            }
            Button {
                text: "Get vars"
                enabled: currentProject > 0
                onClicked: gpm.loadPipelineVariables(currentProject, ref.currentText)
            }
            Button {
                text: "⏵"
                enabled: currentProject > 0
                onClicked: gpm.runPipeline(currentProject, ref.currentText)
            }
            Button {
                id: statisticsButton
                text: "📈"
                enabled: currentProject > 0
                checkable: true
            }
        }
    }
}
