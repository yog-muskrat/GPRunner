﻿import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import mudbay.gprunner.models

Item {
    required property int currentProject

    property var currentMR: null

    id: discussions

    onCurrentMRChanged: {
        if(currentMR) discussionModel.setMR(currentMR)
        else discussionModel.clear()
    }

    function expandCollapseIf(pred, expand) {
        for(let row = 0; row < mrs.rows; row++) {
            let idx = mrs.index(row, 0)
            if(idx.parent.valid) continue;
            if(pred(idx)) expand ? mrs.expand(row) : mrs.collapse(row)
        }
    }

    // TODO: Получать роль по имени
    function isResolved(idx)   { return mrs.model.data(idx, 261) == true }
    function isUnresolved(idx) { return mrs.model.data(idx, 261) != true }

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
                    projectId: discussions.currentProject
                    mr: discussions.currentMR

                    onAddNoteRequested: function(discussionId) {
                        dialog.discussionId = discussionId
                        dialog.open()
                    }

                    onEditNoteRequested: function(discussionId, noteId, noteText) {
                        dialog.discussionId = discussionId
                        dialog.noteId = noteId
                        dialog.noteText = noteText
                        dialog.open()
                    }

                    onRemoveNoteRequested: function(discussionId, noteId) { gpm.removeMRDiscussionNote(currentProject, currentMR.iid, discussionId, noteId) }

                    onResolveRequested: function(discussionId) { gpm.resolveMRDiscussion(currentProject, currentMR.iid, discussionId) }

                    onUnresolveRequested: function(discussionId) { gpm.unresolveMRDiscussion(currentProject, currentMR.iid, discussionId) }
                }

                DiscussionNoteDialog {
                    id: dialog

                    currentProject: discussions.currentProject
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
