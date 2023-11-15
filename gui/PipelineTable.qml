import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import "Utility.js" as Utility

Item {
    required property int currentProject

    clip: true

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

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
                selectionModel: ItemSelectionModel { }

                columnWidthProvider: Utility.calcColumnWidth.bind(this, header)

                delegate: Rectangle {
                    implicitWidth: urlButton.implicitWidth + actionButton.implicitWidth + itemText.implicitWidth
                    implicitHeight: itemText.implicitHeight

                    color: row == pipelines.currentRow ? palette.highlight : palette.base

                    Item {
                        id: urlButton

                        anchors.left: parent.left
                        anchors.verticalCenter: itemText.verticalCenter

                        visible: model.pipelineUrl
                        
                        implicitWidth: visible ? urlButtonLabel.implicitWidth : 0
                        implicitHeight: visible ? urlButtonLabel.implicitHeight : 0

                        HoverHandler
                        {
                            id: urlHover
                            cursorShape: Qt.PointingHandCursor
                        }
                        TapHandler { onTapped: Qt.openUrlExternally(model.pipelineUrl) }

                        Label {
                            id: urlButtonLabel

                            leftPadding: 5
                            text: "🌐"

                            ToolTip.delay: 500
                            ToolTip.timeout: 3000
                            ToolTip.text: "Open in browser"
                            ToolTip.visible: urlHover.hovered
                        }
                    }

                    Item {
                        id: actionButton

                        anchors.left: urlButton.right
                        anchors.verticalCenter: itemText.verticalCenter

                        visible: getActionButtonVisible(model.pipelineStatus)
                        
                        implicitWidth: visible ? actionButtonLabel.implicitWidth : 0
                        implicitHeight: visible ? actionButtonLabel.implicitHeight : 0

                        HoverHandler { cursorShape: Qt.PointingHandCursor }
                        TapHandler { onTapped: pipelineAction(pipelineStatus) }

                        Label {
                            id: actionButtonLabel

                            leftPadding: 5

                            text: getButtonText(pipelineStatus)
                            color: pipelines.currentRow ? palette.highlightedText : palette.text
                        }
                    }

                    Label {
                        id: itemText

                        anchors.left: actionButton.right

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

                    function getActionButtonVisible(status) {
                        return status && status != "success" && status != "skipped";
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
