#pragma once
#include <unordered_map>

#include "symbols.hpp"

#ifndef SIZE_T_MAX
#define SIZE_T_MAX ((size_t)(-1))
#endif

namespace vixen::tokens {
    using namespace symbols;

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
        OperEquals,
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

    #define TYPEMAPPER(NAME, TYPE, REPR) {NAME, {TYPE, REPR}}
    // Unlike the prototype, we are only mapping
    // enumerations that that we can know to be
    // predictable. Such as punctuation, supported
    // operators and keywords.
    // All other token types can be implicitly
    // discerned based on context after parse time.
    static std::unordered_map<std::string, std::pair<TokenType, std::string>> TokenTypeMapping = {
        TYPEMAPPER("<error>", TokenType::Error, "Error"),
        TYPEMAPPER("<error:unknown>", TokenType::ErrorUnknown, "ErrorUnknown"),
        TYPEMAPPER("<error:bad_string>", TokenType::ErrorBadString, "ErrorBadString"),
        TYPEMAPPER("<keyword>", TokenType::Kwd, "Kwd"),
        TYPEMAPPER("as", TokenType::KwdAs, "KwdAs"),
        TYPEMAPPER("break", TokenType::KwdBreak, "KwdBreak"),
        TYPEMAPPER("catch", TokenType::KwdCatch, "KwdCatch"),
        TYPEMAPPER("continue", TokenType::KwdContinue, "KwdContinue"),
        TYPEMAPPER("const", TokenType::KwdConstant, "KwdConstant"),
        TYPEMAPPER("class", TokenType::KwdClass, "KwdClass"),
        TYPEMAPPER("default", TokenType::KwdDefault, "KwdDefault"),
        TYPEMAPPER("delete", TokenType::KwdDelete, "KwdDelete"),
        TYPEMAPPER("else", TokenType::KwdElse, "KwdElse"),
        TYPEMAPPER("for", TokenType::KwdFor, "KwdFor"),
        TYPEMAPPER("from", TokenType::KwdFrom, "KwdFrom"),
        TYPEMAPPER("func", TokenType::KwdFunc, "KwdFunc"),
        TYPEMAPPER("if", TokenType::KwdIf, "KwdIf"),
        TYPEMAPPER("import", TokenType::KwdImport, "KwdImport"),
        TYPEMAPPER("include", TokenType::KwdInclude, "KwdInclude"),
        TYPEMAPPER("new", TokenType::KwdNew, "KwdNew"),
        TYPEMAPPER("nil", TokenType::KwdNil, "KwdNil"),
        TYPEMAPPER("null", TokenType::KwdNull, "KwdNull"),
        TYPEMAPPER("panic", TokenType::KwdPanic, "KwdPanic"),
        TYPEMAPPER("proto", TokenType::KwdProto, "KwdProto"),
        TYPEMAPPER("raise", TokenType::KwdRaise, "KwdRaise"),
        TYPEMAPPER("return", TokenType::KwdReturn, "KwdReturn"),
        TYPEMAPPER("static", TokenType::KwdStatic, "KwdStatic"),
        TYPEMAPPER("try", TokenType::KwdTry, "KwdTry"),
        TYPEMAPPER("while", TokenType::KwdWhile, "KwdWhile"),
        TYPEMAPPER("with", TokenType::KwdWith, "KwdWith"),
        TYPEMAPPER("<name>", TokenType::Name, "Name"),
        TYPEMAPPER("<name:generic>", TokenType::NameGeneric, "NameGeneric"),
        TYPEMAPPER("<numeric>", TokenType::Num, "Num"),
        TYPEMAPPER("0b%", TokenType::NumBin, "NumBin"),
        TYPEMAPPER("%.%", TokenType::NumFlt, "NumFlt"),
        TYPEMAPPER("0x%", TokenType::NumHex, "NumHex"),
        TYPEMAPPER("%", TokenType::OperModulus, "OperModulus"),
        TYPEMAPPER("0o%", TokenType::NumOct, "NumOct"),
        TYPEMAPPER("<operation>", TokenType::Oper, "Oper"),
        TYPEMAPPER("&", TokenType::OperBtAnd, "OperBtAnd"),
        TYPEMAPPER("?", TokenType::OperAsk, "OperAsk"),
        TYPEMAPPER("=", TokenType::OperAssign, "OperAssign"),
        TYPEMAPPER("|", TokenType::OperBtOr, "OperBtOr"),
        TYPEMAPPER("^", TokenType::OperBtXor, "OperBtXor"),
        TYPEMAPPER("--", TokenType::OperDecrement, "OperDecrement"),
        TYPEMAPPER("~", TokenType::OperDelete, "OperDelete"),
        TYPEMAPPER("/", TokenType::OperDivide, "OperDivide"),
        TYPEMAPPER("//", TokenType::OperDivFloor, "OperDivFloor"),
        TYPEMAPPER("==", TokenType::OperEquals, "OperEquals"),
        TYPEMAPPER("++", TokenType::OperIncrement, "OperIncrement"),
        TYPEMAPPER("&&", TokenType::OperLgAnd, "OperLgAnd"),
        TYPEMAPPER("!", TokenType::OperLgNot, "OperLgNot"),
        TYPEMAPPER("||", TokenType::OperLgOr, "OperLgOr"),
        TYPEMAPPER(">", TokenType::OperLgGt, "OperLgGt"),
        TYPEMAPPER(">=", TokenType::OperLgGte, "OperLgGte"),
        TYPEMAPPER("<", TokenType::OperLgLt, "OperLgLt"),
        TYPEMAPPER("<=", TokenType::OperLgLte, "OperLgLte"),
        TYPEMAPPER("-", TokenType::OperMinus, "OperMinus"),
        TYPEMAPPER("-=", TokenType::OperMinusEq, "OperMinusEq"),
        TYPEMAPPER("+", TokenType::OperPlus, "OperPlus"),
        TYPEMAPPER("+=", TokenType::OperPlusEq, "OperPlusEq"),
        TYPEMAPPER("**", TokenType::OperPower, "OperPower"),
        TYPEMAPPER("->", TokenType::OperPtrAttr, "OperPtrAttr"),
        TYPEMAPPER("@", TokenType::OperStamp, "OperStamp"),
        TYPEMAPPER("*", TokenType::OperStar, "OperStar"),
        TYPEMAPPER("<punctuation>", TokenType::Punc, "Punc"),
        TYPEMAPPER(":", TokenType::PuncColon, "PuncColon"),
        TYPEMAPPER(",", TokenType::PuncComma, "PuncComma"),
        TYPEMAPPER(".", TokenType::PuncDot, "PuncDot"),
        TYPEMAPPER("{", TokenType::PuncLBrace, "PuncLBrace"),
        TYPEMAPPER("[", TokenType::PuncLBracket, "PuncLBracket"),
        TYPEMAPPER("(", TokenType::PuncLParen, "PuncLParen"),
        TYPEMAPPER("}", TokenType::PuncRBrace, "PuncRBrace"),
        TYPEMAPPER("]", TokenType::PuncRBracket, "PuncRBracket"),
        TYPEMAPPER(")", TokenType::PuncRParen, "PuncRParen"),
        TYPEMAPPER(";", TokenType::PuncTerminator, "PuncTerminator"),
        TYPEMAPPER("<string>", TokenType::Str, "Str"),
        TYPEMAPPER("`", TokenType::StrSingleBkt, "StrSingleBkt"),
        TYPEMAPPER("\"", TokenType::StrSingleDbl, "StrSingleDbl"),
        TYPEMAPPER("'", TokenType::StrSingleSgl, "StrSingleSgl"),
        TYPEMAPPER("```", TokenType::StrTripleBkt, "StrTripleBkt"),
        TYPEMAPPER("\"\"\"", TokenType::StrTripleDbl, "StrTripleDbl"),
        TYPEMAPPER("'''", TokenType::StrTripleSgl, "StrTripleSgl"),
        TYPEMAPPER("<string:expression>", TokenType::StrExpression, "StrExpression"),
        TYPEMAPPER("<ctrl_character>", TokenType::CTRLChar, "CTRLChar"),
        TYPEMAPPER("EOF", TokenType::CTRLCharEOF, "CTRLCharEOF"),
        TYPEMAPPER("EOL", TokenType::CTRLCharEOL, "CTRLCharEOL")
    };

