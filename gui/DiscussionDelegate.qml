import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    signal addNoteRequested(discussion: var)
    signal editNoteRequested(note: var)

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

    function canResolve(discussion) {
        return discussion && discussion.isResolvable && discussion.author == gpm.currentUser
    }

    function canEdit(note) { 
        if(!note) return false
        return note.author == gpm.currentUser
    }

    id: treeDelegate

    implicitWidth:  isNote ? note.implicitWidth : discussionItem.implicitWidth
    implicitHeight: isNote ? note.implicitHeight : discussionItem.implicitHeight

    Item {
        id: discussionItem
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

            visible: model.discussion && model.discussion.hasUnreadNotes()
            implicitWidth: visible ? unreadIndicatorLabel.implicitWidth : 0
            implicitHeight: visible ? unreadIndicatorLabel.implicitHeight : 0

            UnreadMarker {
                discussion: model.discussion
                id: unreadIndicatorLabel
                rightPadding: 5
            }
        }

        Label {
            id: discussionStatus

            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: treeDelegate.padding * 2

            visible: model.discussion.isResolvable

            text: canResolve(model.discussion) ? (model.discussion.isResolved ? "[Unresolve] ☑" : "[Resolve] ☐") : model.discussion.isResolved ? "☑" : "☐"
            color: canResolve(model.discussion) ? (model.discussion.isResolved ? "green" : "yellow") : palette.text

            HoverHandler {
                enabled: canResolve(discussion)
                cursorShape: Qt.PointingHandCursor
            }

            TapHandler {
                enabled: canResolve(discussion)
                onTapped: {
                    if(discussion.isResolved) discussion.unresolve()
                    else discussion.resolve()
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
            implicitHeight: image.height + padding * 2
            width: parent.width - indent * depth

            RowLayout {
                id: noteHeaderLayout

                y: padding
                spacing: padding

                width: parent.width

                Image {
                    id: image

                    Layout.alignment: Qt.AlignTop
                    Layout.maximumHeight: imgSize
                    Layout.maximumWidth: imgSize
                    Layout.minimumHeight: imgSize
                    Layout.minimumWidth: imgSize

                    source: model.note ? model.note.author.avatarUrl : ""
                    fillMode: Image.PreserveAspectFit
                }

                Label {
                    Layout.alignment: Qt.AlignTop
                
                    text: model.note ? (model.note.author.username + "\n" + new Date(model.note.created).toLocaleString(Qt.locale("ru_RU"), Locale.ShortFormat)) : ""
                    leftPadding: treeDelegate.padding
                }

                TextLinkButton {
                    Layout.alignment: Qt.AlignTop
                    visible: model.note && model.note.url ? true : false
                    url: model.note ? model.note.url : ""
                }

                TextLinkButton {
                    Layout.alignment: Qt.AlignTop

                    visible: canEdit(model.note)
                    text: "✎"

                    DefaultToolTip { toolTipText: "Edit" }
                    HoverHandler { cursorShape: Qt.PointingHandCursor }
                    TapHandler { onTapped: treeDelegate.editNoteRequested(model.note) }
                }

                Label {
                    Layout.alignment: Qt.AlignTop

                    visible: canEdit(model.note)
                    text: "🗑"

                    DefaultToolTip { toolTipText: "Remove" }
                    HoverHandler { cursorShape: Qt.PointingHandCursor }
                    TapHandler { onTapped: model.note.remove() }
                }

                Item { Layout.minimumWidth: 25 }

                Repeater {
                    model: note.reactions

                    EmojiButton {
                        Layout.alignment: Qt.AlignTop
                        Layout.minimumWidth: implicitHeight
                    }
                }

                HorizontalSpacer {}
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
            bottomPadding: treeDelegate.padding
        }

        Item {
            id: noteFooter

            visible: isLastChild(treeView.index(row, column))

            anchors.top: noteBody.bottom
            anchors.left: noteBody.left
            implicitHeight: visible ? footerLayout.implicitHeight + padding * 2 : indent
            implicitWidth: visible ? footerLayout.implicitWidth : 0

            RowLayout {
                id: footerLayout
                anchors.fill: parent

                Button {
                    text: "Add reply"
                    onClicked: treeDelegate.addNoteRequested(model.discussion)
                }

                Button {
                    visible: canResolve(model.discussion) && !model.discussion.isResolved
                    text: "Resolve"
                    onClicked: model.discussion.resolve()
                }

                Button {
                    visible: canResolve(model.discussion) && model.discussion.isResolved
                    text: "Unresolve"
                    onClicked: model.discussion.unresolve()
                }
            }
        }
    }
}