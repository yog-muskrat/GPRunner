import QtQuick
import QtQuick.Controls

Dialog {
    required property int currentProject
    required property var currentMR

    property var discussion
    property var note

    property string noteText

    modal: true

    title: "Note"
    standardButtons: Dialog.Ok | Dialog.Cancel

    contentItem:
    ScrollView {
        //anchors.fill: parent
        TextArea {
            id: noteEdit
            wrapMode: TextEdit.WordWrap
        }
    }

    onNoteTextChanged: {
        noteEdit.text = noteText
    }

    onAccepted: {
        if(note)            { note.body = noteEdit.text }
        else if(discussion) { discussion.addNote(noteEdit.text) }
        else                { currentMR.addDiscussion(noteEdit.text) }
        reset();
    }

    onRejected: reset()

    function reset() {
        discussion = null
        note = null
        noteText = ""
        noteEdit.text = ""
    }
}
