import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import "Utility.js" as Utility

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

    id: root

    implicitWidth:  isNote ? note.implicitWidth : discussionItem.implicitWidth
    implicitHeight: isNote ? note.implicitHeight : discussionItem.implicitHeight

    Item {
        id: discussionItem
        visible: isDiscussion

        implicitWidth: headerLayout.implicitWidth + padding * 2
        implicitHeight: headerLayout.implicitHeight + padding * 2
        width: root.width

        Rectangle {
            anchors.fill: parent
            color: discussionHover.hovered ? (discussion.isResolved ? "#3f4e3f" : palette.highlight) : (discussion.isResolved ? "#2d3c2d" : palette.base)
        }

        RowLayout {
            id: headerLayout

            anchors.fill: parent
            anchors.margins: padding

            ColumnLayout {
                RowLayout {
                    TextLinkButton {
                        url: discussion.url
                        text: discussion.isResolvable ? "Thread" : "Comment"
                    }
                    Label { text: "by" }
                    User  { user: discussion.author }
                    Label {
                        text: "updated at " + Utility.formatDateTime(discussion.updated)
                        font.pointSize: 9
                    }
                }
                RowLayout {
                    Label {
                        text: "Created at " + Utility.formatDateTime(discussion.started)
                        visible: discussion.started != discussion.updated
                        font.pointSize: 9
                    }
                    Label {
                        visible: canResolve(discussion)
                        text: discussion.isResolved ? "[Unresolve]" : "[Resolve]"
                        font.pointSize: 9

                        HoverHandler { cursorShape: Qt.PointingHandCursor }

                        TapHandler {
                            onTapped: {
                                if(discussion.isResolved) discussion.unresolve()
                                else discussion.resolve()
                            }
                        }
                    }
                }
            }

            HorizontalSpacer {}

            UnreadMarker {
                discussion: model.discussion
                visible: model.discussion.hasUnreadNotes
            }

            Label { text: "[" + discussion.noteCount + "]" }

            Label {
                id: indicator

                text: root.expanded ? "Hide thread ˄" : "Show thread ˅"

                TapHandler {
                    onTapped: treeView.toggleExpanded(row)
                }

                HoverHandler {
                    id: discussionHover
                    cursorShape: Qt.PointingHandCursor
                }
            }
        }
        Rectangle {
            height: 1
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            color: palette.window
            visible: !root.expanded
        }
    }

    Item {
        id: note
        visible: root.isNote

        implicitHeight: noteHeader.implicitHeight + noteBody.implicitHeight + noteFooter.implicitHeight
        implicitWidth: Math.max(noteHeader.implicitWidth, noteBody.implicitWidth, noteFooter.implicitWidth)
        width: root.width

        Rectangle {
            anchors.fill: parent
            color: "#3f3f3f"

            Rectangle {
                height: 1
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                color: palette.window

                visible: isLastChild(treeView.index(row, column))
            }
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
                    leftPadding: root.padding
                }

                TextLinkButton {
                    Layout.alignment: Qt.AlignTop
                    visible: model.note && model.note.url ? true : false
                    text: "[Link]"
                    font.pointSize: 9
                    url: model.note ? model.note.url : ""
                }

                Label {
                    Layout.alignment: Qt.AlignTop

                    visible: canEdit(model.note)
                    text: "[Edit]"
                    font.pointSize: 9

                    DefaultToolTip { toolTipText: "Edit" }
                    HoverHandler { cursorShape: Qt.PointingHandCursor }
                    TapHandler { onTapped: root.editNoteRequested(model.note) }
                }

                Label {
                    Layout.alignment: Qt.AlignTop

                    visible: canEdit(model.note)
                    text: "[Remove]"
                    font.pointSize: 9

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

            width: root.width - noteHeader.x
            wrapMode: Text.WordWrap

            text: model.display
            textFormat: Text.MarkdownText
            rightPadding: root.padding
            bottomPadding: root.padding

            onLinkActivated: function(link){ Qt.openUrlExternally(link) }
            HoverHandler {
                enabled: parent.hoveredLink
                cursorShape: Qt.PointingHandCursor
            }
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
                    onClicked: root.addNoteRequested(model.discussion)
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