#include <ncurses.h>
#include <string>

class TextEditor
{
    static const int LINE_NUMBER_COLUMN_WIDTH = 4;
public:
    TextEditor();
    TextEditor(std::string filename);
    void run();

private:
    int width, height;
    int cursorX, targetX, cursorY;
    int scrollX, scrollY;
    std::string content;

    void handleInput(int ch);
    void handleCursorMovement(int ch);
    void draw();

    int getNumLines();
    int getLineLength(int line);

    bool isCursorMovement(int ch);
};
