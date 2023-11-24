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
        implicitWidth:  delegateLayout.implicitWidth
        implicitHeight: delegateLayout.implicitHeight
        color: index == projects.currentIndex ? palette.highlight : "transparent"

        RowLayout {
            id: delegateLayout

            anchors.fill: parent

            Label {
                id: itemText

                padding: 5
                text: model.display
                font.bold: model.hasCurrentUserMRs
                color: index == projects.currentIndex ? palette.highlightedText : palette.text
            }

            UnreadMarker {
                projectId: model.projectId

                visible: model.hasUnreadNotes
                rightPadding: 5
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
