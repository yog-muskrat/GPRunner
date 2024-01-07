import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

ListView {
    id: projects
    property var currentProject

    focus: true
    clip: true

    model: gpm.getProjectModel()

    delegate: Rectangle {
        width: ListView.view.width
        implicitWidth:  delegateLayout.implicitWidth
        implicitHeight: delegateLayout.implicitHeight
        color: index == projects.currentIndex ? palette.highlight : palette.base

        RowLayout {
            id: delegateLayout

            anchors.fill: parent

            Label {
                padding: 5
                text: model.display
                font.bold: model.hasCurrentUserMRs
                color: index == projects.currentIndex ? palette.highlightedText : palette.text
            }

            UnreadMarker {
                project: model.project
                visible: model.hasUnreadNotes
                rightPadding: 5
            }
        }

        TapHandler {
            onTapped: {
                projects.currentIndex = index;
                projects.currentProject = project;
            }
            onDoubleTapped: Qt.openUrlExternally(project.url)
        }
    }
}
