#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <vector>

using namespace std;

struct Node
{
    char c;
    int frequency;
    Node *left;
    Node *right;
};

Node* createNode(char c, int frequency, Node* left, Node* right){
    Node* node = new Node();

    node->c = c;
    node->frequency = frequency;
    node->left = left;
    node->right = right;

    return node;
}

// Comparison object
struct comparison
{
    bool operator()(const Node* left, const Node* right) const{
        return left->frequency > right->frequency;
    }
};

bool isLeaf(Node* node){
    if (node->left == nullptr){
        if (node->right == nullptr){
            return true;
        }
    }
    return false;
}

// Stores the code for each character generated by the Huffman tree in a map.
void encode(Node* root, string code, map<char, string> &huffmanCode){
    if (root == nullptr){
        return;
    }

    if (isLeaf(root)){
        char key = root->c;
        if (code != ""){
            huffmanCode[key] = code;
        } else {
            huffmanCode[key] = "1";
        }
    }

    encode(root->left, (code + "0"), huffmanCode);
    encode(root->right, (code + "1"), huffmanCode);
}

map<int, int> buildFrequencyTable(string text){
    // counts the frequency of each character in the text.
    map<int, int> frequency;
    for (char c : text){
        int ic = (int) c;
        frequency[ic] += 1;
    }

    return frequency;
}

// Builds the huffman tree and 
pair<Node*, map<char, string>> buildHuffmanTree(map<int, int> frequency){

    // Creates a priority queue to store the nodes of the in-progress huffman tree.
    // And for each character present in the text at least once, create a node and
    // put it at the queue.
    priority_queue<Node*, vector<Node*>, comparison> queue;

    for (auto pair: frequency) {
        queue.push(
            createNode((char) pair.first, pair.second, nullptr, nullptr)
        );
    }

    while (queue.size() != 1){
        // removes the 2 nodes with the lowest frequency from the queue and
        // merge them, making a new node whose children are the 2 previous nodes.
        // The new node frequency is the sum of the previous 2 frequencies.
        Node* left = queue.top();
        queue.pop();
        Node* right = queue.top();
        queue.pop();

        queue.push(
            createNode('\0', (left->frequency + right->frequency), left, right)
        );
    }

    Node* huffmanTreeRoot = queue.top();

    // Store the codes
    map<char, string> huffmanCode;
    encode(huffmanTreeRoot, "", huffmanCode);

    // cout << "Huffman codes" << endl;
    // for (auto pair : huffmanCode){
    //     cout << pair.first << " : " << pair.second << endl;
    // }

    pair<Node*, map<char, string>> output;
    output.first = huffmanTreeRoot;
    output.second = huffmanCode;
    return output;
}

string encodeText(string text, map<char, string> huffmanCode){
    string encodedString = "";
    for (char c : text){
        encodedString.append(
            huffmanCode[c]
        );
    }

    return encodedString;
}

string decodeText(string encodedText, map<int, int> frequency){
    pair<Node*, map<char, string>> huffmanPair = buildHuffmanTree(frequency);
    Node* huffmanTreeRoot = huffmanPair.first;
    map<char, string> huffmanCodes = huffmanPair.second;
    string decodedString = "";

    if (isLeaf(huffmanTreeRoot)){
        // Special case: A string with only one character repeated any number of times.
        while(huffmanTreeRoot->frequency--){
            decodedString.push_back(huffmanTreeRoot->c);
        }
    } else {
        map<string, char> invertedHuffmanCodes;

        for (auto pair : huffmanCodes){
            invertedHuffmanCodes[pair.second] = pair.first;
        }

        string encodedSubstring = "";

        char lastC;
        for (char c : encodedText){
            encodedSubstring.push_back(c);
            // Checks if the current code can be found in the key list of the huffman code table.
            if (invertedHuffmanCodes.find(encodedSubstring) != invertedHuffmanCodes.end()){
                decodedString.push_back(
                    invertedHuffmanCodes[encodedSubstring]
                );
                encodedSubstring = "";
            }
            lastC = c;
        }
    }

    return decodedString;
}

void writeCompressedFile(string filePath, map<int, int> frequency, string encodedText){
    ofstream file;
    file.open(filePath, ios::binary | ios::out);
    int charFrequencyAmount = frequency.size();
    file << charFrequencyAmount << endl;

    for (auto pair: frequency) {
        file << pair.first << endl;
        file << pair.second << endl;
    }
    
    // As we can't write individual bits to files, we arrange them in groups of 8
    // and write the bytes.
    int byte_index = 0;
    unsigned char bit_buffer;
    
    int completeByteAmount = (encodedText.size() / 8);
    int remainder = encodedText.size() % 8;

    file << completeByteAmount << endl;
    file << remainder << endl;

    for (char c : encodedText){
        int bit = c - '0';
        if (bit){
            bit_buffer |= (1<<byte_index);
        }
        byte_index++;
        if (byte_index == 8){
            file << bit_buffer;
            byte_index = 0;
            bit_buffer = 0;
        }
    }

    file << bit_buffer;

    file.close();
}

pair<map<int, int>, string> readCompressedFile(string filePath){
    ifstream file;
    string line;

    map<int, int> frequency;

    file.open(filePath, ios::binary | ios::in);

    getline(file, line);
    int charFrequencyAmount = stoi(line);

    for (int i = 0; i < charFrequencyAmount; i++){
        getline(file, line);
        int key = stoi(line);
        getline(file, line);
        frequency[key] = stoi(line);
    }

    getline(file, line);
    int charListSize = stoi(line);

    getline(file, line);
    int remainder = stoi(line);

    string encodedText = "";

    vector<char> completeCharList(charListSize);
    char c;
    char lastC;

    // Reads the bits from the file and puts them in a vector for later processing.
    for (int i = 0; i < charListSize; i++){
        file.get(c);
        completeCharList[i] = c;
    }

    file.get(c);
    lastC = c;

    for (int i = 0; i < charListSize; i++){
        char c = completeCharList[i];
        for (int i = 0; i < 8; i++){
            int bit = (c >> i) & 1;
            encodedText.push_back(bit + '0');
        }
    }

    for (int i = 0; i < remainder; i++){
        int bit = (lastC >> i) & 1;
        encodedText.push_back(bit + '0');
    }

    file.close();
    
    pair<map<int, int>, string> output;
    output.first = frequency;
    output.second = encodedText;
    
    return output;
}

void zip(string text, string compressedFilePath){
    map<int, int> frequency = buildFrequencyTable(text);
    pair<Node*, map<char, string>> huffmanPair = buildHuffmanTree(frequency);
    map<char, string> huffmanCode = huffmanPair.second;

    string encodedString = encodeText(text, huffmanCode);
    writeCompressedFile(compressedFilePath, frequency, encodedString);
}

void unzip(string compressedFilePath, string decompressedTextFilePath){
    pair<map<int, int>, string> output;
    output = readCompressedFile(compressedFilePath);
    map<int, int> frequency = output.first;
    string encodedText = output.second;
    
    string decodedString = decodeText(encodedText, frequency);

    ofstream file;
    file.open(decompressedTextFilePath, ios::out);

    file << decodedString << endl;

    file.close();
}