//
// Created by guoqi on 9/12/15.
//

#include <cassert>
#include "Parser.h"
#include "GlobalConfig.h"

Parser::Parser (Tokenizer &tokenizer)
    : __tokenizer(tokenizer)
{
}

Token::Ptr Parser::currentToken ()
{
    return __tokenizer.currentToken ();
}

Token::Ptr Parser::nextToken ()
{
    return __tokenizer.nextToken ();
}



void MutCParser::parse ()
{
    __ast.program (parseOutBlock ());
}


void MutCParser::decorateAST ()
{
    Stmt::Ptr p = __ast.program ();
    decorateStmts (p);
    __scope_tree.reset ();
}

void MutCParser::decorateStmts (Stmt::Ptr s)
{
    Stmt::Ptr tmp;
    while (s != nullptr)
    {
        s->setScope (__scope_tree.curScope ().get ());
        switch (s->type ())
        {
            case NodeType::FuncStmt:
                __scope_tree.enterScope ();
                decorateStmts (static_pointer_cast <FuncStmt> (s)->block_stmt);
                __scope_tree.closeScope ();
                break;
            case NodeType::IfStmt:
                tmp = s;
                while (tmp != nullptr)
                {
                    decorateExp (static_pointer_cast<IfStmt> (s)->condition);
                    __scope_tree.enterScope ();
                    decorateStmts (static_pointer_cast<IfStmt> (s)->block_stmt);
                    __scope_tree.closeScope ();
                    tmp = static_pointer_cast <IfStmt> (s)->clause_next;
                }
                break;
            case NodeType::WhileStmt:
                decorateExp (static_pointer_cast <WhileStmt> (s)->condition);
                __scope_tree.enterScope ();
                decorateStmts (static_pointer_cast <WhileStmt> (s)->block_stmt);
                __scope_tree.closeScope ();
                break;
            case NodeType::ForStmt:
                decorateStmts (static_pointer_cast <ForStmt> (s)->initial_stmt);
                decorateExp (static_pointer_cast <ForStmt> (s)->condition);
                decorateStmts (static_pointer_cast <ForStmt> (s)->modify_stmt);
                __scope_tree.enterScope ();
                decorateStmts (static_pointer_cast <ForStmt> (s)->block_stmt);
                __scope_tree.closeScope ();
                break;
            case NodeType::AssignmentStmt:
                static_pointer_cast <AssignmentStmt> (s)->sym_entry =
                    __scope_tree.curScope ()->lookUpVar (static_pointer_cast <AssignmentStmt> (s)->lvalue->text ());
                decorateExp (static_pointer_cast <AssignmentStmt> (s)->rvalue);
                break;
            case NodeType::ReturnStmt:
                decorateExp (static_pointer_cast <ReturnStmt> (s)->rslt);
                break;
            default:
                break;
        }
        s = s->next ();
    }
}

void MutCParser::decorateExp (Exp::Ptr exp)
{
    exp->setScope (__scope_tree.curScope ().get ());
    switch (exp->type ())
    {
        case NodeType::UnaryExp:
            decorateExp (static_pointer_cast <UnaryExp> (exp)->expr1);
            break;
        case NodeType::BinaryExp:
            decorateExp (static_pointer_cast <BinaryExp> (exp)->expr1);
            decorateExp (static_pointer_cast <BinaryExp> (exp)->expr2);
            break;
        case NodeType::ArrayExp:
            static_pointer_cast <ArrayExp> (exp)->sym_entry =
                __scope_tree.curScope ()->lookUpVar (static_pointer_cast <ArrayExp> (exp)->array->text ());
            for (auto i : static_pointer_cast <ArrayExp> (exp)->index_list)
            {
                decorateExp (i);
            }
            break;
        case NodeType::FuncExp:
            static_pointer_cast <FuncExp> (exp)->sym_entry =
                __scope_tree.curScope ()->lookUpFunc (static_pointer_cast <FuncExp> (exp)->func->text ());
            for (auto i : static_pointer_cast <FuncExp> (exp)->param_list)
            {
                decorateExp (i);
            }
            break;
        case NodeType::AtomicExp:
            static_pointer_cast <AtomicExp> (exp)->sym_entry =
                __scope_tree.curScope ()->lookUpVar (static_pointer_cast <AtomicExp> (exp)->var->text ());
            break;
        default:
            break;
    }
}


