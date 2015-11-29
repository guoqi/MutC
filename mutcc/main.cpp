#include <iostream>
#include <fstream>
#include <cstdlib>
#include "frontend/BufferReader.h"
#include "frontend/Source.h"
#include "frontend/Tokenizer.h"
#include "frontend/Parser.h"
#include "backend/Code.h"
#include "backend/CodeGen.h"

using namespace std;

int main (int argc, char * argv[]) {
    if (argc != 3) {
        cout << "argument too few" << endl;
        exit(1);
    }

    ifstream file(argv[1]);
    if (file.fail ()) {
        cout << "open file to read failed." << endl;
        exit(1);
    }

    BufferReader reader(file);
    Source source(reader);
    MutCTokenizer * tokenizer = new MutCTokenizer(source);
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

    ofstream output(argv[2]);
    if (output.fail ()) {
        cout << "open file to write failed." << endl;
        exit(1);
    }
    cout << "start outputing..." << endl;
    output << code.toString ();
    cout << "outputing done..." << endl;

    delete codegen;
    delete tokenizer;


    return 0;
}