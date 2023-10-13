#pragma once
#include "symbols.hpp"

namespace vixen::tokens {
    enum class TokenType : uint {
        Error,
        ErrorUnknown,
        ErrorBadString,

        Kwd,
        KwdAs,
        KwdBreak,
        KwdCatch,
        KwdContinue,
        KwdConstant,
        KwdClass,
        KwdDefault,
        KwdDelete,
        KwdElse,
        KwdFor,
        KwdFrom,
        KwdFunc,
        KwdIf,
        KwdImport,
        KwdInclude,
        KwdNew,
        KwdNil,
        KwdNull,
        KwdPanic,
        KwdProto,
        KwdRaise,
        KwdReturn,
        KwdStatic,
        KwdTry,
        KwdWhile,
        KwdWith,

        Name,
        NameGeneric,

        Num,
        NumBin,
        NumFlt,
        NumHex,
        NumInt,
        NumOct,

        Oper,
        OperAddressOf,
        OperAsk,
        OperAssign,
        OperBtAnd,
        OperBtOr,
        OperBtXor,
        OperDecrement,
        OperDelete,
        OperDivide,
        OperDivFloor,
        OperIncrement,
        OperLgAnd,
        OperLgNot,
        OperLgOr,
        OperLgGt,
        OperLgGte,
        OperLgLt,
        OperLgLte,
        OperMinus,
        OperMinusEq,
        OperModulus,
        OperPlus,
        OperPlusEq,
        OperPower,
        OperPtrAttr,
        OperStamp,
        OperStar,

        Punc,
        PuncColon,
        PuncComma,
        PuncDot,
        PuncLBrace,
        PuncLBracket,
        PuncLParen,
        PuncRBrace,
        PuncRBracket,
        PuncRParen,
        PuncTerminator,

        Str,
        StrSingleBkt,
        StrSingleDbl,
        StrSingleSgl,
        StrTripleBkt,
        StrTripleDbl,
        StrTripleSgl,
        StrExpression,

        CTRLChar,
        CTRLCharEOF,
        CTRLCharEOL
    };

    // Unlike the prototype, we are only mapping
    // enumerations that that we can know to be
    // predictable. Such as punctuation, supported
    // operators and keywords.
    // All other token types can be implicitly
    // discerned based on context after parse time.
    static std::unordered_map<std::string, TokenType> TokenTypeMapping = {
        {"<keyword>", TokenType::Kwd},
        {"as", TokenType::KwdAs},
        {"break", TokenType::KwdBreak},
        {"catch", TokenType::KwdCatch},
        {"continue", TokenType::KwdContinue},
        {"const", TokenType::KwdConstant},
        {"class", TokenType::KwdClass},
        {"default", TokenType::KwdDefault},
        {"delete", TokenType::KwdDelete},
        {"else", TokenType::KwdElse},
        {"for", TokenType::KwdFor},
        {"from", TokenType::KwdFrom},
        {"func", TokenType::KwdFunc},
        {"if", TokenType::KwdIf},
        {"import", TokenType::KwdImport},
        {"include", TokenType::KwdInclude},
        {"new", TokenType::KwdNew},
        {"nil", TokenType::KwdNil},
        {"null", TokenType::KwdNull},
        {"panic", TokenType::KwdPanic},
        {"proto", TokenType::KwdProto},
        {"raise", TokenType::KwdRaise},
        {"return", TokenType::KwdReturn},
        {"static", TokenType::KwdStatic},
        {"try", TokenType::KwdTry},
        {"while", TokenType::KwdWhile},
        {"with", TokenType::KwdWith},

        {"<operation>", TokenType::Oper},
        {"?", TokenType::OperAsk},
        {"=", TokenType::OperAssign},
        {"&", TokenType::OperBtAnd},
        {"|", TokenType::OperBtOr},
        {"^", TokenType::OperBtXor},
        {"--", TokenType::OperDecrement},
        {"~", TokenType::OperDelete},
        {"/", TokenType::OperDivide},
        {"//", TokenType::OperDivFloor},
        {"++", TokenType::OperIncrement},
        {"&&", TokenType::OperLgAnd},
        {"!", TokenType::OperLgNot},
        {"||", TokenType::OperLgOr},
        {">", TokenType::OperLgGt},
        {">=", TokenType::OperLgGte},
        {"<", TokenType::OperLgLt},
        {"<=", TokenType::OperLgLte},
        {"-", TokenType::OperMinus},
        {"-=", TokenType::OperMinusEq},
        {"%", TokenType::OperModulus},
        {"+", TokenType::OperPlus},
        {"+=", TokenType::OperPlusEq},
        {"**", TokenType::OperPower},
        {"->", TokenType::OperPtrAttr},
        {"@", TokenType::OperStamp},
        {"*", TokenType::OperStar},

        // You gotta ask yourself one question,
        // 'Am I feelin' lucky?'
        {"<punctuation>", TokenType::Punc},
        {":", TokenType::PuncColon},
        {",", TokenType::PuncComma},
        {".", TokenType::PuncDot},
        {"{", TokenType::PuncLBrace},
        {"[", TokenType::PuncLBracket},
        {"(", TokenType::PuncLParen},
        {"}", TokenType::PuncRBrace},
        {"]", TokenType::PuncRBracket},
        {")", TokenType::PuncRParen},
        {";", TokenType::PuncTerminator},

        {"<string>", TokenType::Str},
        {"`", TokenType::StrSingleBkt},
        {"\"", TokenType::StrSingleDbl},
        {"'", TokenType::StrSingleSgl},
        {"```", TokenType::StrTripleBkt},
        {"\"\"\"", TokenType::StrTripleDbl},
        {"'''", TokenType::StrTripleSgl},

        {"ctrl_character", TokenType::CTRLChar},
        {"EOF", TokenType::CTRLCharEOF},
        {"EOL", TokenType::CTRLCharEOL}
    };

