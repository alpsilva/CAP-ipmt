#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <string_view>
#include "utils.h"
#include <assert.h>
#include <chrono>

#include <fcntl.h>

#include "SA.h"
#include "BWT.h"
#include "SAIS.h"    


using namespace std;

struct Tuple
{
    int first;
    int second;
    int third;
};

struct lexCompResult
{
    int cmpResult;
    int lcp;
};

struct sliceIndexes
{
    int start;
    int finish;
};




vector<int> sortLetters(string text)
{
    vector<int> temp;
    const int textLength = text.length();
    temp.reserve(textLength);
    for (int i = 0; i < textLength; i++)
    {
        temp.push_back(text[i]);
    }
    return temp;
}

//need to implement this  https://sortingsearching.com/2015/09/26/radix-sort.html
bool compare(const struct Tuple a, const struct Tuple b)
{
    if (a.first < b.first)
    {
        return true;
    }
    if (a.first > b.first)
    {
        return false;
    }
    if (a.second < b.second)
    {
        return true;
    }
    if (a.second > b.second)
    {
        return false;
    }
    return false;

}

vector<vector<int>> buildP(string text)
{
    uint64_t n = text.length();

    vector<vector<int>> P;
    if (n == 0)
    {

        P.push_back({});
        P[0].push_back('\n');
        return P;
    }
    int t = ceil(log2(n));

    P.reserve(t + 2);
    vector<int> temp = sortLetters(text);
    P.push_back(temp);
    uint64_t k = 1;
    while (k <= t)
    {
        uint64_t l = pow(2, k - 1);
        vector<struct Tuple> pairs;
        pairs.resize(n);
        for (int i = 0; i < n; i++)
        {
            pairs[i].first = P[k - 1][i];
            if (i + l < n)
            {
                pairs[i].second = P[k - 1][i + l];
            }
            else
            {
                pairs[i].second = -1;
            }
            pairs[i].third = i;
        }

        sort(pairs.begin(), pairs.end(), compare);
        vector<int> Pk;
        Pk.resize(n);
        int order = 0;
        Pk[pairs[0].third] = order;
        for (int i = 1; i < n; i++)
        {
            //thought to use memcmp but apparently you cant because of mutiple reasons:
            //https://stackoverflow.com/questions/141720/how-do-you-compare-structs-for-equality-in-c
            if (pairs[i - 1].first != pairs[i].first || pairs[i - 1].second != pairs[i].second)
            {
                order++;
            }
            Pk[pairs[i].third] = order;
        }
        P.push_back(Pk);
        cout << k << endl;
        k++;
        //this is necessary to free memory so the program doesn't crash. Even so, with a 200mb file the program stagnates at 5.3gb memory usage.

        //P[P.size()-1].shrink_to_fit();
        //cout << k << endl;
    }

    return P;
}

int lcp(const vector<vector<int>>& P, int i, int j)
{
    int n = P[0].size();
    //_ASSERT(i < n && j < n);
    if (i == j)
    {
        return n - i;
    }
    else
    {
        int k = P.size() - 1;
        int lcp = 0;
        while (k >= 0 && i < n && j < n)
        {
            int l = pow(2, k);
            if (P[k][i] == P[k][j])
            {
                lcp += l;
                i += l;
                j += l;
            }
            k -= 1;
        }
        return lcp;
    }
}

void _fillRLlcp(const vector<vector<int>>& P, const vector<int>& sa, vector<int>& leftLcp, vector<int>& rightLcp, int l, int r)
{
    if ((r - l) <= 1)
    {
        return;
    }
    int h = (l + r) / 2;
    leftLcp[h] = lcp(P, sa[l], sa[h]);
    rightLcp[h] = lcp(P, sa[h], sa[r]);
    _fillRLlcp(P, sa, leftLcp, rightLcp, l, h);
    _fillRLlcp(P, sa, leftLcp, rightLcp, h, r);
}

