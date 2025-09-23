// Landon Holmes
// A00116756
//Assignment4.2

#include <iostream>
#include <string>
#include <vector>
#include <cctype>

using namespace std;

// definitions
enum class TokenType {
    CMD,        // ls, cd, cat, print, exec, set, echo
    NAME,       // folder name
    DOT,        
    BACKSLASH,  
    FILENAME,   
    VAR,        // $IDENT 
    NUMBER,     
    OP,         // + - * /
    LPAREN,     // (
    RPAREN,     // )
    EQUAL,      
    EOF_TOK,
    ERROR
};

struct Token {
    TokenType type;
    string lexeme;
    bool valid83;

    Token(TokenType t = TokenType::ERROR, const string& s = "", bool v = true)
        : type(t), lexeme(s), valid83(v) {
    }
};

// util
static bool is_letter(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}
static bool is_digit(char c) {
    return (c >= '0' && c <= '9');
}
static string lower(const string& s) {
    string out; out.reserve(s.size());
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

// tokenizer
vector<Token> tokenize(const string& input) {
    vector<Token> toks;
    size_t n = input.size();
    size_t i = 0;

    auto skip_spaces = [&]() {
        while (i < n && isspace((unsigned char)input[i])) ++i;
        };

    skip_spaces();
    while (i < n) {
        char c = input[i];

        // single tokens
        if (c == '\\') { toks.emplace_back(TokenType::BACKSLASH, "\\"); ++i; skip_spaces(); continue; }
        if (c == '.') { toks.emplace_back(TokenType::DOT, "."); ++i; skip_spaces(); continue; }
        if (c == '(') { toks.emplace_back(TokenType::LPAREN, "("); ++i; skip_spaces(); continue; }
        if (c == ')') { toks.emplace_back(TokenType::RPAREN, ")"); ++i; skip_spaces(); continue; }
        if (c == '=') { toks.emplace_back(TokenType::EQUAL, "="); ++i; skip_spaces(); continue; }
        if (c == '+' || c == '-' || c == '*' || c == '/') {
            string s(1, c); toks.emplace_back(TokenType::OP, s); ++i; skip_spaces(); continue;
        }

        // $ variable
        if (c == '$') {
            size_t start = i;
            ++i; 
            size_t j = i;
            while (j < n && (is_letter(input[j]) || is_digit(input[j]))) ++j;
            if (j == i) {
                // nothing after $ error
                toks.emplace_back(TokenType::ERROR, "$");
                skip_spaces();
                continue;
            }
            string lex = input.substr(start, j - start); 
            toks.emplace_back(TokenType::VAR, lex);
            i = j;
            skip_spaces();
            continue;
        }

        // number
        if (is_digit(c)) {
            size_t start = i;
            while (i < n && is_digit(input[i])) ++i;
            toks.emplace_back(TokenType::NUMBER, input.substr(start, i - start));
            skip_spaces();
            continue;
        }

        // letters run
        if (is_letter(c)) {
            size_t start = i;
            while (i < n && is_letter(input[i])) ++i;
            string word = input.substr(start, i - start);
            char next = (i < n ? input[i] : '\0');

            // check commands
            string lw = lower(word);
            bool is_cmd = false;
            if (lw == "ls" || lw == "cd" || lw == "cat" || lw == "print" || lw == "exec" || lw == "set" || lw == "echo") {
                if (next == '\0' || isspace((unsigned char)next) || next == '\\' || next == '.' || next == '$' || next == '(') {
                    is_cmd = true;
                }
            }
            if (is_cmd) {
                toks.emplace_back(TokenType::CMD, lw);
                skip_spaces();
                continue;
            }

            // parse filename
            if (i < n && input[i] == '.') {
                size_t ext_start = i + 1;
                size_t j = ext_start;
                while (j < n && is_letter(input[j])) ++j;
                if (ext_start < n && j > ext_start) {
                    string name_part = word;
                    string ext_part = input.substr(ext_start, j - ext_start);
                    string full = name_part + "." + ext_part;
                    bool valid = is_valid_dos_83(name_part, ext_part);
                    toks.emplace_back(TokenType::FILENAME, full, valid);
                    i = j;
                    skip_spaces();
                    continue;
                }
            }

            // else plain NAME
            toks.emplace_back(TokenType::NAME, word);
            skip_spaces();
            continue;
        }

        // strange char = ERROR token
        string bad(1, c);
        toks.emplace_back(TokenType::ERROR, bad);
        ++i;
        skip_spaces();
    }

    toks.emplace_back(TokenType::EOF_TOK, "");
    return toks;
}

// symbol table (Looked up what this is and how to build it on stack overflow)
static string sym_lookup(const vector<pair<string, string>>& symtab, const string& name) {
    for (size_t i = 0; i < symtab.size(); ++i) {
        if (symtab[i].first == name) return symtab[i].second;
    }
    return ""; 
}
static void sym_set(vector<pair<string, string>>& symtab, const string& name, const string& value) {
    for (size_t i = 0; i < symtab.size(); ++i) {
        if (symtab[i].first == name) { symtab[i].second = value; return; }
    }
    symtab.push_back(make_pair(name, value));
}
static void print_symtab(const vector<pair<string, string>>& symtab) {
    cout << "Symbol table (" << symtab.size() << " entries):\n";
    for (size_t i = 0; i < symtab.size(); ++i) {
        cout << "  " << symtab[i].first << " = \"" << symtab[i].second << "\"\n";
    }
}

// command represents
struct Filename {
    string full;
    bool valid83;
};
struct Command {
    enum Type { LS, CD, CAT, PRINT, EXEC, SET, ECHO } type;
    bool has_filename;
    Filename filename;    // ls/cat/print/exec
    bool cd_root;
    vector<string> cd_parts; // cd
    string var_name;       // set/echo/$variables
    string set_expr;       
};

// Parser (Used chatGPT for help here
struct Parser {
    const vector<Token>& toks;
    size_t i;
    string error;
    vector<pair<string, string>>& symtab; 

    Parser(const vector<Token>& tokens, vector<pair<string, string>>& st)
        : toks(tokens), i(0), error(""), symtab(st) {
    }

    const Token& cur() const { return toks[i]; }
    void advance() { if (i + 1 < toks.size()) ++i; }

    bool accept(TokenType tt) {
        if (cur().type == tt) { advance(); return true; }
        return false;
    }
    bool expect(TokenType tt, const string& msg) {
        if (cur().type == tt) { advance(); return true; }
        error = msg;
        return false;
    }

    bool parse(Command& out) {
        error.clear();
        if (cur().type != TokenType::CMD) { error = "Expected command"; return false; }
        string cmd = cur().lexeme; 
        advance();

        if (cmd == "ls") return parse_ls(out);
        if (cmd == "cd") return parse_cd(out);
        if (cmd == "cat") return parse_cat_print_exec(out, Command::CAT);
        if (cmd == "print") return parse_cat_print_exec(out, Command::PRINT);
        if (cmd == "exec") return parse_cat_print_exec(out, Command::EXEC);
        if (cmd == "set") return parse_set(out);
        if (cmd == "echo") return parse_echo(out);

        error = "Unknown command '" + cmd + "'";
        return false;
    }

    bool parse_ls(Command& out) {
        out.type = Command::LS;
        out.has_filename = false;
        out.cd_root = false;
        if (cur().type == TokenType::FILENAME) {
            out.has_filename = true;
            out.filename.full = cur().lexeme;
            out.filename.valid83 = cur().valid83;
            advance();
        }
        else if (cur().type == TokenType::VAR) {
            out.has_filename = true;
            out.filename.full = cur().lexeme;
            out.filename.valid83 = false;
            advance();
        }

        if (cur().type != TokenType::EOF_TOK) {
            error = "Unexpected token after ls";
            return false;
        }
        return true;
    }

    bool parse_cd(Command& out) {
        out.type = Command::CD;
        out.cd_parts.clear();
        out.cd_root = false;

        // root default
        if (cur().type == TokenType::EOF_TOK) { out.cd_root = true; return true; }

        // var becomes argument
        if (cur().type == TokenType::VAR) {
            out.cd_root = false;
            out.cd_parts.clear();
            out.cd_parts.push_back(cur().lexeme); 
            advance();
            if (cur().type != TokenType::EOF_TOK) { error = "Unexpected token after cd variable"; return false; }
            return true;
        }

        // else BACKSLASH is needed
        if (!accept(TokenType::BACKSLASH)) { error = "cd argument must start with '\\' or be empty"; return false; }
        if (cur().type == TokenType::EOF_TOK) { out.cd_root = true; return true; }

        // needs NAME or VAR
        while (true) {
            if (cur().type == TokenType::NAME) {
                string nm = cur().lexeme;
                // validate
                if (nm.empty() || nm.size() > 8) { error = "Folder name length invalid"; return false; }
                for (char ch : nm) if (!is_letter(ch)) { error = "Folder name contains invalid char"; return false; }
                out.cd_parts.push_back(nm);
                advance();
            }
            else if (cur().type == TokenType::VAR) {
                out.cd_parts.push_back(cur().lexeme);
                advance();
            }
            else {
                error = "Expected folder name or variable after '\\'";
                return false;
            }

            if (cur().type == TokenType::BACKSLASH) { advance(); continue; }
            else break;
        }

        if (cur().type != TokenType::EOF_TOK) { error = "Unexpected token after cd path"; return false; }
        return true;
    }

    bool parse_cat_print_exec(Command& out, Command::Type t) {
        out.type = t;
        out.has_filename = false;
        // require filename
        if (cur().type == TokenType::FILENAME) {
            out.has_filename = true;
            out.filename.full = cur().lexeme;
            out.filename.valid83 = cur().valid83;
            advance();
        }
        else if (cur().type == TokenType::VAR) {
            out.has_filename = true;
            out.filename.full = cur().lexeme;
            out.filename.valid83 = false;
            advance();
        }
        else {
            error = "Expected filename or variable";
            return false;
        }
        if (cur().type != TokenType::EOF_TOK) { error = "Unexpected token after filename"; return false; }
        return true;
    }

    // $VAR = Expr
    bool parse_set(Command& out) {
        out.type = Command::SET;
        out.var_name = "";
        out.set_expr = "";

        if (cur().type != TokenType::VAR) { error = "SET requires a variable ($name)"; return false; }
        out.var_name = cur().lexeme;
        advance();

        if (!expect(TokenType::EQUAL, "Expected '=' after variable in set")) return false;

        // parse expression and text
        string expr_text;
        if (!parse_expr(expr_text)) return false;
        if (cur().type != TokenType::EOF_TOK) { error = "Unexpected token after expression in set"; return false; }
        out.set_expr = expr_text;
        sym_set(symtab, out.var_name, out.set_expr);

        return true;
    }

    bool parse_echo(Command& out) {
        out.type = Command::ECHO;
        if (cur().type != TokenType::VAR) { error = "echo requires a variable ($name)"; return false; }
        out.var_name = cur().lexeme;
        advance();
        if (cur().type != TokenType::EOF_TOK) { error = "Unexpected token after echo"; return false; }
        return true;
    }

    bool parse_expr(string& out) {
        out.clear();
        string tstr;
        if (!parse_term(tstr)) return false;
        out += tstr;
        while (cur().type == TokenType::OP && (cur().lexeme == "+" || cur().lexeme == "-")) {
            string op = cur().lexeme; advance();
            string rhs;
            if (!parse_term(rhs)) return false;
            out += op;
            out += rhs;
        }
        return true;
    }

    bool parse_term(string& out) {
        out.clear();
        string fstr;
        if (!parse_factor(fstr)) return false;
        out += fstr;
        while (cur().type == TokenType::OP && (cur().lexeme == "*" || cur().lexeme == "/")) {
            string op = cur().lexeme; advance();
            string rhs;
            if (!parse_factor(rhs)) return false;
            out += op;
            out += rhs;
        }
        return true;
    }

    bool parse_factor(string& out) {
        out.clear();
        if (cur().type == TokenType::NUMBER) {
            out = cur().lexeme; advance(); return true;
        }
        if (cur().type == TokenType::VAR) {
            out = cur().lexeme; advance(); return true;
        }
        if (cur().type == TokenType::LPAREN) {
            out.push_back('(');
            advance();
            string inner;
            if (!parse_expr(inner)) return false;
            out += inner;
            if (!accept(TokenType::RPAREN)) { error = "Expected ')'"; return false; }
            out.push_back(')');
            return true;
        }
        error = "Expected number, variable, or parenthesized expression";
        return false;
    }
};

// print AST
void print_command_ast(const Command& c) {
    switch (c.type) {
    case Command::LS:
        cout << "AST: LS";
        if (c.has_filename) cout << " arg=" << c.filename.full;
        else cout << " (no arg)";
        cout << "\n";
        break;
    case Command::CD:
        cout << "AST: CD ";
        if (c.cd_root) cout << "(root)";
        else {
            cout << "path=";
            for (size_t i = 0; i < c.cd_parts.size(); ++i) {
                if (i) cout << "\\";
                cout << c.cd_parts[i];
            }
        }
        cout << "\n";
        break;
    case Command::CAT:
        cout << "AST: CAT file=" << c.filename.full << "\n"; break;
    case Command::PRINT:
        cout << "AST: PRINT file=" << c.filename.full << "\n"; break;
    case Command::EXEC:
        cout << "AST: EXEC file=" << c.filename.full << "\n"; break;
    case Command::SET:
        cout << "AST: SET " << c.var_name << " = " << c.set_expr << "\n"; break;
    case Command::ECHO:
        cout << "AST: ECHO " << c.var_name << "\n"; break;
    }
}

// test main
int main() {
    vector<pair<string, string>> symtab;

    vector<string> tests = {
        "set $X = 123",
        "set $Y = (1+2)*3",
        "echo $X",
        "ls",
        "ls $X",
        "cat README.TXT",
        "set $F = README.TXT",
        "ls $F",
        "cd \\MYDIR\\SUB",
        "cd $F",
        "exec $F",
        "set $Z = $X + 5",
        "echo $Z",
        "unknown cmd"
    };

    for (const string& line : tests) {
        cout << "--- Input = " << line << " ---\n";
        vector<Token> toks = tokenize(line);

        Parser p(toks, symtab);
        Command cmd;
        bool ok = p.parse(cmd);
        if (!ok) {
            cout << "Error: " << p.error << "\n\n";
        }
        else {
            print_command_ast(cmd);
            // Print table
            print_symtab(symtab);
            cout << "\n";
        }
    }

    return 0;
}