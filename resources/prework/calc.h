// 你需要提交这份代码
#pragma once

#include "visitor.h"

struct calculator : visitor {
    /// TODO: 完成所有需求
    ~calculator() override = default;
    std::any visit_num(num_node *ctx) override {
      return ctx->number;
    }
    std::any visit_add(add_node *ctx) override {
      auto lret = visit(ctx->lnode);
      auto rret = visit(ctx->rnode);
      if(auto lnum = std::any_cast<double>(&lret), rnum = std::any_cast<double>(&rret);lnum && rnum) {
        return (*lnum) + (*rnum);
      }
      if(auto lnum = std::any_cast<long long>(&lret), rnum = std::any_cast<long long>(&rret);lnum && rnum) {
        return (*lnum) + (*rnum);
      }
    }
    std::any visit_sub(sub_node *ctx) override {
      auto lret = visit(ctx->lnode);
      auto rret = visit(ctx->rnode);
      if(auto lnum = std::any_cast<double>(&lret), rnum = std::any_cast<double>(&rret);lnum && rnum) {
        return (*lnum) - (*rnum);
      }
      if(auto lnum = std::any_cast<long long>(&lret), rnum = std::any_cast<long long>(&rret);lnum && rnum) {
        return (*lnum) - (*rnum);
      }
    }
    std::any visit_mul(mul_node *ctx) override {
      auto lret = visit(ctx->lnode);
      auto rret = visit(ctx->rnode);
      if(auto lnum = std::any_cast<double>(&lret), rnum = std::any_cast<double>(&rret);lnum && rnum) {
        return (*lnum) * (*rnum);
      }
      if(auto lnum = std::any_cast<long long>(&lret), rnum = std::any_cast<long long>(&rret);lnum && rnum) {
        return (*lnum) * (*rnum);
      }
    }
    std::any visit_div(div_node *ctx) override {
      auto lret = visit(ctx->lnode);
      auto rret = visit(ctx->rnode);
      if(auto lnum = std::any_cast<double>(&lret), rnum = std::any_cast<double>(&rret);lnum && rnum) {
        return (*lnum) / (*rnum);
      }
      if(auto lnum = std::any_cast<long long>(&lret), rnum = std::any_cast<long long>(&rret);lnum && rnum) {
        return (*lnum) / (*rnum);
      }
    }
};
