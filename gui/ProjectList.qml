import QtQuick
import QtQuick.Layouts

Rectangle {
    property int currentProject

    Layout.fillHeight: true
    Layout.fillWidth: true

    border {width: 1; color: "#424242"}

    ListView {
        id: projects

        anchors.fill: parent
        anchors.margins: 1

        focus: true
        clip: true

        model: gpm.projectModel

        delegate: Item {
            width: ListView.view.width
            implicitHeight: itemText.implicitHeight

            Rectangle {
                anchors.fill: parent
                color: index == projects.currentIndex ? "#DEDEFE" : ((index % 2) == 0 ? "#EFEFEF" : "transparent")

                Text {
                    id: itemText
                    anchors.fill: parent
                    padding: 5
                    text: model.display
                    font: model.font
                }

                MouseArea { 
                    anchors.fill: parent
                    onClicked: {
                        projects.currentIndex = index
                        projects.parent.currentProject = projectId
                        gpm.setCurrentProject(projectId)
                    }
                }
            }
        }
    }
}
