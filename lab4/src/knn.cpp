#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <climits>
#include <getopt.h>

#include "utility.hpp"

using utility::Instance;

struct Args {
  bool unitw = false;
  bool e2dist = true;
  unsigned long k = 3;
  std::string train;
  std::string test;
};

struct Record {
  int C, Q, CQ;
};

bool arguments(int argc, char *argv[], Args &args) {
  static const option options[] = {
    {"unitw", no_argument,       nullptr, 'u'},
    {"k",     required_argument, nullptr, 'k'},
    {"dist",  required_argument, nullptr, 'd'},
    {"train", required_argument, nullptr, 't'},
    {"test",  required_argument, nullptr, 'e'},
    {nullptr, no_argument,       nullptr,  0}
  };
  
  int c = 0, idx = 0;
  while ((c = getopt_long(argc, argv, "uk:d:t:e:", options, &idx)) != -1) {
    switch (c) {
      case 'u': {
        args.unitw = true;
        break;
      }
      case 'k': {
        if (!utility::isulong(optarg) || (args.k = std::stoul(optarg))==0) {
          std::cerr << "Error: invalid argument for --k, -k `" << optarg << "`, should be a positive intger.\n";
          return false;
        }
        break;
      }
      case 'd': {
        std::string s = optarg;
        if (s != "e2" && s != "manh") {
          std::cerr << "Error: invalid argument for --dist, -d `" << optarg << "`, should be either `e2` or `manh`.\n";
        }
        args.e2dist = s=="e2";
        break;
      }
      case 't': {
        args.train = optarg;
        break;
      }
      case 'e': {
        args.test = optarg;
        break;
      }
      default:
        return false;
    }
  }

  if (args.train.empty()) {
    std::cerr << "Error: require training data.\n";
    return false;
  }

  if (args.test.empty()) {
    std::cerr << "Error: require test file.\n";
    return false;
  }
  
  argc -= optind;
  argv += optind;
  
  if (argc > 0) {
    std::cerr << "Error: too many arguments.";
    for (int i = 0; i < argc; ++i) {
      std::cerr << " `" << argv[i] << "`";
    }
    std::cerr << "\n";
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

std::string knn(std::vector<Instance> &train, const Instance &query, const Args &args) {
  double (*dist)(const std::vector<double>&, const std::vector<double>&);
  dist = args.e2dist ? &utility::e2dist : &utility::manhdist;

  std::sort(train.begin(), train.end(), [&](const Instance &a, const Instance &b) {
    return dist(a.attr, query.attr) < dist(b.attr, query.attr);
  });

  std::unordered_map<std::string, double> scores;
  for (std::size_t i = 0; i < args.k; ++i) {
    if (args.unitw) scores[ train[i].label ] += 1;
    else scores[ train[i].label ] += 1.0 / std::max(dist(train[i].attr, query.attr), 0.0001);
  }

  std::string predict;
  double maxscore = std::numeric_limits<double>::min();
  for (const auto &p : scores) {
    if (p.second > maxscore) {
      maxscore = p.second;
      predict = p.first;
    }
  }

  return predict;
}

int main(int argc, char *argv[]) {
  Args args;
  if (!arguments(argc, argv, args)) {
    return 1;
  }

  std::vector<Instance> train, test;
  if (!load_csv(args.train, train)) return 1;
  if (!load_csv(args.test, test)) return 1;

  std::map<std::string, Record> summary;
  for (const Instance &query : test) {
    std::string predict = knn(train, query, args);
    std::cout << "want=" << query.label << " got=" << predict << "\n";

    summary[query.label].C += 1;
    summary[predict].Q += 1;
    summary[predict].CQ += query.label==predict;
  }

  for (const auto &p : summary) {
    std::cout << "Label=" << p.first
              << " Precision=" << p.second.CQ << "/" << p.second.Q
              << " Recall=" << p.second.CQ << "/" << p.second.C << "\n";
  }

  return 0;
}