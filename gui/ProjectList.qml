import QtQuick
import QtQuick.Layouts

ListView {
    property int currentProject
    id: projects

    focus: true
    clip: true

    model: gpm.projectModel

    delegate: Item {
        width: ListView.view.width
        implicitHeight: itemText.implicitHeight

        Rectangle {
            anchors.fill: parent
            color: index == projects.currentIndex ? palette.highlight : ((index % 2) == 0 ? palette.alternateBase : palette.base)

            Text {
                id: itemText
                anchors.fill: parent
                padding: 5
                text: model.display
                font.bold: model.font.bold
                color: index == projects.currentIndex ? palette.highlightedText : palette.text
            }

            Rectangle {
                id: unreadIndicator

                anchors.right: parent.right
                anchors.rightMargin: 5
                anchors.verticalCenter: itemText.verticalCenter
                visible: model.hasUnreadNotes

                width: itemText.implicitHeight / 3
                height: width
                radius: width / 2
                color: "#B21818"
            }

            TapHandler { 
                onTapped: {
                    projects.currentIndex = index
                    projects.currentProject = model.projectId
                    gpm.setCurrentProject(projectId)
                }
            }
        }
    }
}
