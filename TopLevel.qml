import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.settings

Item {
    id: root

    property var settings: defaultSettings
    readonly property var defaultSettings: {
        "tts": {
            "autoplay": false,
            "locale": "en",
            "watson": {
                "use": false,
                "voice": "en-US_MichaelV3Voice",
                "apiKey": "",
                "apiUrl": ""
            }
        },
        "chat": {
            "defaultMessages": {
                "messages": [
                    {"role": "system", "content": "Act as friendly and intelligent AI assistant. Your name is 'Intellicute'. Your UI is written with Qt and QML by [Machine Koder](https://machinekoder.com)."}
                ]},
            "model": "gpt-3.5-turbo",
            "maxTokens": 2048,
            "temperature": 0.3,
            "topP": 1.0,
            "frequencyPenalty": 0.0,
            "presencePenalty": 0.0,
        },
        "openai": {
            "accessToken": ""
        }
    }

    function resetSettings() {
        root.settings = root.defaultSettings
    }

    Settings {
        id: settingsStore
        property alias settings: root.settings
    }


    StackView {
        id: stack
        anchors.fill: parent
    }

    Component {
        id: chatPage
        ChatPage {
            settings: root.settings
            onSettingsRequested: stack.push(settingsPage)
        }
    }

    Component {
        id: settingsPage
        SettingsPage {
            settings: root.settings
            onCancel: stack.pop()
            onReset: root.resetSettings()
            onAccept: {
                root.settings = settings
                stack.pop()
            }
        }
    }

    Component.onCompleted: {
        stack.push(chatPage)
    }
}
