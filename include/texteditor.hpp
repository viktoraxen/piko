#pragma once

#include "keyhandler.hpp"

#include <ncurses.h>
#include <string>

// #define DEBUG

using Position = std::pair<int, int>;

class TextEditor
{
    const int LINE_NUMBER_COLUMN_WIDTH = 4;

    enum Color
    {
        BLACK   = COLOR_BLACK,
        RED     = COLOR_RED,
        GREEN   = COLOR_GREEN,
        YELLOW  = COLOR_YELLOW,
        BLUE    = COLOR_BLUE,
        MAGENTA = COLOR_MAGENTA,
        CYAN    = COLOR_CYAN,
        WHITE   = COLOR_WHITE,
        GREY,
    };

    enum ColorPair
    {
        DEFAULT,
        GREY_DEFAULT,
        RED_DEFAULT,
        GREEN_DEFAULT,
    };
    
public:
    TextEditor();
    TextEditor(std::string filename);
    void run();

private:
    int m_width, m_height;
    int m_cursorLine, m_cursorColumn, m_targetColumn;
    // scrollY/X is number of lines/columns not visible on the screen
    int m_scrollX, m_scrollY;
    ColorPair m_currentColor;
    std::string m_filename;
    std::string m_content;
    KeyHandler m_keyHandler;
    std::string m_msg;

    void write();

    void draw();

    void handleCursorMovement(int ch);
    void handleTextEditing(int ch);
    void moveCursorUp();
    void moveCursorDown();
    void moveCursorRight();
    void moveCursorRightInString();
    void moveCursorLeft();
    void moveCursorLeftInString();
    void moveCursorTo(int y, int x);
    void moveCursorTo(Position position) { moveCursorTo(position.first, position.second); };
    void moveToTargetColumn();
    void displayCursor() const;

    void deleteCharLeft();
    void deleteCharRight();
    void deleteWordLeft();
    void deleteWordRight();

    void insertChar(wchar_t ch);

    wchar_t getCharLeft() const;
    wchar_t getCharRight() const;

    bool charIsWordDelimiter(wchar_t ch) const;
    int getCharWidth(wchar_t ch) const;

    int getNumLines() const;
    int getLineLength(int line) const;

    int getStringPosition(int line, int column) const;

    Position getGridPosition(int stringPosition) const;
    Position getPreviousGridPosition(int line, int column) const;
    Position getNextGridPosition(int line, int column) const;

    Position getOnScreenPosition(int line, int column) const;
    Position getOnScreenPosition(Position position) const { return getOnScreenPosition(position.first, position.second); };

    bool isCursorMovement(int ch) const;

    void drawText(int line, int column, std::string text, ColorPair c = DEFAULT);
    void drawText(Position position, std::string text, ColorPair c = DEFAULT) { drawText(position.first, position.second, text, c); };

    void initKeyBindings();
    void bindKey(int key, void(TextEditor::*action)());

    void initColors();
    void useColor(ColorPair pair);
    void resetColor() { attroff(COLOR_PAIR(m_currentColor)); };

    // DEBUG
    WINDOW* m_debugWindow;
    int m_debugHeight, m_debugWidth;
    std::vector<std::string> m_debugLines;

    void initDebug();
    void drawDebug();
    void debug(std::string str);
};
