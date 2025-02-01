#include <texteditor.hpp>
#include <string>
#include <iostream>

int main(int argc, char **argv)
{
    if (argc > 1)
    {
        std::string filename = argv[1];

        TextEditor editor(filename);
        editor.run();

        return 0;
    }

    std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;

    return 0;
}
