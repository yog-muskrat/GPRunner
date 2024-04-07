import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import GPRunner

Rectangle {
    signal addNoteRequested(discussion: var)
    signal editNoteRequested(note: var)

    required property TreeView treeView
    required property var index

    readonly property real padding: 5
    readonly property real imgSize: 48

    id: root

    implicitHeight: noteLayout.implicitHeight + padding * 2
    //implicitWidth:  noteLayout.implicitWidth  + padding * 2 + imgSize

    color: row % 2 ? palette.base: "#3f3f3f"

    function isLastChild(index) { return index.row == (index.model.rowCount(index.parent) - 1) }
    function canEdit(note) { return note.author == gpm.currentUser }
    function canResolve(discussion) { return discussion && discussion.isResolvable && discussion.author == gpm.currentUser }

    Rectangle {
        height: 1
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: palette.window
    }

    ColumnLayout {
        id: noteLayout

        width: root.width - padding * 2 - imgSize
        x: padding + imgSize
        y: padding

        implicitHeight: noteHeader.implicitHeight + noteBody.implicitHeight + noteFooter.implicitHeight

        Item {
            id: noteHeader

            implicitWidth: noteHeaderLayout
            implicitHeight: imgSize + padding * 2

            RowLayout {
                id: noteHeaderLayout
                anchors.fill: parent

                spacing: padding

                User {
                    user: model.note.author
                    size: root.imgSize
                    showLabel: false
                }
            
                ColumnLayout 
                {
                    Label {
                        text: "by " + model.note.author.username + " at " + Utility.formatDateTime(model.note.created)
                        font.pointSize: 9
                    }

                    RowLayout{
                        TextLinkButton {
                            text: "[Link]"
                            font.pointSize: 9
                            url: model.note.url
                        }

                        Label {
                            visible: canEdit(model.note)
                            text: "[Edit]"
                            font.pointSize: 9

                            DefaultToolTip { toolTipText: "Edit" }
                            HoverHandler { cursorShape: Qt.PointingHandCursor }
                            TapHandler { onTapped: root.editNoteRequested(model.note) }
                        }

                        Label {
                            visible: canEdit(model.note)
                            text: "[Remove]"
                            font.pointSize: 9

                            DefaultToolTip { toolTipText: "Remove" }
                            HoverHandler { cursorShape: Qt.PointingHandCursor }
                            TapHandler { onTapped: model.note.remove() }
                        }
                    }
                }

                Item { Layout.minimumWidth: 25 }

                Repeater {
                    model: note.reactions

                    EmojiButton { Layout.alignment: Qt.AlignTop }
                }

                HorizontalSpacer {}
            }
        }

       Label {
           id: noteBody

           Layout.fillWidth: true

            wrapMode: Text.WordWrap

            text: model.display
            textFormat: Text.MarkdownText

            onLinkActivated: function (link) {
                Qt.openUrlExternally(link);
            }
            HoverHandler {
                enabled: parent.hoveredLink
                cursorShape: Qt.PointingHandCursor
            }
        }

        Item {
            id: noteFooter

            implicitWidth: noteFooterLayout.implicitWidth
            implicitHeight: noteFooterLayout.visible ? noteFooterLayout.implicitHeight + padding : padding

            RowLayout {
                id: noteFooterLayout

                visible: isLastChild(treeView.index(row, column))

                Button {
                    text: "Add reply"
                    onClicked: root.addNoteRequested(model.discussion)
                }

                Button {
                    visible: canResolve(model.discussion) 
                    text: model.discussion.isResolved ? "Unresolve" : "Resolve"
                    onClicked: model.discussion.isResolved ? model.discussion.resolve() : model.discussion.unresolve()
                }
            }
        }
    }
}
