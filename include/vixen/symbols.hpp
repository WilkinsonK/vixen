#pragma once
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string.h>

using namespace std;

#define MAX_SIZET (size_t)(-1)

typedef std::string Symbol;
typedef uint Column;
typedef uint Lineno;
#define TRIPLET(T) std::tuple<Lineno, Column, T>

namespace vixen::symbols
{
    bool char_in_string(const char ch, const std::string &str)
    {
        return str.find(ch) != MAX_SIZET;
    }

    bool char_iscomment(const char ch)
    {
        return char_in_string(ch, "#");
    }

    bool char_isdigitchar(const char ch)
    {
        return char_in_string(ch, "1234567890");
    }

    bool char_isdigitext(const char ch)
    {
        return char_in_string(
            ch,
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "1234567890"
            "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~");
    }

    bool char_isdigitsep(const char ch)
    {
        return char_in_string(ch, "-_.bdxo");
    }

    bool char_isgroupchar(const char ch)
    {
        return char_in_string(ch, ")}][{(");
    }

    bool char_isnamechar(const char ch)
    {
        return char_in_string(
            ch,
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "1234567890"
            "_");
    }

    bool char_isnewline(const char ch)
    {
        return char_in_string(ch, "\n");
    }

    bool char_isnoparse(const char ch)
    {
        return char_in_string(ch, " \t\n\r\v\f");
    }

    bool char_ispuncchar(const char ch)
    {
        return !char_isnamechar(ch);
    }

    bool char_isstrchar(const char ch)
    {
        return char_in_string(ch, "'`\"");
    }

    bool char_istermchar(const char ch)
    {
        return char_in_string(ch, ";");
    }

    bool symbol_isname(const Symbol &symbol)
    {
        if (symbol.length() == 0)
            return false;
        if (char_isdigitchar(symbol[0]))
            return false;

        bool (*validator)(char) = [](char ch)
        {
            return char_isnamechar(ch) || char_isdigitchar(ch);
        };
        char upper, lower;

        for (int i = 0; i < std::ceil(symbol.length() / 2); ++i)
        {
            upper = symbol[i];
            lower = symbol[symbol.length() - (i + 1)]; // +1 to skip over '\0'
            if (!(validator(upper) && validator(lower)))
                return false;
        }

        return true;
    }

    bool symbol_isnumeric(const Symbol &symbol)
    {
        bool result = false;

        // Byte array might be numeric if  any chars
        // in the sequence are digits.
        for (char ch : symbol)
            result += char_isdigitchar(ch);
        if (!result)
            return result;

        uint base_notation = 0;
        // Byte array is numeric if there is not mixing
        // of floating point ('.') char and special
        // base notations (0x, 0d, 0b, 0o).
        for (char ch : "xdbo")
        {
            if (char_in_string(ch, symbol) && char_in_string('.', symbol))
                return false;
            if (char_in_string(ch, symbol))
                base_notation = (uint)ch;
        }

        // Byte array is numeric if only digits are
        // found in the sequence.
        for (const char &ch : symbol)
        {
            // If parsing for base 10, binary or octal.
            if (base_notation == 0 || base_notation == 98 || base_notation == 111)
            {
                if (!char_in_string(ch, "1234567890-_.xdbo"))
                    return false;
                // If parsing hex or some other base.
            }
            else
            {
                if (!char_isdigitext(ch))
                    return false;
            }
        }

        // I hate this so much about C++.
        return (std::ranges::count(symbol.begin(), symbol.end(), '.') < 2);
    }

    bool symbol_ispunc(const Symbol &symbol)
    {
        if (symbol.length() == 0)
            return false;

        char upper, lower;
        for (int i = 0; i < std::ceil(symbol.length() / 2); ++i)
        {
            upper = symbol[i];
            lower = symbol[symbol.length() - (i + 1)];
            if (char_isnamechar(upper) || char_isnamechar(lower))
                return false;
        }
        return true;
    }

    bool symbol_isstrsym(const Symbol &symbol)
    {
        std::string stringsyms[] = {"'", "'''", "`", "```", "\"", "\"\"\""};

        if (symbol.length() < 1)
            return false;

        for (std::string sym : stringsyms)
            if (sym == symbol)
                return true;

        return false;
    }

    bool symbol_istermed(const Symbol &symbol, const char next)
    {
        return symbol.length() > 0 && !char_in_string(';', symbol) && char_istermchar(next);
    }

    bool symbol_next_isvalidname(const Symbol &symbol, const char next)
    {
        return symbol_isname(symbol) && !char_ispuncchar(next);
    }