void MutCParser::typeChecking ()
{
    return check (__ast.program ());
}

void MutCParser::check (Stmt::Ptr s)
{
    bool ret;
    Stmt::Ptr tmp;
    while (s != nullptr)
    {
        switch (s->type ())
        {
            case NodeType::FuncStmt:
                check(static_pointer_cast <FuncStmt> (s)->block_stmt);
                break;
            case NodeType::IfStmt:
                tmp = s;
                while (tmp != nullptr) {
                    checkBoolean (static_pointer_cast<IfStmt> (s)->condition);
                    check(static_pointer_cast <IfStmt> (s)->block_stmt);
                    tmp = static_pointer_cast <IfStmt> (s)->clause_next;
                }
                break;
            case NodeType::WhileStmt:
                checkBoolean (static_pointer_cast<WhileStmt> (s)->condition);
                check(static_pointer_cast <WhileStmt> (s)->block_stmt);
                break;
            case NodeType::ForStmt:
                checkAssignment (static_pointer_cast <ForStmt> (s)->initial_stmt);
                checkBoolean (static_pointer_cast<ForStmt> (s)->condition);
                checkAssignment (static_pointer_cast <ForStmt> (s)->modify_stmt);
                break;
            case NodeType::AssignmentStmt:
                checkAssignment (s);
                break;
        }
    }
}

void MutCParser::checkExp (Exp::Ptr exp)
{
    switch (exp->type ())
    {
        case NodeType::UnaryExp:
            checkUnaryExp (exp);
            break;
        case NodeType::BinaryExp:
            checkBinaryExp (exp);
            break;
        case NodeType::ArrayExp:
            checkArrayExp (exp);
            break;
        case NodeType::FuncExp:
            checkFuncExp (exp);
            break;
        default:
            break;
    }
}

void MutCParser::checkUnaryExp (Exp::Ptr unary)
{
    Token::Ptr op = static_pointer_cast <UnaryExp> (unary)->op ();
    if (op->text () == "not")
    {
        checkBoolean (static_pointer_cast<UnaryExp> (unary)->expr1);
    }
    else if (op->text () == "+" || op->text () == "-")
    {
        checkNumber (static_pointer_cast <UnaryExp> (unary)->expr1);
    }
    else if (op->text () == "*")
    {
        checkPointer (static_pointer_cast <UnaryExp> (unary)->expr1);
    }
    else if (op->text () == "@")
    {
        checkVariable (static_pointer_cast <UnaryExp> (unary)->expr1);
    }
}

void MutCParser::checkBinaryExp (Exp::Ptr binary)
{
    Token::Ptr op = static_pointer_cast <BinaryExp> (binary)->op ();
    if (op->text () == "and" || op->text () == "or")
    {
        checkBoolean (static_pointer_cast <BinaryExp> (binary)->expr1);
        checkBoolean (static_pointer_cast <BinaryExp> (binary)->expr2);
    }
    else if (COMPOP.find (op->text ()) != COMPOP.end ())
    {
        checkExp (static_pointer_cast <BinaryExp> (binary)->expr1);
        checkExp (static_pointer_cast <BinaryExp> (binary)->expr2);
    }
    else if (COMPUTEOP.find (op->text ()) != COMPUTEOP.end ())
    {
        checkNumber (static_pointer_cast <BinaryExp> (binary)->expr1);
        checkNumber (static_pointer_cast <BinaryExp> (binary)->expr2);
    }
    else if (op->text () == ".")
    {

    }
    else if (op->text () == "->")
    {

    }
}

void MutCParser::checkArrayExp (Exp::Ptr array)
{
    for (auto i : static_pointer_cast <ArrayExp>(array)->index_list)
    {
        checkInteger (i);
    }
}

void MutCParser::checkFuncExp (Exp::Ptr func)
{
    for (auto i : static_pointer_cast <FuncExp>(func)->param_list)
    {
        checkExp (i);
        // TODO
    }
}

void MutCParser::checkBoolean (Exp::Ptr exp)
{
    const static Type * boolean = new Type(TypeInfo::Boolean);
    checkExp (exp);
    if (! exp->typeInfo (), boolean);
    {
        // TODO error handling: type mismatch
    }
}

