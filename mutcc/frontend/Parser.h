//
// Created by guoqi on 9/12/15.
//

#ifndef MUTC_PARSER_H
#define MUTC_PARSER_H

#include "Tokenizer.h"
#include "AST.h"
#include "SymbolTable.h"
#include "TypeInfo.h"

class Parser {
public:
    Parser(Tokenizer & tokenizer);
    virtual ~Parser(){}

    /*
     * Parse source code and generate AST and symbol table.
     */
    virtual void parse() = 0;

protected:
    /*
     * delegate to __tokenizer
     */
    Token::Ptr nextToken();
    Token::Ptr currentToken();

private:
    Tokenizer   &       __tokenizer;
};


class MutCParser : public Parser
{
public:
    MutCParser(Tokenizer & tokenizer): Parser(tokenizer) {}
    virtual ~MutCParser () {}

    virtual void parse();

private:
    /*
     * generate abstract syntax tree
     */
    Stmt::Ptr parseOutBlock();
    // importStmt := import M;
    // PATHNAME := ID(E|.PATHNAME)    (E means Epsilon)
    Stmt::Ptr parseImport();
    // exportStmt := export{NAMELIST};
    // NAMELIST := ID(E|,NAMELIST)
    Stmt::Ptr parseExport();
    // funcStmt  := {InBlockStmt}
    Stmt::Ptr parseFunc();
    Stmt::Ptr parseInBlock(Scope::Ptr curScope);

    /*
     * In block stmt
     */
    Stmt::Ptr parseIfAndClause (Scope::Ptr curScope, string keyword);
    Stmt::Ptr parseElse (Scope::Ptr curScope);
    Stmt::Ptr parseWhile (Scope::Ptr curScope);
    Stmt::Ptr parseFor (Scope::Ptr curScope);
    Stmt::Ptr parseReturn ();
    Stmt::Ptr parseAssignment();

    /*
     * Expression
     */
    Exp::Ptr parseExp();
    Exp::Ptr parseExpB();
    Exp::Ptr parseExpC();
    Exp::Ptr parseExpD();
    Exp::Ptr parseExpE();
    Exp::Ptr parseExpF();
    Exp::Ptr parseExpG();
    // AT ----- atomic term
    Exp::Ptr parseAtomicTerm();

    void toolExpQ(Exp::Ptr func);
    void toolExpIndex(Exp::Ptr array);


    /*
     * insert into symbol table
     */
    // declStmt := decl ID{PARAMLIST};
    void parseDecl(Scope::Ptr curScope);
    void parseLet(Scope::Ptr  curScope);
    // funcStmt := fn ID(PARAMLIST):TYPE
    // PARAMLIST := ID:TYPE(E|,PARAMLIST) | E
    void parseFunc (Scope::Ptr curScope);

    Type * parseType();

    /*
     * spilt syntax as example:
     * NAMELIST := ID(E|DELIMITER_NAMELIST)
     * _ is used to seperate words
     */
    template<typename T>
    void toolNameList (Stmt::Ptr stmt, string delimiter, string end_word);

    /*
     * spilt syntax as example:
     * PARAMLIST := ID:TYPE(E|,PARAMLIST) | E
     */
    template<typename T>
    void toolParamList (SymEntry::Ptr entry, string delimiter, string end_word);

private:
    AST         __ast;
    ScopeTree   __scope_tree;
};

#endif //MUTC_PARSER_H