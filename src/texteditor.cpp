#include <ncurses.h>
#include <string>
#include <texteditor.hpp>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iostream>

TextEditor::TextEditor()
    : cursorColumn(1)
    , targetColumn(1)
    , cursorLine(1)
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
        move(cursorLine - scrollY - 1, cursorColumn + LINE_NUMBER_COLUMN_WIDTH - scrollX - 1);

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

    for (int i = 0; i < scrollY; i++)
        std::getline(lines, line);

    for (int i = 0; i < getNumLines() - scrollY; i++)
    {
        int lineNum = i + scrollY + 1;
        std::getline(lines, line);

        drawText(i, 0, std::to_string(lineNum), lineNum == cursorLine ? RED_DEFAULT : GREY_DEFAULT);
        drawText(i, LINE_NUMBER_COLUMN_WIDTH, line);
    }

    std::string position = std::to_string(cursorLine) + ":" + std::to_string(cursorColumn);
    position += "(" + std::to_string(targetColumn) + ")";
    drawText(height - 1, width - 10, position, RED_DEFAULT);
}

void TextEditor::handleInput(int ch)
{
    if (isCursorMovement(ch))
        handleCursorMovement(ch);
    else
        handleTextEditing(ch);

    cursorColumn = std::min(getLineLength(cursorLine) + 1, targetColumn);
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
        deleteCharLeft(cursorLine, cursorColumn);
    else if (ch == KEY_DC)
        deleteCharRight(cursorLine, cursorColumn);
    else 
        insertChar(cursorLine, cursorColumn, ch);
}

void TextEditor::moveCursorUp()
{
    if (cursorLine == scrollY + 1)
        scrollY = std::max(0, scrollY - 1);

    cursorLine = std::max(1, cursorLine - 1);
}

void TextEditor::moveCursorDown()
{
    if (cursorLine == getNumLines())
        return;

    if (cursorLine == scrollY + height)
        scrollY = std::min(getNumLines() - 1, scrollY + 1);

    cursorLine = std::min(getNumLines(), cursorLine + 1);
}

void TextEditor::moveCursorRight()
{
    targetColumn = std::min(getLineLength(cursorLine) + 1, cursorColumn + 1);
}

void TextEditor::moveCursorLeft()
{
    targetColumn = std::max(1, cursorColumn - 1);
}

void TextEditor::moveCursorTo(int line, int column)
{
    if (line <= scrollY)
        scrollY = line - 1;
    else if (line > scrollY + height)
        scrollY = line - height;

    targetColumn = column;
    cursorLine = line;
}

void TextEditor::deleteCharLeft(int line, int column)
{
    int stringPosition = getStringPosition(line, column);

    if (stringPosition == 0)
        return;

    content.erase(stringPosition - 1, 1);

    moveCursorTo(getGridPosition(stringPosition - 1));
}

void TextEditor::deleteCharRight(int line, int column)
{
    content.erase(getStringPosition(line, column), 1);
}

void TextEditor::insertChar(int line, int column, char ch)
{
    int stringPosition = getStringPosition(line, column);
    content.insert(stringPosition, 1, ch);

    moveCursorTo(getGridPosition(stringPosition + 1));
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

int TextEditor::getStringPosition(int line, int column)
{
    std::istringstream lines(content);
    std::string currentLine;
    int i = 1;
    int pos = 0;

    while (i++ < line && std::getline(lines, currentLine))
        // +1 to account for \n character
        pos += currentLine.length() + 1;

    // -1 to account for column indexing starting at 1, and string indexing at 0
    return pos + column - 1;
}

std::pair<int, int> TextEditor::getGridPosition(int stringPosition)
{
    std::istringstream lines(content);
    std::string line;
    int maxLines = getNumLines();

    for (int lineNum = 1; lineNum < maxLines; lineNum++)
    {
        std::getline(lines, line);

        if (stringPosition <= line.length())
            return {lineNum, stringPosition + 1};

        stringPosition -= line.length() + 1;
    }

    // Places cursor at the end of previously last line
    return {maxLines, stringPosition + 1};
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
