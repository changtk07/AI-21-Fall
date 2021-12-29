#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <set>

#ifndef path_hpp
#define path_hpp

namespace path {

struct Node;
typedef std::vector<Node> Graph;
typedef std::vector<size_t> Path;

struct Node {
  std::string label;
  int x, y;
  std::set<size_t> adj;
  Node(const std::string &label, const int& x, const int& y);
};

extern std::ostream verbose;

std::string to_string(const path::Path &path, const path::Graph &graph);

class PathSearch {
protected:
  const Graph &G;
  
public:
  PathSearch(const Graph &graph);
  
  virtual ~PathSearch();
  
  virtual void find(const size_t &s, const size_t &g, Path &path) = 0;
};

class BFS : public PathSearch {
public:
  BFS(const Graph &graph);
  
  void find(const size_t &s, const size_t &g, Path &path) override;
};

class ID : public PathSearch {
private:
  int maxdepth;
  bool hit;
  
  bool visit(std::vector<bool> &visited, const size_t &u, const size_t &g, const int &depth, Path &path);
public:
  ID(const Graph &graph, const int &depth);
  
  void find(const size_t &s, const size_t &g, Path &path) override;
};

class ASTAR : public PathSearch {
private:
  double inline dist(const size_t& a, const size_t &b);
  
public:
  ASTAR(const Graph &graph);
  
  void find(const size_t &s, const size_t &g, Path &path) override;
};
}

#endif /* path_hpp */
