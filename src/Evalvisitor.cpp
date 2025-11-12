#include "Evalvisitor.h"
#include "Python3Parser.h"
#include "Python3ParserVisitor.h"
#include "int2048.h"
#include <any>
#include <cstdarg>
#include <cstddef>
#include <string>

EvalVisitor::EvalVisitor() {
  auto print = [this](const std::vector<Arg> &arglist) {
    //std::cerr << arglist.size() << std::endl;
    for (int i = 0; i < arglist.size(); i++) {
      auto data = arglist[i].value_;
      data = GetValue(data);
      if (auto num = std::any_cast<sjtu::int2048>(&data)) {
        //std::cerr << 1 << std::endl;
        num->print();
      }
      if(auto num = std::any_cast<double>(&data)) {
        //std::cerr << 1 << std::endl;
        printf("%.6lf", *num);
      }
      if (auto string_vector = std::any_cast<std::vector<std::string>>(&data)) {
        // std::cerr << i << std::endl;
        for (auto string : *string_vector) {
          printf("%s", string.c_str());
        }
      }
      if (auto flag = std::any_cast<bool>(&data)) {
        printf("%s", (*flag) ? "True" : "False");
      }
      if (auto none = std::any_cast<NoneType>(&data)) {
        printf("%s", "None");
      }
      if (i != arglist.size() - 1) {
        printf(" ");
      }
    }
    printf("\n");
    return std::any();
  };
  auto to_int = [this](const std::vector<Arg> &arglist) {
    if(arglist.size() != 1) {
      throw " only converse 1 element";
    }
    auto data = arglist[0].value_;
    sjtu::int2048 int_num;
    if(auto num = std::any_cast<sjtu::int2048>(&data)) {
      int_num = *num;
    }
    if(auto num = std::any_cast<double>(&data)) {
      //std::cerr << *num << std::endl;
      int_num = *num;
    }
    if(auto str = std::any_cast<std::vector<std::string>>(&data)) {
      std::string str_total;
      for(std::string s : *str) {
        str_total += s;
      }
      int_num.read(str_total);
      //std::cerr << str_total << std::endl;
    }
    if(auto flag = std::any_cast<bool>(&data)) {
      //std::cerr << 1 << std::endl;
      int_num = (*flag) ? 1 : 0;
    }
    //std::cerr << int_num << std::endl;
    return std::any(int_num);
  };
  auto to_double = [this](const std::vector<Arg> &arglist) {
    double double_num = 0;
    if(arglist.size() != 1) {
      throw "only converse 1 element";
    }
    auto data = arglist[0].value_;
    if(auto num = std::any_cast<double>(&data)) {
      double_num = *num;
    }
    if(auto num = std::any_cast<sjtu::int2048>(&data)) {
      double_num = num->ToDouble();
    }
    
    if(auto str = std::any_cast<std::vector<std::string>>(&data)) {
      std::string string_num;
      for(std::string s : *str) {
        string_num += s;
      }
      int sign = 1;
      if(string_num[0] == '-') {
        sign = -1;
      }
      int index = string_num.find('.');
      if(index == std::string::npos) {
          index = string_num.length();
      }
      //std::cerr << index << std::endl;
      for(int i = (sign == 1 ? 0 : 1);i < index; i++) {
        if (string_num[i] >= '0' && string_num[i] <= '9') {
          double_num = double_num * 10 + (string_num[i] - '0');
        } else {
          throw "this is not a number";
        }
      }
      double base = 0.1;
      for(int i = index + 1; i < string_num.length(); base /= 10, i++) {
        if(string_num[i] >= '0' && string_num[i] <= '9') {
          double_num += base * (string_num[i] - '0');
        } else {
          throw "this is not a number";
        }
      }
      double_num *= sign;
      //std::cerr << double_num << std::endl;
    }
    if(auto flag = std::any_cast<bool>(&data)) {
      if(*flag) {
        double_num = 1;
      } else {
        double_num = 0;
      }
    }
    return std::any(double_num);
  };
  auto to_string  = [this](const std::vector<Arg> &arglist) {
    if(arglist.size() != 1) {
      throw "only converse 1 element";
    }
    auto data = arglist[0].value_;
    std::vector<std::string> str;
    if(auto str1 = std::any_cast<std::vector<std::string>>(&data)) {
      str = *str1;
    }
    if(auto num = std::any_cast<sjtu::int2048>(&data))  {
      //std::cerr << *num << std::endl;
      str.push_back(num->ToString());
    }
    if(auto num = std::any_cast<double>(&data)) {
      //std::cerr << *num << std::endl;
      str.push_back(std::to_string(*num));
    }
    if(auto flag = std::any_cast<bool>(&data)) {
      str.push_back((*flag) ? "True" : "False");
    }
    return std::any(str);
  };
  auto to_bool = [this](const std::vector<Arg> &arglist) {
    bool flag;
    if(arglist.size() != 1) {
      throw "only converse 1 element\n";
    }
    auto data = arglist[0].value_;
    if(auto flag1 = std::any_cast<bool>(&data)) {
      flag = *flag1;
    }
    if(auto num = std::any_cast<sjtu::int2048>(&data)) {
      flag = num->ToBool();
    }
    if(auto num = std::any_cast<double>(&data)) {
      if(fabs(*num) < 1e-10) {
        flag = 0;
      } else {
        flag = 1;
      }
    }
    if(auto str = std::any_cast<std::vector<std::string>>(&data)) {
      std::string str_total;
      for(std::string s : *str) {
        str_total += s;
      }
      //std::cerr << str_total << std::endl;
      if(str_total == "") {
        flag = 0;
      } else {
        flag = 1;
      }
    }
    if(auto none = std::any_cast<NoneType>(&data)) {
      flag = false;
    }
    return std::any(flag);
  };
  functions_["print"] = print;
  functions_["int"] = to_int;
  functions_["float"] = to_double;
  functions_["str"] = to_string;
  functions_["bool"] = to_bool;
}

