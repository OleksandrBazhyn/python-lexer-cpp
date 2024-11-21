#include <iostream>
#include <string>
#include <cctype>
#include <map>
#include <vector>
#include <fstream>
#include <filesystem>

enum TokenType {
    NUMBER, STRING, IDENTIFIER, COMMENT, RESERVED, OPERATOR, PUNCTUATION, ERROR, END
};

struct Token {
    std::string lexeme;
    TokenType type;
};

class Lexer {
private:
    std::string input;
    size_t pos;
    std::map<std::string, TokenType> reservedWords = {
        {"if", RESERVED}, {"else", RESERVED}, {"while", RESERVED}, {"return", RESERVED}, {"def", RESERVED}, {"import", RESERVED}, {"from", RESERVED}
    };

    void skipWhitespace() {
        while (pos < input.length() && isspace(input[pos])) {
            pos++;
        }
    }

    Token recognizeNumber() {
        size_t start = pos;

        // Перевіряємо префікс для 16-ткової системи
        if (input[pos] == '0' && pos + 1 < input.length() && tolower(input[pos + 1]) == 'x') {
            pos += 2; // Пропускаємо '0x'
            bool hasHexDigits = false;

            // Перевіряємо, що після префікса є лише шістнадцяткові цифри
            while (pos < input.length() && isxdigit(input[pos])) {
                hasHexDigits = true;
                pos++;
            }

            // Якщо після '0x' не було цифр або зустріли недопустимий символ
            if (!hasHexDigits || (pos < input.length() && isalnum(input[pos]))) {
                // Переміщуємо позицію до кінця лексеми, щоб пропустити всю помилкову частину
                while (pos < input.length() && isalnum(input[pos])) {
                    pos++;
                }
                return { input.substr(start, pos - start), ERROR };
            }

            return { input.substr(start, pos - start), NUMBER };
        }

        // Якщо це звичайне десяткове число
        while (pos < input.length() && isdigit(input[pos])) {
            pos++;
        }

        // Перевіряємо на дробову частину
        if (pos < input.length() && input[pos] == '.') {
            pos++;
            while (pos < input.length() && isdigit(input[pos])) {
                pos++;
            }
        }

        // Перевіряємо, чи після числа немає недійсного символу
        if (pos < input.length() && isalnum(input[pos])) {
            // Пропускаємо залишкові недійсні символи
            while (pos < input.length() && isalnum(input[pos])) {
                pos++;
            }
            return { input.substr(start, pos - start), ERROR };
        }

        return { input.substr(start, pos - start), NUMBER };
    }

    Token recognizeString() {
        char quote = input[pos];
        size_t start = pos;
        pos++;
        bool escaped = false;

        while (pos < input.length()) {
            char current = input[pos];

            if (current == '\\' && !escaped) {
                escaped = true;
                pos++;
                continue;
            }

            if (current == quote && !escaped) {
                pos++;
                break;
            }

            escaped = false;
            pos++;
        }

        return { input.substr(start, pos - start), STRING };
    }

    Token recognizeIdentifier() {
        size_t start = pos;
        while (pos < input.length() && (isalnum(input[pos]) || input[pos] == '_' || input[pos] == '.')) {
            pos++;
        }
        std::string lexeme = input.substr(start, pos - start);
        if (reservedWords.count(lexeme)) {
            return { lexeme, reservedWords[lexeme] };
        }
        return { lexeme, IDENTIFIER };
    }

    Token recognizeComment() {
        size_t start = pos;
        while (pos < input.length() && input[pos] != '\n') {
            pos++;
        }
        return { input.substr(start, pos - start), COMMENT };
    }

    Token recognizeOperatorOrPunctuation() {
        char current = input[pos++];
        return { std::string(1, current), OPERATOR };
    }

public:
    Lexer(const std::string& text) : input(text), pos(0) {}

    Token getNextToken() {
        skipWhitespace();
        if (pos >= input.length()) return { "", END };

        char current = input[pos];

        if (isdigit(current)) {
            return recognizeNumber();
        }
        if (current == '"' || current == '\'') {
            return recognizeString();
        }
        if (isalpha(current) || current == '_') {
            return recognizeIdentifier();
        }
        if (current == '#') {
            return recognizeComment();
        }
        if (ispunct(current)) {
            return recognizeOperatorOrPunctuation();
        }

        Token errorToken = { std::string(1, current), ERROR };
        pos++;
        return errorToken;
    }

    size_t getPosition() const {
        return pos;
    }
};

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case NUMBER: return "NUMBER";
        case STRING: return "STRING";
        case IDENTIFIER: return "IDENTIFIER";
        case COMMENT: return "COMMENT";
        case RESERVED: return "RESERVED";
        case OPERATOR: return "OPERATOR";
        case PUNCTUATION: return "PUNCTUATION";
        case ERROR: return "ERROR";
        case END: return "END";
        default: return "UNKNOWN";
    }
}

std::string readPythonFile(const std::string& filePath) {
    std::string code;
    std::ifstream file(filePath);
    if (!file.is_open()){
        std::cerr << "Error: Could not open the file " << filePath << std::endl;
        return "";
    }

    std::string line;
    while (std::getline(file, line)) {
        code += line + "\n";
    }
    
    file.close();
    return code;
}

void writeToFile(const std::string& filename, const std::string& content) {
    std::filesystem::path dir = std::filesystem::path(filename).parent_path();

    if (!std::filesystem::exists(dir)) {
        std::filesystem::create_directories(dir);
    }

    std::ofstream outFile(filename, std::ios::out);
    if (!outFile) {
        std::cerr << "Error: Could not open or create the file " << filename << " for writing." << std::endl;
        return;
    }
    outFile << content;
    outFile.close();
}

void processCode(const std::string& code) {
    Lexer lexer(code);
    Token token;
    std::string result;
    while ((token = lexer.getNextToken()).type != END) {
        if (token.type == ERROR) {
            std::cerr << "Error: Unrecognized token '" << token.lexeme << "' at position " << lexer.getPosition() << std::endl;
            result += "Error: Unrecognized token '" + token.lexeme + "' at position " + std::to_string(lexer.getPosition()) + "\n";
        } else {
            std::string output = "<" + token.lexeme + ", " + tokenTypeToString(token.type) + ">\n";
            std::cout << output;
            result += output;
        }
    }

    writeToFile("../result/output.txt", result);
}

int main(int argc, char* argv[]) {
    std::string filePath;

    if (argc < 2) {
        filePath = "../python-code-for-analysis/script.py";
    } else {
        filePath = argv[1];
    }

    std::string code = readPythonFile(filePath);

    if (!code.empty()) {
        processCode(code);
    } else {
        std::cerr << "Error: No code to process." << std::endl;
    }

    return 0;
}