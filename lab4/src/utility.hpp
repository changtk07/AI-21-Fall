#ifndef UTILITY_HPP
#define UTILITY_HPP

#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

namespace utility {

struct Instance {
  std::string label;
  std::vector<double> attr;
};

bool isulong(const std::string &str);
bool isint(const std::string &str);
bool isalnum(const std::string &str);

double e2dist(const std::vector<double> &a, const std::vector<double> &b);
double manhdist(const std::vector<double> &a, const std::vector<double> &b);
// double edist(const std::vector<double> &a, const std::vector<double> &b);

}

namespace csv {

void split(const std::string &str, std::vector<std::string> &vec);
bool load(const std::string &filename, std::vector<std::vector<std::string> > &v);
std::size_t convert(const std::vector<std::string> &v, utility::Instance &inst);

}

bool utility::isulong(const std::string &str) {
  std::size_t i = 0;
  try {
    std::stoul(str, &i);
  } catch(const std::exception &e) {
    return false;
  }
  return i==str.size();
}

bool utility::isint(const std::string &str) {
  std::size_t i = 0;
  try {
    std::stoi(str, &i);
  } catch(const std::exception &e) {
    return false;
  }
  return i==str.size();
}

bool utility::isalnum(const std::string &str) {
  for (const char &c :str) {
    if (!std::isalnum(c) && c!='_') return false;
  }
  return true;
}

double utility::e2dist(const std::vector<double> &a, const std::vector<double> &b) {
  double dist = 0;
  for (std::size_t i = 0; i < a.size(); ++i) {
    dist += (a[i]-b[i]) * (a[i]-b[i]);
  }
  return dist;
}

double utility::manhdist(const std::vector<double> &a, const std::vector<double> &b) {
  double dist = 0;
  for (std::size_t i = 0; i < a.size(); ++i) {
    dist += std::abs(a[i]-b[i]);
  }
  return dist;
}

// double utility::edist(const std::vector<double> &a, const std::vector<double> &b) {
//   double dist = 0;
//   for (std::size_t i = 0; i < a.size(); ++i) {
//     dist += std::sqrt((a[i]-b[i])*(a[i]-b[i]));
//   }
//   return dist;
// }

void csv::split(const std::string &str, std::vector<std::string> &vec) {
  std::istringstream iss(str);
  std::string token;
  while (std::getline(iss, token, ',')) {
    vec.push_back(token);
  }
}

bool csv::load(const std::string &filename, std::vector<std::vector<std::string> > &v) {
  std::ifstream in(filename);
  if (!in) return false;
  v.clear();
  std::string line;
  while (std::getline(in, line)) {
    if (line.empty()) continue;
    v.push_back({});
    split(line, v.back());
  }
  return true;
}

std::size_t csv::convert(const std::vector<std::string> &v, utility::Instance &inst) {
  for (std::size_t i = 0; i < v.size()-1; ++i) {
    if (!utility::isint(v[i])) return i+1;
    inst.attr.push_back(std::stoi(v[i]));
  }
  if (!utility::isalnum(v.back())) return v.size();
  inst.label = v.back();
  return 0;
}

#endif /* UTILITY_HPP */