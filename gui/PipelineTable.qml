import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Rectangle {
    Layout.fillHeight: true
    Layout.fillWidth: true

    border {width: 1; color: "#424242"}

    HorizontalHeaderView {
        id: pipelinesHeader

        anchors.left: pipelines.left
        anchors.top: parent.top
        syncView: pipelines
        clip: true
        delegate: headerDelegate

        Component {
            id: headerDelegate

            Item {
                implicitWidth: itemText.implicitWidth
                implicitHeight: itemText.implicitHeight

                Rectangle {
                    anchors.fill: parent
                    border {width: 1; color: "#424242"}
                    color: "#EFEFEF"

                    Text {
                        anchors.centerIn: parent
                        id: itemText
                        padding: 5
                        text: display
                        font.pointSize: 12
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
        }
    }

    TableView {
        id: pipelines

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.top: pipelinesHeader.bottom
        anchors.margins: 1

        Layout.fillHeight: true
        Layout.fillWidth:  true

        focus: true
        clip: true

        model:    gpm.pipelineModel
        delegate: Component {
            Item {
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
                    anchors.left:   itemText.left
                    anchors.top:    itemText.top

                    implicitWidth:  itemText.implicitHeight
                    implicitHeight: itemText.implicitHeight

                    visible:        column == 0 && pipelineStatus != "success"
                    text:           pipelineStatus == "running" ? "⛔" : "↺"
                    onClicked:      pipelineStatus == "running" ? gpm.cancelPipeline(pipelineId) : gpm.retryPipeline(pipelineId)
                }

                function getTextColor(status) {
                    if(status == "success")  return "#429942"
                    if(status == "canceled") return "#999942"
                    if(status == "failed")   return "#FE4242"
                    if(status == "running")  return "#4242FE"
                    return "#424242"
                }
            }
        }
    }
}