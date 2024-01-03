import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import mudbay.gprunner.models
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
                        onTapped: { currentPipeline = pipeline }
                    }

                    RowLayout {
                        id: delegateLayout

                        anchors.fill: parent
                        spacing: 0


                        TextLinkButton {
                            visible: column == PipelineModel.Id
                            leftPadding: 5
                            url: model.pipeline.url
                        }

                        Label {
                            visible: getActionButtonVisible(column, pipeline.state)

                            leftPadding: 5

                            text: getButtonText(pipeline.state)
                            color: pipelines.currentRow ? palette.highlightedText : palette.text

                            HoverHandler { cursorShape: Qt.PointingHandCursor }
                            TapHandler { onTapped: pipelineAction(pipeline.state) }
                        }

                        Image {
                            visible: column == PipelineModel.User
                            Layout.maximumHeight: 28
                            Layout.maximumWidth: Layout.maximumHeight
                            source: pipeline.user ? pipeline.user.avatarUrl : ""
                            fillMode: Image.PreserveAspectFit
                        }

                        Label {
                            padding: 5

                            text: model.display
                            color: getTextColor(pipeline.state)
                        }

                        HorizontalSpacer {}
                    }

                    function getTextColor(state) {
                        if (state == Pipeline.Success)   return "#429942";
                        if (state == Pipeline.Canceled) return "#999942";
                        if (state == Pipeline.Failed)    return "#FE4242";
                        if (state == Pipeline.Running)   return "#4242FE";
                        // TODO: created, waiting_for_resource, preparing, pending, skipped, manual, scheduled
                        return pipelines.currentRow ? palette.highlightedText : palette.text;
                    }

                    function getActionButtonVisible(column, state) {
                        return column == PipelineModel.Status && state != Pipeline.Success && state != Pipeline.Skipped;
                    }

                    function getButtonText(state) {
                        if (state == Pipeline.Running || state == Pipeline.Pending)
                            return "⏹";
                        if (state == Pipeline.Failed || state == Pipeline.Canceled)
                            return "↺";
                        return "";
                    }

                    function pipelineAction(state) {
                        if (state == Pipeline.Running || state == Pipeline.Pending)
                            gpm.cancelPipeline(pipelineId);
                        else
                            gpm.retryPipeline(pipelineId);
                    }
                }
            }
        }
    }
}
