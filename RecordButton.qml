import QtQuick
import QtQuick.Controls
import Intellicute
import core

Button {
    id: control
    text: "\uf3c9"
    font.family: Style.iconFont
    font.pixelSize: 24
    enabled: whisperRequest.status !== WatsonTTSRequest.InProgress
    checkable: true

    QtObject {
        id: d
        property bool blinkActive: false
    }

    // blink timer
    Timer {
        id: blinkTimer
        interval: 500
        running: control.checked
        repeat: true
        onTriggered: {
            d.blinkActive = !d.blinkActive
        }
    }

    contentItem: Text {
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.3
        color: control.checked ? (d.blinkActive ? "red" : "white") : "black"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
}