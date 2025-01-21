#include <ncurses.h>
#include <texteditor.hpp>
#include <algorithm>
#include <sstream>
#include <fstream>

TextEditor::TextEditor()
    : cursorX(0)
    , targetX(0)
    , cursorY(0)
    , scrollX(0)
    , scrollY(0)
    , content("")
{}

TextEditor::TextEditor(std::string filename)
    : TextEditor()
{
    std::ifstream t(filename);
    std::stringstream buffer;
    buffer << t.rdbuf();
    content = buffer.str();
}

void TextEditor::run()
{
    initscr();
    noecho();
    cbreak();

    getmaxyx(stdscr, height, width);

    while (true)
    {
        draw();
        move(cursorY, cursorX);

        int ch = getch();
        clear();

        if (ch == 'q')
            break;

        handleInput(ch);
    }

    endwin();
}

void TextEditor::draw()
{
    std::istringstream lines(content);
    std::string line;

    int i = 0;

    while (i++ < scrollY && std::getline(lines, line));

    i = 0;

    while (std::getline(lines, line))
    {
        mvprintw(i, 0, std::to_string(i + scrollY + 1).c_str());
        mvprintw(i, LINE_NUMBER_COLUMN_WIDTH, line.c_str());
        i++;
    }
}

void TextEditor::handleInput(int ch)
{
    if (isCursorMovement(ch))
        handleCursorMovement(ch);
}

void TextEditor::handleCursorMovement(int ch)
{
    switch (ch)
    {
        case KEY_UP:
        case 'k':
            if (cursorY == 0)
                scrollY = std::max(0, scrollY - 1);
            else
                cursorY = std::max(0, cursorY - 1);
            break;
        case KEY_DOWN:
        case 'j':
            if (cursorY == height - 1)
                scrollY = std::min(getNumLines() - 1, scrollY + 1);
            else
                cursorY = std::min(height - 1, cursorY + 1);
            break;
        case KEY_LEFT:
        case 'h':
            targetX = std::max(0, cursorX - 1);
            break;
        case KEY_RIGHT:
        case 'l':
            targetX = std::min(getLineLength(scrollY + cursorY + 1), cursorX + 1);
            break;
    }

    cursorX = std::min(getLineLength(scrollY + cursorY + 1), targetX);
}

int TextEditor::getNumLines()
{
    std::istringstream lines(content);
    std::string line;
    int numLines = 0;

    while (std::getline(lines, line))
        numLines++;

    return numLines;
}

int TextEditor::getLineLength(int line)
{
    std::istringstream lines(content);
    std::string l;
    int i = 0;

    while (i++ < line && std::getline(lines, l));

    return l.length();
}

bool TextEditor::isCursorMovement(int ch)
{
    return ch == KEY_UP || ch == KEY_DOWN || ch == KEY_LEFT || ch == KEY_RIGHT || ch == 'k' || ch == 'j' || ch == 'h' || ch == 'l';
}
