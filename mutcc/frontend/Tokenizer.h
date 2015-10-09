//
// Created by guoqi on 9/12/15.
//

#ifndef MUTC_TOKENIZER_H
#define MUTC_TOKENIZER_H

#include <memory>
#include "Source.h"
#include "Token.h"

using namespace std;

class Tokenizer
{
public:
    Tokenizer(Source & source);
    virtual ~Tokenizer () {}

    /*
     * Get next token without error handling.
     */
    Token::Ptr nextToken();

    /*
     * Get current token without error handling.
     */
    Token::Ptr currentToken();

protected:
    virtual Token::Ptr extractToken() = 0;

    /*
     * delegate to __source
     */
    char currentChar();
    char nextChar();
    char peekChar();

    Source        &     __source;

private:
    Token::Ptr          __currentToken;
};


class MutCTokenizer: public Tokenizer
{
public:
    MutCTokenizer(Source & source): Tokenizer(source) {}

protected:
    virtual Token::Ptr extractToken();
};


#endif //MUTC_TOKENIZER_H
