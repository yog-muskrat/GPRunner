import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

ListView {
    id: projects
    property int currentProject

    focus: true
    clip: true

    model: gpm.projectModel

    delegate: Rectangle {
        width: ListView.view.width
        implicitHeight: itemText.implicitHeight
        color: index == projects.currentIndex ? palette.highlight : "transparent"

        Label {
            id: itemText
            anchors.fill: parent
            padding: 5
            text: model.display
            font.bold: model.hasCurrentUserMRs
            color: index == projects.currentIndex ? palette.highlightedText : palette.text
        }

        Item {
            id: unreadIndicator

            anchors.right: parent.right
            anchors.verticalCenter: itemText.verticalCenter

            visible: model.hasUnreadNotes
            implicitWidth: visible ? unreadIndicatorLabel.implicitWidth : 0
            implicitHeight: visible ? unreadIndicatorLabel.implicitHeight : 0

            Label {
                id: unreadIndicatorLabel

                rightPadding: 5
                text: "●"
                color: "#B21818"
            }
        }

        TapHandler {
            onTapped: {
                projects.currentIndex = index;
                projects.currentProject = model.projectId;
                gpm.setCurrentProject(projectId);
            }
        }
    }
}
