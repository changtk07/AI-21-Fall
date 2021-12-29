#include "proposition.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <getopt.h>

std::string mode, input;

bool arguments(int argc, char * argv[]) {
  static const option options[] = {
    {"verbose", no_argument,       nullptr, 'v'},
    {"mode",    required_argument, nullptr, 'm'},
    {nullptr,   no_argument,       nullptr,  0}
  };
  
  int c = 0, idx = 0;
  while ((c = getopt_long(argc, argv, "vm:", options, &idx)) != -1) {
    switch (c) {
      case 'v':
        proposition::verbose.rdbuf(std::cout.rdbuf());
        break;
      case 'm':
        mode = optarg;
        if (mode != "cnf" && mode != "dpll" && mode != "solver") {
          std::cerr << "Error: unknown algorithm `" << mode << "`\n";
          return false;
        }
        break;
      default:
        return false;
    }
  }
  
  if (mode.empty()) {
    std::cerr << "Error: missing arguments. Require running mode, use `-m` or `--mode to specify\n";
    return false;
  }
  
  argc -= optind;
  argv += optind;
  
  if (argc != 1) {
    std::cerr << "Error: too" << (argc < 1 ? "few" : "many") << " arguments. Required one argument `${input_file}`\n";
    return false;
  }
  input = argv[0];
  return true;
}

bool convert_to_cnf(const std::string &filename, proposition::Clauses &clauses) {
  std::ifstream in(filename);
  if (!in) {
    std::cerr << "Error: failed to open file `" + filename + "`\n";
    return false;
  }
  std::string bnf;
  unsigned line = 0;
  while (std::getline(in, bnf)) {
    ++line;
    bnf.erase(std::remove_if(bnf.begin(), bnf.end(), isspace), bnf.end());
    if (bnf.empty()) continue;
    if (!proposition::CnfConverter::isvalid(bnf, 0, bnf.size())) {
      std::cerr << "Error: input file is not parseable. line " << line << "\n";
      return false;
    }
    proposition::CnfConverter::solve(bnf);
    proposition::CnfConverter::extract_literals(bnf, clauses);
  }
  return true;
}

bool load_cnf(const std::string &filename, proposition::Clauses &clauses) {
  std::ifstream in(filename);
  if (!in) {
    std::cerr << "Error: failed to open file `" + filename + "`\n";
    return false;
  }
  std::string cnf, token;
  unsigned line = 0;
  while (std::getline(in, cnf)) {
    ++line;
    clauses.push_back({});
    std::istringstream iss(cnf);
    bool neg = false;
    while (iss >> token) {
      if (token == "!") {
        if (neg) {
          std::cerr << "Error: invalid negation detected. line " << line << "\n";
          return false;
        }
        neg = true;
      }
      else {
        if (token[0] == '!') {
          if (neg) {
            std::cerr << "Error: invalid negation detected. line " << line << "\n";
            return false;
          }
          neg = true;
          token.erase(0, 1);
        }
        for (const char &c : token) {
          if (!std::isalnum(c) && c != '_') {
            std::cerr << "Error: invalid token `" << token << "`. line " << line << "\n";
            return false;
          }
        }
        clauses.back()[token] = neg;
        neg = false;
      }
    }
  }
  return true;
}

int main(int argc, char * argv[]) {
  if (!arguments(argc, argv)) return 1;
  
  proposition::Clauses clauses;
  proposition::Assignment asgmt;
  if (mode == "cnf" || mode == "solver") {
    if (!convert_to_cnf(input, clauses)) return 1;
    proposition::verbose << "----------------------------------------\n";
    proposition::verbose << "step 6: remove sentences that includes an atom and its negation [all]\n";
    std::cout << clauses << "\n";
  }
  if (mode == "dpll") {
    if (!load_cnf(input, clauses)) return 1;
    std::cout << clauses << "\n";
  }
  if (mode == "dpll" || mode == "solver") {
    if (proposition::DPLL::solve(clauses, asgmt))
      std::cout << asgmt;
    else
      std::cout << "NO VALID ASSIGNMENT\n";
  }
  
  return 0;
}
