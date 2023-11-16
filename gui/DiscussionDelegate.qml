import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    signal addNoteRequested(discussionId: string)
    signal addDiscussionRequested()
    signal addCommentRequested()
    signal removeNoteRequested(discussionId: string, noteId: int)
    signal editNoteRequested(discussionId: string, noteId: int, noteText: string)
    signal resolveRequested(discussionId: string)
    signal unresolveRequested(discussionId: string)

    readonly property real indent: 20
    readonly property real padding: 5
    readonly property real imgSize: 48

    readonly property bool isNote: depth == 1
    readonly property bool isDiscussion: depth == 0

    // Assigned to by TreeView:
    required property TreeView treeView
    required property bool expanded
    required property int depth
    required property var index

    function isLastChild(index) {
        return index.row == (index.model.rowCount(index.parent) - 1)
    }

    id: treeDelegate

    implicitWidth:  isNote ? note.implicitWidth : discussion.implicitWidth
    implicitHeight: isNote ? note.implicitHeight : discussion.implicitHeight

    Item {
        id: discussion
        visible: isDiscussion

        implicitWidth: discussionHeader.implicitWidth + discussionNotes.implicitWidth + unreadIndicator.implicitWidth + discussionStatus.implicitWidth
        implicitHeight: Math.max(discussionHeader.implicitHeight, discussionStatus.implicitHeight)
        width: treeDelegate.width

        Rectangle {
            anchors.fill: parent
            color: discussionHeaderHover.hovered ? palette.base : palette.alternateBase
        }

        Item { 
            id: discussionHeader

            implicitWidth: indicator.implicitWidth + discussionTitle.implicitWidth
            implicitHeight: Math.max(indicator.implicitHeight, discussionTitle.implicitHeight)

            TapHandler {
                onTapped: treeView.toggleExpanded(row)
            }

            HoverHandler {
                id: discussionHeaderHover
                cursorShape: Qt.PointingHandCursor
            }

            Label {
                id: indicator

                text: ">"
                rotation: treeDelegate.expanded ? 90 : 0
                padding: treeDelegate.padding
            }

            Label {
                id: discussionTitle

                anchors.left: indicator.right
                anchors.verticalCenter: indicator.verticalCenter

                text: model.display
                padding: treeDelegate.padding
            }
        }

        Item {
            id: discussionNotes

            visible: model.noteCount > 0
            implicitWidth: visible ? noteCountLabel.implicitWidth : 0
            implicitHeight: visible ? noteCountLabel.implicitHeight : 0

            anchors.right: unreadIndicator.left
            anchors.verticalCenter: discussionHeader.verticalCenter

            Label {
                id: noteCountLabel

                visible: model.noteCount > 0

                rightPadding: 5
                text: "[" + model.noteCount + "]"
            }
        }

        Item {
            id: unreadIndicator

            anchors.right: discussionStatus.left
            anchors.verticalCenter: discussionHeader.verticalCenter

            visible: model.hasUnreadNotes
            implicitWidth: visible ? unreadIndicatorLabel.implicitWidth : 0
            implicitHeight: visible ? unreadIndicatorLabel.implicitHeight : 0

            UnreadMarker {
                id: unreadIndicatorLabel

                rightPadding: 5
            }
        }

        Label {
            id: discussionStatus

            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: treeDelegate.padding * 2

            visible: model.resolvable

            text: model.canResolve ? "[Resolve] ☐" : model.canUnresolve ? "[Unresolve] ☑" : model.resolved ? "☑" : "☐"
            color: model.canResolve ? "yellow" : model.canUnresolve ? "green" : palette.text

            HoverHandler {
                enabled: model.canResolve || model.canUnresolve
                cursorShape: Qt.PointingHandCursor
            }

            TapHandler {
                enabled: model.canResolve || model.canUnresolve
                onTapped: {
                    if(model.canResolve) treeDelegate.resolveRequested(model.discussionId)
                    else if(model.canUnresolve) treeDelegate.unresolveRequested(model.discussionId)
                }
            }
        }
    }

    Item {
        id: note
        visible: treeDelegate.isNote

        implicitHeight: noteHeader.implicitHeight + noteBody.implicitHeight + noteFooter.implicitHeight
        implicitWidth: Math.max(noteHeader.implicitWidth, noteBody.implicitWidth, noteFooter.implicitWidth)
        width: treeDelegate.width

        Rectangle {
            anchors.fill: parent
            color: palette.base
        }

        Item {
            id: noteHeader

            x: indent * depth
            y: padding
            implicitHeight: image.height + padding
            implicitWidth: image.width + authorLabel.implicitWidth

            Image {
                id: image

                width:  imgSize
                height: imgSize
                source: model.author.avatarUrl
                fillMode: Image.PreserveAspectFit
            }

            Label {
                id: authorLabel

                anchors.left: image.right
                anchors.top: image.top
                text: model.author.username + "\n" + new Date(model.created).toLocaleString(Qt.locale("ru_RU"), Locale.ShortFormat)
                leftPadding: treeDelegate.padding
            }
        }

        Label {
            id: noteBody

            anchors.top: noteHeader.bottom
            anchors.left: noteHeader.left

            width: treeDelegate.width - noteHeader.x
            wrapMode: Text.WordWrap

            text: model.display
            textFormat: Text.MarkdownText
            rightPadding: treeDelegate.padding
            bottomPadding: treeDelegate.indent
        }

        Item {
            id: noteFooter

            visible: isLastChild(treeView.index(row, column)) || model.canEdit

            anchors.top: noteBody.bottom
            anchors.left: noteBody.left
            anchors.bottomMargin: treeDelegate.padding
            implicitHeight: visible ? 40 : 0

            RowLayout {
                anchors.fill: parent

                Button {
                    text: "Add reply"
                    onClicked: treeDelegate.addNoteRequested(model.discussionId)
                }

                Button {
                    visible: model.canEdit
                    text: "Edit note"
                    onClicked: treeDelegate.editNoteRequested(model.discussionId, model.noteId, model.display)
                }

                Button {
                    visible: model.canEdit
                    text: "Remove note"
                    onClicked: treeDelegate.removeNoteRequested(model.discussionId, model.noteId)
                }

                Button {
                    visible: model.canResolve
                    text: "Resolve"
                    onClicked: treeDelegate.resolveRequested(model.discussionId)
                }

                Button {
                    visible: model.canUnresolve
                    text: "Unresolve"
                    onClicked: treeDelegate.unresolveRequested(model.discussionId)
                }
            }
        }
    }
}