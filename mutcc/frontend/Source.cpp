//
// Created by guoqi on 9/12/15.
//

#include "Source.h"

const int EOL = -2;

Source::Source (BufferReader &reader)
    : __lineNum (0), __position (-2), __currentChar(-1), __reader(reader), __iseof(false)
{
}

Source::~Source ()
{
}


char Source::currentChar ()
{
    if (__iseof) { return EOF; } // 避免多次触发异常，因为触发一次异常后，当前类成员的状态处于不稳定的状态，再次触发就可能会导致一些奇怪的情况发生

    try {
        if (__position == -2)
        {
            // at first call
            __currentRow = __reader.readLine ();
            ++__lineNum;
            return nextChar ();
        }
        else if (__position == -1)
        {
            // at line beginning
            return nextChar ();
        }
        else if (__position >= __currentRow.size ())
        {
            // end-of-line
            __currentRow = __reader.readLine ();
            ++__lineNum;
            __position = -1;
            return nextChar ();
        }
        else
        {
            // normal
            return __currentRow.at (__position);
        }
    }
    catch (EOFException & e)
    {
        __iseof = true;
        return EOF;
    }
}

char Source::nextChar ()
{
    ++__position;
    return currentChar ();
}

char Source::peekChar ()
{
    if (currentChar () == EOF)
    {
        return EOF;
    }
    int next_column = __position + 1;
    return next_column >= __currentRow.size () ? EOL : __currentRow[next_column];
}