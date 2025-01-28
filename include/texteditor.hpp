#include <ncurses.h>
#include <string>

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
    void moveCursorLeft();
    void moveCursorTo(int y, int x);
    void moveCursorTo(std::pair<int, int> position) { moveCursorTo(position.first, position.second); };

    int getNumLines();
    int getLineLength(int line);
    int getStringPosition(int line, int column);
    std::pair<int, int> getGridPosition(int stringPosition);

    bool isCursorMovement(int ch);

    void drawText(int y, int x, std::string text, ColorPair c = DEFAULT);

    void initColors();
    void useColor(ColorPair pair);
    void resetColor() { attroff(COLOR_PAIR(currentColor)); };
};