void MutCParser::checkInteger (Exp::Ptr exp)
{
    // TODO
}

void MutCParser::checkNumber (Exp::Ptr exp)
{
    // TODO
}

void MutCParser::checkString (Exp::Ptr exp)
{
    // TODO
}

void MutCParser::checkPointer (Exp::Ptr exp)
{
    // TODO
}

void MutCParser::checkArray (Exp::Ptr array)
{
    // TODO
}

void MutCParser::checkStruct (Exp::Ptr exp)
{
    // TODO
}

void MutCParser::checkVariable (Exp::Ptr exp)
{
    // TODO
}

void MutCParser::checkAssignment (Stmt::Ptr assignment)
{
    SymEntry::Ptr entry = static_pointer_cast <AssignmentStmt> (assignment)->sym_entry;
    checkExp (static_pointer_cast <AssignmentStmt> (assignment)->rvalue);
    if (! compareType (static_pointer_cast <VarEntry> (entry)->typeInfo (),
        static_pointer_cast <AssignmentStmt> (assignment)->rvalue->typeInfo ()))
    {
        // TODO error handling: type mismatch
    }
}

bool MutCParser::compareType (Type *type1, Type *type2)
{
    // TODO
}


Stmt::Ptr MutCParser::parseOutBlock ()
{
    Stmt::Ptr root = make_shared <Stmt>();
    Stmt::Ptr p = root;

    root->next (nullptr);

    while(nextToken ()->type () != TokenType::Terminator)
    {
        Token::Ptr curToken = currentToken ();
        string text = curToken->text ();

        if (curToken->type () == TokenType::Keyword)
        {
            if (text == "import")
            {
                p->next (parseImport ());
            }
            else if (text == "export")
            {
                p->next (parseExport ());
            }
            else if (text == "decl")
            {
                parseDecl (__scope_tree.globalScope ());
                continue;
            }
            else if (text == "fn")
            {
                // first we need parse function header and then we should parse function body.
                parseFunc (__scope_tree.globalScope ());
                p->next (parseFunc ());
            }
            else if (text == "let")
            {
                parseLet (__scope_tree.globalScope ());
                continue;
            }
            else
            {
                // TODO error handling: unexpected keyword
            }
        }
        else
        {
            // TODO error handling: unexpected token out of block
        }

        p = p->next ();
        p->next (nullptr);
    }

    return root->next ();
}



Stmt::Ptr MutCParser::parseImport ()
{
    Stmt::Ptr p = make_shared <ImportStmt> ();

    assert(currentToken ()->type () == TokenType::Keyword && currentToken ()->text () == "import");
    nextToken ();   // skip import
    toolNameList<ImportStmt> (p, ".", ";");
    assert(currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == ";");
    return p;
}

Stmt::Ptr MutCParser::parseExport ()
{
    Stmt::Ptr p = make_shared <ExportStmt> ();

    assert(currentToken ()->type () == TokenType::Keyword && currentToken ()->text () == "export");
    assert(nextToken ()->type () == TokenType::Symbol && currentToken ()->text () == "{");
    nextToken ();   // skip {
    toolNameList<ExportStmt> (p, ",", "}");
    assert(currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == "}");
    assert(nextToken ()->type () == TokenType::Symbol && currentToken ()->text () == ";");
    return p;
}


Stmt::Ptr MutCParser::parseFunc ()
{
    Stmt::Ptr p = make_shared <FuncStmt> ();

    assert (currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == "{");
    nextToken ();
    Scope::Ptr scope = make_shared <Scope> ();
    static_pointer_cast <FuncStmt> (p)->block_stmt = parseInBlock (scope);
    __scope_tree.globalScope ()->addChild (scope);
    scope->setParent (__scope_tree.globalScope ().get ());
    assert (currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == "}");
    return p;
}

