#include <iostream>
#include <fstream>
#include <memory>
#include <cstdio>
#include "frontend/BufferReader.h"
#include "frontend/Source.h"
#include "frontend/Tokenizer.h"
#include "frontend/Parser.h"
#include "backend/Code.h"
#include "backend/CodeGen.h"

using namespace std;

int main ()
{
    ifstream file("/home/guoqi/ClionProjects/MutC/unittest/mutcc/testcases/test1.mc");
    if (file.fail ()) {
        cout << "Open file failed." << endl;
        exit(1);
    }

    BufferReader reader(file);
    Source source(reader);
    // shared_ptr<MutCTokenizer> tokenizer = make_shared <MutCTokenizer> (source);
    MutCTokenizer * tokenizer = new MutCTokenizer(source);

    /*
    while (tokenizer->nextToken ()->type () != TokenType::Terminator)
    {
        printf("(%d, %d) %s\n", tokenizer->currentToken ()->lineNum (), tokenizer->currentToken ()->position (), tokenizer->currentToken ()->text ().c_str ());
    }
     */

    // shared_ptr<MutCParser> parser = make_shared <MutCParser> (tokenizer.get ());
    MutCParser * parser = new MutCParser(tokenizer);

    parser->parse ();
    parser->ast ().printAST ();
    cout << "start decorating..." << endl;
    parser->decorateAST ();
    cout << "decorating done..." << endl;

    // shared_ptr<CodeGen> codegen = make_shared <CodeGen> (parser.get ());
    CodeGen * codegen = new CodeGen(parser);
    cout << "start generating..." << endl;
    Code & code = codegen->generate ();
    cout << "generating done..." << endl;

    cout << code.toString () << endl;

    delete codegen;
    delete parser;
    delete tokenizer;

    return 0;
}