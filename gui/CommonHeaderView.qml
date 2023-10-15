import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

HorizontalHeaderView {
    clip: true

    delegate: Rectangle {
        implicitWidth:  itemText.implicitWidth
        implicitHeight: itemText.implicitHeight
        border {width: 1; color: "#424242"}
        color: "#EFEFEF"

        Text {
            id: itemText

            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

            text: display
            padding: 5
            font.pointSize: 12
        }
    }
}
