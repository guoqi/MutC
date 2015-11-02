//
// Created by guoqi on 9/12/15.
//

#ifndef MUTC_SOURCEREADER_H
#define MUTC_SOURCEREADER_H

#include <string>
#include "BufferReader.h"

using namespace std;

// end-of-line
extern const int EOL;


class Source
{
public:
    Source (BufferReader & reader);
    ~Source ();


    /*
     * Get current character with pointer moving.
     * return EOF if there is end-of-file.
     */
    char currentChar();

    /*
     * Get next character with pointer moving.
     * return EOF if there is end-of-file.
     */
    char nextChar();
    /*
     * Get next character without pointer moving.
     * return EOF if there is end-of-file.
     */
    char peekChar();

    inline int lineNum() const {
        return __lineNum;
    }

    inline int position() const {
        return __position;
    }

private:
    int                 __lineNum;
    int                 __position;
    char                __currentChar;
    string              __currentRow;
    BufferReader    &   __reader;
    bool                __iseof;
};

#endif //MUTC_SOURCEREADER_H
