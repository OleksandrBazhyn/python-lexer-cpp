import webbrowser
import telebot

TOKEN = '7111111111111111111111JJ111111111111111111111111111111111'

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

ii = 2.11
x = 0x111ab
a = 17
o = 0x11abj