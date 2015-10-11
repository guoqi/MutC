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

    virtual typename TEntry::Ptr lookUp(string name);
    virtual void insert(typename TEntry::Ptr & value);

    inline vector<typename TEntry::Ptr> & table() { return __table; };

private:
    vector<typename TEntry::Ptr>    __table;
};


typedef SymbolTable<VarEntry>  VarSymbolTable;
typedef SymbolTable<TypeEntry> TypeSymbolTable;
typedef SymbolTable<FuncEntry> FuncSymbolTable;

class SymEntry
{
public:
    typedef shared_ptr<SymEntry> Ptr;
    SymEntry(Token::Ptr token, EntryType type): __entry_type(type), __token(token), __address(0) {}
    virtual ~SymEntry() {}

    inline Token::Ptr token() { return __token; }
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

    void addChild(Scope::Ptr child);
    void setParent(Scope * parent);

    TypeEntry::Ptr lookUpType(string name);
    VarEntry::Ptr  lookUpVar(string name);
    FuncEntry::Ptr lookUpFunc(string name);

    inline const vector<Scope::Ptr> & children() const { return __children; }

    inline const Scope * parent() const { return __parent; }

    // self-defined type symbol table
    TypeSymbolTable     typeTable;
    // variable symbol table
    VarSymbolTable      varTable;
    // function symbol table
    FuncSymbolTable     funcTable;

private:
    vector<Scope::Ptr>  __children;
    Scope   *           __parent;
};


class ScopeTree
{
public:
    ScopeTree();
    void enterScope();
    void closeScope();

    inline Scope::Ptr curScope() { return __cur_scope; }

    inline Scope::Ptr globalScope() { return __global_scope; }
    inline Scope::Ptr globalScope(Scope::Ptr & global) { __global_scope = global; return __global_scope; }

    inline void reset() { __cur_scope = __global_scope; __cur_index = 0; }

private:
    Scope::Ptr              __global_scope;
    Scope::Ptr              __cur_scope;    // current scope ptr
    int                     __cur_index;    // current scope index
    stack<Scope::Ptr>       __stack_scope;
    stack<int>              __stack_index;
};


#endif //MUTCC_SYMBOLTABLE_H
