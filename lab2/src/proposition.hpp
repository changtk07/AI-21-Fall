#ifndef proposition_hpp
#define proposition_hpp

#pragma once
#include <iostream>
#include <string>
#include <list>
#include <map>
#include <unordered_map>

namespace proposition {

typedef std::map<std::string, int8_t> Assignment;
typedef std::unordered_map<std::string, bool> Clause;
typedef std::list<Clause> Clauses;

extern const int8_t unbound;
extern std::ostream verbose;

class CnfConverter {
private:
  static bool isliteral(const std::string &bnf, std::size_t &i);
  
  static bool isop(const std::string &bnf, std::size_t &i);
  
  static std::size_t find_next_term(const std::string &bnf, std::size_t i);
  
  static void group(std::string &bnf, const std::size_t &i);
  
  static void elim_iff(std::string &bnf, const std::size_t &i);
  
  static void elim_imply(std::string &bnf, const std::size_t &i);
  
  static void reduce_negation(std::string &bnf);
  
  static void de_morgan(std::string &bnf);
  
  static void reduce_parenthesis(std::string &bnf);
  
  static void distribution(std::string &bnf, const std::size_t &i);
  
  static void flatten(std::string &bnf);
  
public:
  static bool isvalid(const std::string &bnf, std::size_t i, std::size_t k);
  
  static void solve(std::string &bnf);
  
  static void extract_literals(const std::string &cnf, Clauses &clauses);
};

class DPLL {
private:
  static void update(Clauses &clauses, const std::string literal, const bool val);
  
  static bool unit_clause(Clauses &clauses, Assignment &asgmt);
  
  static bool pure_literal(Clauses &clauses, Assignment &asgmt);
  
  static bool dpll(Clauses &clauses, Assignment &asgmt);
  
public:
  static bool solve(Clauses &clauses, Assignment &asgmt);
};

}

std::ostream& operator<<(std::ostream& os, const proposition::Assignment &asgmt);

std::ostream& operator<<(std::ostream& os, const proposition::Clauses& clauses);

#endif /* proposition_hpp */