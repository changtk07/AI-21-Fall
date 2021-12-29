#include "proposition.hpp"
#include <cassert>
#include <cctype>
#include <utility>
#include <algorithm>
#include <iterator>

const int8_t proposition::unbound = -1;
std::ostream proposition::verbose(nullptr);

std::ostream& operator<<(std::ostream& os, const proposition::Assignment &asgmt) {
  for (auto &p : asgmt) {
    os << p.first << " = " << (p.second ? "true" : "false") << "\n";
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const proposition::Clauses& clauses) {
  for (auto &clause : clauses) {
    for (auto &literal : clause) {
      os << (literal.second ? "!" : "") << literal.first << " ";
    }
    os << "\n";
  }
  return os;
}

bool proposition::CnfConverter::isliteral(const std::string &bnf, std::size_t &i) {
  if (i >= bnf.size()) return false;
  if (!std::isalnum(bnf[i]) && bnf[i] != '_') return false;
  while (++i < bnf.size() && (std::isalnum(bnf[i]) || bnf[i] == '_'));
  return true;
}

bool proposition::CnfConverter::isop(const std::string &bnf, std::size_t &i) {
  if (i >= bnf.size()) return false;
  if (bnf[i]=='&' || bnf[i]=='|') { ++i; return true; }
  if (i+1<bnf.size() && bnf[i]=='=' && bnf[i+1]=='>') { i+=2; return true; }
  if (i+2<bnf.size() && bnf[i]=='<' && bnf[i+1]=='=' && bnf[i+2]=='>') { i+=3; return true; }
  return false;
}

std::size_t proposition::CnfConverter::find_next_term(const std::string &bnf, std::size_t i) {
  while (bnf[i] == '!') ++i;
  if (bnf[i] == '(') {
    int depth = 0;
    while (++i < bnf.size()) {
      if (bnf[i] == ')' && depth == 0) break;
      if (bnf[i] == '(') ++depth;
      if (bnf[i] == ')') --depth;
    }
    if (i >= bnf.size()) return -1;
    ++i;
  }
  else {
    while (i < bnf.size() && (std::isalnum(bnf[i]) || bnf[i] == '_')) ++i;
  }
  return i;
}

void proposition::CnfConverter::group(std::string &bnf, const std::size_t &i=0) {
  std::size_t idx[4] = {};
  std::size_t j = i, k;
  while (true) {
    while (bnf[j] == '!') ++j;
    if (bnf[j] == '(') group(bnf, j+1);
    k = find_next_term(bnf, j);
    if (k == bnf.size() || bnf[k] == ')') break;
    std::size_t tmp;
    if (bnf[k] == '&')      tmp = 0;
    else if (bnf[k] == '|') tmp = 1;
    else if (bnf[k] == '=') tmp = 2;
    else if (bnf[k] == '<') tmp = 3;
    else assert(false);
    idx[tmp] = k;
    j = k + (tmp ? tmp : 1);
  }
  
  for (j = 3; j <= 3 && !idx[j]; --j);
  if (j > 3) return;
  
  std::string left = bnf.substr(0, i);
  std::string right = bnf.substr(k, bnf.size());
  std::string term1 = bnf.substr(i, idx[j]-i);
  idx[j] += j ? j : 1;
  std::string term2 = bnf.substr(idx[j], k-idx[j]);
  
  group(term1, 0);
  group(term2, 0);
  
  if (find_next_term(term1, 0) != term1.size())
    term1 = "(" + term1 + ")";
  if (find_next_term(term2, 0) != term2.size())
    term2 = "(" + term2 + ")";
  
  static const std::string op[4] = {"&", "|", "=>", "<=>"};
  bnf = left + term1 + op[j] + term2 + right;
}

void proposition::CnfConverter::elim_iff(std::string &bnf, const std::size_t &i=0) {
  std::size_t tmp;
  tmp = i;
  while (bnf[tmp] == '!') ++tmp;
  if (bnf[tmp] == '(') elim_iff(bnf, tmp+1);
  std::size_t j = find_next_term(bnf, i);
  
  if (j >= bnf.size() || bnf[j] == ')') return;
  
  std::size_t x;
  if (bnf[j] == '&' || bnf[j] == '|') x = 1;
  else if (bnf[j] == '=') x = 2;
  else if (bnf[j] == '<') x = 3;
  else assert(false);
  
  tmp = j+x;
  while (bnf[tmp] == '!') ++tmp;
  if (bnf[tmp] == '(') elim_iff(bnf, tmp+1);
  std::size_t k = find_next_term(bnf, j+x);
  
  if (bnf[j] != '<') return;
  
  std::string left = bnf.substr(0, i);
  std::string right = bnf.substr(k, bnf.size()-k);
  std::string term1 = bnf.substr(i, j-i);
  std::string term2 = bnf.substr(j+x, k-j-x);
  
  bnf = left + "(" + term1 + "=>" + term2 + ")&(" + term2 + "=>" + term1 + ")" + right;
}

void proposition::CnfConverter::elim_imply(std::string &bnf, const std::size_t &i=0) {
  std::size_t tmp;
  tmp = i;
  while (bnf[tmp] == '!') ++tmp;
  if (bnf[tmp] == '(') elim_imply(bnf, tmp+1);
  std::size_t j = find_next_term(bnf, i);
  
  if (j >= bnf.size() || bnf[j] == ')') return;
  
  std::size_t x;
  if (bnf[j] == '&' || bnf[j] == '|') x = 1;
  else if (bnf[j] == '=') x = 2;
  else if (bnf[j] == '<') x = 3;
  else assert(false);
  
  tmp = j+x;
  while (bnf[tmp] == '!') ++tmp;
  if (bnf[tmp] == '(') elim_imply(bnf, tmp+1);
  std::size_t k = find_next_term(bnf, j+x);
  
  if (bnf[j] != '=') return;
  
  std::string left = bnf.substr(0, i);
  std::string right = bnf.substr(k, bnf.size()-k);
  std::string term1 = bnf.substr(i, j-i);
  std::string term2 = bnf.substr(j+x, k-j-x);
  
  bnf = left + "!" + term1 + "|" + term2 + right;
}

void proposition::CnfConverter::reduce_negation(std::string &bnf) {
  std::size_t i = 0;
  while ((i = bnf.find("!!", i)) != std::string::npos) {
    std::size_t j = i;
    while (bnf[j] == '!') ++j;
    bnf.erase(i, j-i-((j-i)%2));
  }
}

void proposition::CnfConverter::de_morgan(std::string &bnf) {
  std::size_t i = 0;
  while ((i = bnf.find("!(", i)) != std::string::npos) {
    std::size_t j = find_next_term(bnf, i+2);
    std::size_t k = bnf[j]!=')' ? find_next_term(bnf, j+1) : j;
    
    std::string left = bnf.substr(0, i) + "(";
    std::string right = bnf.substr(k, bnf.size()-k);
    std::string term1 = bnf.substr(i+2, j-i-2);
    std::string term2 = j!=k ? bnf.substr(j+1, k-j-1) : "";
    std::string op = j!=k ? (bnf[j]=='&' ? "|" : "&") : "";
    
    if (term1[0] == '!') term1.erase(0, 1);
    else term1 = "!" + term1;
    if (term2.empty());
    else if (term2[0] == '!') term2.erase(0, 1);
    else term2 = "!" + term2;
    
    bnf = left + term1 + op + term2 + right;
  }
}

void proposition::CnfConverter::reduce_parenthesis(std::string &bnf) {
  std::size_t i = 0, j = bnf.size();
  while (bnf[i] == '(') {
    j = find_next_term(bnf, i);
    if (j == bnf.size()) {
      bnf.erase(j-1, 1);
      bnf.erase(i, 1);
    }
    else break;
  }
  while ((i = bnf.find("(", i)) != std::string::npos) {
    j = find_next_term(bnf, i+1);
    if (bnf[i]=='(' && bnf[j]==')') {
      bnf.erase(j, 1);
      bnf.erase(i, 1);
    }
    else ++i;
  }
}

void proposition::CnfConverter::distribution(std::string &bnf, const std::size_t &i=0) {
  if (bnf[i] == '(') distribution(bnf, i+1);
  std::size_t j = find_next_term(bnf, i);
  
  if (j >= bnf.size() || bnf[j] == ')') return;
  
  if (bnf[j+1] == '(') distribution(bnf, j+2);
  std::size_t k = find_next_term(bnf, j+1);
  
  if (bnf[j] != '|') return;
  
  std::size_t j1 = bnf[i]=='(' ? find_next_term(bnf, i+1) : 0;
  std::size_t j2 = bnf[j+1]=='(' ? find_next_term(bnf, j+2) : 0;
  
  if ((!j1 || bnf[j1]!='&') && (!j2 || bnf[j2]!='&')) return;
  
  std::string left = bnf.substr(0, i);
  std::string right = bnf.substr(k, bnf.size()-k);
  std::string term1 = (j1 && bnf[j1]=='&') ? bnf.substr(i+1, j1-i-1) : bnf.substr(i, j-i);
  std::string term2 = (j1 && bnf[j1]=='&') ? bnf.substr(j1+1, j-j1-2) : "";
  std::string term3 = (j2 && bnf[j2]=='&') ? bnf.substr(j+2, j2-j-2) : bnf.substr(j+1, k-j-1);
  std::string term4 = (j2 && bnf[j2]=='&') ? bnf.substr(j2+1, k-j2-2) : "";
  
  std::string nt1 = "(" + term1 + "|" + term3 + ")";
  std::string nt2 = "(" + term1 + "|" + term4 + ")";
  std::string nt3 = "(" + term2 + "|" + term3 + ")";
  std::string nt4 = "(" + term2 + "|" + term4 + ")";
  
  if ((j1 && bnf[j1]=='&') || (j2 && bnf[j2]=='&')) distribution(nt1, 1);
  if (j2 && bnf[j2]=='&')                           distribution(nt2, 1);
  if (j1 && bnf[j1]=='&')                           distribution(nt3, 1);
  if (j1 && bnf[j1]=='&' && j2 && bnf[j2]=='&')     distribution(nt4, 1);
  
  if (j1 && bnf[j1]=='&' && j2 && bnf[j2]=='&')
    bnf = left + "(" + nt1 + "&" + nt2 + ")&(" + nt3 + "&" + nt4 + ")" + right;
  
  else if (j1 && bnf[j1]=='&')
    bnf = left + nt1 + "&" + nt3 + right;
  
  else if (j2 && bnf[j2]=='&')
    bnf = left + nt1 + "&" + nt2 + right;
}

void proposition::CnfConverter::flatten(std::string &bnf) {
  std::size_t i = 0;
  while ((i = bnf.find("(", i)) != std::string::npos) {
    std::size_t j = find_next_term(bnf, i);
    if ((i > 0 && bnf[i-1] == '&') || (j < bnf.size() && bnf[j] == '&')) { ++i; continue; }
    bnf.erase(j-1, 1);
    bnf.erase(i, 1);
  }
  
  i = 0;
  while ((i = bnf.find("(", i)) != std::string::npos) {
    if (bnf[i+1] != '(') { ++i; continue; }
    std::size_t j = find_next_term(bnf, i);
    bnf.erase(j-1, 1);
    bnf.erase(i, 1);
  }
}

bool proposition::CnfConverter::isvalid(const std::string &bnf, std::size_t i, std::size_t k) {
  while (i < k && bnf[i] == '!') ++i;
  if (!isliteral(bnf, i)) {
    if (i >= k || bnf[i] != '(') return false;
    std::size_t x = find_next_term(bnf, i);
    if (x > bnf.size()) return false;
    if (!isvalid(bnf, i+1, x-1)) return false;
    i = x;
  }
  while (i != k) {
    if (!isop(bnf, i)) return false;
    while (i < k && bnf[i] == '!') ++i;
    if (!isliteral(bnf, i)) {
      if (i >= k || bnf[i] != '(') return false;
      std::size_t x = find_next_term(bnf, i);
      if (x > bnf.size()) return false;
      if (!isvalid(bnf, i+1, x-1)) return false;
      i = x;
    }
  }
  return true;
}

void proposition::CnfConverter::solve(std::string &bnf) {
  verbose << "----------------------------------------\n" << bnf << "\n";
  reduce_parenthesis(bnf);
  verbose << "step 0.1: preprocess, remove redundant parenthesis\n" << bnf << "\n";
  group(bnf);
  verbose << "step 0.2: preprocess, grouping\n" << bnf << "\n";
  elim_iff(bnf);
  verbose << "step 1: eliminate <=>\n" << bnf << "\n";
  elim_imply(bnf);
  verbose << "step 2: eliminate =>\n" << bnf << "\n";
  reduce_negation(bnf);
  verbose << "step 3.1: replace !!A with A\n" << bnf << "\n";
  de_morgan(bnf);
  verbose << "step 3.2: de morgan's law\n" << bnf << "\n";
  distribution(bnf);
  verbose << "step 4: distribution\n" << bnf << "\n";
  flatten(bnf);
  verbose << "step 5: flatten formula\n" << bnf << "\n";
}

void proposition::CnfConverter::extract_literals(const std::string &cnf, Clauses &clauses) {
  std::size_t i = 0, j = 0;
  if (cnf.find("&") == std::string::npos) {
    clauses.push_back({});
    while (i < cnf.size()) {
      j = find_next_term(cnf, i);
      bool neg = cnf[i] == '!';
      std::string literal = neg ? cnf.substr(i+1, j-i-1) : cnf.substr(i, j-i);
      // remove A | !A clauses
      if (clauses.back().find(literal) != clauses.back().end() && clauses.back().at(literal) != neg) {
        clauses.pop_back();
        break;
      }
      clauses.back()[literal] = neg;
      i = j+1;
    }
  }
  else {
    while (i != std::string::npos) {
      i += i!=0;
      clauses.push_back({});
      if (cnf[i] != '(') {
        j = find_next_term(cnf, i);
        bool neg = cnf[i] == '!';
        std::string literal = neg ? cnf.substr(i+1, j-i-1) : cnf.substr(i, j-i);
        clauses.back()[literal] = neg;
      }
      else {
        while (cnf[i] != ')') {
          j = find_next_term(cnf, ++i);
          bool neg = cnf[i] == '!';
          std::string literal = neg ? cnf.substr(i+1, j-i-1) : cnf.substr(i, j-i);
          // remove A | !A clauses
          if (clauses.back().find(literal) != clauses.back().end() && clauses.back().at(literal) != neg) {
            clauses.pop_back();
            break;
          }
          clauses.back()[literal] = neg;
          i = j;
        }
      }
      i = cnf.find("&", i);
    }
  }
}

void proposition::DPLL::update(Clauses &clauses, const std::string literal, const bool val) {
  for (auto it = clauses.begin(); it != clauses.end(); ++it) {
    if (it->find(literal) == it->end()) continue;
    if (val != it->at(literal)) it = std::prev(clauses.erase(it));
    else it->erase(literal);
    if (it->empty()) {
      verbose << (val ? "!" : "") << literal << " contradiction\n\n";
      return;
    }
  }
  verbose << clauses << "\n";
}

bool proposition::DPLL::unit_clause(Clauses &clauses, Assignment &asgmt) {
  for (const auto &clause : clauses) {
    if (clause.size() == 1) {
      const std::string &literal = clause.begin()->first;
      const bool &polarity = clause.begin()->second;
      asgmt[literal] = !polarity;
      verbose << "easy case(unit clause): " << literal << "=" << (!polarity ? "true" : "false") << "\n";
      update(clauses, literal, !polarity);
      return true;
    }
  }
  return false;
}

bool proposition::DPLL::pure_literal(Clauses &clauses, Assignment &asgmt) {
  for (auto &p : asgmt) {
    if (p.second != unbound) continue;
    const std::string &literal = p.first;
    
    bool found = true;
    int8_t polarity = unbound;
    for (const auto &clause : clauses) {
      if (clause.find(literal) == clause.end()) continue;
      if (polarity == unbound) {
        polarity = clause.at(literal);
      }
      else if (polarity != clause.at(literal)) {
        found = false;
        break;
      }
    }
    
    if (found && polarity != unbound) {
      p.second = !polarity;
      verbose << "easy case(pure literal): " << p.first << "=" << (p.second ? "true" : "false") << "\n";
      update(clauses, p.first, p.second);
      return true;
    }
  }
  return false;
}

bool proposition::DPLL::dpll(Clauses &clauses, Assignment &asgmt) {
  static const auto isempty = [](const Clause &c) -> bool { return c.empty(); };

  while (true) {
    if (clauses.empty()) {
      for (auto &p : asgmt) {
        if (p.second == unbound) {
          p.second = false;
          verbose << "unbound: " << p.first << "=false\n";
        }
      }
      verbose << "\n";
      return true;
    }
    else if (std::find_if(clauses.begin(), clauses.end(), isempty) != clauses.end()) return false;
    else if (unit_clause(clauses, asgmt));
    else if (pure_literal(clauses, asgmt));
    else break;
  }

  auto it = std::find_if(asgmt.begin(), asgmt.end(), [](const std::pair<std::string, int8_t> &a) -> bool {
    return a.second == unbound;
  });

  {
    it->second = true;
    Clauses clauses_c = clauses;
    Assignment asgmt_c = asgmt;
    verbose << "hard case: guess " << it->first << "=true\n";
    update(clauses_c, it->first, true);
    if (dpll(clauses_c, asgmt_c)) {
      asgmt = std::move(asgmt_c);
      return true;
    }
  }

  it->second = false;
  verbose << "hard case failed, try: " << it->first << "=false\n";
  update(clauses, it->first, false);
  return dpll(clauses, asgmt);
}

bool proposition::DPLL::solve(Clauses &clauses, Assignment &asgmt) {
  for (auto &clause : clauses) {
    for (auto &literal : clause) {
      asgmt[literal.first] = unbound;
    }
  }
  return dpll(clauses, asgmt);
}