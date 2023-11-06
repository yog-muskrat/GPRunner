import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

SplitView {
  property int currentProject: 0

  onCurrentProjectChanged: ref.model = gpm.getProjectBranches(currentProject)

  PipelineTable {
    id: pipelines
    SplitView.minimumWidth: 500
    implicitWidth: 750
  }

  ColumnLayout {
    Layout.fillWidth: true
    Layout.fillHeight: true
    implicitWidth: 250

    PipelineVariablesTable {
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
        onClicked: gpm.loadPipelineVariables(ref.currentText)
      }
      Button {
        text: "⏵"
        enabled: currentProject > 0
        onClicked: gpm.runPipeline(ref.currentText)
      }
      Button {
        text: "📈"
        enabled: currentProject > 0
        onClicked: {
          pipelines.visible = !pipelines.visible;
          statistics.visible = !statistics.visible;
        }
      }
    }
  }
}
