#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <unordered_map>
#include <getopt.h>

#include "matrix.hpp"
#include "mdp.hpp"

using matrix::Matrix;

std::string input;

bool arguments(int argc, char * argv[]) {
  static const option options[] = {
    {"df",    required_argument, nullptr, 'd'},
    {"min",   no_argument,       nullptr, 'm'},
    {"tol",   required_argument, nullptr, 't'},
    {"iter",  required_argument, nullptr, 'i'},
    {nullptr, no_argument,       nullptr,  0}
  };
  
  int c = 0, idx = 0;
  while ((c = getopt_long(argc, argv, "md:t:i:", options, &idx)) != -1) {
    switch (c) {
      case 'm': {
        mdp::min = true;
        break;
      }
      case 'd': {
        if (!mdp::Parser::isdouble(optarg)) {
          std::cerr << "Error: invalid argument for --df, -d `" << optarg << "`, should be a float number\n";
          return false;
        }
        mdp::df = std::stod(optarg);
        if (mdp::df < 0 || mdp::df > 1) {
          std::cerr << "Error: the argument for --df, -d should be in range [0, 1]\n";
          return false;
        }
        break;
      }
      case 't': {
        if (!mdp::Parser::isdouble(optarg)) {
          std::cerr << "Error: invalid argument for --tol, -t `" << optarg << "`, should be a float number\n";
          return false;
        }
        mdp::tol = std::stod(optarg);
        if (mdp::tol <= 0) {
          std::cerr << "Error: the argument for --tol, -t should be positive\n";
          return false;
        }
        break;
      }
      case 'i': {
        if (!mdp::Parser::islongint(optarg)) {
          std::cerr << "Error: invalid argument for --iter, -i `" << optarg << "`, should be an integer\n";
          return false;
        }
        long x = std::stol(optarg);
        if (x < 0) {
          std::cerr << "Error: the argument for --iter, -i should be non-negative integer\n";
          return false;
        }
        mdp::iter = x;
        break;
      }
      default:
        return false;
    }
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

int main(int argc, char * argv[]) {
  if (!arguments(argc, argv)) return 1;

  mdp::States states;
  if (!mdp::Parser::load(input, states)) return 1;

  mdp::MDP mdp(std::move(states));

  while (true) {
    mdp.value_iteration();
    if (mdp.greedy_policy_computation()) break;
  }

  for (std::size_t i = 0; i < mdp.states.size(); ++i) {
    const mdp::State &state = mdp.states[i];
    if (!state.decision || state.edges.size()==1) continue;
    std::cout << state.symbol << " -> " << mdp.states[ state.edges[0] ].symbol << "\n";
  }
  std::cout << "\n";

  for (std::size_t i = 0; i < mdp.states.size(); ++i) {
    const mdp::State &state = mdp.states[i];
    std::cout << std::fixed << std::setprecision(3) << state.symbol << "=" << mdp.V(i) << " ";
  }
  std::cout << "\n";

  return 0;
}