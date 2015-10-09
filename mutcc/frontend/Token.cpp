//
// Created by guoqi on 9/12/15.
//

#include <cctype>
#include <sstream>
#include <cassert>
#include "Token.h"
#include "GlobalConfig.h"

Token::Token (Source &source)
{
    __lineNum = source.lineNum ();
    __position = source.position ();
}


void Token::build (Source &source)
{
    extract (source);
}


void IdentifierToken::extract (Source &source)
{
    stringstream ss;
    bool at_start = true;
    while(source.currentChar () != EOF && source.currentChar () != EOL)
    {
        if (at_start && ! isalpha(source.currentChar ()) && source.currentChar () != '_')
        {
            // TODO error handling: start not with an alphabet or symbol '_'
            at_start = false;
            // break;
        }
        else
        {
            if (isalnum (source.currentChar ()) || source.currentChar () == '_') {
                ss << source.currentChar ();
                source.nextChar ();
            }
            else {
                break;
            }
            at_start = false;
        }
    }
    __text = ss.str ();
    if (KEYWORD_SET.find(__text) != KEYWORD_SET.end ()) {
        __type = TokenType::Keyword;
    }
    else {
        __type = TokenType::Identifier;
    }
}

/*
void KeywordToken::extract (Source &source)
{
    // TODO
}
 */

void SymbolToken::extract (Source &source)
{
    stringstream ss;

    switch (source.currentChar ())
    {
        // single symbol
        case '{':
        case '}':
        case '\"':
        case '.':
        case '(':
        case ')':
        case '#':
        case '\\':
        case '@':
        case '[':
        case ']':
        case '&':
        case '|':
        case '^':
        case '~':
        case ':':
        case ';':
        case ',':
            ss << source.currentChar ();
            break;
        case '-':
            ss << source.currentChar ();
            if (source.peekChar () == '>' || source.peekChar () == '=') {
                ss << source.nextChar ();
            }
            break;
        case '!':
            ss << source.currentChar ();
            if (source.peekChar () == '=') {
                ss << source.nextChar ();
            }
            else {
                // TODO error handling: unkown symbol
            }
            break;
        case '*':
        case '+':
        case '/':
        case '=':
        case '<':
        case '>':
        case '%':
            ss << source.currentChar ();
            if (source.peekChar () == '=') {
                ss << source.nextChar ();
            }
            break;
        default:
            // TODO error handling: unkonwn symbol
            break;
    }
    __text = ss.str ();
    source.nextChar ();
}


void NumberToken::extract (Source &source)
{
    stringstream ss;
    __type = TokenType::Integer;

    while (source.currentChar () != EOF && source.currentChar () != EOL
           && ((source.currentChar () >= '0' && source.currentChar () <= '9') || source.currentChar () == '.'))
    {
        if (source.currentChar () == '.')
        {
            __type = TokenType::Real;
        }
        ss << source.currentChar ();
        source.nextChar ();
    }

    __text = ss.str ();

    if (__type == TokenType::Integer) {
        __value.integer = atol (__text.c_str ());
    }
    else {
        __value.real = atof (__text.c_str ());
    }
}

void StringToken::extract (Source &source)
{
    stringstream ss;
    bool at_start = true;
    while (source.currentChar () != EOF && source.currentChar () != EOL)
    {
        if (at_start) {
            if (source.currentChar () != '"') {
                // TODO error handling: string not start with symbol "
            }
            at_start = false;
        }
        else {
            if (source.currentChar () == '"') {
                break;
            }
            ss << source.currentChar ();
            source.nextChar ();
        }
    }
    __text = ss.str ();
}

void TerminatorToken::extract (Source &source)
{
    assert(source.currentChar () == EOF);
    return;
}
