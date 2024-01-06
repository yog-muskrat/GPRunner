import QtQuick
import QtQuick.Controls

Label {
	property var project
	property var mr
	property var discussion

	text: "●"
	color: "#B21818"

	HoverHandler { cursorShape: Qt.PointingHandCursor }

	TapHandler {
		onTapped: {
			if(discussion)   discussion.markRead()
			else if(mr)      mr.markDiscussionsRead()
			else if(project) project.markDiscussionsRead()
		}
	}
}
