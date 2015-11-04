//
// Created by guoqi on 9/19/15.
//

#ifndef MUTCC_GLOBALCONFIG_H
#define MUTCC_GLOBALCONFIG_H

#include <set>
#include <string>

using namespace std;

// Keyword set
const set<string> KEYWORD_SET = {
    "let",
    "fn",
    "decl",
    "if",
    "elif",
    "else",
    "while",
    "for",
    "sizeof",
    "import",
    "true",
    "false",
    "int",
    "real",
    "bool",
    "struct",
    "array",
    "ptr",
    "and",
    "or",
    "not"
    "export",
    "return",
    "print"
};

const set<string> ASSIGNMENTOP = {
    "=",
    "+=",
    "-=",
    "*=",
    "/=",
    "%="
};

const set<string> COMPOP = {
    "==",
    "!=",
    ">",
    "<",
    ">=",
    "<="
};

const set<string> COMPUTEOP = {
    "+",
    "-",
    "*",
    "/",
    "%"
};


const set<string> SYMBOL_SET = {
    "{",
    "}",
    "\"",
    ".",
    "(",
    ")",
    "#",
    "\\",
    "@",
    "[",
    "]",
    "&",
    "|",
    "^",
    "~",
    ":",
    ";",

    "*",
    "+",
    "-",
    "/",
    "=",
    "<",
    ">",
    "%",

    "->",
    "==",
    "!=",
    ">=",
    "<=",
    "+=",
    "-=",
    "*=",
    "/=",
    "%="
};


#endif //MUTCC_GLOBALCONFIG_H
