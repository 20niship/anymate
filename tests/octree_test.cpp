#include <chrono>
#include <movutl/core/octree.hpp>
#include <math.h>
#include <random>

using namespace std;
using namespace mu::core;


int BruteForceSearch(const Vec<Vec3>& dataset, const Vec3& query) {
  int ind     = -1;
  double dist = 10000.0;

  for(size_t i = 0; i < dataset.size(); i++) {
    const auto tdist = (query - dataset[i]).norm();
    if(tdist < dist) {
      dist = tdist;
      ind  = i;
    }
  }
  return ind;
}

int main() {
  cout << "start" << endl;
  const int nData = 1000000;

  mu::core::Vec<mu::core::Vec3> dataset;
  dataset.resize(nData);

  std::random_device rd;
  std::mt19937 mt(rd());

  // datasetを作成
  std::uniform_real_distribution<float> score(-10.0, 10.0);
  for(int i = 0; i < nData; i++) dataset[i] = {score(mt), score(mt), score(mt)};

  const Vec3 query(score(mt), score(mt), score(mt));
  DISP(query);

  std::cout << "start octree" << std::endl;
  mu::core::Octree2<double, 10> octree;
  std::cout << "inserting......" << std::endl;
  octree.set_dataset(&dataset);
  octree.report();
  std::cout << "Done!" << std::endl;


  // Bruteforce
  {
    const auto start = chrono::system_clock::now();
    int ind          = BruteForceSearch(dataset, query);
    const auto end   = chrono::system_clock::now();
    double msec      = (double)chrono::duration_cast<chrono::nanoseconds>(end - start).count() / 1000.0;

    const auto dist = (dataset[ind] - query).norm();
    std::cout << "bruteforce : " << dist << " at " << ind << " = " << dataset[ind] << std::endl;
    std::cout << "time:" << msec << std::endl;
  }

  {
    const auto start = chrono::system_clock::now();
    const auto res   = octree.findNearest(query, SearchMethod::Sphere, 0.2);
    const auto end   = chrono::system_clock::now();
    double msec      = (double)chrono::duration_cast<chrono::nanoseconds>(end - start).count() / 1000.0;

    if(res.size() > 0) {
      for(const auto& i : res) {
        const auto p    = dataset[i];
        const auto dist = (p - query).norm();
        std::cout << dist << " at " << i << " =  " << p << std::endl;
      }
    }
    std::cout << "time:" << msec << std::endl;
  }

  return 0;
}
