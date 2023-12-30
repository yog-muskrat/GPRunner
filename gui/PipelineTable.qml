import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import "Utility.js" as Utility

Item {
    required property int currentProject
    property var currentPipeline

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
                    implicitWidth: delegateLayout.implicitWidth
                    implicitHeight: delegateLayout.implicitHeight

                    color: row == pipelines.currentRow ? palette.highlight : palette.base

                    TapHandler {
                        onTapped: {
                            var proxyIdx = pipelines.model.index(row, 0)
                            var srcIdx = pipelines.model.mapToSource(proxyIdx)
                            currentPipeline = pipelines.model.sourceModel.pipelineAtIndex(srcIdx)
                        }
                    }

                    RowLayout {
                        id: delegateLayout

                        anchors.fill: parent
                        spacing: 0


                        TextLinkButton {
                            visible: model.pipelineUrl
                            leftPadding: 5
                            url: model.pipelineUrl
                        }

                        Label {
                            visible: getActionButtonVisible(model.pipelineStatus)

                            leftPadding: 5

                            text: getButtonText(pipelineStatus)
                            color: pipelines.currentRow ? palette.highlightedText : palette.text

                            HoverHandler { cursorShape: Qt.PointingHandCursor }
                            TapHandler { onTapped: pipelineAction(pipelineStatus) }
                        }

                        Image {
                            visible: model.pipelineUser ? true : false
                            Layout.maximumHeight: 28
                            Layout.maximumWidth: Layout.maximumHeight
                            source: model.pipelineUser ? model.pipelineUser.avatarUrl : ""
                            fillMode: Image.PreserveAspectFit
                        }

                        Label {
                            padding: 5

                            text: model.display
                            color: getTextColor(pipelineStatus)
                        }

                        HorizontalSpacer {}
                    }

                    function getTextColor(status) {
                        if (status == "success")  return "#429942";
                        if (status == "canceled") return "#999942";
                        if (status == "failed")   return "#FE4242";
                        if (status == "running")  return "#4242FE";
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