void fillRLlcp(const vector<vector<int>>& P, const vector<int>& sa, vector<int>& leftLcp, vector<int>& rightLcp)
{
    int n = sa.size();
    _fillRLlcp(P, sa, leftLcp, rightLcp, 0, n - 1);
}

/*
//maybe use pointer to c_string instead of creating substring is faster?
bool lexicographicallyLessEqual(string_view x, string_view y, size_t n)
{
    size_t xLenCompare = min(x.length(), n);
    size_t yLenCompare = min(y.length(), n);
    return (x.substr(0, xLenCompare) <= y.substr(0, yLenCompare));
}

//maybe use pointer to c_string instead of creating substring is faster?
struct lexCompResult lexicographicallyCompare(string_view x, string_view y)
{
    int lcp = 0;
    uint32_t xLen = x.length();
    uint32_t yLen = y.length();

    const uint32_t minLengthXandY = min(xLen, yLen);

    for (uint32_t i = 0; i < minLengthXandY; i++)
    {
        if (x[i] == y[i])
        {
            lcp++;
        }
        else if (x[i] < y[i])
        {
            return { -1, lcp };
        }
        else
        {
            return { 1, lcp };
        }
    }

    if (xLen == yLen)
    {
        return { 0, lcp };
    }
    else if (xLen < yLen)
    {
        return { -1, lcp };
    }
    else
    {
        return { 1, lcp };
    }
}

int successor(const string& text, const string& pattern, int* sa, int* leftLcp, int* rightLcp)
{
    int m = pattern.length();
    int n = text.length();
    if (n == 0) return -1;
    struct lexCompResult firstComp = lexicographicallyCompare(pattern, string_view(text).substr(sa[0]));
    struct lexCompResult lastComp = lexicographicallyCompare(pattern, string_view(text).substr(sa[n - 1]));
    if (firstComp.cmpResult <= 0)
    {
        return 0;
    }
    else if (lastComp.cmpResult > 0)
    {
        return n;
    }
    else
    {
        int l = 0;
        int r = n - 1;
        int H = 0;
        while ((r - l) > 1)
        {
            int h = (l + r) / 2;
            if (firstComp.lcp >= lastComp.lcp)
            {
                if (firstComp.lcp <= leftLcp[h])
                {
                    struct lexCompResult tempResult = lexicographicallyCompare(string_view(pattern).substr(firstComp.lcp), string_view(text).substr(sa[h] + firstComp.lcp));
                    H = firstComp.lcp + tempResult.lcp;
                }
                else
                {
                    H = leftLcp[h];
                }
            }
            else
            {
                if (lastComp.lcp <= rightLcp[h])
                {
                    struct lexCompResult tempResult = lexicographicallyCompare(string_view(pattern).substr(lastComp.lcp), string_view(text).substr(sa[h] + lastComp.lcp));
                    H = lastComp.lcp + tempResult.lcp;
                }
                else
                {
                    H = rightLcp[h];
                }
            }
            if (H == m || pattern[H] <= text[sa[h] + H])
            {
                r = h;
                lastComp.lcp = H;
            }
            else
            {
                l = h;
                firstComp.lcp = H;
            }
        }
        return r;
    }
}

int predecessor(const string& text, const string& pattern, int* sa, int* leftLcp, int* rightLcp)
{
    int m = pattern.length();
    int n = text.length();
    if (n == 0) return -1;
    struct lexCompResult firstComp = lexicographicallyCompare(pattern, string_view(text).substr(sa[0]));
    struct lexCompResult lastComp = lexicographicallyCompare(pattern, string_view(text).substr(sa[n - 1]));
    if (firstComp.cmpResult < 0)
    {
        return -1;
    }
    else if (lastComp.cmpResult >= 0)
    {
        return n - 1;
    }
    else
    {
        int l = 0;
        int r = n - 1;
        int H = 0;
        while ((r - l) > 1)
        {
            int h = (l + r) / 2;
            if (firstComp.lcp >= lastComp.lcp)
            {
                if (firstComp.lcp <= leftLcp[h])
                {
                    struct lexCompResult tempResult = lexicographicallyCompare(string_view(pattern).substr(firstComp.lcp), string_view(text).substr(sa[h] + firstComp.lcp));
                    H = firstComp.lcp + tempResult.lcp;
                }
                else
                {
                    H = leftLcp[h];
                }
            }
            else
            {
                if (lastComp.lcp <= rightLcp[h])
                {
                    struct lexCompResult tempResult = lexicographicallyCompare(string_view(pattern).substr(lastComp.lcp), string_view(text).substr(sa[h] + lastComp.lcp));
                    H = lastComp.lcp + tempResult.lcp;
                }
                else
                {
                    H = rightLcp[h];
                }
            }
            if (H == m || pattern[H] > text[sa[h] + H])
            {
                l = h;
                firstComp.lcp = H;
            }
            else
            {
                r = h;
                lastComp.lcp = H;
            }
        }
        return l;
    }
}


vector<int> buildSarr(vector<int> P)
{
    int n = P.size();
    vector<int> sa;
    sa.resize(n);
    if (n == 1)
    {
        sa[0] = '\n';
        return sa;
    }

    for (int i = 0; i < n; i++)
    {

        sa[P[i]] = i;
    }
    return sa;
}

struct sliceIndexes sarrSearch(const string& text, const string& pattern, int* sa, int* leftLcp, int* rightLcp)
{
    int L = successor(text, pattern, sa, leftLcp, rightLcp);
    int R = predecessor(text, pattern, sa, leftLcp, rightLcp);

    if (L <= R)
    {
        return { L, R + 1 };
    }
    else
    {
        return { 0, 0 };
    }
}


*/
void saveIndexFile(string fileName) {

    FILE* textFile;
    if ((textFile = fopen(fileName.data(), "r")) == NULL) {
        cout << "couldn't open textFile\n" << endl;
        exit(1);
    }
    fseek(textFile, 0, SEEK_END);
    const size_t fileSize = ftell(textFile);
    rewind(textFile);
    uint8_t* str = (uint8_t*)malloc(sizeof(uint8_t) * fileSize);
    fread(str, sizeof(uint8_t), fileSize, textFile);
    fclose(textFile);


    /*vector<int> tempTextForBuildingSuffixArray;
    for (int i = 0; i < fileSize; i++)
    {
        tempTextForBuildingSuffixArray.push_back(str[i]);
    }*/

    string patt("hellish");
    uint8_t* pattern = (uint8_t*)malloc(sizeof(uint8_t) * 8);
    strcpy((char*)pattern, patt.data());

    struct StringRemapTable rt;
    createRemapTable(&rt, str);
    uint8_t* remapped = (uint8_t *)malloc(sizeof(uint8_t) * fileSize);

    remapString(remapped, str, &rt);
    size_t remappedStringSize = strlen((char*)remapped) + 1;
    uint8_t* remmapedPattern = (uint8_t*)malloc(sizeof(uint8_t) * 8);
    remapString(remmapedPattern, pattern, &rt);

    struct SuffixArray* newSa = buildSuffixArray(remapped, remappedStringSize, rt.alphabetSize);
    struct BwtTable bwtTable;
    createBwtTable(&bwtTable, newSa, &rt);


    /*struct Iterator iter;
    size_t pos;
    createIterator(&iter, &bwt, remmapedPattern);
    int count = 0;
    vector<uint32_t> positions;
    unordered_map<uint32_t, uint32_t> asd;
    while (findNextMatch(&iter, &pos))
    {
        uint32_t startPos = pos;
        int64_t finishPos = pos;
        while (str[startPos] != '\n' || startPos == 0) startPos--;
        while (str[finishPos] != '\n' || finishPos == (fileSize - 1)) finishPos++;
        fwrite(str + (startPos * sizeof(char)), sizeof(char), finishPos - startPos, stdout);
        printf("\n");
        count++;
    }
    cout << count << endl;*/

    

    FILE* saveFile;
    if ((saveFile = fopen((fileName + ".idx").data(), "wb")) == NULL)
    {
        cout << "failed to create textFile" << endl;
    }
    uint32_t count = 0;
    fwrite(&fileSize, sizeof(size_t), 1, saveFile);
    fwrite(&newSa->length, sizeof(size_t), 1, saveFile);
    fwrite(&bwtTable.characterTableSize, sizeof(size_t), 1, saveFile);
    fwrite(&bwtTable.oTableSize, sizeof(size_t), 1, saveFile);
    fwrite(&bwtTable.oIndicesSize, sizeof(size_t), 1, saveFile);

    count = fwrite(&bwtTable.stringRemapTable->alphabetSize, sizeof(uint8_t), 1, saveFile);
    if (count != 1)
    {
        printf("couldn't write suffixArray3");
    }
    count = fwrite(bwtTable.stringRemapTable->charTable, sizeof(char), 256, saveFile);
    if (count != 256)
    {
        printf("couldn't write suffixArray4");
    }
    count = fwrite(bwtTable.stringRemapTable->remappedCharTable, sizeof(char), 192, saveFile);
    if (count != 192)
    {
        printf("couldn't write suffixArray5");
    }


    fwrite(str, sizeof(uint8_t), fileSize, saveFile);
    count = fwrite(newSa->suffixArray, sizeof(uint32_t), bwtTable.suffixArray->length, saveFile);
    if (count != bwtTable.suffixArray->length)
    {
        printf("couldn't write suffixArray");
    }
    count = fwrite(bwtTable.characterTable, sizeof(uint32_t), bwtTable.characterTableSize, saveFile);
    if (count != bwtTable.characterTableSize)
    {
        printf("couldn't write suffixArray1");
    }
    count = fwrite(bwtTable.oTable, sizeof(uint32_t), bwtTable.oTableSize, saveFile);
    if (count != bwtTable.oTableSize)
    {
        printf("couldn't write suffixArray2");
    }
    

    fclose(saveFile);
    free(newSa->text);
    free(newSa->suffixArray);
    free(bwtTable.characterTable);
    free(bwtTable.oTable);
    free(bwtTable.oIndices);
    return;

   
    /*vector<int> sa;
    vector<vector<int>> P;
    const size_t textLen = text.length();
    P = buildP(text);
    //sa = buildSarr(P[P.size() - 1]);
    sa.insert(sa.begin(), text.length());

    for (int i = 0; i < P.size(); i++)
    {
        P[i].resize(0);
    }

    vector<uint32_t> cTable(128, 0);
    uint32_t* oTable;
    uint32_t** oIndices;

    const uint32_t oTableSize = 128 * (sa.size());
    oTable = (uint32_t*)malloc(sizeof(uint32_t) * oTableSize);
    if (!oTable)
    {
        cout << "couldn't allocate otable" << endl;
    }

    const uint32_t oIndicesSize = sa.size();
    oIndices = (uint32_t**)malloc(sizeof(uint32_t*) * oIndicesSize);
    if (!oIndices)
    {
        cout << "couldn't allocate oIndices" << endl;
    }*/
/*
    uint32_t L;
    uint32_t R;
    int64_t i;
    uint32_t pos;

    string pattern = "lorem";

    createBwtTable(text, sa, cTable, &oTable, &oIndices);
    initBwtIterators(&L, &R, &i, text, pattern, sa, cTable, oTable, oIndices);
    uint32_t count = 0;
    while (nextBwtMatch(&L, &R, &i, &pos, sa))
    {
        assert(pattern == text.substr(pos, pattern.length()));
        count++;
    }
    
    cout << count << endl;*/


    /*for (int i = 0; i < sa.size(); i++)
    {
        assert(sa[i] == newsa[i]);
    }

    vector<int> leftLcp(textLen, -1);
    vector<int> rightLcp(textLen, -1);
    fillRLlcp(P, sa, leftLcp, rightLcp);*/

    /*;*/
}