std::any EvalVisitor::GetValue(std::any &data) {
  if (auto name = std::any_cast<std::string>(&data)) {
    if(variables_stack_.back().count(*name)) {
      return variables_stack_.back()[*name];
    }
    if(variables_stack_.front().count(*name)) {
      return variables_stack_.front()[*name];
    }
  }
  return data;
}

std::any EvalVisitor::Operation(std::any data1, std::any data2, OperationType type) {
  if (type == kDiv) {
    std::vector<Arg> arglist;
    arglist.push_back(Arg(data1));
    data1 = functions_["float"](arglist);
    arglist[0] = Arg(data2);
    data2 = functions_["float"](arglist);
  }
  if (data2.type() == typeid(double)) {
    std::vector<Arg> arglist;
    arglist.push_back(Arg(data1));
    data1 = functions_["float"](arglist);
  }
  if (data1.type() == typeid(double)) {
    std::vector<Arg> arglist;
    arglist.push_back(Arg(data2));
    data2 = functions_["float"](arglist);
  }
  if (data1.type() != typeid(std::vector<std::string>) && data2.type() == typeid(std::vector<std::string>)) {
    //std::cerr << 1 << std::endl;
    std::swap(data1, data2);
  }
  if (type == kEqual || type == kNot_Equal) {
    if (data1.type() == typeid(std::vector<std::string>)) {
      std::vector<Arg> arglist;
      arglist.push_back(Arg(data1));
      if (data2.type() == typeid(sjtu::int2048)) {
        try {
          data1 = functions_["int"](arglist);
        } catch(const char *err) {
          return false;
        }
      }
      if (data2.type() == typeid(bool)) {
        //std::cerr << 1 << std::endl;
        data1 = functions_["bool"](arglist);
      }
    }
  }
  if(data1.type() == typeid(bool)) {
    std::vector<Arg> arglist;
    arglist.push_back(Arg(data1));
    data1 = functions_["int"](arglist);
  }
  if(data2.type() == typeid(bool)) {
    //std::cerr << 1 << std::endl;
    std::vector<Arg> arglist;
    arglist.push_back(Arg(data2));
    data2 = functions_["int"](arglist);
  }
  if (auto num1 = std::any_cast<sjtu::int2048>(&data1), num2 = std::any_cast<sjtu::int2048>(&data2); num1 && num2) {

    //std::cerr << 1 << std::endl;
    //std::cerr << *num1 << " " << *num2 << " " << type << std::endl;
    switch (type) {
    case kAdd:
      return (*num1) + (*num2);
    case kMinus:
      return (*num1) - (*num2);
    case kMulti:
      return (*num1) * (*num2);
    case kIDiv:
      try {
        return (*num1) / (*num2);
      } catch(const char *err) {
        std::cerr << err << std::endl;
        return std::any(sjtu::int2048(0));
      }
    case kMod:
      try {
        return (*num1) % (*num2);
      } catch(const char *err) {
        std::cerr << err << std::endl;
        return std::any(sjtu::int2048(0));
      }
    case kLess:
      return (*num1) < (*num2);
    case kGreater:
      return (*num1) > (*num2);
    case kLess_Equal:
      return (*num1) <= (*num2);
    case kGreater_Equal:
      return (*num1) >= (*num2);
    case kEqual:
      return (*num1) == (*num2);
    case kNot_Equal:
      return (*num1) != (*num2);
    default:
      throw "this operation is invalid";
    }
  }
  if(auto num1 = std::any_cast<double>(&data1), num2 = std::any_cast<double>(&data2); num1 && num2) {
    //std::cerr << 2 << std::endl;
    switch (type) {
    case kAdd:
      return (*num1) + (*num2);
    case kMinus:
      return (*num1) - (*num2);
    case kMulti:
      return (*num1) * (*num2);
    case kDiv:
      if(fabs(*num2) < 1e-10) {
        std::cerr << "divide a zero" << std::endl;
        return std::any(0);
      }
      return (*num1) / (*num2);
    case kIDiv:
      if(fabs(*num2) < 1e-10) {
        std::cerr << "divide a zero" << std::endl;
        return std::any(0);
      }
      return sjtu::int2048(floor((*num1) / (*num2)));
    case kMod:
      if(fabs(*num2) < 1e-10) {
        std::cerr << "divide a zero" << std::endl;
        return std::any(0);
      }
      return ((*num1) - floor((*num1) / (*num2)) * (*num2));
    case kLess:
      return (*num1) < (*num2);
    case kGreater:
      return (*num1) > (*num2);
    case kLess_Equal:
      return (*num1) <= (*num2);
    case kGreater_Equal:
      return (*num1) >= (*num2);
    case kEqual:
      return fabs((*num1) - (*num2)) < 1e-10;
    case kNot_Equal:
      return fabs((*num1) - (*num2)) > 1e-10;
    default:
      throw "this operation is invalid\n";
    }
  }
  if (auto string1 = std::any_cast<std::vector<std::string>>(&data1), string2 = std::any_cast<std::vector<std::string>>(&data2); string1 && string2) {
    //std::cerr << 3 << std::endl;
    std::string str1;
    for(auto string : *string1) {
      str1 += string;
    }
    *string1 = std::vector<std::string>(1, str1);
    std::string str2;
    for(auto string : *string2) {
      str2 += string;
    }
    *string2 = std::vector<std::string>(1, str2);
    std::vector<std::string> data(*string1);
    switch (type) {
    case kAdd:
      data.insert(data.end(), string2->begin(), string2->end());
      return data;
    case kLess:
      return str1 < str2;
    case kGreater:
      return str1 > str2;
    case kEqual:
      return str1 == str2;
    case kLess_Equal:
      return str1 <= str2;
    case kGreater_Equal:
      return str1 >= str2;
    case kNot_Equal:
      return str1 != str2;
    default:
      throw "this operation is invalid\n";
    }
  }
  if (data1.type() == typeid(std::vector<std::string>) &&
      data2.type() == typeid(sjtu::int2048)) {
      //std::cerr << 4 << std::endl;
    auto string = std::any_cast<std::vector<std::string>>(data1);
    auto num = std::any_cast<sjtu::int2048>(data2);
    std::vector<std::string> data;
    //std::cerr << num << std::endl;
    std::string str1;
    for(auto str : string) {
      str1 += str;
    }
    string = std::vector<std::string>(1, str1);
    for (; num > 0; num = num - 1) {
      // std::cerr << data.size() << std::endl;
      data.insert(data.end(), string.begin(), string.end());
    }
    return data;
  }
  throw "unbelievable operation\n";
}

