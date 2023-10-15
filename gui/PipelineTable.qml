import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Rectangle {
    border {width: 1; color: "#424242"}

    ColumnLayout {
        anchors.fill:    parent
        anchors.margins: 1
      
        spacing: 0

        CommonHeaderView {
            syncView: pipelines
        }

        TableView {
            id: pipelines

            Layout.fillHeight: true
            Layout.fillWidth:  true

            focus: true
            clip: true

            model:    gpm.pipelineModel
            delegate: Item {
                implicitWidth: itemText.implicitWidth
                implicitHeight: itemText.implicitHeight

                Rectangle {
                    anchors.fill: parent
                    color: (row % 2) == 0 ? "#EFEFEF" : "transparent"
                }

                Text {
                    id: itemText

                    anchors.fill: parent

                    leftPadding:  column == 0  ? implicitHeight + 5 : 5
                    rightPadding: 5

                    text: display
                    font.pointSize: 12
                    color: getTextColor(pipelineStatus)
                }

                RoundButton {
                    anchors.left: itemText.left
                    anchors.top:  itemText.top

                    implicitWidth:  itemText.implicitHeight
                    implicitHeight: itemText.implicitHeight

                    visible:   getButtonVisible(column, pipelineStatus)
                    text:      getButtonText(pipelineStatus)
                    onClicked: (pipelineStatus == "running" || pipelineStatus == "pending") ? gpm.cancelPipeline(pipelineId) : gpm.retryPipeline(pipelineId)
                }

                function getTextColor(status) {
                    if(status == "success")  return "#429942"
                    if(status == "canceled") return "#999942"
                    if(status == "failed")   return "#FE4242"
                    if(status == "running")  return "#4242FE"
                    // TODO: created, waiting_for_resource, preparing, pending, skipped, manual, scheduled
                    return "#424242"
                }

                function getButtonVisible(column, status) {
                    return column == 0 && status != "success" && status != "skipped"
                }

                function getButtonText(status) {
                    if (status == "running" || status == "pending") return "⛔"
                    if (status == "failed" || status == "canceled") return  "↺"
                    return ""
                }
            }
        }
    }
}