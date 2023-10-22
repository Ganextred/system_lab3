#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <unordered_map>
#include <winnls.h>


using namespace std;

enum class TokenType {
    Number,
    StringLiteral,
    PreprocessorDirective,
    Comment,
    ReservedWord,
    Operator,
    Punctuation,
    Identifier,
    Error,
    NewLine,
    Space
};

struct Token {
    enum TokenType type;
    string value;
};

const vector<pair<regex, enum TokenType>> token_patterns = {
        {regex(R"(\b\d+(\.\d+)?\b)"),                                                                                          TokenType::Number},
        {regex(R"(\"[^\"]*\")"),                                                                                               TokenType::StringLiteral},
        {regex(R"(#[^\s]+)"),                                                                                                  TokenType::PreprocessorDirective},
        {regex(R"(\/\/[^\n]*|\/\*[\s\S]*\*\/)"),                                                                               TokenType::Comment},
        {regex(R"(\b(class|int|float|string|using|namespace|return|if|else|while|for|continue|break|switch|case|default)\b)"), TokenType::ReservedWord},
        {regex(R"((%|\+\+|\+|\-|\/|\*|\<|\>|=|&|\||\^|\?))"),                                                                  TokenType::Operator},
        {regex(R"(([\.,;:\{\}\[\]\(\)]))"),                                                                                    TokenType::Punctuation},
        {regex(R"(\b[_a-zA-Z][\w]*)"),                                                                                         TokenType::Identifier},
        {regex(R"(\n)"),                                                                                                       TokenType::NewLine},
        {regex(R"( )"),                                                                                                        TokenType::Space}

};

vector<Token> lex(const string &input) {
    vector<Token> tokens;
    smatch match;

    size_t start = 0;
    while (start < input.size()) {
        bool matched = false;
        for (const auto &[pattern, type]: token_patterns) {
            if (regex_search(input.begin() + start, input.end(), match, pattern, regex_constants::match_continuous)) {
                tokens.push_back({type, match.str()});
                start += match.length();
                matched = true;
                break;
            }
        }

        if (!matched) {
            if (input[start] == ' ') {
                ++start;
            } else {
                tokens.push_back({TokenType::Error, string(1, input[start])});
                ++start;
            }
        }
    }

    return tokens;
}

string type_color(const enum TokenType type) {
    switch (type) {
        case TokenType::Number:
            return "\033[33m"; // Red
        case TokenType::StringLiteral:
            return "\033[32m"; // Green
        case TokenType::PreprocessorDirective:
            return "\033[33m"; // Yellow
        case TokenType::Comment:
            return "\033[34m"; // Blue
        case TokenType::ReservedWord:
            return "\033[35m"; // Magenta
        case TokenType::Operator:
            return "\033[36m"; // Cyan
        case TokenType::Punctuation:
            return "\033[31m"; // White
        case TokenType::Identifier:
            return "\033[0m"; // Reset
        case TokenType::Error:
            return "\033[41m"; // Red background
        default:
            return "";
    }
}

string token_type_name(enum TokenType type) {
    switch (type) {
        case TokenType::Number:
            return "Number";
        case TokenType::StringLiteral:
            return "StringLiteral";
        case TokenType::PreprocessorDirective:
            return "PreprocessorDirective";
        case TokenType::Comment:
            return "Comment";
        case TokenType::ReservedWord:
            return "ReservedWord";
        case TokenType::Operator:
            return "Operator";
        case TokenType::Punctuation:
            return "Punctuation";
        case TokenType::Identifier:
            return "Identifier";
        case TokenType::Space:
            return "Space";
        case TokenType::NewLine:
            return "NewLine";
        default:
            return "Unknown";
    }
}


int main() {
    system(("chcp "s + to_string(CP_UTF8)).c_str());

    ifstream inputFile("input.txt");

    string content;

    if (inputFile.is_open()) {
        string line;
        while (getline(inputFile, line)) {
            content += line + '\n';
        }
        inputFile.close();
    } else {
        cerr << "Unable to open input.txt" << endl;
        return 1;
    }

    const vector<Token> tokens = lex(content);

    for (const Token &token: tokens) {
        cout << type_color(token.type) << token.value << "\033[0m";
    }
    cout << endl;


    unordered_map<enum TokenType, vector<string>> token_tables;

    for (const Token &token: tokens) {
        token_tables[token.type].push_back(token.value);
    }

    for (const auto &[type, values]: token_tables) {
        cout << token_type_name(type) << " tokens (count: " << values.size() << "):\n";
        for (const string &value: values) {
            if (type != TokenType::Space and type != TokenType::NewLine)
                cout << "  " << value << " ";
        }
        cout << "\n";
    }


    return 0;
}