std::any EvalVisitor::visitFile_input(Python3Parser::File_inputContext *ctx) {
  std::map<std::string, std::any> variables;
  variables_stack_.push_back(variables);
  auto stmt_vector = ctx->stmt();
  try {
    for(auto stmt : stmt_vector) {
      visit(stmt);
    }
  } catch (const char *err) {
    std::cerr << err << std::endl;
    exit(1);
  }
  return std::any();
}

std::any EvalVisitor::visitFuncdef(Python3Parser::FuncdefContext *ctx) {
  //std::cerr << "funcdef" << std::endl;
  std::string name = ctx->NAME()->toString();
  Function function;
  auto ret = visit(ctx->parameters());
  if(auto arglist = std::any_cast<std::vector<DefineArg>>(&ret)) {
    function.arglist_ = *arglist;
  }
  function.suite_ = ctx->suite();
  user_functions_[name] = function;
  return std::any();
}

std::any EvalVisitor::visitParameters(Python3Parser::ParametersContext *ctx) {
  if(ctx->typedargslist()) {
    return visit(ctx->typedargslist());
  }
  return std::any(std::vector<DefineArg>());
}

std::any EvalVisitor::visitTypedargslist(Python3Parser::TypedargslistContext *ctx) {
  auto tfp_vector = ctx->tfpdef();
  auto test_vector = ctx->test();
  std::vector<DefineArg> arglist;
  for(int i = 0; i < tfp_vector.size() - test_vector.size(); i++) {
    DefineArg argument;
    auto ret = visit(tfp_vector[i]);
    if(auto name = std::any_cast<std::string>(&ret)) {
      argument.name_ = *name;
    }
    argument.default_ = std::any();
    arglist.push_back(argument);
  }
  for(int i = 0; i < test_vector.size(); i++) {
    DefineArg argument;
    auto ret = visit(tfp_vector[i + tfp_vector.size() - test_vector.size()]);
    if(auto name = std::any_cast<std::string>(&ret)) {
      argument.name_ = *name;
    }
    auto ret1 = visit(test_vector[i]);
    argument.default_ = ret1;
    arglist.push_back(argument);
  }
  return arglist;
}

