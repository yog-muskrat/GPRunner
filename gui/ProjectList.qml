import QtQuick
import QtQuick.Layouts

Rectangle {
    Layout.fillHeight: true
    Layout.fillWidth: true

    border {width: 1; color: "#424242"}

    ListView {
        property int currentProject

        id: projects

        anchors.fill: parent
        anchors.margins: 1

        focus: true
        clip: true
        model: gpm.projectModel
        delegate: projectItemDelegate

        Component {
            id: projectItemDelegate

            Item {
                id: delegateItem

                width: ListView.view.width
                implicitHeight: itemText.implicitHeight

                Rectangle {
                    anchors.fill: parent
                    color: index == delegateItem.ListView.view.currentIndex ? "#DEDEFE" : "transparent"
                }

                RowLayout {
                    anchors.fill: parent
                    spacing: 2

                    Item {
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
                                delegateItem.ListView.view.currentProject = projectId

                                gpm.loadPipelines(projectId)
                            }
                        }
                    }
                }
            }
        }
    }
}
