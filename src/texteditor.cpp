#include <functional>
#include <ncurses.h>
#include <texteditor.hpp>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iostream>

TextEditor::TextEditor()
    : m_cursorColumn(1)
    , m_targetColumn(1)
    , m_cursorLine(1)
    , m_scrollX(0)
    , m_scrollY(0)
    , m_content("")
{}

TextEditor::TextEditor(std::string filename)
    : TextEditor()
{
    std::ifstream t(filename);
    std::stringstream buffer;
    buffer << t.rdbuf();
    m_content = buffer.str();
}

void TextEditor::run()
{
    initscr();

    initColors();
    initKeyBindings();

    // Pressed character does not print to screen
    noecho();

    // Pressed character is handled immediately, without waiting for Enter
    cbreak();

    keypad(stdscr, TRUE);

    getmaxyx(stdscr, m_height, m_width);

    initDebug();

    while (true)
    {
        draw();
        displayCursor();
        drawDebug();

        int ch = getch();
        wclear(stdscr);

        if (ch == 'q')
            break;

        handleInput(ch);
    }

    endwin();
}

void TextEditor::draw()
{
    std::istringstream lines(m_content);
    std::string line;

    for (int i = 0; i < m_scrollY; i++)
        std::getline(lines, line);

    for (int i = 0; i < getNumLines() - m_scrollY; i++)
    {
        int lineNum = i + m_scrollY + 1;
        std::getline(lines, line);

        drawText(i, 0, std::to_string(lineNum), lineNum == m_cursorLine ? RED_DEFAULT : GREY_DEFAULT);
        drawText(i, LINE_NUMBER_COLUMN_WIDTH, line);
    }

    std::string position = std::to_string(m_cursorLine) + ":" + std::to_string(m_cursorColumn);
    position += "(" + std::to_string(m_targetColumn) + ")";
    drawText(m_height - 1, m_width - 10, position, RED_DEFAULT);

    refresh();
}

void TextEditor::handleInput(int ch)
{
    printDebug("Key pressed: " + std::to_string(ch));

    auto it = m_keyBindings.find(ch);

    if (it != m_keyBindings.end())
        it->second();
    else
        insertChar(ch);
}

void TextEditor::moveCursorUp()
{
    if (m_cursorLine == m_scrollY + 1)
        m_scrollY = std::max(0, m_scrollY - 1);

    m_cursorLine = std::max(1, m_cursorLine - 1);

    moveToTargetColumn();
}

void TextEditor::moveCursorDown()
{
    if (m_cursorLine == getNumLines())
        return;

    if (m_cursorLine == m_scrollY + m_height)
        m_scrollY = std::min(getNumLines() - 1, m_scrollY + 1);

    m_cursorLine = std::min(getNumLines(), m_cursorLine + 1);

    moveToTargetColumn();
}

void TextEditor::moveCursorRight()
{
    m_targetColumn = std::min(getLineLength(m_cursorLine) + 1, m_cursorColumn + 1);
    moveToTargetColumn();
}

void TextEditor::moveCursorRightInString()
{
    moveCursorTo(getNextGridPosition(m_cursorLine, m_cursorColumn));
}

void TextEditor::moveCursorLeft()
{
    m_targetColumn = std::max(1, m_cursorColumn - 1);
    moveToTargetColumn();
}

void TextEditor::moveCursorLeftInString()
{
    moveCursorTo(getPreviousGridPosition(m_cursorLine, m_cursorColumn));
}

void TextEditor::moveCursorTo(int line, int column)
{
    if (line <= m_scrollY)
        m_scrollY = line - 1;
    else if (line > m_scrollY + m_height)
        m_scrollY = line - m_height;

    m_targetColumn = column;
    m_cursorLine = line;
    moveToTargetColumn();
}

void TextEditor::moveToTargetColumn()
{
    m_cursorColumn = std::min(getLineLength(m_cursorLine) + 1, m_targetColumn);
}

void TextEditor::displayCursor() const
{
    Position cursorPosition = getOnScreenPosition(m_cursorLine, m_cursorColumn);
    move(cursorPosition.first, cursorPosition.second);
}

void TextEditor::deleteCharLeft()
{
    int stringPosition = getStringPosition(m_cursorLine, m_cursorColumn);

    if (stringPosition <= 0 || stringPosition >= m_content.length())
        return;

    moveCursorLeftInString();
    m_content.erase(stringPosition - 1, 1);
}

void TextEditor::deleteWordLeft()
{
    if (charIsWordDelimiter(getCharLeft()))
        deleteCharLeft();

    while (!charIsWordDelimiter(getCharLeft()))
        deleteCharLeft();
}

void TextEditor::deleteCharRight()
{
    int stringPosition = getStringPosition(m_cursorLine, m_cursorColumn);

    if (stringPosition < 0 || stringPosition >= m_content.length())
        return;

    m_content.erase(stringPosition, 1);
}

