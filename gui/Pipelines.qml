import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

SplitView {
    property int currentProject: 0

    onCurrentProjectChanged: ref.model = gpm.getProjectBranches(currentProject)

    Item {
        SplitView.minimumWidth: 500
        SplitView.fillWidth: true
        implicitWidth: 750

        PipelineTable {
            id: pipelines
            anchors.fill: parent
        }

        PipelineStatistics {
            id: statistics
            anchors.fill: parent
            visible: false
        }
    }

    Item {
        implicitWidth: 250

        ColumnLayout {
            anchors.fill: parent

            PipelineVariablesTable{
                Layout.fillWidth:  true
                Layout.fillHeight: true
                enabled: currentProject > 0
            }

            ComboBox {
                id: ref

                enabled:  currentProject > 0
                editable: true

                model: ["master"]
                implicitContentWidthPolicy: ComboBox.WidestText
            }

            RowLayout {
                Button {
                    text: "Get vars"
                    enabled: currentProject > 0
                    onClicked: gpm.loadPipelineVariables(ref.currentText)
                }
                Button {
                    text: "⏵ Run"
                    enabled: currentProject > 0
                    onClicked: gpm.runPipeline(ref.currentText)
                }
                Button {
                    text: "📈 Analyze"
                    enabled: currentProject > 0
                    onClicked: {
                        pipelines.visible = !pipelines.visible
                        statistics.visible = !statistics.visible
                    }
                }
            }
        }
    }
}
