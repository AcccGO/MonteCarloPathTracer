#include "parse_string.h"

using namespace std;

ParseString::ParseString(string str)
{
    this->str   = str;
    this->index = 0;
}

ParseString::ParseString()
{
}

ParseString::~ParseString()
{
}

void ParseString::init(string str)
{
    this->str   = str;
    this->index = 0;
}

void ParseString::SkipDelimiters()
{
    int len = this->str.size();
    int i;
    for (i = this->index; i < len; ++i) {
        char c = this->str.at(i);
        if (c == '\t' || c == ' ' || c == '(' || c == ')' || c == '"')
            continue;
        break;
    }
    this->index = i;
}

void ParseString::SkipToNextWord()
{
    this->SkipDelimiters();
    int n = GetWordLength(this->str, this->index);
    this->index += (n + 1);
}

string ParseString::GetWord()
{
    this->SkipDelimiters();
    int n = GetWordLength(this->str, this->index);
    if (n == 0)
        return "";
    string word = this->str.substr(this->index, n);
    this->index += (n + 1);

    return word;
}

int ParseString::GetInt()
{
    int i;
    i = atoi(this->GetWord().c_str());
    return i;
}

double ParseString::GetFloat()
{
    double f;
    f = atof(this->GetWord().c_str());
    return f;
}

int GetWordLength(const string &str, int start)
{
    int n   = 0;
    int len = str.size();
    int i;
    for (i = start; i < len; i++) {
        char c = str.at(i);
        if (c == '\t' || c == ' ' || c == '(' || c == ')' || c == '"')
            break;
    }
    return i - start;
}

vector<string> Split(const string &s, const string &seperator)
{
    vector<string>    result;
    string::size_type i = 0;

    while (i != s.size()) {
        int flag = 0;
        while (i != s.size() && flag == 0) {
            flag = 1;
            for (string::size_type x = 0; x < seperator.size(); x++)
                if (s[i] == seperator[x]) {
                    ++i;
                    flag = 0;
                    break;
                }
        }

        flag                = 0;
        string::size_type j = i;
        while (j != s.size() && flag == 0) {
            for (string::size_type x = 0; x < seperator.size(); x++)
                if (s[j] == seperator[x]) {
                    flag = 1;
                    break;
                }
            if (flag == 0)
                ++j;
        }
        if (i != j) {
            result.push_back(s.substr(i, j - i));
            i = j;
        }
    }
    return result;
}