Stmt::Ptr MutCParser::parseInBlock (Scope::Ptr curScope)
{
    Stmt::Ptr root = make_shared <Stmt> ();
    Stmt::Ptr p = root;

    root->next (nullptr);

    while (currentToken ()->type () != TokenType::Terminator
        && ! (currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == "}"))
    {
        Token::Ptr curToken = currentToken ();
        string text = curToken->text ();

        if (curToken->type () == TokenType::Keyword)
        {
            if (text == "if")
            {
                p->next (parseIfAndClause (curScope, "if"));
                p = p->next ();
                p->next (nullptr);
                continue;
            }
            else if (text == "while")
            {
                p->next (parseWhile (curScope));
            }
            else if (text == "for")
            {
                p->next (parseFor (curScope));
            }
            else if (text == "return")
            {
                p->next (parseReturn ());
            }
            else if (text == "let")
            {
                parseLet (curScope);
                nextToken ();
                continue;
            }
            else
            {
                // TODO error handling: unexpected keyword
            }
        }
        else if (curToken->type () == TokenType::Identifier)
        {
            // asume it is an assignment statement
            p->next (parseAssignment ());
            p = p->next ();
            p->next (nullptr);
            continue;
        }
        else
        {
            // TODO error handling: unexpected token in block
        }
        nextToken ();
        p = p->next ();
        p->next (nullptr);
    }

    return root->next ();
}

// 该函数调用之后会导致token往前读入一个，因此在外围不用再读取下一个token
Stmt::Ptr MutCParser::parseIfAndClause (Scope::Ptr curScope, string keyword)
{
    Stmt::Ptr p = make_shared <IfStmt> ();
    assert(currentToken ()->type () == TokenType::Keyword && currentToken ()->text () == keyword);
    assert(nextToken ()->type () == TokenType::Symbol && currentToken ()->text () == "(");
    nextToken ();
    static_pointer_cast <IfStmt> (p)->condition = parseExp ();
    assert(currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == ")");
    assert(nextToken ()->type () == TokenType::Symbol && currentToken ()->text () == "{");
    Scope::Ptr scope = make_shared <Scope> ();
    static_pointer_cast <IfStmt> (p)->block_stmt = parseInBlock (scope);
    curScope->addChild (scope);
    scope->setParent (curScope.get ());
    assert(currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == "}");
    nextToken ();
    if (currentToken ()->type () == TokenType::Keyword && currentToken ()->text () == "elif")
    {
        static_pointer_cast <IfStmt> (p)->clause_next = parseIfAndClause (curScope, "elif");
    }
    else if (currentToken ()->type () == TokenType::Keyword && currentToken ()->text () == "else")
    {
        static_pointer_cast <IfStmt> (p)->clause_next = parseElse (curScope);
    }
    return p;
}

Stmt::Ptr MutCParser::parseElse (Scope::Ptr curScope)
{
    Stmt::Ptr p = make_shared <IfStmt> ();
    assert (currentToken ()->type () == TokenType::Keyword && currentToken ()->text () == "else");
    assert (nextToken ()->type () == TokenType::Symbol && currentToken ()->text () == "{");
    nextToken ();
    static_pointer_cast <IfStmt> (p)->condition = nullptr;
    Scope::Ptr scope = make_shared <Scope> ();
    static_pointer_cast <IfStmt> (p)->block_stmt = parseInBlock (scope);
    curScope->addChild (scope);
    scope->setParent (curScope.get ());
    assert (currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == "}");
    return p;
}

Stmt::Ptr MutCParser::parseWhile (Scope::Ptr curScope)
{
    Stmt::Ptr p = make_shared <WhileStmt> ();
    assert(currentToken ()->type () == TokenType::Keyword && currentToken ()->text () == "while");
    assert(nextToken ()->type () == TokenType::Symbol && currentToken ()->text () == "(");
    nextToken ();
    static_pointer_cast <WhileStmt> (p)->condition = parseExp ();
    assert(currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == ")");
    assert(nextToken ()->type () == TokenType::Symbol && currentToken ()->text () == "{");
    Scope::Ptr scope = make_shared <Scope> ();
    static_pointer_cast <WhileStmt> (p)->block_stmt = parseInBlock (scope);
    curScope->addChild (scope);
    scope->setParent (curScope.get ());
    assert(currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == "}");
    return p;
}

