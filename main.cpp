#include <crow.h>
#include <filesystem>
#include <iostream>
#include <ostream>
#include <vector>

void help() {
  // Print usage and help information
  std::cout << "usage: report_cpp [arg [...]].\n";
  std::cout << "\nArguments:\n";
  std::cout << "  --help      -h  - Show this message\n";
  std::cout << "  --http      -H  - Start HTTP server\n";
  std::cout
      << "  --address   -a  - HTTP server address (defaults to 0.0.0.0)\n";
  std::cout << "  --port      -p  - HTTP server port (defaults to 8080)\n";
  std::cout << "  --input     -i  - Input image path\n";
  std::cout << "  --output    -o  - Output image path\n";
}

struct ImageOptions {
  std::string imagePath;
  std::string outputPath;
  std::string Addr = "0.0.0.0";
  int Port = 8080;
  bool useHTTP;
};

ImageOptions parseCommandLine(int argc, char *argv[]) {
  ImageOptions opts;

  // Ensure there are enough command-line arguments
  if (argc <= 1) {
    std::cerr << "Not enough arguments.\n";
    help();
    throw std::invalid_argument("Not enough arguments.");
  }

  std::vector<std::string> args{argv + 1, argv + argc};
  args.push_back("a");

  for (auto it = args.begin(); it != args.end(); it++) {
    if (*it == "-h" || *it == "--help") {
      help();
      exit(0);
    } else if (*it == "-H" || *it == "--http") {
      opts.useHTTP = true;

      it++;
    } else if (*it == "-a" || *it == "--address") {
      if (std::next(it) == args.end()) {
        continue;
      }

      const char *next_arg = std::next(it)->c_str();
      opts.Addr = next_arg;

      it++;
    } else if (*it == "-p" || *it == "--port") {
      if (std::next(it) == args.end()) {
        continue;
      }
      const char *next_arg = std::next(it)->c_str();

      opts.Port = std::atoi(next_arg);
      it++;
    } else if (*it == "-i" || *it == "--input") {
      if (std::next(it) == args.end()) {
        std::cerr << "-i/--input needs a path to an image.\n";
        help();
        throw std::invalid_argument("-i/--input needs a path to an image.");
      }

      const char *next_arg = std::next(it)->c_str();
      if (!std::filesystem::exists(next_arg)) {
        std::cerr << "Image '" << next_arg << "' doesn't exist!\n";
        throw std::invalid_argument("Image not found.");
      }

      opts.imagePath = std::string(next_arg);
      it++;
    } else if (*it == "-o" || *it == "--output") {
      if (std::next(it) == args.end()) {
        std::cerr << "-o/--output needs an output filename.\n";
        help();
        throw std::invalid_argument("-o/--output needs an output filename.");
      }

      opts.outputPath = std::next(it)->c_str();
      it++;
    } else {
      std::cerr << "Unknown option '" << *it << "'!\n";
      help();
      throw std::invalid_argument("Unknown option.");
    }
  }

  return opts;
}

int main(int argc, char *argv[]) {
  ImageOptions opts = parseCommandLine(argc, argv);
  // start crow web server if opts.useHTTP is true
  if (opts.useHTTP) {
    std::cout << "Starting HTTP server at " << opts.Addr << ":" << opts.Port
              << std::endl;
    crow::SimpleApp app;
    // TODO create route that accepts image
    app.bindaddr(opts.Addr).port(opts.Port).run();
  }
  // do local image manipulation if HTTP not used
  else {
    // TODO create function that accepts input and output path to image
  }
}