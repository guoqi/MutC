//
// Created by guoqi on 9/17/15.
// Abstract synatic tree definition
// Note that it does not include any type info which will be indicated in symbol table
//

#ifndef MUTCC_AST_H
#define MUTCC_AST_H

#include <memory>
#include <vector>
#include "Token.h"
#include "SymbolTable.h"

using namespace std;

enum class NodeType {
    UnaryExp,
    BinaryExp,
    ArrayExp,
    FuncExp,
    AtomicExp,

    ImportStmt,
    ExportStmt,
    FuncStmt,
    LetStmt,
    IfStmt,
    WhileStmt,
    ForStmt,
    ReturnStmt,
    AssignmentStmt
};

class Scope;

class Node
{
public:
    typedef shared_ptr<Node> Ptr;

    virtual ~Node() {}

    inline NodeType type() { return __type; }
    inline Scope * curScope() { return __curScope; }
    inline void setScope(Scope * scope) { __curScope = scope; }

    virtual void printNode() = 0;

protected:
    NodeType    __type;
    Scope *     __curScope; // 当前所属作用域
};

class  Exp: public Node
{
public:
    typedef shared_ptr<Exp> Ptr;

    Exp(): Node() {}
    virtual ~Exp() {}

    virtual Type * typeInfo() = 0;
    virtual void printNode() = 0;
};

// unary operator expression
struct UnaryExp: public Exp
{
public:
    UnaryExp(Token::Ptr token): __operator(token) { __type = NodeType::UnaryExp; }
    virtual ~UnaryExp () {}

    Exp::Ptr     expr1;
    // operator
    inline Token::Ptr op() { return __operator; }

    virtual Type * typeInfo();
    virtual void printNode();

protected:
    Token::Ptr      __operator;
};

// binary operator expression
struct BinaryExp: public Exp
{
public:
    BinaryExp(Token::Ptr token): __operator(token) { __type = NodeType::BinaryExp; }
    virtual ~BinaryExp () {}

    Exp::Ptr    expr1;
    Exp::Ptr    expr2;
    // operator
    inline Token::Ptr op() { return __operator; }

    virtual Type * typeInfo();
    virtual void printNode();

protected:
    Token::Ptr      __operator;
};

struct ArrayExp: public Exp
{
public:
    ArrayExp(Token::Ptr token): array(token) { __type = NodeType::ArrayExp; }
    virtual ~ArrayExp () {}

    virtual Type * typeInfo();
    virtual void printNode();

    Token::Ptr                array;
    vector<Exp::Ptr>          index_list;
    SymEntry    *             sym_entry;
};

struct FuncExp: public Exp
{
public:
    FuncExp(Token::Ptr token): func(token) { __type = NodeType::FuncExp; }
    virtual ~FuncExp () {}

    virtual Type * typeInfo();
    virtual void printNode();

    Token::Ptr            func;
    vector<Exp::Ptr>      param_list;
    SymEntry    *         sym_entry;
};

struct AtomicExp: public Exp
{
public:
    AtomicExp(Token::Ptr token): var(token) { __type = NodeType::AtomicExp; }
    virtual ~AtomicExp() {}

    virtual Type * typeInfo();
    virtual void printNode();

    Token::Ptr          var;
    SymEntry    *       sym_entry;
};


class Stmt: public Node
{
public:
    typedef shared_ptr<Stmt>    Ptr;

    Stmt(): __next() {}
    virtual ~Stmt () {}

    virtual void printNode() {}

    inline Stmt::Ptr next(Stmt::Ptr n) { __next = n; }
    inline Stmt::Ptr next(void) { return __next; }

protected:
    Stmt::Ptr   __next; // pointer to next statement
};

// import statement
struct ImportStmt: public Stmt
{
    ImportStmt(): Stmt() { __type = NodeType::ImportStmt; }

    virtual void printNode();

    vector<Token::Ptr>  token_list;  // path identifier's list.
};

struct ExportStmt: public Stmt
{
    ExportStmt(): Stmt() { __type = NodeType::ExportStmt; }

    virtual void printNode();

    vector<Token::Ptr>  token_list; // exported token's list.
};

struct FuncStmt: public Stmt
{
    FuncStmt(): Stmt() { __type = NodeType::FuncStmt; }

    virtual void printNode();

    Stmt::Ptr               block_stmt; // root node of inner-block statement linked list
    Token::Ptr              name;
    SymEntry    *           sym_entry;
};

struct LetStmt: public Stmt
{
    LetStmt(): Stmt() { __type = NodeType::LetStmt; }

    virtual void printNode();

    Token::Ptr      var;
    SymEntry *      sym_entry;
};

struct IfStmt: public Stmt
{
    typedef shared_ptr<IfStmt>  Ptr;

    IfStmt(): Stmt() { __type = NodeType::IfStmt; clause_next = nullptr; }

    virtual void printNode();

    Exp::Ptr    condition;  // condition expression
    Stmt::Ptr   block_stmt; // root node of inner-block statement linked list
    // if stmt
    Stmt::Ptr   clause_next;
};

struct WhileStmt: public Stmt
{
    WhileStmt(): Stmt() { __type = NodeType::WhileStmt; }

    virtual void printNode();

    Exp::Ptr    condition;  // condition expression
    Stmt::Ptr   block_stmt; // root node of inner-block statement linked list
};

struct AssignmentStmt: public Stmt
{
    typedef shared_ptr<AssignmentStmt> Ptr;

    AssignmentStmt(): Stmt() { __type = NodeType::AssignmentStmt; }

    virtual void printNode();

    Token::Ptr  op; // assignment operator
    Token::Ptr  lvalue; // left value
    SymEntry *  sym_entry;
    Exp::Ptr    rvalue; // right value
};

struct ReturnStmt: public Stmt
{
    ReturnStmt(): Stmt() { __type = NodeType::ReturnStmt; }

    virtual void printNode();

    Exp::Ptr    rslt; // return value
};

struct ForStmt: public Stmt
{
    ForStmt(): Stmt() { __type = NodeType::ForStmt; }

    virtual void printNode();

    // assignment stmt
    Stmt::Ptr               initial_stmt; // initial assignment statement
    Exp::Ptr                condition;  // condition expression
    // assignment stmt
    Stmt::Ptr               modify_stmt; // every time loop runs it will be executed to modify condition variable.
    Stmt::Ptr               block_stmt;
};





class AST
{
public:
    /*
     * preorder traversal the tree and print every node.
     */
    void printAST();

    inline Stmt::Ptr program() { return __program; }
    inline Stmt::Ptr program(Stmt::Ptr program) { __program = program; return __program; }

private:
    Stmt::Ptr    __program;    // node of out-block statement linked list
};




#endif //MUTCC_AST_H
