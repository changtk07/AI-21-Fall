#include "path.hpp"
#include <cmath>
#include <queue>
#include <algorithm>

std::ostream path::verbose(nullptr);

// constructors & destructors
path::Node::Node(const std::string &label, const int& x, const int& y) : label(label), x(x), y(y) {}

path::PathSearch::PathSearch(const Graph &graph) : G(graph) {}

path::BFS::BFS(const Graph &graph) : PathSearch(graph) {}

path::ID::ID(const Graph &graph, const int& depth) : PathSearch(graph), maxdepth(depth) {}

path::ASTAR::ASTAR(const Graph &graph) : PathSearch(graph) {}

path::PathSearch::~PathSearch() {}

// member functions
std::string path::to_string(const path::Path &path, const path::Graph &graph) {
  std::string s;
  if (!path.empty()) {
    s += graph[path[0]].label;
    for (size_t i = 1; i < path.size(); ++i) {
      s += " -> " + graph[path[i]].label;
    }
  }
  return s;
}

// member funcs
void path::BFS::find(const size_t &s, const size_t &g, Path &path) {
  std::vector<bool> visited(G.size());
  std::vector<size_t> prev(G.size(), -1);
  std::queue<size_t> Q({s});
  visited[s] = true;
  
  while (!Q.empty()) {
    size_t cur = Q.front();
    Q.pop();
    if (cur == g) break;
    
    verbose << "Expanding: " << G[cur].label << "\n";
    
    for (const size_t &next : G[cur].adj) {
      if (visited[next]) continue;
      visited[next] = true;
      Q.push(next);
      prev[next] = cur;
    }
  }
  
  if (prev[g] == (size_t)-1) return;
  
  size_t cur = g;
  while (cur != (size_t)-1) {
    path.push_back(cur);
    cur = prev[cur];
  }
  std::reverse(path.begin(), path.end());
}

bool path::ID::visit(std::vector<bool> &visited, const size_t &u, const size_t &g, const int &depth, Path &path) {
  visited[u] = true;
  
  if (u == g) {
    path.push_back(u);
    return true;
  }
  
  if (depth == maxdepth) {
    verbose << "hit depth=" << depth << ": " << G[u].label << "\n";
    hit = true;
    return false;
  }
  
  verbose << "Expand: " << G[u].label << "\n";
  
  for (const size_t &v : G[u].adj) {
    if (visited[v]) continue;
    
    if (visit(visited, v, g, depth+1, path)) {
      path.push_back(u);
      return true;
    }
  }
  return false;
}

void path::ID::find(const size_t &s, const size_t &g, Path &path) {
  hit = true;
  while (hit) {
    hit = false;
    std::vector<bool> visited(G.size());
    if (visit(visited, s, g, 0, path)) break;
    ++maxdepth;
  }
  std::reverse(path.begin(), path.end());
}

double path::ASTAR::dist(const size_t &u, const size_t &v) {
  return std::sqrt(std::pow(G[u].x-G[v].x, 2) + std::pow(G[u].y-G[v].y, 2));
}

void path::ASTAR::find(const size_t &s, const size_t &g, Path &path) {
  typedef std::pair<double, Path> Record;
  
  auto cmp = [](const Record &a, const Record &b) {
    return a.first > b.first;
  };
  std::priority_queue<Record, std::vector<Record>, decltype(cmp)> Q(cmp);
  
  Q.push({dist(s, g), {s}});
  
  while (!Q.empty()) {
    Path cur_path = Q.top().second;
    double cur_val = Q.top().first;
    Q.pop();
    
    if (cur_path.back() == g) {
      path = std::move(cur_path);
      break;
    }
    
    if (cur_path.size() > 1)
      verbose << "adding " << to_string(cur_path, G) << "\n";
    
    for (const size_t &next : G[cur_path.back()].adj) {
      double gval = cur_val - dist(cur_path.back(),g) + dist(cur_path.back(), next);
      double hval = dist(next, g);
      
      verbose << G[cur_path.back()].label << " -> " << G[next].label << " ; g=" << gval << " h=" << hval << " = " << gval+hval << "\n";
      
      if (std::find(cur_path.begin(), cur_path.end(), next) == cur_path.end()) {
        Path next_path;
        next_path.insert(next_path.end(), cur_path.begin(), cur_path.end());
        next_path.push_back(next);
        Q.push({gval+hval, next_path});
      }
    }
  }
}
