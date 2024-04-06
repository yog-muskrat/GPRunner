import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import mudbay.gprunner.models

import GPRunner

Item {
    property var currentProject
    property var currentPipeline

    clip: true

    onCurrentProjectChanged: {
        currentPipeline = null
        if(currentProject) {
            pipelines.model.sourceModel.setProject(currentProject)
        }
        else {
            pipelines.model.sourceModel.clear()
        }
    }

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

                model: gpm.getPipelineModel()
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
                            url: model.pipeline ? model.pipeline.url : ""
                        }

                        Label {
                            visible: getActionButtonVisible(column, pipeline)

                            leftPadding: 5

                            text: getButtonText(pipeline)
                            color: pipelines.currentRow ? palette.highlightedText : palette.text

                            HoverHandler { cursorShape: Qt.PointingHandCursor }
                            TapHandler { onTapped: pipelineAction(pipeline) }
                        }

                        Image {
                            visible: column == PipelineModel.User
                            Layout.maximumHeight: 28
                            Layout.maximumWidth: Layout.maximumHeight
                            source: pipeline ? (pipeline.user.avatarUrl) : ""
                            fillMode: Image.PreserveAspectFit
                        }

                        Label {
                            padding: 5

                            text: model.display
                            color: getTextColor(column, pipeline)
                        }

                        HorizontalSpacer {}
                    }

                    function getTextColor(column, pipeline) {
                        if (pipeline != null && column == PipelineModel.Status) {
                            if (pipeline.state == Pipeline.Success)   return "#429942";
                            if (pipeline.state == Pipeline.Canceled)  return "#999942";
                            if (pipeline.state == Pipeline.Failed)    return "#FE4242";
                            if (pipeline.state == Pipeline.Running)   return "#4242FE";
                            // TODO: created, waiting_for_resource, preparing, pending, skipped, manual, scheduled
                        }
                        return pipelines.currentRow ? palette.highlightedText : palette.text;
                    }

                    function getActionButtonVisible(column, pipeline) {
                        if(pipeline) {
                            return column == PipelineModel.Status && pipeline.state != Pipeline.Success && pipeline.state != Pipeline.Skipped;
                        }
                        return false
                    }

                    function getButtonText(pipeline) {
                        if (pipeline) {
                            if (pipeline.state == Pipeline.Running || pipeline.state == Pipeline.Pending)  return "⏹";
                            if (pipeline.state == Pipeline.Failed  || pipeline.state == Pipeline.Canceled) return "↺";
                        }
                        return "";
                    }

                    function pipelineAction(pipeline) {
                        if (pipeline) {
                            if (pipeline.state == Pipeline.Running || pipeline.state == Pipeline.Pending) {
                                pipeline.cancel()
                            }
                            else {
                                pipeline.retry()
                            }
                        }
                    }
                }
            }
        }
    }
}
