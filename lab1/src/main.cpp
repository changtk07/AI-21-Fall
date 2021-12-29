#include "path.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <getopt.h>

std::string start, goal, alg, input;
int depth = 0;

bool inline isint(const std::string& s) {
  if (s.empty()) return false;
  size_t x = s.find_first_not_of("0123456789", 1);
  return (x == std::string::npos) && (isdigit(s[0]) || ((s[0] == '+' || s[0] == '-') && s.size() > 1));
}

bool arguments(int argc, char * argv[]) {
  const option options[] = {
    {"verbose", no_argument,       nullptr, 'v'},
    {"start",   required_argument, nullptr, 's'},
    {"goal",    required_argument, nullptr, 'g'},
    {"alg",     required_argument, nullptr, 'a'},
    {"depth",   required_argument, nullptr, 'd'},
    {nullptr,   no_argument,       nullptr,  0}
  };
  
  int c = 0, idx = 0;
  while ((c = getopt_long(argc, argv, "vs:g:a:d:", options, &idx)) != -1) {
    switch (c) {
      case 'v':
        path::verbose.rdbuf(std::cout.rdbuf());
        path::verbose << std::fixed << std::setprecision(2);
        break;
      case 's':
        start = optarg;
        break;
      case 'g':
        goal = optarg;
        break;
      case 'a':
        alg = optarg;
        if (alg != "BFS" && alg != "ID" && alg != "ASTAR") {
          std::cerr << "Unknown algorithm `" << alg << "`\n";
          return false;
        }
        break;
      case 'd':
        if (!isint(optarg)) {
          std::cerr << "Invalid argument. The argument of `-d` or `--depth` should be an integer type\n";
          return false;
        }
        depth = std::stoi(optarg);
        if (depth <= 0) {
          std::cerr << "Invalid argument. The argument of `-d` or `--depth` should be positive\n";
          return false;
        }
        break;
      default:
        return false;
    }
  }
  
  if (start.empty()) {
    std::cerr << "Missing arguments. Require a start node, use `-s` or `--start to specify\n";
    return false;
  }
  if (goal.empty()) {
    std::cerr << "Missing arguments. Require a goal node , use `-g` or `--goal to specify\n";
    return false;
  }
  if (alg.empty()) {
    std::cerr << "Missing arguments. Require an algorithm, use `-a` or `--alg to specify\n";
    return false;
  }
  if (alg == "ID" && depth == 0) {
    std::cerr << "Missing arguments. Algorithm ID requires an initial depth, use `-d` or `--depth to specify\n";
    return false;
  }
  if (alg != "ID" && depth != 0) {
    std::cerr << "Invalid arguments. Algorithm " << alg << " does not require an initial depth\n";
    return false;
  }
  
  argc -= optind;
  argv += optind;
  
  if (argc != 1) {
    std::cerr << (argc < 1 ? "Too few" : "Too many") << " arguments. Required one argument `<input_file>`\n";
    return false;
  }
  input = argv[0];
  return true;
}

bool load_vertices(const std::string &filename, path::Graph &graph) {
  std::ifstream in(filename);
  
  std::string line;
  size_t linenum = 0;
  while (std::getline(in, line)) {
    ++linenum;
    if (line.empty() || line[0] == '#') continue;
    
    std::istringstream ss(line);
    std::vector<std::string> tokens;
    std::string token;
    while (ss >> token)
      tokens.push_back(token);
    
    if (tokens.size() == 2) continue;
    else if (tokens.size() == 3 && isint(tokens[1]) && isint(tokens[2])) {
      graph.push_back(path::Node(tokens[0], stoi(tokens[1]), stoi(tokens[2])));
    }
    else {
      std::cerr << "Invalid input file format. line " << linenum << "\n";
      return false;
    }
  }
  return true;
}

bool load_edges(const std::string &filename, path::Graph &graph, std::unordered_map<std::string, size_t> &table) {
  std::ifstream in(filename);
  
  std::string line;
  size_t linenum = 0;
  while (std::getline(in, line)) {
    ++linenum;
    if (line.empty() || line[0] == '#') continue;
    
    std::istringstream ss(line);
    std::vector<std::string> tokens;
    std::string token;
    while (ss >> token)
      tokens.push_back(token);
    
    if (tokens.size() == 3) continue;
    
    if (table.find(tokens[0]) == table.end() || table.find(tokens[1]) == table.end()) {
      std::cerr << "Reference to vertex not in the file. line " << linenum << "\n";
      return false;
    }
    size_t u = table[tokens[0]], v = table[tokens[1]];
    graph[u].adj.insert(v);
    graph[v].adj.insert(u);
    
  }
  return true;
}

int main(int argc, char * argv[]) {
  if (!arguments(argc, argv)) return 1;
  
  path::Graph graph;
  if (!load_vertices(input, graph)) return 1;
  std::sort(graph.begin(), graph.end(), [](const path::Node &a, const path::Node &b) {
    return a.label < b.label;
  });
  
  size_t s, g;
  {
    std::unordered_map<std::string, size_t> table;
    for (size_t i = 0; i < graph.size(); ++i) {
      if (table.find(graph[i].label) != table.end()) {
        std::cerr << "Redefinition of vertex `" << graph[i].label << "`\n";
        return 1;
      }
      table[ graph[i].label ] = i;
    }
    if (!load_edges(input, graph, table)) return 1;
    if (table.find(start) == table.end()) {
      std::cerr << "Start node not in the file `" << input << "`\n";
      return 1;
    }
    if (table.find(goal) == table.end()) {
      std::cerr << "Goal node not in the file `" << input << "`\n";
      return 1;
    }
    s = table[start];
    g = table[goal];
  }
  
  path::PathSearch *ps = nullptr;
  if (alg == "BFS")     ps = new path::BFS(graph);
  else if (alg == "ID") ps = new path::ID(graph, depth);
  else                  ps = new path::ASTAR(graph);
  
  path::Path path;
  ps->find(s, g, path);
  
  delete ps;
  
  if (path.empty())
    std::cout << "No Solution\n";
  else
    std::cout << "Solution: " << path::to_string(path, graph) << "\n";
  
  return 0;
}