    TokenType tokens_find_errunk(Symbol _) {
        return TokenType::ErrorUnknown;
    }

    TokenType tokens_find_gentype(Symbol symbol) {
        if (!TokenTypeMapping.contains(symbol))
            return TokenType::NameGeneric;
        return TokenTypeMapping[symbol];
    }

    TokenType tokens_find_numtype(Symbol symbol) {
        if (symbol.find("0b") != SIZE_T_MAX)
            return TokenType::NumBin;
        if (symbol.find(".") != SIZE_T_MAX)
            return TokenType::NumFlt;
        if (symbol.find("0x") != SIZE_T_MAX)
            return TokenType::NumHex;
        if (symbol.find("0o") != SIZE_T_MAX)
            return TokenType::NumOct;

        return TokenType::NumInt;
    }

    class Token {
        public:
            Symbol      symbol;
            TokenType   type;
            Lineno      lineno;
            Column      column;
            std::string file;

        public:
            Token() {}

            Token(
                Lineno lineno,
                Column column,
                Symbol symbol,
                std::string file = "") {

                this->lineno = lineno;
                this->column = column;
                this->symbol = symbol;
                this->file   = file;

                if (vixen::symbols::symbol_isnumeric(symbol))
                    this->type = tokens_find_numtype(symbol);
                else
                    this->type = tokens_find_gentype(symbol);
            }
    };

    bool tokens_isfloat(Token token) {
        return token.type == TokenType::NumFlt;
    }

    bool tokens_isgeneric(Token token) {
        return token.type == TokenType::NameGeneric;
    }

    bool tokens_isinteger(Token token) {
        TokenType types[] = {
            TokenType::NumBin,
            TokenType::NumHex,
            TokenType::NumInt,
            TokenType::NumOct
        };

        for (const auto& type : types) {
            if (token.type == type)
                return true;
        }
        return false;
    }

    class Lexer : public vixen::symbols::BasicSymbolParser {
        public:
            Lexer() : vixen::symbols::BasicSymbolParser() {}
            Lexer(std::string& data) : vixen::symbols::BasicSymbolParser(data) {}
            Lexer(std::ifstream& file) : vixen::symbols::BasicSymbolParser(file) {}


            Token next_token() {
                Lineno lineno;
                Column column;
                Symbol symbol;

                std::tie(
                    lineno,
                    column,
                    symbol) = vixen::symbols::BasicSymbolParser::next();

                return Token(lineno, column, symbol);
            }
    };
};