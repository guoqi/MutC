//
// Created by guoqi on 9/22/15.
//

#include <exception>
#include <cassert>
#include <map>
#include "SymbolTable.h"



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

Scope::~Scope ()
{
    for (auto & i : __children) {
        delete i;
    }
}

void Scope::addChild (Scope * child)
{
    __children.push_back (child);
}

void Scope::setParent (Scope * parent)
{
    __parent = parent;
}

VarEntry * Scope::lookUpVar (string name)
{
    VarEntry * p = varTable.lookUp (name);
    Scope * s = __parent;
    while (p == nullptr && s != nullptr) {
        p = s->varTable.lookUp (name);
        s = s->__parent;
    }
    return p;
}

TypeEntry * Scope::lookUpType (string name)
{
    TypeEntry * p = typeTable.lookUp (name);
    Scope * s = __parent;
    while (p == nullptr && s != nullptr)
    {
        p = s->typeTable.lookUp (name);
        s = s->__parent;
    }
    return p;
}

FuncEntry * Scope::lookUpFunc (string name)
{
    FuncEntry * p = funcTable.lookUp (name);
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
    __global_scope = make_shared <Scope> ();
    __cur_scope = __global_scope.get ();
    __cur_index = 0;
}

void ScopeTree::enterScope ()
{
    if (__cur_scope->children ().empty ()) {
        // return;
        throw std::exception();
    }
    __stack_scope.push (__cur_scope);
    __stack_index.push (__cur_index);
    __cur_scope = __cur_scope->children ().at (__cur_index);
    __cur_index = 0;
}

void ScopeTree::closeScope ()
{
    /*
    if (__stack_scope.empty () || __stack_index.empty ()) {
        return;
    }
     */
    assert (! __stack_scope.empty ());
    assert (! __stack_index.empty ());

    if (__cur_index >= __cur_scope->children ().size () && ! __stack_scope.empty () && ! __stack_index.empty ())
    {
        __cur_scope = __stack_scope.top ();
        __cur_index = __stack_index.top () + 1;
        __stack_scope.pop ();
        __stack_index.pop ();
    }

    /*
    Scope * tmp_scope;
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

    __cur_scope = tmp_scope;
    __cur_index = tmp_index;
     */
}