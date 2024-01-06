import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    required property var currentProject

    id: root

    ColumnLayout {
        anchors.fill: parent

        PipelineVariablesTable {
            id: variables
            currentProject: root.currentProject

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
                enabled: currentProject != null
                onClicked: gpm.loadPipelineVariables(currentProject.id, ref.currentText)
            }
            Button {
                text: "⏵"
                enabled: currentProject != null
                onClicked: gpm.runPipeline(currentProject.id, ref.currentText)
            }
        }
    }
}
