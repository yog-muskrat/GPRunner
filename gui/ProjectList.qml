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

        Item {
            id: projectAvatar

            anchors.left: parent.left
            anchors.verticalCenter: itemText.verticalCenter
            
            //width: itemText.implicitHeight
            width: 0
            height: width

//            Image {
//                anchors.fill: parent
//                source: "image://gpr/project_" + model.projectId
//                fillMode: Image.PreserveAspectFit
//            }
        }

        Label {
            id: itemText

            anchors.left: projectAvatar.right
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
            onDoubleTapped: Qt.openUrlExternally(model.projectUrl)
        }
    }
}