    bool symbol_next_isvalidnum(const Symbol &symbol, const char next)
    {
        // Some exceptions are made in the event
        // that the first char of a symbol might
        // be a '.' or the numeric might be signed
        // '-'.
        if (
            !symbol_isnumeric(symbol) && symbol.compare(".") != 0 && symbol.compare("-") != 0)
            return false;

        // If symbol has floating point ('.').
        if (char_in_string('.', symbol))
        {
            // Next character should not be
            // punctuation.
            return !char_ispuncchar(next);
        }

        return char_isdigitchar(next) || char_isdigitsep(next) || !char_ispuncchar(next);
    }

    bool symbol_next_isvalidpunc(const Symbol &symbol, const char next)
    {
        return !(symbol_ispunc(symbol) && char_isnamechar(next));
    }

    template <typename T>
    // Parses a buffer of data into symbols that are
    // usable for token parsing.
    class SymbolParser
    {
    public:
        // Read head is at the end of data stream or
        // not.
        virtual bool end() = 0;
        // Character at read head.
        virtual char head() = 0;
        // Parse next symbol.
        virtual T next() = 0;
    };

    // Populates a class definition with empty
    // constructors that inherit directly from the
    // parent `BasicSymbolParser`.
    #define BasicSymbolParser__init__(CLASS_NAME, RT)                  \
        CLASS_NAME() : BasicSymbolParser<RT>() {}                      \
        CLASS_NAME(std::string &data) : BasicSymbolParser<RT>(data) {} \
        CLASS_NAME(std::ifstream &file, const std::string &filename = "") : BasicSymbolParser<RT>(file, filename) {}

    // Parses generic symbols into a tuple of metadata
    // `(line_number, start_column, symbol)`.
    template <typename T>
    class BasicSymbolParser : public SymbolParser<T>
    {
    protected:
        std::string data;
        uint dimension_line;
        std::string file;
        uint last_line_at;
        uint read_head;
        bool string_parsing;
        std::string symbol_ribbon[3];

    public:
        BasicSymbolParser() {}

        BasicSymbolParser(const std::string data)
        {
            this->data = data;
            this->dimension_line = 1;
            this->last_line_at = 0;
            this->read_head = 0;
            this->string_parsing = false;
        }

        BasicSymbolParser(std::ifstream &file, const std::string &filename = "")
        {
            this->file = filename;
            std::string buf;
            while (std::getline(file, buf))
                this->data.append(buf + "\n");

            this->dimension_line = 1;
            this->last_line_at = 0;
            this->read_head = 0;
            this->string_parsing = false;
        }

        // Last symbol parsed by this parser.
        const std::string last_symbol()
        {
            return this->symbol_ribbon[2];
        }

        // Move the read head forward.
        void advance()
        {
            if (char_isnewline(this->head()))
            {
                this->dimension_line++;
                this->last_line_at = this->read_head;
            }
            this->read_head++;
        }

        // Move the read head to next valid
        // non-whitespace character.
        void advancew()
        {
            if (this->string_parsing)
                return;
            while (char_isnoparse(this->head()) && !this->end())
                this->advance();
        }

        // Move the read head to next valid
        // non-comment character.
        void advancec()
        {
            if (this->string_parsing)
                return;
            while (char_iscomment(this->head()))
            {
                // Comments cannot exist inline
                // with code. The end of comments
                // are determined based on the end
                // of line or EOF.
                while (!char_isnewline(this->head()) && !this->end())
                    this->advance();

                // Second pass ensures additional
                // whitespace after a comment is
                // eliminated.
                this->advancew();
            }
        }

        // Current column position relative to
        // last newline.
        uint column()
        {
            return this->read_head - this->last_line_at;
        }

        bool end()
        {
            return this->read_head >= this->data.length();
        }

        char head()
        {
            if (this->end())
                return this->data[this->data.length() - 1];
            return this->data[this->read_head];
        }

        // The current line number.
        uint lineno()
        {
            return this->dimension_line;
        }

        // Get a 'slice' of `head` length from
        // data stream relative to read head.
        std::string lookahead(uint head)
        {
            return this->data.substr(this->read_head, head);
        }

        // The last symbol parsed is equal to the
        // lookahead slice.
        bool lookahead_matchlast()
        {
            std::string last = this->last_symbol();
            std::string look = this->lookahead(last.length());
            return look == last;
        }

