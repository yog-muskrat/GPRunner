import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import "Utility.js" as Utility
import mudbay.gprunner.models

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
                            color: jobs.currentRow ? palette.highlightedText : palette.text
                        }
                    }
                }
            }
        }
    }
}
