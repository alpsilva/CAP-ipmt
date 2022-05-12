#include "Huffman.cpp"
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
        cout << "cheguei aqui" << endl;
        ifstream t(file);
        stringstream buffer;
        buffer << t.rdbuf();

        cout << "Li a string" << endl;


        string text = buffer.str();
        string compressedFilePath = "";

        for (int i = 0; i < file.size(); i++){
            char c = file[i];
            if (c != '.'){
                compressedFilePath.push_back(c);
            } else {
                i = file.size();
            }
        }

        compressedFilePath += ".bin";
        
        zip(text, compressedFilePath);
    }

    else if (mode == "unzip"){
        if (argc < 3){ 
            cout << "You have to provide a .myz file." << endl;
        }
        file = argv[2];

        string decompressedTextFilePath = "";

        for (int i = 0; i < file.size(); i++){
            char c = file[i];
            if (c != '.'){
                decompressedTextFilePath.push_back(c);
            } else {
                i = file.size();
            }
        }

        decompressedTextFilePath += ".txt";

        unzip(file, decompressedTextFilePath);
    }

    else if (mode == "index"){
        // Todo
    }

    else if (mode == "search"){
        // Todo
    }

    else {
        cout << "Invalid command. Use \"ipmt --help\" to see available options." << endl;
    }

}