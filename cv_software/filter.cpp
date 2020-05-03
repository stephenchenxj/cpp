#include <chrono>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

struct Point {
  float x{0};
  float y{0};
  float z{0};
  float intensity{0};
  bool groundtruth_bloom_classification{0};
  bool bloom_classification{0};
};

using PointCloud = std::vector<Point>;

PointCloud loadFile(const std::string &file_path) {
  PointCloud cloud{};

  std::ifstream csv_file(file_path);
  std::string line;

  // Loop over all lines in the file
  while (std::getline(csv_file, line)) {
    Point p{};
    std::stringstream ss(line);
    double element{0};

    ss >> element;
    ss.ignore();
    p.x = element;

    ss >> element;
    ss.ignore();
    p.y = element;

    ss >> element;
    ss.ignore();
    p.z = element;

    ss >> element;
    ss.ignore();
    p.intensity = element;

    ss >> element;
    ss.ignore();
    ss >> element;
    ss.ignore();

    ss >> element;
    ss.ignore();
    p.groundtruth_bloom_classification = element;

    cloud.push_back(p);
  }

  return cloud;
}

float computeDepth(Point p) {
  return std::pow(p.x, 2) + std::pow(p.y, 2) + std::pow(p.z, 2);
}

void filter(PointCloud &cloud) {
  const std::int64_t WIDTH{1000};
  const std::int64_t HEIGHT = cloud.size() / WIDTH;

  const std::int64_t WINDOW_HEIGHT{13 / 2};
  const std::int64_t WINDOW_WIDTH{21 / 2};

  const float MAX_BLOOM_INTENSITY{50};
  const float RETRO_INTENSITY{2048};
  const float NEIGHBOR_DEPTH_DIFFERENCE{0.1};

  const std::int64_t MIN_RETRO_COUNT{21};
  const std::int64_t MIN_TINY_COUNT{21};

  for (std::int64_t row = 0; row < HEIGHT; ++row) {
    for (std::int64_t col = 0; col < WIDTH; ++col) {

      std::int64_t current_index{row * WIDTH + col};
      auto &current_point = cloud.at(current_index);

      std::int64_t num_neighbor_retros{0};
      std::int64_t num_neighbor_tinys{0};

      for (std::int64_t kr{-WINDOW_HEIGHT}; kr <= WINDOW_HEIGHT; ++kr) {
        for (std::int64_t kc{-WINDOW_WIDTH}; kc <= WINDOW_WIDTH; ++kc) {

          std::int64_t neighbor_row{row + kr};
          std::int64_t neighbor_col{col + kc};
          std::int64_t neighbor_index{neighbor_row * WIDTH + neighbor_col};

          if (neighbor_row < 0 || neighbor_row >= HEIGHT || neighbor_col < 0 ||
              neighbor_col >= WIDTH) {
            continue;
          }

          auto neighbor_point = cloud.at(neighbor_index);

          auto current_depth = computeDepth(current_point);
          auto neighbor_depth = computeDepth(neighbor_point);

          bool points_are_neighbors =
              neighbor_depth - current_depth < NEIGHBOR_DEPTH_DIFFERENCE;

          if (points_are_neighbors &&
              current_point.intensity <= MAX_BLOOM_INTENSITY) {
            if (neighbor_point.intensity >= RETRO_INTENSITY) {
              num_neighbor_retros++;
            } else if (neighbor_point.intensity <= MAX_BLOOM_INTENSITY) {
              num_neighbor_tinys++;
            }
          }
        }
      }

      if (num_neighbor_retros > MIN_RETRO_COUNT &&
          num_neighbor_tinys > MIN_TINY_COUNT) {
        current_point.bloom_classification = 1;
      }
    }
  }
}

double computeRecall(const PointCloud &cloud) {
  double s{0.0};

  // Compute recall

  return s;
}

double computePrecision(const PointCloud &cloud) {
  double s{0.0};

  // Compute precision

  return s;
}

int main(int argc, char **argv) {
  if (argc > 1) {
    using namespace std::chrono;
    std::cout << "File chosen: " << argv[1] << std::endl;

    auto start = high_resolution_clock::now();
    auto cloud = loadFile(argv[1]);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    std::cout << "Loading file took: " << duration.count() << " usec"
              << std::endl;

    start = high_resolution_clock::now();
    filter(cloud);
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    std::cout << "Filter took: " << duration.count() << " usec" << std::endl;

    std::cout << "recall = " << computeRecall(cloud) << std::endl;
    std::cout << "precision = " << computePrecision(cloud) << std::endl;
  }

  return EXIT_SUCCESS;
}