    TokenType tokens_find_errunk(Symbol _) {
        return TokenType::ErrorUnknown;
    }

    std::string tokens_find_genname(Symbol symbol) {
        if (!TokenTypeMapping.contains(symbol))
            return "NameGeneric";
        return TokenTypeMapping[symbol].second;
    }

    std::string tokens_find_genname(TokenType type) {
        for (const auto& [key, value] : TokenTypeMapping) {
            if (value.first == type)
                return value.second;
        }
        return "ErrorUnknown";
    }

    TokenType tokens_find_gentype(Symbol symbol) {
        if (!TokenTypeMapping.contains(symbol))
            return TokenType::NameGeneric;
        return TokenTypeMapping[symbol].first;
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

                if (symbol_isnumeric(symbol))
                    this->type = tokens_find_numtype(symbol);
                else
                    this->type = tokens_find_gentype(symbol);
            }
        private:
            friend std::ostream& operator<<(std::ostream& os, const Token& token) {
                os << "Token";
                if (token.symbol.find('\'') == SIZE_T_MAX)
                    os << "['" << token.symbol << "']";
                else
                    os << "[\"" << token.symbol << "\"]";

                os << "@(";
                os << "lineno: " << token.lineno << ", ";
                os << "column: " << token.column;
                if (token.file.length() > 0)
                    os << ", file: '" << token.file << "'";
                os << ")";

                return os;
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

    class Lexer : public BasicSymbolParser<Token> {
        public:
            BasicSymbolParser__init__(Lexer, Token)

            Token next() {
                Lineno lineno;
                Column column;
                Symbol symbol;

                std::tie(
                    lineno,
                    column,
                    symbol) = BasicSymbolParser<Token>::next_raw();

                return Token(lineno, column, symbol, this->file);
            }
    };
};