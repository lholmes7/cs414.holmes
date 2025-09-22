// Landon Holmes
// CS414
// Assignment3.2.ccp

#include <iostream>
#include <string>
#include <vector>
#include <cctype>

using namespace std;

enum class TokenType {
    CMD,        
    NAME,       // a folder name
    DOT,        
    BACKSLASH,  
    FILENAME,   // name.ext should satisfy 8.3
    EOF_TOK,    // end of file token
    ERROR       
};

static string token_type_to_string(TokenType t) {
    switch (t) {
    case TokenType::CMD: return "CMD";
    case TokenType::NAME: return "NAME";
    case TokenType::DOT: return "DOT";
    case TokenType::BACKSLASH: return "BACKSLASH";
    case TokenType::FILENAME: return "FILENAME";
    case TokenType::EOF_TOK: return "EOF";
    case TokenType::ERROR: return "ERROR";
    }
    return "UNKNOWN";
}

struct Token {
    TokenType type;
    string lexeme;
    bool valid;

    Token(TokenType t, const string& s, bool v = true)
        : type(t), lexeme(s), valid(v) {
    }
};

static bool is_letter(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

static string lower(const string& s) {
    string out;
    out.reserve(s.size());
    for (char c : s) out.push_back(static_cast<char>(tolower((unsigned char)c))); //convert to lowercase
    return out;
}

// DOS 8.3 check
static bool is_valid_dos_83(const string& name_part, const string& ext_part) {
    if (name_part.empty() || name_part.size() > 8) return false;
    if (ext_part.empty() || ext_part.size() > 3) return false;
    for (char c : name_part) if (!is_letter(c)) return false;
    for (char c : ext_part) if (!is_letter(c)) return false;
    return true;
}

vector<Token> tokenize(const string& input) {               // asked chatgpt for assistance in tokenizer
    vector<Token> tokens;
    size_t n = input.size();
    size_t i = 0;

    auto skip_spaces = [&]() {
        while (i < n && isspace((unsigned char)input[i])) ++i;
        };

    skip_spaces();
    while (i < n) {
        char c = input[i];

        if (c == '\\') {
            tokens.emplace_back(TokenType::BACKSLASH, "\\");
            ++i;
            skip_spaces();
            continue;
        }

        if (c == '.') {
            tokens.emplace_back(TokenType::DOT, ".");
            ++i;
            skip_spaces();
            continue;
        }

        if (is_letter(c)) {
            size_t start = i;
            while (i < n && is_letter(input[i])) ++i;
            string word = input.substr(start, i - start);

            char next = (i < n ? input[i] : '\0');

            // Detect possible commands
            string lw = lower(word);
            bool maybe_cmd = false;
            if (lw == "ls" || lw == "cd" || lw == "cat" || lw == "print" || lw == "exec") {
                if (next == '\0' || isspace((unsigned char)next) || next == '\\' || next == '.') {
                    maybe_cmd = true;
                }
            }
            if (maybe_cmd) {
                tokens.emplace_back(TokenType::CMD, lw); 
                skip_spaces();
                continue;
            }

            // parse a filename like NAME.EXT
            if (i < n && input[i] == '.') {
                size_t dot_pos = i;
                size_t ext_start = i + 1;
                size_t j = ext_start;
                while (j < n && is_letter(input[j])) ++j;
                if (ext_start < n && j > ext_start) {
                    string name_part = word;                   
                    string ext_part = input.substr(ext_start, j - ext_start);
                    string full = name_part + "." + ext_part;
                    bool valid = is_valid_dos_83(name_part, ext_part);
                    tokens.emplace_back(TokenType::FILENAME, full, valid);
                    i = j; 
                    skip_spaces();
                    continue;
                }
            }

            tokens.emplace_back(TokenType::NAME, word);
            skip_spaces();
            continue;
        }

        // non whitespace = ERROR
        string bad(1, c);
        tokens.emplace_back(TokenType::ERROR, bad, false);
        ++i;
        skip_spaces();
    }

    tokens.emplace_back(TokenType::EOF_TOK, "");
    return tokens;
}

// Token print helper
void print_tokens(const vector<Token>& toks) {
    for (const auto& t : toks) {
        cout << token_type_to_string(t.type) << "    ";
        if (!t.lexeme.empty()) cout << "'" << t.lexeme << "'    ";
        if (t.type == TokenType::FILENAME) {
            cout << (t.valid ? "(valid 8.3 syntax)" : "(troublesome 8.3 syntax)");
        }
        cout << "\n";
    }
}

// main test
int main() {
    vector<string> tests = {
        "ls",
        "ls README.TXT",
        "cd",
        "cd \\",                      // root
        "cd \\FOLDER",                // go to folder
        "cd \\F1\\SUB",               // note: F1 contains digit -> NAME token allowed but folder validity not checked here
        "cat NOTES.TXT",
        "print DOC.TXT",
        "exec GAME.EXE",
        "ls TERRIBLENAME.TOO_LONGEXT",     // invalid ext length
        "RUN$ BAD"                    // invalid char
    };

    for (const auto& cmd : tests) {
        cout << "--- Input = " << cmd << " ---\n";
        auto toks = tokenize(cmd);
        print_tokens(toks);
        cout << "\n";
    }

    return 0;
}