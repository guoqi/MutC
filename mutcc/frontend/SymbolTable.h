//
// Created by guoqi on 9/22/15.
//

#ifndef MUTCC_SYMBOLTABLE_H
#define MUTCC_SYMBOLTABLE_H

#include <map>
#include <memory>
#include <vector>
#include <stack>
#include <string>
#include <cstdint>
#include "Token.h"
#include "TypeInfo.h"

using namespace std;

enum class EntryType {
    // symbol table entry type
    Type,
    Variable,
    Function
};


class VarEntry;
class TypeEntry;
class FuncEntry;
class Type;

template<typename TEntry>
class SymbolTable
{
public:
    SymbolTable() {}
    virtual ~SymbolTable () {}

    TEntry * lookUp (string name);
    void insert(typename TEntry::Ptr value);

    inline vector<typename TEntry::Ptr> & table() { return __table; };

private:
    vector<typename TEntry::Ptr>    __table;
};

template<typename TEntry>
void SymbolTable<TEntry>::insert (typename TEntry::Ptr value)
{
    __table.push_back (value);
}

template<typename TEntry>
TEntry * SymbolTable<TEntry>::lookUp (string name)
{
    for (auto & i : __table) {
        if (i->token()->text() == name) {
            return i.get();
        }
    }
    return nullptr;
}


typedef SymbolTable<VarEntry>  VarSymbolTable;
typedef SymbolTable<TypeEntry> TypeSymbolTable;
typedef SymbolTable<FuncEntry> FuncSymbolTable;

class SymEntry
{
public:
    typedef shared_ptr<SymEntry> Ptr;
    SymEntry(Token::Ptr token, EntryType type): __entry_type(type), __token(token), __address(0) {}
    virtual ~SymEntry() {}

    inline Token::Ptr & token() { return __token; }
    inline EntryType entryType() { return __entry_type; }

    inline void address(uint64_t address) { __address = address; }
    inline uint64_t address() { return __address; }

private:
    Token::Ptr      __token;
    EntryType       __entry_type;
    uint64_t        __address;
};

class VarEntry: public SymEntry
{
public:
    typedef shared_ptr<VarEntry> Ptr;
    VarEntry(Token::Ptr token, Type * typeInfo): SymEntry(token, EntryType::Variable), __type_info(typeInfo) {}
    virtual ~VarEntry () {}

    inline Type * typeInfo() { return __type_info; }

private:
    Type  *   __type_info;
};

class TypeEntry: public SymEntry
{
public:
    typedef shared_ptr<TypeEntry> Ptr;
    TypeEntry(Token::Ptr token): SymEntry(token, EntryType::Type) {}
    virtual ~TypeEntry () {}

    VarSymbolTable                 memlist; // member map
};

class FuncEntry: public SymEntry
{
public:
    typedef shared_ptr<FuncEntry> Ptr;
    FuncEntry(Token::Ptr token): SymEntry(token, EntryType::Function) {}
    virtual ~FuncEntry () {}

    VarSymbolTable                  memlist; // parameter map
    Type    *                       ret_type;   // return type;
};



/*
class VarSymbolTable: public SymbolTable
{
public:
    virtual ~VarSymbolTable () {}

    virtual SymEntry::Ptr lookUp(string name);
    virtual void insert(SymEntry::Ptr & value);

private:
    map<string, VarEntry::Ptr>   __var_table;
};

class TypeSymbolTable: public SymbolTable
{
public:
    virtual ~TypeSymbolTable () {}

    virtual SymEntry::Ptr lookUp(string name);
    virtual void insert(SymEntry::Ptr & value);

private:
    map<string, TypeEntry::Ptr>  __type_table;
};

class FuncSymbolTable: public SymbolTable
{
public:
    virtual ~FuncSymbolTable () {}

    virtual SymEntry::Ptr lookUp(string name);
    virtual void insert(SymEntry::Ptr & value);

private:
    map<string, FuncEntry::Ptr>  __func_table;
};
 */


// scope
class Scope
{
public:
    typedef shared_ptr<Scope> Ptr;

    ~Scope ();

    void addChild(Scope * child);
    void setParent(Scope * parent);

    TypeEntry * lookUpType (string name);
    VarEntry * lookUpVar (string name);
    FuncEntry * lookUpFunc (string name);

    inline const vector<Scope *> & children() const { return __children; }

    inline const Scope * parent() const { return __parent; }

    // self-defined type symbol table
    TypeSymbolTable     typeTable;
    // variable symbol table
    VarSymbolTable      varTable;
    // function symbol table
    FuncSymbolTable     funcTable;

private:
    vector<Scope *>     __children;
    Scope   *           __parent;
};


class ScopeTree
{
public:
    ScopeTree();
    void enterScope();
    void closeScope();

    inline Scope * curScope() { return __cur_scope; }

    inline Scope::Ptr & globalScope() { return __global_scope; }
    inline Scope::Ptr & globalScope(Scope::Ptr & global) { __global_scope = global; __cur_scope = __global_scope.get (); __cur_index = 0; return __global_scope; }

    inline void reset() { __cur_scope = __global_scope.get (); __cur_index = 0; }

private:
    Scope::Ptr              __global_scope;
    Scope *                 __cur_scope;    // current scope ptr
    int                     __cur_index;    // current scope index
    stack<Scope *>          __stack_scope;
    stack<int>              __stack_index;
};


#endif //MUTCC_SYMBOLTABLE_H
