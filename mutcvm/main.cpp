#include <iostream>
#include <fstream>
#include "VirtualMachine.h"

using namespace std;

int main (int argc, char * argv[])
{
    if (argc != 2) {
        cout << "argument too few" << endl;
        return -1;
    }

    ifstream file(argv[1]);
    if (file.fail ()) {
        cout << "open file failed." << endl;
        return -1;
    }

    VM * vm = initVM (file);

    vm->execute ();
    return 0;
}