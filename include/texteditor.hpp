#include <ncurses.h>
#include <string>

class TextEditor
{
    static const int LINE_NUMBER_COLUMN_WIDTH = 4;
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
    };

public:
    TextEditor();
    TextEditor(std::string filename);
    void run();

private:
    int width, height;
    int cursorX, targetX, cursorY;
    int scrollX, scrollY;
    ColorPair currentColor;
    std::string content;

    void handleInput(int ch);
    void handleCursorMovement(int ch);
    void draw();

    int getNumLines();
    int getLineLength(int line);

    bool isCursorMovement(int ch);

    void drawText(int y, int x, std::string text, ColorPair c = DEFAULT);

    void initColors();
    void useColor(ColorPair pair);
    void resetColor() { attroff(COLOR_PAIR(currentColor)); };
};
