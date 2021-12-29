// Author: Kevin Chang: tc3149@nyu.edu

#ifndef mdp_hpp
#define mdp_hpp

#pragma once
#include "matrix.hpp"
#include <fstream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <numeric>

namespace mdp {

bool min = false;
double df = 1.0;
double tol = 0.01;
unsigned iter = 100;

struct State {
  bool decision;
  int reward;
  std::string symbol;
  std::vector<unsigned> edges;
  std::vector<double> probs;

  State() = default;
  ~State() = default;
};

typedef std::vector<State> States;
typedef std::unordered_map<std::string, std::size_t> Table;

class MDP {
public:
  States states;
  matrix::Matrix<double> R, V, P;

  MDP() = default;
  MDP(States &&);
  ~MDP() = default;
  void value_iteration();
  bool greedy_policy_computation();

private:
  bool tolerance(const matrix::Matrix<double> &, const matrix::Matrix<double> &);
};

class Parser {
public:
  static bool load(const std::string &, States &);
  static bool isdouble(const std::string &, const std::size_t &);
  static bool islongint(const std::string &, const std::size_t &);

private:
  static std::size_t find_first_not_space(const std::string &, const std::size_t &);
  static std::size_t find_first_symbol(const std::string &, const std::size_t &, std::string &);
  static std::size_t find_first_sign(const std::string &, const std::size_t &, std::string &);
  static std::size_t find_first_val(const std::string &, const std::size_t &, std::string &);
  static int load_symbols(const std::string &, States &, Table &, std::string &, int &);
  static int load_others(const std::string &, States &, Table &, std::string &, int &);
};

}

///////////////////////////////////////////////////////////////////////////////

mdp::MDP::MDP(States &&s) {
  states = std::move(s);

  R = matrix::Matrix<double>(states.size(), 1);
  V = matrix::Matrix<double>(states.size(), 1);
  P = matrix::Matrix<double>(states.size(), states.size());

  for (std::size_t i = 0; i < states.size(); ++i) {
    R(i) = states[i].reward;
    // V(i) = R(i);

    if (states[i].decision) {
      double q = (1-states[i].probs[0]) / (states[i].edges.size()-1);
      while (states[i].probs.size() < states[i].edges.size()) {
        states[i].probs.push_back(q);
      }

      auto cmp = [&](const std::size_t &a, const std::size_t &b) {
        return V(a) < V(b);
      };
      auto it = std::minmax_element(states[i].edges.begin(), states[i].edges.end(), cmp);
      std::iter_swap(states[i].edges.begin(), min ? it.first : it.second);
    }

    for (std::size_t k = 0; k < states[i].edges.size(); ++k) {
      const std::size_t &j = states[i].edges[k];
      P(i, j) += states[i].probs[k];
    }
  }

}

bool mdp::MDP::tolerance(const matrix::Matrix<double> &a, const matrix::Matrix<double> &b) {
  for (unsigned i = 0; i < a.size(); ++i) {
    if (std::abs(a.at(i)-b.at(i)) > tol) {
      return false;
    } 
  }
  return true;
}

void mdp::MDP::value_iteration() {
  for (unsigned i = 0; i <= iter; ++i) {
    matrix::Matrix<double> nV = R + df * P * V;
    std::swap(V, nV);
    if (tolerance(V, nV)) break;
  }
}

bool mdp::MDP::greedy_policy_computation() {
  matrix::Matrix<double> nP(P.rows(), P.cols());

  for (std::size_t i = 0; i < states.size(); ++i) {
    if (states[i].decision) {
      auto cmp = [&](const std::size_t &a, const std::size_t &b) {
        return V(a) < V(b);
      };
      auto it = std::minmax_element(states[i].edges.begin(), states[i].edges.end(), cmp);
      std::iter_swap(states[i].edges.begin(), min ? it.first : it.second);
    }
    for (std::size_t k = 0; k < states[i].edges.size(); ++k) {
      const std::size_t &j = states[i].edges[k];
      nP(i, j) += states[i].probs[k];
    }
  }

  std::swap(nP, P);
  return nP == P;
}

///////////////////////////////////////////////////////////////////////////////

bool mdp::Parser::isdouble(const std::string &str, const std::size_t &begin = 0) {
  std::size_t i = begin;
  try {
    std::stod(str.substr(i), &i);
  } catch(const std::exception &e) {
    return false;
  }
  return i == str.size();
}

bool mdp::Parser::islongint(const std::string &str, const std::size_t &begin = 0) {
  std::size_t i = begin;
  try {
    std::stol(str.substr(i), &i);
  } catch(const std::exception &e) {
    return false;
  }
  return i == str.size();
}

std::size_t mdp::Parser::find_first_not_space(const std::string &s, const std::size_t &begin) {
  std::size_t i = begin;
  while (i < s.size() && std::isspace(s[i])) ++i;
  return i < s.size() ? i : std::string::npos;
}

std::size_t mdp::Parser::find_first_symbol(const std::string &s, const std::size_t &begin, std::string &token) {
  std::size_t i = find_first_not_space(s, begin);
  if (i == std::string::npos) return i;
  std::size_t j = i;
  token.clear();
  while (j < s.size() && (std::isalnum(s[j]) || s[j]=='_')) {
    token += s[j];
    ++j;
  }
  return i < j ? j : std::string::npos;
}

std::size_t mdp::Parser::find_first_sign(const std::string &s, const std::size_t &begin, std::string &token) {
  std::size_t i = find_first_not_space(s, begin);
  if (i == std::string::npos) return i;
  token = s[i]; 
  return (s[i]=='=' || s[i]==':' || s[i]=='%') ? i+1 : std::string::npos;
}

