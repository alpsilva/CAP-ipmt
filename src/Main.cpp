#include "Huffman.cpp"
#include "sufixArray.cpp"
#include <sstream>

using namespace std;

#define VERSION 0.1

int main(int argc, char *argv[]){
    string helpText = R"(
        -h or --help -> Information about IPMT.
        -v or --version -> Current version of IPMT.
        
        Optional operational args:
        -c or --count       -> Informs PMT to return the amount of occurrences that match the given pattern(s).
        -p or --patten      -> Receives the next string. Informs PMT to use the appointed file to read a list of patterns and use them in the search.
        How to use IPMT:
        ipmt zip <textFilePath>
        ipmt unzip <compressedFilePath>
    )";
    // Obligatory
    string textFile = "";
    
    if (argc < 2){
        cout << "You have to provide a command. Use \"ipmt --help\" to see available options." << endl;
    }
    string mode = argv[1];
    string file;
    if (mode == "zip"){
        if (argc < 3){ 
            cout << "You have to provide a textfile." << endl;
        }
        file = argv[2];
        
        ifstream t(file);
        stringstream buffer;
        buffer << t.rdbuf();

        string text = buffer.str();
        string compressedFilePath = file + ".myz";
        
        zip(text, compressedFilePath);
    }

    else if (mode == "unzip"){
        if (argc < 3){ 
            cout << "You have to provide a .myz file." << endl;
        }
        file = argv[2];

        string decompressedTextFilePath = file.substr(0, (file.size() - 4));

        unzip(file, decompressedTextFilePath);
    }

    else if (mode == "index"){
        if (argc < 3){ 
            cout << "You have to provide a textfile." << endl;
        }
        file = argv[2];
        saveIndexFile(file);
    }
    else if (mode == "search"){
        if (argc < 4){ 
            cout << "You have to provide a pattern and a textfile." << endl;
        }
        string patternFile = "";
        bool isCount = false;
        int optionalArgs = argc - 2;
        for (int i = 2; i < optionalArgs; i++){
            string arg = argv[i];
            if (arg == "-p" || arg == "--pattern") {
                patternFile = argv[++i];
                optionalArgs++;
            } else if (arg == "-c" || arg == "--count") {
                isCount = true;
            } else {
                cout << "Option does not exist: " << arg << endl;
                return 0;
            }
        }
        vector<string> patternList;
        if (patternFile.size() > 0){
            patternList = readStringFromFile(patternFile.data());

        } else {
            patternList.push_back(argv[argc-2]);
        }
        file = argv[argc-1];
        searchPattern(file, patternList, isCount);
    }

    else {
        cout << "Invalid command. Use \"ipmt --help\" to see available options." << endl;
    }

}
