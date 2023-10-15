import QtQuick
import QtQuick.Layouts

Rectangle {
    property int currentProject

    Layout.fillHeight: true
    Layout.fillWidth: true

    border {width: 1; color: "#424242"}

    ListView {
        anchors.fill: parent
        anchors.margins: 1

        focus: true
        clip: true
        model: gpm.projectModel
        delegate: Item {
            id: delegateItem

            width: ListView.view.width
            implicitHeight: itemText.implicitHeight

            Rectangle {
                anchors.fill: parent
                color: index == delegateItem.ListView.view.currentIndex ? "#DEDEFE" : "transparent"

                Item {
                    anchors.fill: parent
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Text {
                        id: itemText
                        anchors.fill: parent
                        padding: 2
                        text: display
                        font.pointSize: 12.
                    }
    
                    MouseArea { 
                        anchors.fill: parent
                        onClicked: {
                            delegateItem.ListView.view.currentIndex = index
                            delegateItem.ListView.view.parent.currentProject = projectId
                            gpm.setCurrentProject(projectId)
                        }
                    }
                }
            }
        }
    }
}
