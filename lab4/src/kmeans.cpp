#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <climits>
#include <algorithm>
#include <getopt.h>

#include "utility.hpp"

using utility::Instance;

struct Args {
  bool e2dist = true;
  std::string data;
  std::vector<Instance> centroids;
};

std::vector<double>& operator+=(std::vector<double> &lhs, const std::vector<double> &rhs) {
  for (std::size_t i = 0; i < lhs.size(); ++i) {
    lhs[i] += rhs[i];
  }
  return lhs;
}

std::vector<double>& operator/=(std::vector<double> &lhs, const double &rhs) {
  for (std::size_t i = 0; i < lhs.size(); ++i) {
    lhs[i] /= rhs;
  }
  return lhs;
}

bool arguments(int argc, char *argv[], Args &args) {
  static const option options[] = {
    {"dist",  required_argument, nullptr, 'd'},
    {"data",  required_argument, nullptr, 'f'},
    {nullptr, no_argument,       nullptr,  0}
  };
  
  int c = 0, idx = 0;
  while ((c = getopt_long(argc, argv, "d:f:", options, &idx)) != -1) {
    switch (c) {
      case 'd': {
        std::string s = optarg;
        if (s != "e2" && s != "manh") {
          std::cerr << "Error: invalid argument for --dist, -d `" << optarg << "`, should be either `e2` or `manh`.\n";
        }
        args.e2dist = s=="e2";
        break;
      }
      case 'f': {
        args.data = optarg;
        break;
      }
      default:
        return false;
    }
  }

  if (args.data.empty()) {
    std::cerr << "Error: require data file.\n";
    return false;
  }
  
  argc -= optind;
  argv += optind;

  std::size_t nattr = 0;
  for (int i = 0; i < argc; ++i) {
    std::vector<std::string> v;
    csv::split(argv[i], v);
    v.push_back("C" + std::to_string(i+1));

    if (nattr == 0) {
      nattr = v.size();
    }

    if (nattr != v.size()) {
      std::cerr << "Error: the size of centroids attributes do not match, `" << argv[i] << "`.\n";
      return false;
    }

    args.centroids.push_back(Instance());
    if (csv::convert(v, args.centroids.back())) {
      std::cerr << "Error: invalid centroid `" << argv[i] << "`.\n";
      return false;
    }
  }

  if (args.centroids.empty()) {
    std::cerr << "Error: require at least one centroid.\n";
    return false;
  }

  return true;
}

bool load_csv(const std::string &filename, std::vector<Instance> &inst) {
  std::vector<std::vector<std::string> > v;
  if (!csv::load(filename, v)) {
    std::cerr << "Error: cannot open file `" << filename << "`.\n";
    return false;
  }
  for (std::size_t row = 1, col; row <= v.size(); ++row) {
    inst.push_back(Instance());
    if ((col = csv::convert(v[row-1], inst.back()))) {
      std::cerr << "Error: invalid token `" << v[row-1][col-1] << "`, row " << row << " col " << col << "\n"; 
      return false;
    }
  }
  return true;
}

std::vector<std::vector<std::size_t> > kmeans(const std::vector<Instance> &data, std::vector<Instance> &centroids, const bool &e2dist) {
  double (*dist)(const std::vector<double>&, const std::vector<double>&);
  dist = e2dist ? &utility::e2dist : &utility::manhdist;
  
  std::vector<std::vector<std::size_t> > c1(centroids.size()), c2(centroids.size());

  while (true) {
    std::fill(c2.begin(), c2.end(), std::vector<std::size_t>());
    for (std::size_t i = 0; i < data.size(); ++i) {
      double mindist = std::numeric_limits<double>::max();
      std::size_t idx;
      for (std::size_t j = 0; j < centroids.size(); ++j) {
        double d = dist(data[i].attr, centroids[j].attr);
        if (d < mindist) {
          mindist = d;
          idx = j;
        }
      }
      c2[idx].push_back(i);
    }

    if (c1 == c2) break;

    for (std::size_t i = 0; i < centroids.size(); ++i) {
      if (c2[i].empty()) continue;
      std::fill(centroids[i].attr.begin(), centroids[i].attr.end(), 0);
      for (const std::size_t &j : c2[i]) {
        centroids[i].attr += data[j].attr;
      }
    }
    for (std::size_t i = 0; i < centroids.size(); ++i) {
      if (c2[i].empty()) continue;
      centroids[i].attr /= c2[i].size();
    }
    
    c1.swap(c2);
  }
  return c1;
}

int main(int argc, char *argv[]) {
  Args args;
  if (!arguments(argc, argv, args)) {
    return 1;
  }

  std::vector<Instance> data;
  load_csv(args.data, data);

  if (data.front().attr.size() != args.centroids.front().attr.size()) {
    std::cerr << "Error: the attributes sizes of centroids and data file do not match.\n";
    return 1;
  }

  auto C = kmeans(data, args.centroids, args.e2dist);
  for (std::size_t i = 0; i < C.size(); ++i) {
    std::cout << args.centroids[i].label << " = {";
    for (std::size_t j = 0; j < C[i].size(); ++j) {
      std::cout << (j ? "," : "") << data[C[i][j]].label;
    }
    std::cout << "}\n";
  }

  std::cout << std::fixed << std::setprecision(3);

  for (std::size_t i = 0; i < args.centroids.size(); ++i) {
    std::cout << args.centroids[i].label << " : [";
    for (std::size_t j = 0; j < args.centroids[i].attr.size(); ++j) {
      std::cout << (j ? "," : "") << args.centroids[i].attr[j];
    }
    std::cout << "]\n";
  }

  return 0;
}

