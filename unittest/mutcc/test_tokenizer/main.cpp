#include <iostream>
#include <fstream>
#include <memory>
#include <cstdio>
#include "frontend/BufferReader.h"
#include "frontend/Source.h"
#include "frontend/Tokenizer.h"

using namespace std;

int main ()
{
    ifstream file("/home/guoqi/ClionProjects/MutC/unittest/mutcc/testcases/test2.mc");
    if (file.fail ()) {
        cout << "Open file failed." << endl;
        exit(1);
    }

    BufferReader reader(file);
    Source source(reader);
    shared_ptr<MutCTokenizer> tokenizer = make_shared <MutCTokenizer> (source);

    while (tokenizer->nextToken ()->type () != TokenType::Terminator)
    {
        printf("(%d, %d) %s\n", tokenizer->currentToken ()->lineNum (), tokenizer->currentToken ()->position (), tokenizer->currentToken ()->text ().c_str ());
    }
    return 0;
}