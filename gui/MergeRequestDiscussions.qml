import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

ScrollView {
    required property int currentProject
    required property int currentMrId
    required property int currentMrIid

    id: scrollView

    onCurrentMrIdChanged: gpm.setCurrentMR(currentProject, currentMrId)

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

            onAddDiscussionRequested: {
                dialog.open()
            }

            onAddCommentRequested: {
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

            currentProject: scrollView.currentProject
            currentMrIid: scrollView.currentMrIid

            width: Math.max(parent.width / 2, 500)
            height: Math.max(parent.height / 2, 250)

            anchors.centerIn: parent
        }
    }
}