Stmt::Ptr MutCParser::parseFor (Scope::Ptr curScope)
{
    Stmt::Ptr p = make_shared <ForStmt> ();
    assert(currentToken ()->type () == TokenType::Keyword && currentToken ()->text () == "for");
    assert(nextToken ()->type () == TokenType::Symbol && currentToken ()->text () == "(");
    nextToken ();
    static_pointer_cast <ForStmt> (p)->initial_stmt = parseAssignment ();
    assert(currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == ";");
    nextToken ();
    static_pointer_cast <ForStmt> (p)->condition = parseExp ();
    assert(currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == ";");
    nextToken ();
    static_pointer_cast <ForStmt> (p)->modify_stmt = parseAssignment ();
    assert(currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == ")");
    assert(nextToken ()->type () == TokenType::Symbol && currentToken ()->text () == "{");
    Scope::Ptr scope = make_shared <Scope> ();
    static_pointer_cast <ForStmt> (p)->block_stmt = parseInBlock (scope);
    curScope->addChild (scope);
    scope->setParent (curScope.get ());
    assert(currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == "}");
    return p;
}

Stmt::Ptr MutCParser::parseReturn ()
{
    Stmt::Ptr p = make_shared <ReturnStmt> ();
    assert(currentToken ()->type () == TokenType::Keyword && currentToken ()->text () == "return");
    nextToken ();
    static_pointer_cast <ReturnStmt> (p)->rslt = parseExp ();
    assert(currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == ";");
    return p;
}

Stmt::Ptr MutCParser::parseAssignment ()
{
    Stmt::Ptr p = make_shared <AssignmentStmt> ();
    assert (currentToken ()->type () == TokenType::Identifier);
    static_pointer_cast <AssignmentStmt> (p)->lvalue = currentToken ();
    if (ASSIGNMENTOP.find (nextToken ()->text ()) == ASSIGNMENTOP.end ())
    {
        // TODO error handling: undefined assignment operator
    }
    static_pointer_cast <AssignmentStmt> (p)->op = currentToken ();
    nextToken ();
    static_pointer_cast <AssignmentStmt> (p)->rvalue = parseExp ();
    return p;
}

Exp::Ptr MutCParser::parseExp ()
{
    return parseExpB();
}

Exp::Ptr MutCParser::parseExpB ()
{
    Exp::Ptr p;
    if (currentToken ()->type () == TokenType::Keyword && currentToken ()->text () == "not")
    {
        p = make_shared <UnaryExp> (currentToken ());
        nextToken ();
        static_pointer_cast <UnaryExp> (p)->expr1 = parseExpB ();
    }
    else
    {
        Exp::Ptr expr1 = parseExpC ();
        if (currentToken ()->type () == TokenType::Keyword)
        {
            if (currentToken ()->text () == "and" || currentToken ()->text () == "or")
            {
                p = make_shared <BinaryExp> (currentToken ());
                nextToken ();
                static_pointer_cast <BinaryExp> (p)->expr1 = expr1;
                static_pointer_cast <BinaryExp> (p)->expr2 = parseExpB ();
            }
            else
            {
                // TODO error handling: expected logical operator
            }
        }
        else
        {
            p = expr1;
        }
    }
    return p;
}

Exp::Ptr MutCParser::parseExpC ()
{
    Exp::Ptr p;
    Exp::Ptr expr1 = parseExpD ();
    if (currentToken ()->type () == TokenType::Symbol)
    {
        if (COMPOP.find (currentToken ()->text ()) != COMPOP.end ())
        {
            p = make_shared <BinaryExp> (currentToken ());
            nextToken ();
            static_pointer_cast <BinaryExp> (p)->expr1 = expr1;
            static_pointer_cast <BinaryExp> (p)->expr2 = parseExpD ();
        }
        else
        {
            // TODO error handling: expected comparsion operator
        }
    }
    else
    {
        p = expr1;
    }
    return p;
}

Exp::Ptr MutCParser::parseExpD ()
{
    Exp::Ptr p;
    Exp::Ptr expr1 = parseExpE ();
    if (currentToken ()->type () == TokenType::Symbol)
    {
        if (currentToken ()->text () == "+" || currentToken ()->text () == "-")
        {
            p = make_shared <BinaryExp> (currentToken ());
            nextToken ();
            static_pointer_cast <BinaryExp> (p)->expr1 = expr1;
            static_pointer_cast <BinaryExp> (p)->expr2 = parseExpD ();
        }
        else
        {
            // TODO error handling: expected plus/minus operator
        }
    }
    else
    {
        p = expr1;
    }
    return p;
}

