import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import mudbay.gprunner.models

Item {
    property var currentMR: null

    id: discussions

    onCurrentMRChanged: {
        if(currentMR) discussionModel.setMR(currentMR)
        else discussionModel.clear()
    }

    function isResolved(idx)   { return mrs.model.data(idx, DiscussionModel.Discussion).isResolved }
    function isUnresolved(idx) { return !mrs.model.data(idx, DiscussionModel.Discussion).isResolved }

    function expandCollapseIf(pred, expand) {
        for(let row = 0; row < mrs.rows; row++) {
            let idx = mrs.index(row, 0)
            if(idx.parent.valid) continue;
            if(pred(idx)) expand ? mrs.expand(row) : mrs.collapse(row)
        }
    }

    DiscussionModel {
        id: discussionModel
        manager: gpm
    }

    ColumnLayout
    {
        anchors.fill: parent

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            TreeView {
                id: mrs
                anchors.fill: parent

                clip: true

                model: discussionModel
                columnWidthProvider: function (column) { return width }

                delegate: DiscussionDelegate {
                    onAddNoteRequested: function(discussion) {
                        dialog.discussion = discussion
                        dialog.open()
                    }

                    onEditNoteRequested: function(note) {
                        dialog.note = note
                        dialog.noteText = note.body
                        dialog.open()
                    }
                }

                DiscussionNoteDialog {
                    id: dialog

                    currentMR: discussions.currentMR

                    width: Math.max(parent.width / 2, 500)
                    height: Math.max(parent.height / 2, 350)

                    anchors.centerIn: parent
                }
            }
        }

        RowLayout {
                visible: currentMR != null

            Button {
                text: "🗨"
                onClicked: { dialog.open() }
                DefaultToolTip {
                    toolTipText: "Add discussion"
                }
            }

            HorizontalSpacer {}

            Button {
                visible: mrs.rows > 0
                text: "Expand/collapse all"
                onClicked: {
                    expandCollapseIf((x)=>true, !mrs.isExpanded(0))
                }
            }

            Button {
                visible: mrs.rows > 0
                text: "Collapse resolved"
                onClicked: { expandCollapseIf(isResolved, false) }
            }

            Button {
                visible: mrs.rows > 0
                text: "Expand unresolved"
                onClicked: { expandCollapseIf(isUnresolved, true) }
            }
        }
    }
}
