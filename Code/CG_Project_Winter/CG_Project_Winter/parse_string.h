#pragma once
#include <string>
#include <vector>

using namespace std;

int GetWordLength(const string &str, int start);

vector<string> Split(const string &s, const string &seperator);

class ParseString
{
private:
    string str;
    int    index;

public:
    ParseString(string str);
    ParseString();
    ~ParseString();

    void   init(string str);
    void   SkipDelimiters();
    void   SkipToNextWord();
    string GetWord();
    int    GetInt();
    double GetFloat();
};