std::any EvalVisitor::visitTfpdef(Python3Parser::TfpdefContext *ctx) {
  return ctx->NAME()->toString();
}

std::any EvalVisitor::visitStmt(Python3Parser::StmtContext *ctx) {
  if(ctx->simple_stmt()) {
    return visit(ctx->simple_stmt());
  } 
  if(ctx->compound_stmt()){
    return visit(ctx->compound_stmt());
  }
  throw "simple or compound";
}

std::any EvalVisitor::visitSimple_stmt(Python3Parser::Simple_stmtContext *ctx) {
  auto ret = visit(ctx->small_stmt());
  /*if(auto control = std::any_cast<Control>(&ret)) {
    std::cerr << control->type_ << std::endl;
  }*/
  return ret;
}

std::any EvalVisitor::visitSmall_stmt(Python3Parser::Small_stmtContext *ctx) {
  if(ctx->expr_stmt()) {
    return visit(ctx->expr_stmt());
  }
  if(ctx->flow_stmt()) {
    return visit(ctx->flow_stmt());
  }
  throw "small : not expr nor flow";
}

std::any EvalVisitor::visitFlow_stmt(Python3Parser::Flow_stmtContext *ctx) {
  Control control;
  if(ctx->break_stmt()) {
    control.type_ = kBreak;
  }
  if(ctx->continue_stmt()) {
    control.type_ = kContinue;
  }
  if(ctx->return_stmt()) {
    control.type_ = kReturn;
    auto ret = visit(ctx->return_stmt());
    if(auto testlist = std::any_cast<std::vector<std::any>>(&ret)) {
      for(auto test : *testlist) {
        auto value = GetValue(test);
        control.value_.push_back(GetValue(test));
        /*if(auto num = std::any_cast<sjtu::int2048>(&value)) {
          std::cerr << *num << std::endl;
        }*/
      }
    }
    /*if(auto num = std::any_cast<sjtu::int2048>(&control.value_)) {
      std::cerr << *num << std::endl;
    }*/
  }
  return control;
}

std::any EvalVisitor::visitReturn_stmt(Python3Parser::Return_stmtContext *ctx) {
  if(ctx->testlist()) {
    return visit(ctx->testlist());
  }
  return std::any();
}

std::any EvalVisitor::visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) {//only for =
  //std::cerr << "expr" << std::endl;
  auto testlist_vector = ctx->testlist();
  auto ret = visit(testlist_vector.back());
  auto testlist = std::any_cast<std::vector<std::any>>(ret);
  for(auto &test : testlist) {
    test = GetValue(test);
    /*if(auto num = std::any_cast<sjtu::int2048>(&test)) {
      std::cerr << *num << " ";
    }*/
  }
  //std::cerr << std::endl;
  // auto num = std::any_cast<sjtu::int2048>(&ret);
  // std::cerr << *num << std::endl;
  if (ctx->augassign()) {
    auto ret1 = visit(testlist_vector[0]);
    auto testlist1 = std::any_cast<std::vector<std::any>>(ret1);
    auto ret_op = visit(ctx->augassign());
    auto op = std::any_cast<OperationType>(ret_op);
    if(auto name = std::any_cast<std::string>(&testlist1[0])) {
      if(variables_stack_.back().count(*name)) {
        variables_stack_.back()[*name] = Operation(variables_stack_.back()[*name], testlist[0], op);
      } else {
        if(variables_stack_.front().count(*name)) {
          variables_stack_.front()[*name] = Operation(variables_stack_.front()[*name], testlist[0], op);
        } else {
          throw "can't find the variable\n";
        }
      }
    }
    return std::any();
  } else {
    for (int i = testlist_vector.size() - 2; i >= 0; i--) {
      auto ret1 = visit(testlist_vector[i]);
      if (auto testlist1 = std::any_cast<std::vector<std::any>>(&ret1)) {
        for (int i = 0; i < testlist1->size(); i++) {
          if (auto name = std::any_cast<std::string>(&(*testlist1)[i])) {
            // std::cerr << *name << std::endl;
            bool find = 0;
            if(variables_stack_.back().count(*name) != 0) {
              variables_stack_.back()[*name] = testlist[i];
            } else {
              if(variables_stack_.front().count(*name) != 0) {
                variables_stack_.front()[*name] = testlist[i];
              } else {
                variables_stack_.back()[*name] = testlist[i];
              }
            }
          }
        }
      }
    }
  }
  return std::any();
}

std::any EvalVisitor::visitAugassign(Python3Parser::AugassignContext *ctx) {
  if (ctx->ADD_ASSIGN()) {
    return kAdd;
  }
  if (ctx->DIV_ASSIGN()) {
    return kDiv;
  }
  if (ctx->IDIV_ASSIGN()) {
    return kIDiv;
  }
  if (ctx->MOD_ASSIGN()) {
    return kMod;
  }
  if (ctx->MULT_ASSIGN()) {
    return kMulti;
  }
  if (ctx->SUB_ASSIGN()) {
    return kMinus;
  }
  throw "unknown operation\n";
}