Exp::Ptr MutCParser::parseExpE ()
{
    Exp::Ptr p;
    Exp::Ptr expr1 = parseExpF ();
    if (currentToken ()->type () == TokenType::Symbol)
    {
        if (currentToken ()->text () == "*" || currentToken ()->text () == "/" || currentToken ()->text () == "%")
        {
            p = make_shared <BinaryExp> (currentToken ());
            nextToken ();
            static_pointer_cast <BinaryExp> (p)->expr1 = expr1;
            static_pointer_cast <BinaryExp> (p)->expr2 = parseExpE ();
        }
        else
        {
            // TODO error handling: expected multiplus/division/module operator
        }
    }
    else
    {
        p = expr1;
    }
    return p;
}

Exp::Ptr MutCParser::parseExpF ()
{
    Exp::Ptr p;
    if (currentToken ()->type () == TokenType::Symbol)
    {
        string text = currentToken ()->text ();
        if (text == "*" || text == "-" || text == "+" || text == "@")
        {
            p = make_shared <UnaryExp> (currentToken ());
            nextToken ();
            static_pointer_cast <UnaryExp> (p)->expr1 = parseExpG ();
        }
        else
        {
            // TODO error handling: unknown symbol
        }
    }
    else
    {
        p = parseExpG ();
    }
    return p;
}

Exp::Ptr MutCParser::parseExpG ()
{
    Exp::Ptr p;
    Exp::Ptr expr1 = parseAtomicTerm ();
    if (currentToken ()->type () == TokenType::Symbol)
    {
        if (currentToken ()->text () == "." || currentToken ()->text () == "->")
        {
            p = make_shared <BinaryExp> (currentToken ());
            nextToken ();
            static_pointer_cast <BinaryExp> (p)->expr1 = expr1;
            static_pointer_cast <BinaryExp> (p)->expr2 = parseExpG ();
        }
        else
        {
            // TODO error handling: unknown symbol
        }
    }
    else
    {
        p = expr1;
    }
    return p;
}

Exp::Ptr MutCParser::parseAtomicTerm ()
{
    Exp::Ptr p;
    if (currentToken ()->type () == TokenType::Integer
        || currentToken ()->type () == TokenType::Real
        || currentToken ()->type () == TokenType::String
        || currentToken ()->type () == TokenType::Keyword && (currentToken ()->text () == "true" || currentToken ()->text () == "false"))
    {
        p = make_shared <AtomicExp> (currentToken ());
        nextToken ();
    }
    else if (currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == "(")
    {
        nextToken ();
        p = parseExp ();
        assert (currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == ")");
    }
    else if (currentToken ()->type () == TokenType::Identifier)
    {
        Token::Ptr tmp = currentToken ();
        nextToken ();
        if (currentToken ()->type () == TokenType::Symbol)
        {
            if (currentToken ()->text () == "[")
            {
                p = make_shared<ArrayExp> (tmp);
                nextToken ();
                toolExpIndex (p);
                assert (currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == "]");
            }
            else if (currentToken ()->text () == "(")
            {
                p = make_shared<FuncExp> (tmp);
                nextToken ();
                if (currentToken ()->type () != TokenType::Symbol || currentToken ()->text () != ")")
                {
                    toolExpQ (p);
                }
            }
            else
            {
                // TODO error handling: unknown symbol
            }
        }
        else
        {
            p = make_shared <AtomicExp> (tmp);
        }
    }
    else
    {
        // TODO error handling: unexpected token
    }
    return p;
}

void MutCParser::toolExpQ (Exp::Ptr func)
{
    if (currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == ")")
    {
        return;
    }
    else if (! (currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == ","))
    {
        // TODO error handling: expect an expression or )
    }
    static_pointer_cast <FuncExp> (func)->param_list.push_back (parseExp ());
    toolExpQ (func);
}

void MutCParser::toolExpIndex (Exp::Ptr array)
{
    static_pointer_cast <ArrayExp> (array)->index_list.push_back (parseExp ());
    assert (currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == "]");
    nextToken ();
    if (currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == "[")
    {
        toolExpIndex (array);
    }
}