void TextEditor::deleteWordRight()
{
    if (charIsWordDelimiter(getCharRight()))
        deleteCharRight();

    while (!charIsWordDelimiter(getCharRight()))
        deleteCharRight();
}

void TextEditor::insertChar(char ch)
{
    int stringPosition = getStringPosition(m_cursorLine, m_cursorColumn);

    if (stringPosition < 0 || stringPosition >= m_content.length())
        return;

    m_content.insert(stringPosition, 1, ch);

    moveCursorTo(getGridPosition(stringPosition + 1));
}

char TextEditor::getCharLeft() const
{
    int stringPosition = getStringPosition(m_cursorLine, m_cursorColumn);

    if (stringPosition <= 0 || stringPosition >= m_content.length())
        return '\0';

    return m_content.at(stringPosition - 1);
}

char TextEditor::getCharRight() const
{
    int stringPosition = getStringPosition(m_cursorLine, m_cursorColumn);

    if (stringPosition < 0 || stringPosition >= m_content.length())
        return '\0';

    return m_content.at(stringPosition);
}

bool TextEditor::charIsWordDelimiter(char ch) const
{
    return ch == ' ' || ch == '\n' || ch == '\t' || ch == '\0';
}

int TextEditor::getNumLines() const
{
    std::istringstream lines(m_content);
    std::string line;
    int numLines = 0;

    while (std::getline(lines, line))
        numLines++;

    return numLines;
}

int TextEditor::getLineLength(int line) const
{
    std::istringstream lines(m_content);
    std::string l;
    int i = 0;

    while (i++ < line && std::getline(lines, l));

    return l.length();
}

int TextEditor::getStringPosition(int line, int column) const
{
    std::istringstream lines(m_content);
    std::string currentLine;
    int i = 1;
    int pos = 0;

    while (i++ < line && std::getline(lines, currentLine))
        // +1 to account for \n character
        pos += currentLine.length() + 1;

    // -1 to account for column indexing starting at 1, and string indexing at 0
    return pos + column - 1;
}

Position TextEditor::getGridPosition(int stringPosition) const
{
    std::istringstream lines(m_content);
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

Position TextEditor::getPreviousGridPosition(int line, int column) const
{
    return getGridPosition(getStringPosition(line, column) - 1);
}

Position TextEditor::getNextGridPosition(int line, int column) const
{
    return getGridPosition(getStringPosition(line, column) + 1);
}

Position TextEditor::getOnScreenPosition(int line, int column) const
{
    return {line - m_scrollY - 1, column + LINE_NUMBER_COLUMN_WIDTH - m_scrollX - 1};
}

bool TextEditor::isCursorMovement(int ch) const
{
    return ch == KEY_UP || ch == KEY_DOWN || ch == KEY_LEFT || ch == KEY_RIGHT;
}

void TextEditor::drawText(int y, int x, std::string text, ColorPair c)
{
    useColor(c);
    mvprintw(y, x, text.c_str());
    resetColor();
}

void TextEditor::initKeyBindings()
{
    bindKey(KEY_UP,        &TextEditor::moveCursorUp);
    bindKey(KEY_DOWN,      &TextEditor::moveCursorDown);
    bindKey(KEY_LEFT,      &TextEditor::moveCursorLeft);
    bindKey(KEY_RIGHT,     &TextEditor::moveCursorRight);
    bindKey(KEY_BACKSPACE, &TextEditor::deleteCharLeft);
    bindKey(KEY_DC,        &TextEditor::deleteCharRight);
    bindKey('\'',          &TextEditor::deleteWordLeft);
    bindKey('*',           &TextEditor::deleteWordRight);
}

void TextEditor::bindKey(int key, void(TextEditor::*action)())
{
    m_keyBindings[key] = [this, action]() { (this->*action)(); };
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
    m_currentColor = pair;
    attron(COLOR_PAIR(m_currentColor));
}

void TextEditor::initDebug()
{
    m_debugHeight = m_height / 2;
    m_debugWidth = m_width / 2;

    m_debugWindow = newwin(m_debugHeight, m_debugWidth, 0, m_width - m_debugWidth - 1);

    if (m_debugWindow == NULL)
    {
        endwin();
        std::cerr << "Error creating debug window" << std::endl;
        exit(1);
    }
}

void TextEditor::drawDebug()
{
    wclear(m_debugWindow);
    box(m_debugWindow, 0, 0);

    int i = 0;

    for (const auto& line : m_debugLines)
    {
        int posY = m_debugHeight - 2 - i;
        mvwprintw(m_debugWindow, posY, 1, line.c_str());

        i++;

        if (i >= m_debugHeight - 2)
            break;
    }

    wrefresh(m_debugWindow);
}

void TextEditor::printDebug(std::string str)
{
    m_debugLines.insert(m_debugLines.begin(), str);
}