std::any EvalVisitor::visitCompound_stmt(Python3Parser::Compound_stmtContext *ctx) {
  if(ctx->if_stmt()) {
    return visit(ctx->if_stmt());
  }
  if(ctx->while_stmt()) {
    return visit(ctx->while_stmt());
  }
  if(ctx->funcdef()) {
    return visit(ctx->funcdef());
  }
  return std::any();
}

std::any EvalVisitor::visitIf_stmt(Python3Parser::If_stmtContext *ctx) {
  auto test_vector = ctx->test();
  auto ret_suite = ctx->suite();
  for (int i = 0; i < test_vector.size(); i++) {
    auto ret_test = visit(test_vector[i]);
    ret_test = GetValue(ret_test);
    if(ret_test.type() != typeid(bool)) {
      std::vector<Arg> arglist;
      arglist.push_back(Arg(ret_test));
      ret_test = functions_["bool"](arglist);
    }
    if (auto flag = std::any_cast<bool>(&ret_test)) {
      if (*flag) {
        // std::cerr << 1 << std::endl;
        auto ret = visit(ret_suite[i]);
        /*if (auto control = std::any_cast<Control>(&ret)) {
          if (control->type_ == kReturn) {
            std::cerr << "if find the return" << std::endl;
          }
        }*/
        return ret;
      }
    }
  }
  if(ctx->ELSE()) {
    return visit(ret_suite.back());
  }
  return std::any();
}

std::any EvalVisitor::visitWhile_stmt(Python3Parser::While_stmtContext *ctx) {
  while(1) {
    auto ret_test = visit(ctx->test());
    //std::cerr << "test" << std::endl;
    ret_test = GetValue(ret_test);
    //std::cerr << "while" << std::endl;
    if(ret_test.type() != typeid(bool)) {
      std::vector<Arg> arglist;
      arglist.push_back(Arg(ret_test));
      ret_test = functions_["bool"](arglist);
    }
    if(auto flag = std::any_cast<bool>(&ret_test)) {
      if(!(*flag)) {
        break;
      }
    }
    auto ret = visit(ctx->suite());
    if(auto control = std::any_cast<Control>(&ret)) {
      if(control->type_ == kContinue) {
        continue;
      }
      if(control->type_ == kBreak) {
        break;
      }
      if(control->type_ == kReturn) {
        return *control;
      }
    }
  }
  return std::any();
}

std::any EvalVisitor::visitSuite(Python3Parser::SuiteContext *ctx) {
  if(ctx->simple_stmt()) {
    auto ret = visit(ctx->simple_stmt());
    return ret;
  }
  auto stmt_vector = ctx->stmt();
  for(auto stmt : stmt_vector) {
    auto ret = visit(stmt);
    if(auto control = std::any_cast<Control>(&ret)) {
      return *control;
    }
  }
  return std::any(NoneType());
}

std::any EvalVisitor::visitTest(Python3Parser::TestContext *ctx) {
  return visit(ctx->or_test());
}

std::any EvalVisitor::visitOr_test(Python3Parser::Or_testContext *ctx) {
  auto test_vector = ctx->and_test();
  auto ret = visit(test_vector[0]);
  /*if(auto arg = std::any_cast<std::vector<Arg>>(&ret)) {
    std::cerr << "arg" << std::endl;
  }*/
  // auto num = std::any_cast<sjtu::int2048>(&ret);
  // num->print();
  if (test_vector.size() == 1) {
    return ret;
  } else {
    ret = GetValue(ret);
    std::vector<Arg> arglist;
    arglist.push_back(Arg(ret));
    ret = functions_["bool"](arglist);
    auto flag = std::any_cast<bool>(&ret);
    for (int i = 1; i < test_vector.size() && (*flag) != 1; i++) {
      auto ret1 = visit(test_vector[i]);
      ret1 = GetValue(ret1);
      std::vector<Arg> arglist;
      arglist.push_back(Arg(ret1));
      ret1 = functions_["bool"](arglist);
      auto flag1 = std::any_cast<bool>(&ret1);
      *flag = (*flag) || (*flag1);
    }
    return *flag;
  }
}

std::any EvalVisitor::visitAnd_test(Python3Parser::And_testContext *ctx) {
  auto test_vector = ctx->not_test();
  auto ret = visit(test_vector[0]);
  if (test_vector.size() == 1) {
    return ret;
  } else {
    ret = GetValue(ret);
    std::vector<Arg> arglist;
    arglist.push_back(Arg(ret));
    ret = functions_["bool"](arglist);
    auto flag = std::any_cast<bool>(&ret);
    for (int i = 1; i < test_vector.size() && flag != 0; i++) {
      auto ret1 = visit(test_vector[i]);
      ret1 = GetValue(ret1);
      std::vector<Arg> arglist;
      arglist.push_back(Arg(ret1));
      ret1 = functions_["bool"](arglist);
      auto flag1 = std::any_cast<bool>(&ret1);
      *flag = (*flag) && (*flag1);
    }
    return *flag;
  }
}

