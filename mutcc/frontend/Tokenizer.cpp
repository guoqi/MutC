//
// Created by guoqi on 9/12/15.
//

#include "Tokenizer.h"

Tokenizer::Tokenizer (Source &source)
    : __source(source)
{
}

Token::Ptr Tokenizer::nextToken ()
{
    __currentToken = extractToken ();
    return currentToken ();
}

Token::Ptr Tokenizer::currentToken ()
{
    return __currentToken;
}

char Tokenizer::currentChar ()
{
    return __source.currentChar ();
}

char Tokenizer::nextChar ()
{
    return __source.nextChar ();
}

char Tokenizer::peekChar ()
{
    return __source.peekChar ();
}


Token::Ptr MutCTokenizer::extractToken ()
{
    Token::Ptr token;
    // terminate
    if (currentChar () == EOF)
    {
        token = make_shared <TerminatorToken> (__source);
    }
    // skip space character
    else if (isspace (currentChar ()))
    {
        nextChar ();
        return extractToken ();
    }
    // number
    else if (currentChar () >= '0' && currentChar () <= '9')
    {
        token = make_shared<NumberToken> (__source);
    }
    // identifier
    else if (isalpha (currentChar ()) || currentChar () == '_')
    {
        token = make_shared <IdentifierToken> (__source);
    }
    // symbol
    else if (! isalnum (currentChar ()) && currentChar () != '_')
    {
        token = make_shared <SymbolToken> (__source);
    }
    // string
    else if (currentChar () == '"')
    {
        token = make_shared <StringToken> (__source);
    }
    else
    {
        // TODO error handling: unexpected beginning character
        return nullptr;
    }
    token->build (__source);
    return token;
}