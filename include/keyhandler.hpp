#pragma once

#include <functional>
#include <map>
#include <string>

using Key = wint_t;

#define KEY_C_BACKSPACE 8
#define KEY_C_DC 520
#define KEY_C_W 23

class KeyHandler
{
public:
    void handleKey(Key key);
    void bindKey(Key key, std::function<void()> callback);
    void setDebugPrintCallback(std::function<void(std::string)> debugPrintCallback);

private:
    std::map<Key, std::function<void()>> m_keyBindings;

    // DEBUG
    std::function<void(std::string)> m_debug;

    void debug(std::string msg);
};