std::any EvalVisitor::visitNot_test(Python3Parser::Not_testContext *ctx) {
  if (ctx->not_test()) {
    auto ret = visit(ctx->not_test());
    ret = GetValue(ret);
    std::vector<Arg> arglist;
    arglist.push_back(Arg(ret));
    ret = functions_["bool"](arglist);
    auto flag = std::any_cast<bool>(&ret);
    return std::any(!(*flag));
  } else {
    if (ctx->comparison()) {
      return visit(ctx->comparison());
    } else {
      throw "not : argument should be not_test or comparison\n";
    }
  }
}

std::any EvalVisitor::visitComparison(Python3Parser::ComparisonContext *ctx) {
  auto arith_vector = ctx->arith_expr();
  auto ret = visit(arith_vector[0]);
  if (arith_vector.size() == 1) {
    // std::cerr << "finish" << std::endl;
    return ret;
  } else {
    ret = GetValue(ret);
    std::any ret1;
    auto op_vector = ctx->comp_op();
    bool flag = 1;
    for (int i = 1; i < arith_vector.size() && flag == 1; i++) {
      ret1 = visit(arith_vector[i]);
      ret1 = GetValue(ret1);
      auto ret_op = visit(op_vector[i - 1]);
      auto op = std::any_cast<OperationType>(&ret_op);
      //std::cerr << *op << std::endl;
      auto ret_oper = Operation(ret, ret1, *op);
      //std::cerr << "finish oper" << std::endl;
      flag = flag && std::any_cast<bool>(ret_oper);
      //std::cerr << flag << std::endl;
      ret = ret1;
    }
    return flag;
  }
}

std::any EvalVisitor::visitComp_op(Python3Parser::Comp_opContext *ctx) {
  if (ctx->LESS_THAN()) {
    return kLess;
  }
  if (ctx->GREATER_THAN()) {
    return kGreater;
  }
  if (ctx->EQUALS()) {
    return kEqual;
  }
  if (ctx->GT_EQ()) {
    return kGreater_Equal;
  }
  if (ctx->LT_EQ()) {
    return kLess_Equal;
  }
  if (ctx->NOT_EQ_2()) {
    return kNot_Equal;
  }
  throw "no this comp_op";
}

std::any EvalVisitor::visitArith_expr(Python3Parser::Arith_exprContext *ctx) { // only int2048 +-, wait for float string
  // std::cerr << "arith_expr" << std::endl;
  auto term_vector = ctx->term();
  auto ret = visit(term_vector[0]);
  // auto num = std::any_cast<sjtu::int2048>(&ret);
  // std::cerr << *num << std::endl;
  if (term_vector.size() == 1) {
    // std::cerr << "finish" << std::endl;
    return ret;
  } else {
    auto op_vector = ctx->addorsub_op();
    // std::cerr << "have op" << std::endl;
    ret = GetValue(ret);
    std::any ans(ret);
    for (int i = 1; i < term_vector.size(); i++) {
      ret = visit(term_vector[i]);
      ret = GetValue(ret);
      auto ret_op = visit(op_vector[i - 1]);
      if (auto op = std::any_cast<OperationType>(&ret_op)) {
        ans = Operation(ans, ret, *op);
      }
    }
    // std::cerr << *data << std::endl;
    return ans;
  }
}

std::any EvalVisitor::visitAddorsub_op(Python3Parser::Addorsub_opContext *ctx) {
  if (ctx->ADD()) {
    return kAdd;
  } else {
    if (ctx->MINUS()) {
      return kMinus;
    } else {
      throw "op : should be + or -";
    }
  }
}

std::any EvalVisitor::visitTerm(Python3Parser::TermContext *ctx) {
  auto factor_vector = ctx->factor();
  auto ret = visit(factor_vector[0]);
  // auto num = std::any_cast<sjtu::int2048>(&ret);
  // std::cerr << *num << std::endl;
  if (factor_vector.size() == 1) {
    //std::cerr << 111 << std::endl;
    return ret;
  } else {
    auto op_vector = ctx->muldivmod_op();
    // std::cerr << "have op" << std::endl;
    ret = GetValue(ret);
    std::any ans(ret);
    for (int i = 1; i < factor_vector.size(); i++) {
      ret = visit(factor_vector[i]);
      ret = GetValue(ret);
      auto ret_op = visit(op_vector[i - 1]);
      if (auto op = std::any_cast<OperationType>(&ret_op)) {
        ans = Operation(ans, ret, *op);
      }
    }
    // std::cerr << *data << std::endl;
    return ans;
  }
}

std::any
EvalVisitor::visitMuldivmod_op(Python3Parser::Muldivmod_opContext *ctx) {
  if (ctx->STAR()) {
    return kMulti;
  }
  if (ctx->IDIV()) {
    return kIDiv;
  }
  if (ctx->DIV()) {
    return kDiv;
  }
  if (ctx->MOD()) {
    return kMod;
  }
  throw "op : invalid";
}

