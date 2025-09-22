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
    NAME,       
    DOT,        
    BACKSLASH,  
    FILENAME,   
    EOF_TOK,    
    ERROR       
};

struct Token {
    TokenType type;
    string lexeme; // raw input text (had to research in StackOverflow to find something like this.)
    bool valid;

    Token(TokenType t = TokenType::ERROR, const string& s = "", bool v = true)
        : type(t), lexeme(s), valid(v) {
    }
};

static bool is_letter(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

static string lower(const string& s) {
    string out;
    out.reserve(s.size());
    for (char c : s) out.push_back(static_cast<char>(tolower((unsigned char)c)));
    return out;
}

static bool is_valid_dos_83(const string& name_part, const string& ext_part) {
    if (name_part.empty() || name_part.size() > 8) return false;
    if (ext_part.empty() || ext_part.size() > 3) return false;
    for (char c : name_part) if (!is_letter(c)) return false;
    for (char c : ext_part) if (!is_letter(c)) return false;
    return true;
}

vector<Token> tokenize(const string& input) {
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

            if (i < n && input[i] == '.') {
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

        string bad(1, c);
        tokens.emplace_back(TokenType::ERROR, bad, false);
        ++i;
        skip_spaces();
    }

    tokens.emplace_back(TokenType::EOF_TOK, "");
    return tokens;
}

// Definitions
struct Filename {
    string full;   // like "README.TXT"
    string name;   // pre dot
    string ext;    // post dot
    bool valid83;  // does it satisfy 8.3
};

// Command 
struct Command {
    enum Type { LS, CD, CAT, PRINT, EXEC } type;

    bool has_filename;
    Filename filename;

    bool cd_root;
    vector<string> cd_parts; 

    Command() : has_filename(false), cd_root(false) {}
};

struct Parser {
    const vector<Token>& toks;
    size_t i;
    string error;

    Parser(const vector<Token>& tokens) : toks(tokens), i(0), error("") {}

    const Token& cur() const { return toks[i]; }
    void advance() { if (i + 1 < toks.size()) ++i; }

    bool accept_type(TokenType tt) {
        if (cur().type == tt) { advance(); return true; }
        return false;
    }

    bool expect_type(TokenType tt, const string& msg) {
        if (cur().type == tt) { advance(); return true; }
        error = msg;
        return false;
    }

    // Parse command
    bool parse_command(Command& cmd_out) {
        error.clear();

        if (cur().type != TokenType::CMD) {
            error = "Expected command (ls|cd|cat|print|exec)";
            return false;
        }

        string c = cur().lexeme; // lowercase
        advance();

        if (c == "ls") {
            cmd_out.type = Command::LS;
            if (cur().type == TokenType::FILENAME) {
                parse_filename_into(cmd_out.filename);
                cmd_out.has_filename = true;
                advance();
            }
            else {
                cmd_out.has_filename = false;
            }
            if (cur().type != TokenType::EOF_TOK) {
                error = "Unexpected token after ls command";
                return false;
            }
            return true;
        }

        if (c == "cd") {
            cmd_out.type = Command::CD;
            // default to root
            if (cur().type == TokenType::EOF_TOK) {
                cmd_out.cd_root = true;
                cmd_out.cd_parts.clear();
                return true;
            }

            if (cur().type == TokenType::BACKSLASH) {
                advance();
                if (cur().type == TokenType::EOF_TOK) {
                    cmd_out.cd_root = true;
                    return true;
                }
                // expect one name if not root
                if (cur().type != TokenType::NAME) {
                    error = "Expected folder name after '\\' in path";
                    return false;
                }
                cmd_out.cd_parts.clear();
                while (cur().type == TokenType::NAME) {
                    string nm = cur().lexeme;
                    // validate name length
                    bool ok = true;
                    if (nm.empty() || nm.size() > 8) ok = false;
                    for (char ch : nm) if (!is_letter(ch)) ok = false;
                    if (!ok) {
                        error = "Folder name '" + nm + "' invalid (must be 1..8 letters)";
                        return false;
                    }
                    cmd_out.cd_parts.push_back(nm);
                    advance();

                    if (cur().type == TokenType::BACKSLASH) {
                        advance(); 
                        if (cur().type != TokenType::NAME) {
                            error = "Expected folder name after '\\'";
                            return false;
                        }
                        continue;
                    }
                    else {
                        break;
                    }
                }
                if (cur().type != TokenType::EOF_TOK) {
                    error = "Unexpected token after folder path";
                    return false;
                }
                cmd_out.cd_root = false;
                return true;
            }
            else {
                error = "cd argument must start with '\\' or be empty (cd to root)";
                return false;
            }
        }

        if (c == "cat" || c == "print" || c == "exec") {
            if (c == "cat") cmd_out.type = Command::CAT;
            if (c == "print") cmd_out.type = Command::PRINT;
            if (c == "exec") cmd_out.type = Command::EXEC;

            //filename not found
            if (cur().type != TokenType::FILENAME) {
                error = string("Command '") + c + "' requires a filename argument";
                return false;
            }
            parse_filename_into(cmd_out.filename);
            cmd_out.has_filename = true;
            advance();

            if (cur().type != TokenType::EOF_TOK) {
                error = "Unexpected token after filename";
                return false;
            }
            return true;
        }

        error = "Unknown command";
        return false;
    }

    // helper: fill Filename object from current token (must be FILENAME)
    void parse_filename_into(Filename& f) {
        if (cur().type == TokenType::FILENAME) {
            string s = cur().lexeme;
            size_t p = s.find('.');
            if (p == string::npos) {
                f.full = s;
                f.name = s;
                f.ext = "";
                f.valid83 = false;
            }
            else {
                f.full = s;
                f.name = s.substr(0, p);
                f.ext = s.substr(p + 1);
                // redundancy
                f.valid83 = cur().valid;
            }
        }
        else {
            f.full = "";
            f.name = "";
            f.ext = "";
            f.valid83 = false;
        }
    }
};

//Printing AST
void print_command_ast(const Command& cmd) {
    switch (cmd.type) {
    case Command::LS:
        cout << "AST: LS";
        if (cmd.has_filename) {
            cout << " filename='" << cmd.filename.full << "'";
            cout << (cmd.filename.valid83 ? " valid 8.3" : " Troublesome 8.3");
        }
        else {
            cout << " (no argument)";
        }
        cout << "\n";
        break;
    case Command::CD:
        cout << "AST: CD ";
        if (cmd.cd_root) {
            cout << "(root) '\\'\n";
        }
        else {
            cout << "path = ";
            for (size_t j = 0; j < cmd.cd_parts.size(); ++j) {
                if (j) cout << "\\";
                cout << cmd.cd_parts[j];
            }
            cout << "\n";
        }
        break;
    case Command::CAT:
        cout << "AST: CAT filename='" << cmd.filename.full << "'";
        cout << (cmd.filename.valid83 ? " valid 8.3" : " (Troublesome 8.3") << "\n";
        break;
    case Command::PRINT:
        cout << "AST: PRINT filename='" << cmd.filename.full << "'";
        cout << (cmd.filename.valid83 ? " valid 8.3" : " (Troublesome 8.3") << "\n";
        break;
    case Command::EXEC:
        cout << "AST: EXEC filename='" << cmd.filename.full << "'";
        cout << (cmd.filename.valid83 ? " valid 8.3" : " Troublesome 8.3") << "\n";
        break;
    default:
        cout << "AST: <unknown>\n";
    }
}

// Test Main

int main() {
    vector<string> tests = {
        "ls",
        "ls README.TXT",
        "cd",
        "cd \\",                      
        "cd \\FOLDER",                
        "cd \\F1\\SUB",               
        "cat NOTES.TXT",
        "print DOC.TXT",
        "exec GAME.EXE",
        "ls TERRIBLENAME.TOOLONGEXT",      
        "cd \\TOO_LONG_FOLDER_NAME",  
        "unknowncmd arg"
    };

    for (const auto& cmdline : tests) {
        cout << "--- Input = " << cmdline << " ---\n";
        vector<Token> toks = tokenize(cmdline);

        Parser p(toks);
        Command ast;
        bool ok = p.parse_command(ast);
        if (!ok) {
            cout << "Parse error: " << p.error << "\n\n";
        }
        else {
            print_command_ast(ast);
            cout << "\n";
        }
    }

    return 0;
}