void searchPattern(string fileName, vector<string> patterns, bool isCount) 
{
    cout << (fileName).data() << endl;
    FILE* indexFile;
    if ((indexFile = fopen((fileName).data(), "rb")) == NULL) {
        cout << "couldn't open textFile\n" << endl;
        exit(1);
    }

    size_t strLen;
    size_t suffixArrayLen;
    size_t bwtCharacterTableSize;
    size_t bwtOTableSize;
    size_t bwtOIndicesSize;
    fread(&strLen, sizeof(size_t), 1, indexFile);
    fread(&suffixArrayLen, sizeof(size_t), 1, indexFile);
    fread(&bwtCharacterTableSize, sizeof(size_t), 1, indexFile);
    fread(&bwtOTableSize, sizeof(size_t), 1, indexFile);
    fread(&bwtOIndicesSize, sizeof(size_t), 1, indexFile);


    uint8_t* str = (uint8_t*)malloc(sizeof(uint8_t) * strLen);
    BwtTable bwtTable;
    bwtTable.characterTable = (uint32_t*)malloc(sizeof(uint32_t) * bwtCharacterTableSize);
    bwtTable.oTable = (uint32_t*)malloc(sizeof(uint32_t) * bwtOTableSize);
    bwtTable.oIndices = (uint32_t**)malloc(sizeof(uint32_t*) * bwtOIndicesSize);
    StringRemapTable rt;
    bwtTable.stringRemapTable = &rt;
    struct SuffixArray* suffixArray = (struct SuffixArray*)malloc(sizeof(struct SuffixArray));
    suffixArray->suffixArray = (uint32_t*)malloc(sizeof(uint32_t) * suffixArrayLen);
    suffixArray->length = suffixArrayLen;

    fread(&bwtTable.stringRemapTable->alphabetSize, sizeof(uint8_t), 1, indexFile);
    fread(bwtTable.stringRemapTable->charTable, sizeof(char), 256, indexFile);
    fread(bwtTable.stringRemapTable->remappedCharTable, sizeof(char), 192, indexFile);


    fread(str, sizeof(uint8_t), strLen, indexFile);
    suffixArrayLen;
    fread(suffixArray->suffixArray, sizeof(uint32_t), suffixArray->length, indexFile);

    fread(bwtTable.characterTable, sizeof(uint32_t), bwtCharacterTableSize, indexFile);
    fread(bwtTable.oTable, sizeof(uint32_t), bwtOTableSize, indexFile);
    

    fclose(indexFile);
    bwtTable.suffixArray = suffixArray;
    recreateOIndices(&bwtTable);


    for(string pattern : patterns)
    {
        if(pattern.size() == 0) continue;
        auto start = std::chrono::high_resolution_clock::now();

        uint8_t* remmapedPattern = (uint8_t*)malloc(sizeof(uint8_t) * pattern.length());
        remapString(remmapedPattern, (const uint8_t*) pattern.data(), &rt);

        struct Iterator iter;
        size_t pos;

        createIterator(&iter, &bwtTable, remmapedPattern);
        int count = 0;
        while (findNextMatch(&iter, &pos))
        {
            uint32_t startPos = pos;
            int64_t finishPos = pos;
            if(!isCount)
            {
                while (str[startPos] != '\n' || startPos == 0) startPos--;
                while (str[finishPos] != '\n' || finishPos == (strLen - 1)) finishPos++;
                fwrite(str + (startPos * sizeof(char)), sizeof(char), finishPos - startPos, stdout);
                printf("\n");
            }
            else
            {
                count++;
            }            
        }

        if(isCount) cout << count << endl;

        auto end = std::chrono::high_resolution_clock::now();
        auto int_s = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "search elapsed time is " << int_s.count() << " microseconds )" << std::endl;
        free(remmapedPattern);
    }
    

    free(suffixArray->suffixArray);
    free(bwtTable.characterTable);
    free(bwtTable.oTable);
    free(bwtTable.oIndices);
    return;

    

    /*fseek(indexFile, 0 ,SEEK_END);
    size_t fileSize = ftell(indexFile);
    rewind(indexFile);*/
    /*size_t textLen;
    fread(&textLen, sizeof(size_t), 1, indexFile);
    size_t suffixArrayLength;
    fread(&suffixArrayLength, sizeof(size_t), 1, indexFile);
    size_t lcpVectorsLength;
    fread(&lcpVectorsLength, sizeof(size_t), 1, indexFile);
    char* str = (char*)malloc(sizeof(char) * textLen);
    fread(str, sizeof(char), textLen, indexFile);
    int* sa = (int*)malloc(sizeof(int) * suffixArrayLength);
    fread(sa, sizeof(int), suffixArrayLength, indexFile);
    int* Llcp = (int*)malloc(sizeof(int) * lcpVectorsLength);
    fread(Llcp, sizeof(int), lcpVectorsLength, indexFile);
    int* Rlcp = (int*)malloc(sizeof(int) * lcpVectorsLength);
    fread(Rlcp, sizeof(int), lcpVectorsLength, indexFile);
    fclose(indexFile);


    string text(str);
    string pattern = "hellish";*/

    

    //struct sliceIndexes occ = sarrSearch(text, pattern, sa, Llcp, Rlcp);
    //cout << occ.finish - occ.start << endl;

    
    /*free(sa);
    free(Llcp);
    free(Rlcp);*/


}

