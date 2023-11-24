import QtQuick
import QtQuick.Controls

Label {
	property int projectId
	property int mrIid
	property string discussionId

	text: "●"
	color: "#B21818"

	HoverHandler { cursorShape: Qt.PointingHandCursor }

	TapHandler {
		onTapped: {
			if(projectId > 0 && mrIid > 0 && discussionId) gpm.markDiscussionsRead(projectId, mrIid, discussionId)
			else if(projectId > 0 && mrIid > 0)            gpm.markDiscussionsRead(projectId, mrIid)
			else if(projectId > 0)                         gpm.markDiscussionsRead(projectId)
		}
	}
}
