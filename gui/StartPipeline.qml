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
                text: "Reset vars"
                enabled: currentProject != null
                onClicked: {
                    if(currentProject) variables.setVariables(currentProject.variables)
                }
            }
            Button {
                text: "⏵"
                enabled: currentProject != null
                onClicked: {
                    if(currentProject) currentProject.runPipeline(variables.getVariables(), ref.currentText)
                }
            }
        }
    }
}
