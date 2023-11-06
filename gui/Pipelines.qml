import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

SplitView {
  property int currentProject: 0

  onCurrentProjectChanged: ref.model = gpm.getProjectBranches(currentProject)

  StackLayout {
    currentIndex: statisticsButton.checked ? statistics.StackLayout.index : pipelines.StackLayout.index

    SplitView.minimumWidth: 500
    implicitWidth: 750

    PipelineTable {
      id: pipelines
    }

    PipelineStatistics {
      id: statistics
    }
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
        id: statisticsButton
        text: "📈"
        enabled: currentProject > 0
        checkable: true
      }
    }
  }
}
