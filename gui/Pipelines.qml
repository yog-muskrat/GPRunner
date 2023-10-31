import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

SplitView {
    property int currentProject: 0

    onCurrentProjectChanged: ref.model = gpm.getProjectBranches(currentProject)

    PipelineTable {
        SplitView.minimumWidth: 750
        Layout.fillWidth:  true
        Layout.fillHeight: true
    }

    ColumnLayout {
        SplitView.minimumWidth: 250

        PipelineVariablesTable{
            Layout.fillWidth:  true
            Layout.fillHeight: true
            enabled: currentProject > 0
        }

        RowLayout {
            Text { text: "Ref"}

            ComboBox {
                id: ref

                Layout.fillWidth: true
                Layout.horizontalStretchFactor: 1

                enabled:  currentProject > 0
                editable: true

                model: ["master"]
            }
            Button {
                text: "Get vars"
                enabled: currentProject > 0
                onClicked: gpm.loadPipelineVariables(ref.currentText)
            }
            Button {
                text: "⏵ Run pipeline"
                enabled: currentProject > 0
                onClicked: gpm.runPipeline(ref.currentText)
            }
        }
    }
}
