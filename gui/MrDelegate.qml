import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qt.labs.qmlmodels

Rectangle {
    property int padding: 5

    default property alias content: mainLayout.children

    id: root
    color: TableView.view.currentRow == row ? palette.highlight : palette.base
    implicitWidth: mainLayout.implicitWidth + (column == 0 ? padding * 2 : padding)
    implicitHeight: mainLayout.implicitHeight + padding * 2

    TapHandler {
        onTapped: {
            root.TableView.view.mrSelected(model.mr)
        }
    }

    RowLayout {
        id: mainLayout
        spacing: padding
        anchors.fill: parent
        anchors.leftMargin: column == 0 ? padding : 0
        anchors.rightMargin: padding
        anchors.topMargin: padding
        anchors.bottomMargin: padding
    }
}
