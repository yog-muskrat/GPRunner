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

            MouseArea { 
                anchors.fill: parent
                onClicked: {
                    projects.currentIndex = index
                    projects.currentProject = projectId
                    gpm.setCurrentProject(projectId)
                }
            }
        }
    }
}
