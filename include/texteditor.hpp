#include <ncurses.h>
#include <string>

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
        DEFAULT = 1,
        GREY_DEFAULT,
        RED_DEFAULT,
    };

public:
    TextEditor();
    TextEditor(std::string filename);
    void run();

private:
    int width, height;
    int cursorLine, cursorColumn, targetColumn;
    // scrollY/X is number of lines/columns not visible on the screen
    int scrollX, scrollY;
    ColorPair currentColor;
    std::string content;

    void draw();

    void handleInput(int ch);
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

    void insertChar(char ch);

    char getCharLeft() const;
    char getCharRight() const;

    bool charIsWordDelimiter(char ch) const;

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

    void initColors();
    void useColor(ColorPair pair);
    void resetColor() { attroff(COLOR_PAIR(currentColor)); };
};