std::any EvalVisitor::visitFactor(Python3Parser::FactorContext *ctx) {
  if(ctx->factor()) {
    auto ret = visit(ctx->factor());
    if(ctx->MINUS()) {
      ret = GetValue(ret);
      if(auto num = std::any_cast<sjtu::int2048>(&ret)) {
        return std::any(-(*num));
      } 
      if(auto num = std::any_cast<double>(&ret)) {
        return -(*num);
      }
      throw "- before a not num";
    }
    if(ctx->ADD()) {
      ret = GetValue(ret);
      if(auto num = std::any_cast<sjtu::int2048>(&ret)) {
        return *num;
      } 
      if(auto num = std::any_cast<double>(&ret)) {
        return *num;
      }
      throw "+ before a not num\n";
    }
    return ret;
  }
  if (ctx->atom_expr()) {
    return visit(ctx->atom_expr());
  } 
  throw "factor wrong\n";
}

std::any EvalVisitor::visitAtom_expr(Python3Parser::Atom_exprContext *ctx) {
  // std::cerr << "atom_expr" << std::endl;
  auto ret1 = visit(ctx->atom());
  /*if(auto num = std::any_cast<double>(&ret1)) {
    std::cerr << *num << std::endl;
  }*/
  if (auto name = std::any_cast<std::string>(&ret1)) {
    //std::cerr << *name << std::endl;
    if(ctx->trailer()) {
      if (functions_.count(*name)) {
        //std::cerr << *name << std::endl;
        auto ret2 = visit(ctx->trailer());
        if (auto arglist = std::any_cast<std::vector<Arg>>(&ret2)) {
          //std::cerr << *name << "have arg" << std::endl;
          try {
            auto ret = functions_[*name](*arglist);
            return functions_[*name](*arglist);
          } catch(const char *err) {
            std::cerr << err << std::endl;
            return std::any();
          }
        }
      }
      if (user_functions_.count(*name)) {
        //std::cerr << *name << std::endl;
        Function function = user_functions_[*name];
        auto ret2 = visit(ctx->trailer());
        std::map<std::string, std::any> variables;
        if(auto arglist = std::any_cast<std::vector<Arg>>(&ret2)) {
          for(auto arg : *arglist) {
            if(arg.name_ == "") {
              variables[function.arglist_[arg.index_].name_] = arg.value_;
            } else {
              variables[arg.name_] = arg.value_;
            }
          }
          for(auto arg : function.arglist_) {
            if(variables.count(arg.name_) == 0) {
              variables[arg.name_] = arg.default_;
            }
          }
        }
        variables_stack_.push_back(variables);
        auto ret = visit(function.suite_);
        if(auto control = std::any_cast<Control>(&ret)) {
          if(control->type_ == kReturn) {
            if(control->value_.empty()) {
              ret = std::any(NoneType());
            } else {
              if(control->value_.size() == 1) {
                ret = control->value_[0];
              } else {
                ret = control->value_;
              }
            }
            /*if(auto value = std::any_cast<std::vector<std::any>>(&ret)) {
              std::cerr << 1 << std::endl;
            }*/
          }
        }
        /*if(auto num = std::any_cast<sjtu::int2048>(&ret)) {
          std::cerr << *num << std::endl;
        }*/
        //std::cerr << *name << std::endl;
        variables_stack_.pop_back();
        return ret;
      }
    }
  }
  return ret1;
}

std::any EvalVisitor::visitTrailer(Python3Parser::TrailerContext *ctx) {
  // std::cerr << "trailer" << std::endl;
  if (ctx->arglist()) {
    // std::cerr << "finish" << std::endl;
    return visit(ctx->arglist());
  } else {
    return std::vector<Arg>();
  }
}

std::any EvalVisitor::visitAtom(Python3Parser::AtomContext *ctx) {
  if (ctx->NUMBER()) {
    auto string_num = ctx->NUMBER()->toString();
    if(int index = string_num.find('.'); index != std::string::npos) {
      double num = 0;
      for(int i = index - 1, base = 1; i >= 0; i--, base *= 10) {
        num += base * (string_num[i] - '0');
      }
      double base = 0.1;
      for(int i = index + 1; i < string_num.length(); base /= 10, i++) {
        num += base * (string_num[i] - '0');
      }
      //std::cerr << num << std::endl;
      return num;
    } else {
      try {
        sjtu::int2048 num(ctx->NUMBER()->toString());
        return num;
      } catch(const char *err) {
        std::cerr << err << std::endl;
        return std::any(sjtu::int2048(0));
      }
      
    }
  }
  if (ctx->NAME()) {
    std::string name = ctx->NAME()->toString();
    return name;
  }
  if (ctx->STRING(0)) {
    std::vector<std::string> data;
    auto string_vector = ctx->STRING();
    for (auto string : string_vector) {
      // std::cerr << string->toString() << std::endl;
      auto str = string->toString();
      data.push_back(str.substr(1, str.length() - 2));
    }
    return data;
  }
  if (ctx->TRUE()) {
    return bool(1);
  }
  if (ctx->test()) {
    return visit(ctx->test());
  }
  if (ctx->FALSE()) {
    return bool(0);
  }
  if (ctx->NONE()) {
    return NoneType();
  }
  if(ctx->format_string()) {
    return visit(ctx->format_string());
  }
  return std::any();
}

