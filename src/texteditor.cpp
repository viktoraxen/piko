#include <ncurses.h>
#include <texteditor.hpp>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iostream>

TextEditor::TextEditor()
    : cursorX(LINE_NUMBER_COLUMN_WIDTH)
    , targetX(LINE_NUMBER_COLUMN_WIDTH)
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

    initColors();

    // Pressed character does not print to screen
    noecho();

    // Pressed character is handled immediately, without waiting for Enter
    cbreak();

    keypad(stdscr, TRUE);

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
        drawText(i, 0, std::to_string(i + scrollY + 1), i == cursorY ? RED_DEFAULT : GREY_DEFAULT);
        drawText(i, LINE_NUMBER_COLUMN_WIDTH, line);
        i++;
    }
}

void TextEditor::handleInput(int ch)
{
    if (isCursorMovement(ch))
        handleCursorMovement(ch);
    else
        handleTextEditing(ch);

    cursorX = std::min(getLineLength(scrollY + cursorY + 1) + LINE_NUMBER_COLUMN_WIDTH, targetX);
}

void TextEditor::handleCursorMovement(int ch)
{
    switch (ch)
    {
        case KEY_UP:
            moveCursorUp();
            break;
        case KEY_DOWN:
            moveCursorDown();
            break;
        case KEY_LEFT:
            moveCursorLeft();
            break;
        case KEY_RIGHT:
            moveCursorRight();
            break;
    }
}

void TextEditor::handleTextEditing(int ch)
{
    if (ch == KEY_BACKSPACE)
    {
        if (getInTextPosition() == 0)
            return;

        int textPosition = getInTextPosition();
        content.erase(textPosition - 1, 1);
        moveCursorTo(getOnScreenPosition(textPosition - 1));
    }
    else if (ch == '\n')
    {
        int textPosition = getInTextPosition();
        content.insert(getInTextPosition(), 1, '\n');
        moveCursorTo(getOnScreenPosition(textPosition + 1));
    }
    else 
    {
        content.insert(getInTextPosition(), 1, ch);
        moveCursorRight();
    }
}

void TextEditor::moveCursorUp()
{
    if (cursorY == 0)
        scrollY = std::max(0, scrollY - 1);
    else
        cursorY = std::max(0, cursorY - 1);
}

void TextEditor::moveCursorDown()
{
    if (scrollY + cursorY == getNumLines() - 1)
        return;

    if (cursorY == height - 1)
        scrollY = std::min(getNumLines() - 1, scrollY + 1);
    else
        cursorY = std::min(height - 1, cursorY + 1);
}

void TextEditor::moveCursorRight()
{
    targetX = std::min(getLineLength(scrollY + cursorY + 1) + LINE_NUMBER_COLUMN_WIDTH, cursorX + 1);
}

void TextEditor::moveCursorLeft()
{
    targetX = std::max(LINE_NUMBER_COLUMN_WIDTH, cursorX - 1);
}

void TextEditor::moveCursorTo(int y, int x)
{
    targetX = x;
    cursorY = y;
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

int TextEditor::getInTextPosition()
{
    std::istringstream lines(content);
    std::string line;
    int i = 0;
    int pos = 0;

    while (i++ < scrollY + cursorY && std::getline(lines, line))
        pos += line.length() + 1;

    return pos + cursorX - LINE_NUMBER_COLUMN_WIDTH;
}

std::pair<int, int> TextEditor::getOnScreenPosition(int inTextPosition)
{
    std::istringstream lines(content);
    std::string line;

    int y = 0;

    while (y++ < scrollY && std::getline(lines, line))
        inTextPosition -= line.length() + 1;

    y = 0;

    while (std::getline(lines, line))
    {
        if (inTextPosition < line.length() + 1)
            return {y, inTextPosition + LINE_NUMBER_COLUMN_WIDTH};

        inTextPosition -= line.length() + 1;
        y++;
    }

    return {y, LINE_NUMBER_COLUMN_WIDTH};
}

bool TextEditor::isCursorMovement(int ch)
{
    return ch == KEY_UP || ch == KEY_DOWN || ch == KEY_LEFT || ch == KEY_RIGHT;
}

void TextEditor::drawText(int y, int x, std::string text, ColorPair c)
{
    useColor(c);
    mvprintw(y, x, text.c_str());
    resetColor();
}

void TextEditor::initColors()
{
    start_color();
    use_default_colors();

    if (can_change_color())
    {
        init_color(GREY, 0, 0, 0);

        init_pair(DEFAULT,      WHITE, -1);
        init_pair(GREY_DEFAULT, GREY,  -1);
        init_pair(RED_DEFAULT,  RED,   -1);

    }
}

void TextEditor::useColor(ColorPair pair)
{
    currentColor = pair;
    attron(COLOR_PAIR(currentColor));
}