Type* MutCParser::parseType ()
{
    Type * p;
    if (currentToken ()->type () == TokenType::Keyword)
    {
        string text = currentToken ()->text ();
        if (text == "int")
        {
            p = new AtomicType(TypeInfo::Integer);
            p->next (nullptr);
        }
        else if (text == "real")
        {
            p = new AtomicType(TypeInfo::Real);
            p->next (nullptr);
        }
        else if (text == "bool")
        {
            p = new AtomicType(TypeInfo::Boolean);
            p->next (nullptr);
        }
        else if (text == "str")
        {
            p = new AtomicType(TypeInfo::String);
            p->next (nullptr);
        }
        else if (text == "ptr")
        {
            p = new AtomicType(TypeInfo::Pointer);
            assert(nextToken ()->type () == TokenType::Symbol && currentToken ()->text () == "@");
            nextToken ();
            p->next (parseType ());
        }
        else if (text == "array")
        {
            assert(nextToken ()->type () == TokenType::Symbol && currentToken ()->text () == "@");
            nextToken ();
            Type * t = parseType ();
            assert(currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == "[");
            assert(nextToken ()->type () == TokenType::Integer);
            p = new ArrayType(currentToken ()->value ().integer);
            p->next (t);
            assert(nextToken ()->type () == TokenType::Symbol && currentToken ()->text () == "]");
        }
        else
        {
            // TODO error handling: unexpected keyword
        }
    }
    else if (currentToken ()->type () == TokenType::Identifier)
    {
        p = new StructType(currentToken ()->text ());
    }
    else
    {
        // TODO error handling: unexpected token
    }
    nextToken ();
    return p;
}

void MutCParser::parseDecl (Scope::Ptr curScope)
{
    assert(currentToken ()->type () == TokenType::Keyword && currentToken ()->text () == "decl");
    TypeEntry::Ptr p = make_shared <TypeEntry> (nextToken ());
    nextToken ();
    toolParamList<TypeEntry> (p, ";", "}");
    assert(currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == "}");
    assert(nextToken ()->type () == TokenType::Symbol && currentToken ()->text () == ";");
    curScope->typeTable.insert (p);
}

void MutCParser::parseLet (Scope::Ptr curScope)
{
    assert(currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == "let");
    Token::Ptr name = nextToken ();
    assert(nextToken ()->type () == TokenType::Symbol && currentToken ()->text () == ":");
    nextToken ();
    VarEntry::Ptr p = make_shared <VarEntry> (name, parseType ());
    assert(currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == ";");
    curScope->varTable.insert (p);
}

void MutCParser::parseFunc (Scope::Ptr curScope)
{
    assert(currentToken ()->type () == TokenType::Keyword && currentToken ()->text () == "fn");
    FuncEntry::Ptr p = make_shared <FuncEntry> (nextToken ());
    assert(currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == "(");
    nextToken ();
    toolParamList<FuncEntry> (p, ",", ")");
    assert(currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == ")");
    assert (nextToken ()->type () == TokenType::Symbol && currentToken ()->text () == ":");
    p->ret_type = parseType ();
    curScope->funcTable.insert (p);
}

template <typename T>
void MutCParser::toolParamList (SymEntry::Ptr entry, string delimiter, string end_word)
{
    if (currentToken ()->type () == TokenType::Identifier)
    {
        Token::Ptr param = currentToken ();
        assert(nextToken ()->type () == TokenType::Symbol && currentToken ()->text () == ":");
        nextToken (); // consume :
        VarEntry::Ptr p = make_shared <VarEntry> (param, parseType ());
        static_pointer_cast <T> (entry)->memlist.insert (p);
    }
    else if (currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == end_word)
    {
        return;
    }
    else if (! (currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == delimiter))
    {
        // TODO error handling: expect a id or a delimiter
    }
    nextToken ();
    toolParamList<T> (entry, delimiter, end_word);
}

template <typename T>
void MutCParser::toolNameList (Stmt::Ptr stmt, string delimiter, string end_word)
{
    if (currentToken ()->type () == TokenType::Identifier)
    {
        static_pointer_cast <T> (stmt)->token_list.push_back (currentToken ());
    }
    else if (currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == end_word)
    {
        return;
    }
    else if (! (currentToken ()->type () == TokenType::Symbol && currentToken ()->text () == delimiter))
    {
        // TODO error handling: expect a id or a delimiter
    }
    nextToken ();
    toolNameList<T> (stmt, delimiter, end_word);
}


