//
// Created by guoqi on 9/16/15.
//

#ifndef MUTCC_BUFFERREADER_H
#define MUTCC_BUFFERREADER_H

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <exception>

using namespace std;

// Indicates end-of-file
struct EOFException : public std::exception
{
};

/*
 * Buffer Reader
 * Get a line or a character.
 */
class BufferReader
{
public:
    BufferReader(ifstream & input_file);
    BufferReader(stringstream & text);
    ~BufferReader ();

    /*
     * @return Get a character from input stream.
     * @throw Throw IOException if there is an exception
     */
    char readChar();
    /*
     * @return Get a line from input stream.
     * @throw Throw IOException if there is an exception
     */
    string readLine();

private:
    istream  &  __input;
    string      __line;
};

#endif //MUTCC_BUFFERREADER_H
