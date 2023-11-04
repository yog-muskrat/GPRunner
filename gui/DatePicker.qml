import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Rectangle {
	//property Date datetime

	border {width: 1; color: "#424242"}
    width: 200
    height: 180

	ListView {
		id: listview

        anchors.fill: parent
        snapMode: ListView.SnapOneItem
        orientation: ListView.Horizontal
        highlightRangeMode: ListView.StrictlyEnforceRange

        clip: true
        model: CalendarModel {
            from: new Date(2023, 0, 1)
            to: new Date(2023, 11, 31)
        }

        delegate: MonthGrid {
            width: listview.width
            height: listview.height

            month: model.month
            year: model.year
            locale: Qt.locale("en_US")
        }

        ScrollIndicator.horizontal: ScrollIndicator { }
	}
}
