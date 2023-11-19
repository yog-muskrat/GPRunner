﻿import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    required property int currentProject
    required property int currentMrId
    required property int currentMrIid

    id: discussions

    onCurrentMrIdChanged: gpm.setCurrentMR(currentProject, currentMrId)

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

                model: gpm.discussionModel
                columnWidthProvider: function (column) { return width }

                delegate: DiscussionDelegate {
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

                    onRemoveNoteRequested: function(discussionId, noteId) { gpm.removeMRDiscussionNote(currentProject, currentMrIid, discussionId, noteId) }

                    onResolveRequested: function(discussionId) { gpm.resolveMRDiscussion(currentProject, currentMrIid, discussionId) }

                    onUnresolveRequested: function(discussionId) { gpm.unresolveMRDiscussion(currentProject, currentMrIid, discussionId) }
                }

                DiscussionNoteDialog {
                    id: dialog

                    currentProject: discussions.currentProject
                    currentMrIid: discussions.currentMrIid

                    width: Math.max(parent.width / 2, 500)
                    height: Math.max(parent.height / 2, 250)

                    anchors.centerIn: parent
                }
            }
        }

        Button {
            visible: currentMrIid > 0

            text: "Add discussion"
            onClicked: { dialog.open() }
        }
    }
}