std::any EvalVisitor::visitFormat_string(Python3Parser::Format_stringContext *ctx) {
  auto str_vector = ctx->FORMAT_STRING_LITERAL();
  auto test_vector = ctx->testlist();
  auto brace_vector = ctx->OPEN_BRACE();
  std::vector<std::string> str;
  int i, j;
  //std::cerr << test_vector.size() << std::endl;
  for (i = 0, j = 0; i < str_vector.size() && j < test_vector.size();) {
    if (str_vector[i]->getSymbol()->getTokenIndex() < brace_vector[j]->getSymbol()->getTokenIndex()) {
      auto data = str_vector[i]->toString();
      auto pos = data.find("{{");
      while (pos != std::string::npos) {
        //std::cerr << pos << std::endl;
        data.replace(pos, 2, "{");
        pos = data.find("{{");
      }
      pos = data.find("}}");
      while (pos != std::string::npos) {
        data.replace(pos, 2, "}");
        pos = data.find("}}");
      }
      str.push_back(data);
      //std::cerr << i << " " << str_vector[i]->toString() << std::endl;
      i++;
    } else {
      auto ret = visit(test_vector[j]);
      //std::cerr << "test" << std::endl;
      if (auto testlist = std::any_cast<std::vector<std::any>>(&ret)) {
        //std::cerr << "test" << std::endl;
        //std::cerr << j << std::endl;
        for (auto test : *testlist) {
          test = GetValue(test);
          std::vector<Arg> arglist;
          arglist.push_back(Arg(test));
          test = functions_["str"](arglist);
          auto str_vector1 = std::any_cast<std::vector<std::string>>(test);
          for (auto data : str_vector1) {
            //std::cerr << j << " " << data << std::endl;
            str.push_back(data);
          }
        }
      }
      j++;
    }
  }
  for (; i < str_vector.size(); i++) {
    auto data = str_vector[i]->toString();
    auto pos = data.find("{{");
    while (pos != std::string::npos) {
      //std::cerr << pos << std::endl;
      data.replace(pos, 2, "{");
      pos = data.find("{{");
    }
    pos = data.find("}}");
    while (pos != std::string::npos) {
      data.replace(pos, 2, "}");
      pos = data.find("}}");
    }
    str.push_back(data);
  }
  for (; j < test_vector.size(); j++) {
    auto ret = visit(test_vector[j]);
    if (auto testlist = std::any_cast<std::vector<std::any>>(&ret)) {
      // std::cerr << j << std::endl;
      for (auto test : *testlist) {
        test = GetValue(test);
        std::vector<Arg> arglist;
        arglist.push_back(Arg(test));
        test = functions_["str"](arglist);
        auto str_vector1 = std::any_cast<std::vector<std::string>>(test);
        for (auto data : str_vector1) {
          //std::cerr << j << " " << data << std::endl;
          str.push_back(data);
        }
      }
    }
  }
  return str;
}

std::any EvalVisitor::visitTestlist(Python3Parser::TestlistContext *ctx) {
  auto test_vector = ctx->test();
  std::vector<std::any> test_list;
  for(auto test : test_vector) {
    auto ret = visit(test);
    if(auto tuple = std::any_cast<std::vector<std::any>>(&ret)) {
      for(auto test : *tuple) {
        test_list.push_back(test);
      }
    } else {
      test_list.push_back(ret);
    }
  }
  return test_list;
}

std::any EvalVisitor::visitArglist(Python3Parser::ArglistContext *ctx) {
  // std::cerr << "have arglist" << std::endl;
  std::vector<Arg> arglist;
  auto arg_vector = ctx->argument();
  for (int i = 0; i < arg_vector.size(); i++) {
    auto ret = visit(arg_vector[i]);
    Arg arg = std::any_cast<Arg>(ret);
    if(arg.name_ == "") {
      arg.index_ = i;
    }
    arglist.push_back(arg);
  }
  return arglist;
}

std::any EvalVisitor::visitArgument(Python3Parser::ArgumentContext *ctx) {
  auto test_vector = ctx->test();
  Arg arg;
  if (test_vector.size() == 1) {
    auto ret = visit(test_vector[0]);
    arg.value_ = GetValue(ret);
  } else {
    arg.name_ = std::any_cast<std::string>(visit(test_vector[0]));
    auto ret = visit(test_vector[1]);
    arg.value_ = GetValue(ret);
  }
  return arg;
}