std::size_t mdp::Parser::find_first_val(const std::string &s, const std::size_t &begin, std::string &token) {
  std::size_t i = find_first_not_space(s, begin);
  if (i == std::string::npos) return i;
  std::size_t j = i;
  token.clear();
  while (j < s.size() && !std::isspace(s[j])) {
    token += s[j];
    ++j;
  }
  return isdouble(token) ? j : std::string::npos;
}

int mdp::Parser::load_symbols(const std::string &filename, States &states, Table &table, std::string &token, int &linenum) {
  std::ifstream in(filename);
  if (!in) return 1;
  linenum = 0;
  states.clear();
  table.clear();

  std::string line;
  while (std::getline(in, line)) {
    ++linenum;
    std::size_t i = 0;
    i = find_first_not_space(line, i);
    if (i == std::string::npos || line[i] == '#') continue;
    i = find_first_symbol(line, i, token);
    if (i == std::string::npos) return 2;

    if (table.find(token) == table.end()) {
      table[token] = -1;
      states.push_back(State());
      states.back().symbol = token;
    }

    i = find_first_sign(line, i, token);
    if (i == std::string::npos) return 2;
    if (token == "=") {
      i = find_first_val(line, i, token);
    }
    else if (token == ":") {
      i = find_first_not_space(line, i);
      if (i == std::string::npos || line[i] != '[') return 2;
      
      std::size_t j = find_first_not_space(line, i+1);
      if (i == std::string::npos) return 2;
      if (line[j] != ']') {
        while (line[i++] != ']') {
          i = find_first_symbol(line, i, token);
          if (i == std::string::npos) return 2;
          i = find_first_not_space(line, i);
          if (i == std::string::npos || (line[i] != ',' && line[i] != ']')) return 2;
        }
      }
    }
    else {
      i = find_first_val(line, i, token);
      if (i == std::string::npos) return 2;
      std::size_t j;
      while ((j = find_first_val(line, i, token)) != std::string::npos) i = j;
    }

    if (find_first_not_space(line, i) != std::string::npos) return 2;
  }

  std::sort(states.begin(), states.end(), [](const State &a, const State &b) {
    return a.symbol < b.symbol;
  });
  for (std::size_t i = 0; i < states.size(); ++i) {
    table[ states[i].symbol ] = i;
  }

  return 0;
}

int mdp::Parser::load_others(const std::string &filename, States &states, Table &table, std::string &token, int &linenum) {
  std::ifstream in(filename);
  if (!in) return 1;
  linenum = 0;

  std::string line;
  while (std::getline(in, line)) {
    ++linenum;
    std::size_t i = 0;
    i = find_first_not_space(line, i);
    if (i == std::string::npos || line[i] == '#') continue;
    i = find_first_symbol(line, i, token);
    State &state = states[ table[token] ];
    i = find_first_sign(line, i, token);
    if (token == "=") {
      i = find_first_val(line, i, token);
      state.reward = std::stod(token);
    }
    else if (token == ":") {
      i = find_first_not_space(line, i)+1;
      while ((i = find_first_symbol(line, i, token)) != std::string::npos) {
        if (table.find(token) == table.end()) return 2;
        state.edges.push_back( table[token] );
        i = find_first_not_space(line, i)+1;
      }
    }
    else {
      while ((i = find_first_val(line, i, token)) != std::string::npos) {
        state.probs.push_back(std::stod(token));
        if (state.probs.back() < 0 || state.probs.back() > 1) return 3;
      }
    }
  }

  return 0;
}

bool mdp::Parser::load(const std::string &filename, States &states) {
  states.clear();
  Table table;
  std::string token;
  int err, linenum;
  err = load_symbols(filename, states, table, token, linenum);
  switch(err) {
    case 1: {
      std::cerr << "Error: unable to open file `" << filename << "`\n";
      return false;
    }
    case 2: {
      std::cerr << "Error: unable to parse file, line " << linenum << "\n";
      return false;
    }
  }
  err = load_others(filename, states, table, token, linenum);
  switch(err) {
    case 1: {
      std::cerr << "Error: unable to open file `" << filename << "`\n";
      return false;
    }
    case 2: {
      std::cerr << "Error: undefined symbol `" << token << "`. line "<< linenum << "\n";
      return false;
    }
    case 3: {
      std::cerr << "Error: probabilties should be in range [0, 1]. line "<< linenum << "\n";
      return false;
    }
  }

  for (State &state : states) {
    if (state.edges.empty() && !state.probs.empty()) {
      std::cerr << "Error: terminal node `" << state.symbol << "` has probabilities.\n";
      return false;
    }
    else if (!state.edges.empty() && state.probs.empty()) {
      state.probs.push_back(1);
    }
    
    state.decision = state.probs.size()==1;
    if (state.decision && state.edges.size()==1 && state.probs[0]!=1) {
      std::cerr << "Error: single edge desicion node `" << state.symbol << "` should have probabilty 1.0\n";
      return false;
    }
    if (!state.decision && state.edges.size() != state.probs.size()) {
      std::cerr << "Error: chance node `" << state.symbol << "` the numbers of edges and probs do not match\n";
      return false;
    }
    if (!state.decision && !state.probs.empty() && std::accumulate(state.probs.begin(), state.probs.end(), 0.0) != 1) {
      std::cerr << "Error: chance node `" << state.symbol << "` probabilities should sum up to 1.0\n";
      return false;
    }
  }

  return true;
}

#endif /* mdp_hpp */