        TRIPLET(Symbol)
        next_raw()
        {
            // Advance past all whitespace and chars
            // considered invalid for parsing.
            // This includes characters after a comment
            // char (#) and newline chars.
            this->advancew();
            this->advancec();

            std::string symbol;
            TRIPLET(std::string)
            token;

            if (this->end())
            {
                if (this->lineno() > 1)
                    symbol = "EOF";
                else
                    symbol = "EOL";
                return {this->lineno(), this->column(), symbol};
            }

            if (this->string_parsing)
            {
                token = this->next_punc();
            }
            else if (char_isnamechar(this->head()) && !char_isdigitchar(this->head()))
            {
                token = this->next_name();
            }
            else if (char_isdigitchar(this->head()))
            {
                token = this->next_numeric();
            }
            else if (char_isdigitsep(this->head()) && char_isdigitchar(this->lookahead(2)[1]))
            {
                token = this->next_numeric();
            }
            else
            {
                token = this->next_punc();
            }

            this->symbol_ribbon[0] = this->symbol_ribbon[1];
            this->symbol_ribbon[1] = this->symbol_ribbon[2];
            this->symbol_ribbon[2] = std::get<2>(token);

            return token;
        }

        // Parse next name symbol.
        TRIPLET(Symbol)
        next_name()
        {
            std::string symbol("");
            uint column = this->column();
            uint lineno = this->lineno();
            char head = this->head();

            while (1)
            {
                symbol += head;
                this->advance();

                head = this->head();
                if (char_isnoparse(head))
                    break;
                if (char_iscomment(head))
                    break;
                if (char_istermchar(head))
                    break;
                // Punctuation cannot exist in a name.
                // Unless said name is numeric, then
                // '.' is accepted.
                if (!symbol_next_isvalidname(symbol, head))
                    break;
                if (this->end())
                    break;
            }

            return {lineno, column, symbol};
        }

        // Parse next numeric symbol.
        TRIPLET(Symbol)
        next_numeric()
        {
            std::string symbol("");
            uint column = this->column();
            uint lineno = this->lineno();
            char head = this->head();

            while (1)
            {
                symbol += head;
                this->advance();

                head = this->head();
                if (char_isnoparse(head))
                    break;
                if (char_iscomment(head))
                    break;
                if (char_istermchar(head))
                    break;
                if (!symbol_next_isvalidnum(symbol, head))
                    break;
                if (this->end())
                    break;
            }

            return {lineno, column, symbol};
        }

        // Parse next punctuation symbol.
        TRIPLET(Symbol)
        next_punc()
        {
            std::string symbol("");
            uint column = this->column();
            uint lineno = this->lineno();
            char head = this->head();

            while (1)
            {
                symbol += head;
                this->advance();

                head = this->head();
                if (!this->string_parsing)
                {
                    if (char_isnoparse(head))
                        break;
                    if (char_iscomment(head))
                        break;
                    if (char_isgroupchar(head))
                        break;
                    if (char_istermchar(head))
                        break;

                    // Gratuitious most likely.
                    if (char_isgroupchar(symbol[0]))
                        break;

                    // Names cannot exist in puncutation.
                    if (!symbol_next_isvalidpunc(symbol, head))
                        break;

                    // String parsing has most likely
                    // started. Valid string tokens
                    // must not contain non-string
                    // notation chars.
                    if (symbol_isstrsym(symbol) && !char_isstrchar(head))
                        break;

                    if (char_istermchar((char)symbol[0]))
                        break;

                    // If string parsing, ignore whatever
                    // character might come next as it is
                    // most likely being escaped.
                }
                else if ('\\' == symbol.back())
                    continue;

                // If future char sequence is the
                // same as the openening string
                // sequence.
                else if (this->lookahead_matchlast())
                    break;

                // If the symbol matches the
                // openening string sequence.
                else if (this->symbol_ribbon[1] == symbol && symbol_isstrsym(this->symbol_ribbon[1]))
                    break;

                if (this->end())
                    break;
            }

            if (symbol_isstrsym(symbol))
                this->string_parsing = !(this->string_parsing);

            return {lineno, column, symbol};
        }

        // Read-only shortcut to if string parsing
        // or not.
        bool string_mode() {
            return this->string_parsing;
        }
    };

    // Parses raw symbol metadata from an input.
    class RawParser : public BasicSymbolParser<TRIPLET(Symbol)>
    {
    public:
        BasicSymbolParser__init__(RawParser, TRIPLET(Symbol))

        TRIPLET(Symbol) next() {
            return BasicSymbolParser<TRIPLET(Symbol)>::next_raw();
        }
    };
};
