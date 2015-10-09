//
// Created by guoqi on 9/12/15.
//

#ifndef MUTC_TOKENTYPE_H
#define MUTC_TOKENTYPE_H

#include <string>
#include <memory>
#include <cctype>
#include "Source.h"

using namespace std;

enum class TokenType {
    /*
     * variable name, self-defined type name or function name.
     * rules: [a-zA-Z][a-zA-Z0-9_]*
     */
    Identifier,

    /*
     * some reversed identifier
     * rules: must be one of keyword set
     */
    Keyword,

    /*
     * special symbols
     * rules: must be one of symbol set
     */
    Symbol,

    /*
     * literal constant
     * integer: [0-9]+  all based decimal
     * real: [0-9]+\.[0-9]+ all based decimal
     * string: "[.]*"
     */
    Integer,
    Real,
    String,

    /*
     * end of file
     */
    Terminator
};

class Token
{
public:
    typedef shared_ptr<Token>   Ptr;
    typedef union Value {
        uint64_t integer;
        double   real;
    }Value;

    Token(Source & source);
    virtual ~Token () {}

    inline int lineNum() const {
        return __lineNum;
    }

    inline int position() const {
        return __position;
    }

    inline TokenType type() const {
        return __type;
    }

    inline string text() const {
        return __text;
    }

    inline Value value() const {
        return __value;
    }

    virtual void build(Source & source);

protected:
    /*
     * Extract Token from source input stream.
     * Can be override by inherited class. Default action is extract a character.
     */
    virtual void extract(Source & source) = 0;

protected:
    int         __lineNum;  // starting row number
    int         __position;   // starting column number
    TokenType   __type;     // type of token
    string      __text;     // text of token
    Value       __value;    // value of token
};


/*
 * including keyword
 * Also, the type of keyword token is Keyword
 */
class IdentifierToken : public Token
{
public:
    IdentifierToken (Source & source): Token(source) {}
    virtual ~IdentifierToken () {}

protected:
    virtual void extract(Source & source);
};


/*
class KeywordToken: public Token
{
public:
    KeywordToken (Source & source): Token(source), __type(TokenType::Keyword) {}
    virtual ~KeywordToken () {}

protected:
    virtual void extract(Source & source);
};
 */


class SymbolToken: public Token
{
public:
    SymbolToken(Source & source): Token(source) { __type = TokenType::Symbol; }
    virtual ~SymbolToken () {}

protected:
    virtual void extract(Source & source);
};


/*
 * Including integer token and real number token
 * Here we must evaluate its value judge its type
 */
class NumberToken : public Token
{
public:
    NumberToken (Source & source): Token(source) {}
    virtual ~NumberToken () {}

protected:
    virtual void extract(Source & source);
};


class StringToken: public Token
{
public:
    StringToken(Source & source): Token(source) { __type = TokenType::String; }
    virtual ~StringToken() {}

protected:
    virtual void extract(Source & source);
};


class TerminatorToken : public Token
{
public:
    TerminatorToken (Source & source): Token(source) { __type = TokenType::Terminator; }
    virtual ~TerminatorToken () {}

protected:
    virtual void extract(Source & source);
};


#endif //MUTC_TOKENTYPE_H
