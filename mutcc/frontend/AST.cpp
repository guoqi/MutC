//
// Created by guoqi on 9/17/15.
//

#include <iostream>
#include "AST.h"
#include "GlobalConfig.h"

Type* UnaryExp::typeInfo ()
{
    if (__operator->text () == "+" || __operator->text () == "-" || __operator->text () == "not")
    {
        return expr1->typeInfo ();
    }
    else if (__operator->text () == "*")
    {
        return expr1->typeInfo ()->next ();
    }
    else if (__operator->text () == "@")
    {
        Type * root = new Type(TypeInfo::Pointer);
        root->next (expr1->typeInfo ());
        return root;
    }
    return nullptr;
}

Type* BinaryExp::typeInfo ()
{
    if (__operator->text () == "." || __operator->text () == "->")
    {
        string type_name;
        if (__operator->text () == ".") {
            type_name = static_cast <StructType *> (expr1->typeInfo ())->name ();
        }
        else {
            type_name = static_cast <StructType *> (expr1->typeInfo ()->next ())->name ();
        }
        TypeEntry * type_entry = curScope ()->lookUpType (type_name);
        // TODO
    }
    return expr1->typeInfo ();
}

Type* ArrayExp::typeInfo ()
{
    Type * p = static_cast <VarEntry *> (sym_entry)->typeInfo ();
    for (auto i=index_list.begin (); i!=index_list.end () && p != nullptr; ++i)
    {
        p = p->next ();
    }
    if (p == nullptr) {
        // TODO error handling
    }
    return p;
}

Type* FuncExp::typeInfo ()
{
    return static_cast <FuncEntry *> (sym_entry)->ret_type;
}

Type* AtomicExp::typeInfo ()
{
    if (sym_entry == nullptr) {
        switch (var->type ()) {
            case TokenType::Integer:
                return new AtomicType(TypeInfo::Integer);
            case TokenType::Real:
                return new AtomicType(TypeInfo::Real);
            case TokenType::String:
                return new AtomicType(TypeInfo::String);
            default:
                break;
        }
    }
    return static_cast <VarEntry *> (sym_entry)->typeInfo ();
}



void AST::printAST ()
{
    Stmt::Ptr p = __program;
    while (p != nullptr) {
        p->printNode ();
        p = p->next ();
    }
}

void ImportStmt::printNode ()
{
    // TODO
}

void ExportStmt::printNode ()
{
    // TODO
}

void FuncStmt::printNode ()
{
    char buffer[1024];
    sprintf(buffer, "<function name=%s>", name->text ().c_str ());
    cout << buffer << endl;
    cout << "<block>" << endl;
    Stmt::Ptr p = block_stmt;
    while (p != nullptr) {
        p->printNode ();
        p = p->next ();
    }
    cout << "</block>" << endl;
    cout << "</function>" << endl;
}

void LetStmt::printNode ()
{
    cout << "<let>" << var->text () << "</let>" << endl;
}

void IfStmt::printNode ()
{
    cout << "<branch>" << endl;
    cout << "<condition>" << endl;
    if (condition == nullptr) {
        cout << "null" << endl;
    }
    else {
        condition->printNode ();
    }
    cout << "</condition>" << endl;
    cout << "<block>" << endl;
    Stmt::Ptr p = block_stmt;
    while (p != nullptr) {
        p->printNode ();
        p = p->next ();
    }
    cout << "</block>" << endl;
    cout << "</branch>" << endl;
    clause_next->printNode ();
}

void WhileStmt::printNode ()
{
    cout << "<while>" << endl;
    cout << "<condition>" << endl;
    if (condition == nullptr) {
        cout << "null" << endl;
    }
    else {
        condition->printNode ();
    }
    cout << "</condition>" << endl;
    cout << "<block>" << endl;
    Stmt::Ptr p = block_stmt;
    while (p != nullptr) {
        p->printNode ();
        p = p->next ();
    }
    cout << "</block>" << endl;
    cout << "</while>" << endl;
}

void AssignmentStmt::printNode ()
{
    cout << "<assignment>" << endl;
    cout << "<lvalue>" << lvalue->text () << "</lvalue>" << endl;
    cout << "<rvalue>" << endl;
    rvalue->printNode ();
    cout << "</rvalue>" << endl;
    cout << "</assignment>" << endl;
}

void ReturnStmt::printNode ()
{
    cout << "<return>" << endl;
    rslt->printNode ();
    cout << "</return>" << endl;
}

void ForStmt::printNode ()
{
    cout << "<for>" << endl;
    cout << "<initial>" << endl;
    initial_stmt->printNode ();
    cout << "</initial>" << endl;
    cout << "<condition>" << endl;
    condition->printNode ();
    cout << "</condition>" << endl;
    cout << "<modify>" << endl;
    modify_stmt->printNode ();
    cout << "</modify>" << endl;
    cout << "<block>" << endl;
    Stmt::Ptr p = block_stmt;
    while (p != nullptr) {
        p->printNode ();
        p = p->next ();
    }
    cout << "</block>" << endl;
    cout << "</for>" << endl;
}

void UnaryExp::printNode ()
{
    cout << "<unary_expression>" << endl;
    cout << "<operator>" << __operator->text () << "</operator>" << endl;
    expr1->printNode ();
    cout << "</unary_expression>" << endl;
}

void BinaryExp::printNode ()
{
    cout << "<binary_expression>" << endl;
    cout << "<operator>" << __operator->text () << "</operator>" << endl;
    cout << "<left>" << endl;
    expr1->printNode ();
    cout << "</left>" << endl;
    cout << "<right>" << endl;
    expr2->printNode ();
    cout << "</right>" << endl;
    cout << "</binary_expression>" << endl;
}

void ArrayExp::printNode ()
{
    cout << "<array_expression>" << endl;
    cout << "<array_name>" << array->text () << "</array_name>" << endl;
    cout << "<index_list>" << endl;
    for (auto & i : index_list) {
        i->printNode ();
    }
    cout << "</index_list>" << endl;
    cout << "</array_expression>" << endl;
}

void FuncExp::printNode ()
{
    cout << "<function_expression>" << endl;
    cout << "<function_name>" << func->text () << "</function_name>" << endl;
    cout << "<param_list>" << endl;
    for (auto & i : param_list) {
        i->printNode ();
    }
    cout << "</param_list>" << endl;
    cout << "</function_expression>" << endl;
}

void AtomicExp::printNode ()
{
    cout << "<atomic_expression>" << var->text () << "</atomic_expression>" << endl;
}
