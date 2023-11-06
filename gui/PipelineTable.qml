import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import "Utility.js" as Utility

Item {
  clip: true

  ColumnLayout {
    spacing: 0
    anchors.fill: parent

    HorizontalHeaderView {
      id: header
      syncView: pipelines
      Layout.fillWidth: true
    }

    ScrollView {
      Layout.fillHeight: true
      Layout.fillWidth: true

      TableView {
        id: pipelines

        anchors.fill: parent

        focus: true
        clip: true

        model: gpm.pipelineModel
        selectionModel: ItemSelectionModel {}

        columnWidthProvider: Utility.calcColumnWidth.bind(this, header)

        delegate: Rectangle {
          implicitWidth: itemText.implicitWidth + button.implicitWidth
          implicitHeight: itemText.implicitHeight
          color: row == pipelines.currentRow ? palette.highlight : ((row % 2) == 0 ? palette.alternateBase : palette.base)

          Text {
            id: button

            visible: getButtonVisible(column, pipelineStatus)
            padding: 5

            text: getButtonText(pipelineStatus)
            color: pipelines.currentRow ? palette.highlightedText : palette.text

            MouseArea {
              anchors.fill: parent
              cursorShape: Qt.PointingHandCursor
              onClicked: pipelineAction(pipelineStatus)
            }
          }

          Text {
            id: itemText

            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.left: button.visible ? button.right : parent.left

            padding: 5

            text: model.display
            color: getTextColor(pipelineStatus)
          }

          function getTextColor(status) {
            if (status == "success")
              return "#429942";
            if (status == "canceled")
              return "#999942";
            if (status == "failed")
              return "#FE4242";
            if (status == "running")
              return "#4242FE";
            // TODO: created, waiting_for_resource, preparing, pending, skipped, manual, scheduled
            return pipelines.currentRow ? palette.highlightedText : palette.text;
          }

          function getButtonVisible(column, status) {
            return column == 0 && status != "success" && status != "skipped";
          }

          function getButtonText(status) {
            if (status == "running" || status == "pending")
              return "⏹";
            if (status == "failed" || status == "canceled")
              return "↺";
            return "";
          }

          function pipelineAction(status) {
            if (status == "running" || status == "pending")
              gpm.cancelPipeline(pipelineId);
            else
              gpm.retryPipeline(pipelineId);
          }
        }
      }
    }
  }
}
