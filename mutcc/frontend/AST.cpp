//
// Created by guoqi on 9/17/15.
//

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
    if (__operator == "." || __operator == "->")
    {
        string type_name;
        if (__operator == ".") {
            type_name = static_cast <StructType *> (expr1->typeInfo ())->name ();
        }
        else {
            type_name = static_cast <StructType *> (expr1->typeInfo ()->next ())->name ();
        }
        TypeEntry::Ptr type_entry = curScope ()->lookUpType (type_name);
        // TODO
    }
    return expr1->typeInfo ();
}

Type* ArrayExp::typeInfo ()
{
    Type * p = static_pointer_cast <VarEntry> (sym_entry)->typeInfo ();
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
    return static_pointer_cast <FuncEntry> (sym_entry)->ret_type;
}

Type* AtomicExp::typeInfo ()
{
    return static_pointer_cast <VarEntry> (sym_entry)->typeInfo ();
}
