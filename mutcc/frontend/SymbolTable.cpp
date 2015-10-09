//
// Created by guoqi on 9/22/15.
//

#include <exception>
#include <cassert>
#include <map>
#include "SymbolTable.h"

template<typename TEntry>
void SymbolTable::insert (TEntry::Ptr &value)
{
    __table.insert(pair<string, TEntry>(value->token()->text(), value));
}

template<typename TEntry>
TEntry::Ptr SymbolTable::lookUp (string name)
{
    map<string, TEntry::Ptr>::const_iterator iter = __table.find(name);
    if (iter != __table.end ())
    {
        return iter->second;
    }
    return nullptr;
}

/*
SymEntry::Ptr VarSymbolTable::lookUp (string name)
{
    map<string, VarEntry::Ptr>::const_iterator iter = __var_table.find (name);
    if (iter != __var_table.end ()) {
        return iter->second;
    }
    return nullptr;
}

void VarSymbolTable::insert (SymEntry::Ptr & value)
{
    __var_table.insert (pair<string, VarEntry::Ptr>(value->token ()->text (), value));
}

SymEntry::Ptr TypeSymbolTable::lookUp (string name)
{
    map<string, TypeEntry::Ptr>::const_iterator iter = __type_table.find (name);
    if (iter != __type_table.end ()) {
        return iter->second;
    }
    return nullptr;
}

void TypeSymbolTable::insert (SymEntry::Ptr & value)
{
    __type_table.insert (pair<string, TypeEntry::Ptr>(value->token ()->text (), value));
}

SymEntry::Ptr FuncSymbolTable::lookUp (string name)
{
    map<string, FuncEntry::Ptr>::const_iterator iter = __func_table.find (name);
    if (iter != __func_table.end ()) {
        return iter->second;
    }
    return nullptr;
}

void FuncSymbolTable::insert (SymEntry::Ptr &value)
{
    __func_table.insert (pair<string, FuncEntry::Ptr>(value->token ()->text (), value));
}
*/

void Scope::addChild (Scope::Ptr child)
{
    __children.push_back (child);
}

void Scope::setParent (Scope * parent)
{
    __parent = parent;
}

VarEntry::Ptr Scope::lookUpVar (string name)
{
    VarEntry::Ptr p = varTable.lookUp (name);
    Scope * s = __parent;
    while (p == nullptr && s != nullptr) {
        p = s->varTable.lookUp (name);
        s = s->__parent;
    }
    return p;
}

TypeEntry::Ptr Scope::lookUpType (string name)
{
    TypeEntry::Ptr p = typeTable.lookUp (name);
    Scope * s = __parent;
    while (p == nullptr && s != nullptr)
    {
        p = s->typeTable.lookUp (name);
        s = s->__parent;
    }
    return p;
}

FuncEntry::Ptr Scope::lookUpFunc (string name)
{
    FuncEntry::Ptr p = funcTable.lookUp (name);
    Scope * s = __parent;
    while (p == nullptr && s != nullptr)
    {
        p = s->funcTable.lookUp (name);
        s = s->__parent;
    }
    return p;
}



ScopeTree::ScopeTree ()
{
    __cur_scope = __global_scope;
    __cur_index = 0;
}

void ScopeTree::enterScope ()
{
    if (__cur_scope->children ().empty ()) {
        throw std::exception();
    }
    __stack_scope.push (__cur_scope);
    __stack_index.push (__cur_index);
    __cur_scope = __cur_scope->children ().at (__cur_index);
    __cur_index = 0;
}

void ScopeTree::closeScope ()
{
    assert(! __stack_scope.empty ());
    assert(! __stack_index.empty ());

    Scope::Ptr tmp_scope;
    int tmp_index;

    tmp_scope = __stack_scope.top ();
    tmp_index = __stack_index.top () + 1;
    __stack_scope.pop ();
    __stack_index.pop ();

    while (tmp_index >= tmp_scope->children ().size () && !__stack_scope.empty () && !__stack_index.empty ())
    {
        tmp_scope = __stack_scope.top ();
        tmp_index = __stack_index.top () + 1;
        __stack_scope.pop ();
        __stack_index.pop ();
    }
}