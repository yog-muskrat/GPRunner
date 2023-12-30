import QtQuick
import QtQuick.Controls

Dialog {

    contentItem: ColumnLayout {
        PipelineVariablesTable {
            currentProject: pipelines.currentProject

            Layout.fillWidth: true
            Layout.fillHeight: true
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
        }
    }
}
