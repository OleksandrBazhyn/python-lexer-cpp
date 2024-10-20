#include <iostream>
#include <string>
#include <cctype>
#include <map>
#include <vector>

enum TokenType {
	NUMBER, STRING, IDENTIFIER, COMMENT, RESERVED, OPERATOR, PUNCTUATION, ERROR, END // в Python немає препроцесорних директив
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
		{"if", RESERVED}, {"else", RESERVED}, {"while", RESERVED}, {"return", RESERVED}
	};

	void skipWhitespace() {
		while (pos < input.length() && isspace(input[pos])) {
			pos++;
		}
	}

	Token recognizeNumber() {
		size_t start = pos;
		while (pos < input.length() && isdigit(input[pos])) {
			pos++;
		}
		if (pos < input.length() && input[pos] == '.') {
			pos++;
			while (pos < input.length() && isdigit(input[pos])) {
				pos++;
			}
		}
		return { input.substr(start, pos - start), NUMBER };
	}

	Token recognizeString() {
		char quote = input[pos];
		size_t start = pos;
		pos++;
		while (pos < input.length() && input[pos] != quote) {
			pos++;
		}
		pos++; // Пропустити закриваючу лапку
		return { input.substr(start, pos - start), STRING };
	}

	Token recognizeIdentifier() {
		size_t start = pos;
		while (pos < input.length() && (isalnum(input[pos]) || input[pos] == '_')) {
			pos++;
		}
		std::string lexeme = input.substr(start, pos - start);
		if (reservedWords.count(lexeme)) {
			return { lexeme, RESERVED };
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
		if (isalpha(current || current == '_')) {
			return recognizeIdentifier();
		}
		if (current == '#') {
			return recognizeComment();
		}
		if (ispunct(current)) {
			return recognizeOperatorOrPunctuation();
		}
		return { std::string(1, current), ERROR };
	}
};

int main() {
	std::string code = R"(
		def func(a,b):
			# Some comment from me
			if a > b:
				return a
			else:
				return b
	)";

	Lexer lexer(code);
	Token token;
	while ((token = lexer.getNextToken()).type != END) {
		std::cout << "<" << token.lexeme << ", " << token.type << ">" << std::endl;
	}

	return 0;
}