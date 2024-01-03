import QtQuick
import QtQuick.Controls

Dialog {
    required property int currentProject
    required property var currentMR

    property string discussionId: ""
    property int noteId: 0
    property string noteText

    modal: true

    title: "Note"
    standardButtons: Dialog.Ok | Dialog.Cancel

    contentItem:
    ScrollView {
        TextArea {
            id: note
            anchors.fill: parent
            wrapMode: TextEdit.WordWrap
        }
    }

    onNoteTextChanged: {
        note.text = noteText
    }

    onAccepted: {
        if(discussionId && noteId > 0) {
            gpm.editMRDiscussionNote(currentProject, currentMR.iid, discussionId, noteId, note.text)
        }
        else if(discussionId) {
            gpm.addMRDiscussionNote(currentProject, currentMR.iid, discussionId, note.text)
        }
        else {
            gpm.addMRDiscussion(currentProject, currentMR.iid, note.text)
        }
        reset();
    }

    onRejected: reset()

    function reset() {
        discussionId = ""
        noteId = 0
        noteText = ""
        note.text = ""
    }
}
