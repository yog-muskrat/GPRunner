import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import mudbay.gprunner.models

import GPRunner

Item {
    property var pipeline: null

    onPipelineChanged: {
        if(pipeline) {
            jobModel.setPipeline(pipeline)
        }
        else {
            jobModel.clear()
        }
    }

    JobModel { id: jobModel }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        HorizontalHeaderView {
            id: header
            syncView: jobs
            Layout.fillWidth: true
        }

        ScrollView {
            Layout.fillHeight: true
            Layout.fillWidth: true

            TableView {
                id: jobs

                anchors.fill: parent

                focus: true
                clip: true

                model: jobModel
                selectionModel: ItemSelectionModel { }

                columnWidthProvider: Utility.calcColumnWidth.bind(this, header)

                delegate: Rectangle {
                    implicitWidth: delegateRow.implicitWidth
                    implicitHeight: delegateRow.implicitHeight

                    color: row == jobs.currentRow ? palette.highlight : palette.base

                    RowLayout {
                        id: delegateRow
                        anchors.fill: parent

                        TextLinkButton {
                            visible: column == JobModel.Id
                            url: model.url
                        }

                        Label {
                            text: model.display
                            padding: 5
                            color: getTextColor(column, job)
                        }
                    }

                    function getTextColor(column, job) {
                        if (job != null && column == JobModel.Status) {
                            if (job.status == "success")   return "#429942";
                            if (job.status == "canceled")  return "#999942";
                            if (job.status == "failed")    return "#FE4242";
                            if (job.status == "running")   return "#4242FE";
                            // TODO: created, waiting_for_resource, preparing, pending, skipped, manual, scheduled
                        }
                        return jobs.currentRow ? palette.highlightedText : palette.text;
                    }
                }
            }
        }
    }
}
