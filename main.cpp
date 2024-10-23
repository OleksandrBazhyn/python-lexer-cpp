#include <iostream>
#include <string>
#include <cctype>
#include <map>
#include <vector>

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
        while (pos < input.length() && isxdigit(input[pos])) {
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

int main() {
    std::string code = R"(
    import — webbrowser
    import telebot

    TOKEN = '7012456183:AAHqzxkqLGx2qyZvnRxpTSHlMvQTPwE3Hb8'

    bot = telebot.TeleBot(TOKEN)

    # Message Handlers
    # Commands
    @bot.message_handler(commands=['start'])
    def start(message): 
        markup = telebot.types.InlineKeyboardMarkup()
        markup.add(telebot.types.InlineKeyboardButton(text='Зв\'язатись з оператором', callback_data='operator'))
        markup.add(telebot.types.InlineKeyboardButton(text='Перейти на сайт', url='https://www.hive.report/'))
        bot.send_message(message.chat.id, f'<em>Вітаю, {message.from_user.first_name}!</em>\nБудь ласка оберіть тип Вашого звернення', parse_mode='html', reply_markup=markup)

    @bot.message_handler(commands=['site'])
    def website(message):
        bot.send_message(message.chat.id, 'https://www.hive.report')
        webbrowser.open('https://www.hive.report')

    # Files
    @bot.message_handler(content_types=['photo'])
    def get_photo(message):
        bot.send_message(message.chat.id, 'Дякую за знімок!')

    # User Message
    @bot.message_handler()
    def hello(message):
        if message.text.lower() == 'привіт':
            bot.send_message(message.chat.id, f'Привіт, {message.from_user.first_name}!')

        elif message.text.lower() == 'пока':
            bot.send_message(message.chat.id, f'Гарного дня, {message.from_user.first_name}!')


    print("Bot is Started.")
    bot.infinity_polling()
    )";

    Lexer lexer(code);
    Token token;
    while ((token = lexer.getNextToken()).type != END) {
        if (token.type == ERROR) {
            std::cerr << "Error: Unrecognized token '" << token.lexeme << "' at position " << lexer.getPosition() << std::endl;
        } else {
            std::cout << "<" << token.lexeme << ", " << tokenTypeToString(token.type) << ">" << std::endl;
        }
    }

    std::cin;

    return 0;
}
