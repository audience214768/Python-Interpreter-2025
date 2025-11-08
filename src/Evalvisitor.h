#pragma once
#include "Python3Parser.h"
#include <any>
#ifndef PYTHON_INTERPRETER_EVALVISITOR_H
#define PYTHON_INTERPRETER_EVALVISITOR_H


#include "Python3ParserBaseVisitor.h"
#include "int2048.h"
#include <functional>
#include <vector>
#include <string>
#include <map>

struct Arg {
  int index_;
  std::string name_;
  std::any value_;
  Arg() {}
  Arg(std::any value):value_(value) {}
  Arg(int index, std::any value):index_(index), value_(value) {}
  Arg(std::string name, std::any value):name_(name), value_(value) {}
};

using Callable = std::function<std::any(const std::vector<Arg>&)>;

struct NoneType {};

struct DefineArg {
  std::string name_;
  std::any default_;
};

struct Function {
  Python3Parser::SuiteContext * suite_;
  std::vector<DefineArg> arglist_;
};

enum OperationType{kAdd, kMinus, kMulti, kIDiv, kDiv, kMod, kLess, kGreater, kEqual, kGreater_Equal, kLess_Equal, kNot_Equal};

enum ControlType{kBreak, kContinue, kReturn};

struct Control {
  ControlType type_;
  std::vector<std::any> value_;
};

class EvalVisitor : public Python3ParserBaseVisitor {
	// TODO: override all methods of Python3ParserBaseVisitor
private:
  std::vector<std::map<std::string, std::any>> variables_stack_;
  std::map<std::string, Callable> functions_;
  std::map<std::string, Function> user_functions_;
  std::any GetValue(std::any &);
  std::any Operation(std::any &, std::any& , OperationType);
public:
  EvalVisitor();
  std::any visitFile_input(Python3Parser::File_inputContext *);
  std::any visitFuncdef(Python3Parser::FuncdefContext *);
  std::any visitParameters(Python3Parser::ParametersContext *);
  std::any visitTypedargslist(Python3Parser::TypedargslistContext *);
  std::any visitTfpdef(Python3Parser::TfpdefContext *);
  std::any visitStmt(Python3Parser::StmtContext *);
  std::any visitSimple_stmt(Python3Parser::Simple_stmtContext *);
  std::any visitSmall_stmt(Python3Parser::Small_stmtContext *);
  std::any visitFlow_stmt(Python3Parser::Flow_stmtContext *);
  std::any visitReturn_stmt(Python3Parser::Return_stmtContext *);
  std::any visitExpr_stmt(Python3Parser::Expr_stmtContext *);
  std::any visitAugassign(Python3Parser::AugassignContext *);
  std::any visitCompound_stmt(Python3Parser::Compound_stmtContext *);
  std::any visitIf_stmt(Python3Parser::If_stmtContext *);
  std::any visitWhile_stmt(Python3Parser::While_stmtContext *);
  std::any visitSuite(Python3Parser::SuiteContext *);
  std::any visitTest(Python3Parser::TestContext *);
  std::any visitOr_test(Python3Parser::Or_testContext *);
  std::any visitAnd_test(Python3Parser::And_testContext *);
  std::any visitNot_test(Python3Parser::Not_testContext *);
  std::any visitComparison(Python3Parser::ComparisonContext *);
  std::any visitComp_op(Python3Parser::Comp_opContext *);
  std::any visitArith_expr(Python3Parser::Arith_exprContext *);
  std::any visitAddorsub_op(Python3Parser::Addorsub_opContext *);
  std::any visitTerm(Python3Parser::TermContext *);
  std::any visitMuldivmod_op(Python3Parser::Muldivmod_opContext *);
  std::any visitFactor(Python3Parser::FactorContext *);
  std::any visitAtom_expr(Python3Parser::Atom_exprContext *);
  std::any visitTrailer(Python3Parser::TrailerContext *);
  std::any visitAtom(Python3Parser::AtomContext *);
  std::any visitFormat_string(Python3Parser::Format_stringContext *);
  std::any visitTestlist(Python3Parser::TestlistContext *);
  std::any visitArglist(Python3Parser::ArglistContext *);
  std::any visitArgument(Python3Parser::ArgumentContext *);
};


#endif//PYTHON_INTERPRETER_EVALVISITOR_H
