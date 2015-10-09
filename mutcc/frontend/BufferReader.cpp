//
// Created by guoqi on 9/16/15.
//

#include "BufferReader.h"

using namespace std;

BufferReader::BufferReader (ifstream & input_file)
    : __input(input_file)
{
}

BufferReader::BufferReader (stringstream & text)
    : __input(text)
{
}

BufferReader::~BufferReader ()
{
}


char BufferReader::readChar ()
{
    if (__input.eof()) {
        throw EOFException();
    }
    char c;
    __input >> c;
    return c;
}


string BufferReader::readLine ()
{
    if (__input.eof()) {
        throw EOFException();
    }
    string s;
    getline(__input, s);
    return s;
}