import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        HorizontalHeaderView { syncView: pipelines }

        ScrollView {
            Layout.fillHeight: true
            Layout.fillWidth:  true

            TableView {
                id: pipelines

                focus: true
                clip: true

                model:          gpm.pipelineModel
                selectionModel: ItemSelectionModel { }

                delegate: Item {
                    implicitWidth: itemText.implicitWidth
                    implicitHeight: itemText.implicitHeight

                    Rectangle {
                        anchors.fill: parent
                        color: row == pipelines.currentRow ? palette.highlight : ((row % 2) == 0 ? palette.alternateBase : palette.base)
                    }

                    Text {
                        id: itemText

                        anchors.fill: parent

                        leftPadding:  column == 0  ? implicitHeight + 5 : 5
                        rightPadding: 5
                        topPadding: 5
                        bottomPadding: 5

                        text: model.display
                        font: model.font
                        color: getTextColor(pipelineStatus)
                    }

                    MouseArea {
                        cursorShape: Qt.PointingHandCursor
                        width:  itemText.implicitHeight
                        height: itemText.implicitHeight
                        visible:   getButtonVisible(column, pipelineStatus)
                        onClicked: (pipelineStatus == "running" || pipelineStatus == "pending") ? gpm.cancelPipeline(pipelineId) : gpm.retryPipeline(pipelineId)

                        Text {
                            anchors.centerIn: parent
                            text: getButtonText(pipelineStatus)
                            color: pipelines.currentRow ? palette.highlightedText : palette.text
                            font: model.font
                        }
                    }

                    function getTextColor(status) {
                        if(status == "success")  return "#429942"
                        if(status == "canceled") return "#999942"
                        if(status == "failed")   return "#FE4242"
                        if(status == "running")  return "#4242FE"
                        // TODO: created, waiting_for_resource, preparing, pending, skipped, manual, scheduled
                        return pipelines.currentRow ? palette.highlightedText : palette.text
                    }

                    function getButtonVisible(column, status) {
                        return column == 0 && status != "success" && status != "skipped"
                    }

                    function getButtonText(status) {
                        if (status == "running" || status == "pending") return "⏹"
                        if (status == "failed" || status == "canceled") return  "↺"
                        return ""
                    }
                }
            }
        }
    }
}