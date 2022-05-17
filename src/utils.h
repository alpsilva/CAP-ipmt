#include <string>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

// TODO: Create a function that reads the searchable text and separate it file lines.

vector<string> readStringFromFile(const char* fileName)
{
    std::ifstream file(fileName);
    if (!file)
    {
        cout << "couldn't open file" << endl;
        abort();
    }
    vector<string> text;
    string buffer;
    while (getline(file, buffer))
    {
        text.push_back(buffer);
    }
    file.close();
    return text;
}