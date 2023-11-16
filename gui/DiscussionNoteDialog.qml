import QtQuick
import QtQuick.Controls

Dialog {
    required property int currentProject
    required property int currentMrIid

    property string discussionId: ""
    property int noteId: 0
    property string noteText

    modal: true

    title: "Note"
    standardButtons: Dialog.Ok | Dialog.Cancel

    contentItem: TextArea {
        id: note
        textFormat: TextEdit.MarkdownText
        text: parent.noteText
    }

    onAccepted: {
        if(discussionId && noteId > 0) {
            gpm.editMRDiscussionNote(currentProject, currentMrIid, discussionId, noteId, note.text)
        }
        else if(discussionId) {
            gpm.addMRDiscussionNote(currentProject, currentMrIid, discussionId, note.text)
        }
        else {
            gpm.addMRNote(currentProject, currentMrIid, note.text)
        }
        reset();
    }

    onRejected: reset()

    function reset() {
        discussionId = ""
        noteId = 0
        noteText = 0
    }
}