int suffixArray()
{

    //saveIndexFile();
    //onlyOneLine();
    /*
    //string text = "aeaeabd";
    //string ab = "abcde";
    //11107
    string pattern = "lor";
    vector<string> text = readStringFromFile("english.50MB");
    string ab = R"( !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~¡¢£€¥Š§š©ª«¬­®¯°±²³Žµ¶·ž¹º»ŒœŸ¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ)";
    ab = '\n' + ab + '\222';
    unordered_map<int, char> mappedChars = mapCharacters(ab);
    //unordered_map<int, int> mappedInt = mapCharactersReverse();
    vector<vector<int>> sa;
    vector<vector<vector<int>>> P;
    int count = 0;
    int patternLength = pattern.length();

    for (int i = 0; i < text.size(); i++)
    {
        vector<vector<int>> Pl = buildP(text[i], mappedChars);;
        vector<int> sal = buildSarr(Pl[Pl.size()-1]);
        P.push_back(Pl);
        sa.push_back(sal);
    }*/


    /*vector<int> datavec;

    ifstream infile;
    infile.open ("datafile.bin", ios::in | ios::binary);

    while (infile) {
        int val;
        infile.read(reinterpret_cast<char *>(&val), sizeof(int));
        if (infile.bad()) {
            throw std::runtime_error("Failed to read from infile!");
        }
        if (infile.eof()) break;
        datavec.push_back(val);
    }

    vector<int> ps;*/
    /*const uint32_t textLen = text[i].length();
            if (textLen >= patternLength)
            {


                vector<int> leftLcp(textLen, -1);
                vector<int> rightLcp(textLen, -1);
                fillRLlcp(P, sa, leftLcp, rightLcp);
                vector<int> occ = sarrSearch(text[i], pattern, sa, leftLcp, rightLcp);
                for(int j = 0; j < occ.size(); j++)
                {
                    assert(pattern == text[i].substr(occ[j], patternLength));
                }
                count += occ.size();
            }*/

            /*for (int i = 0; i < sa.size(); i++)
            {
                if (sa[i].size() >= patternLength)
                {
                    const uint32_t textLen = text[i].length();
                    vector<int> leftLcp(textLen, -1);
                    vector<int> rightLcp(textLen, -1);
                    fillRLlcp(P[i], sa[i], leftLcp, rightLcp);
                    vector<int> occ = sarrSearch(text[i], pattern, sa[i], leftLcp, rightLcp);
                    count += occ.size();
                }
            }
            cout << count << endl;*/




            //cout << count << endl;
            /*for(int i = 0; i < ps.size(); i++)
            {
                if( (find(datavec.begin(), datavec.end(), ps[i]) != datavec.end()) )
                {

                }
                else
                {
                    cout << ps[i] << endl;
                }
            }*/


    return 0;
}

