#include "keyhandler.hpp"

#include <ncurses.h>

void KeyHandler::handleKey(Key key)
{
    debug("Key: " + std::to_string(static_cast<int>(key)));

    if (m_keyBindings.find(key) == m_keyBindings.end())
        return;

    m_keyBindings[key]();
}

void KeyHandler::bindKey(Key key, std::function<void()> callback)
{
    m_keyBindings[key] = [callback]() { (callback)(); };
}

void KeyHandler::setDebugPrintCallback(std::function<void(std::string)> debugPrintCallback)
{
    m_debug = debugPrintCallback;
}

void KeyHandler::debug(std::string msg)
{
    if (m_debug == nullptr)
        return;

    m_debug(msg);
}
