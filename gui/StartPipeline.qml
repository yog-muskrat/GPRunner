import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    required property int currentProject

    ColumnLayout {
        anchors.fill: parent

        PipelineVariablesTable {
            id: variables
            currentProject: currentProject

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
                text: "Update vars"
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
