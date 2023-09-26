#include "Magick++/Functions.h"
#include "manipulate.h"
#include <Magick++.h>
#include <crow.h>
#include <filesystem>
#include <iostream>
#include <vector>

void help() {
  // Print usage and help information
  std::cout << "        usage: gazou -i /path/to/image [operations[]] -o "
               "/path/to/output.\n";
  std::cout << "as web server: gazou -H -a 127.0.0.1 -p 8080\n";

  std::cout << "\nArguments:\n";
  std::cout << "  --help      -h    - Show this message\n";
  std::cout << "  --input     -i    - Input image path\n";
  std::cout << "  --output    -o    - Output image path\n";

  std::cout << "\nOperations:\n";
  std::cout << "  resize [size or width x height]         - resize to pixel\n";
  std::cout << "  crop [width, height, offsetX, offsetY]  - crop to "
               "argument, see man page\n";

  std::cout << "\nHTTP server will ignore all other arguments:\n";
  std::cout << "  --http      -H  - Start HTTP server\n";
  std::cout
      << "  --address   -a  - HTTP server address (defaults to 127.0.0.1)\n";
  std::cout << "  --port      -p  - HTTP server port (defaults to 8080)\n";
}

struct ImageOptions {
  std::string imagePath;
  std::string outputPath;
  std::string Addr = "127.0.0.1";
  int Port = 8080;
  bool useHTTP = false;
  std::vector<std::string> operations; // Store the specified operations
  std::unordered_map<std::string, std::string>
      operationArgs; // Store arguments for operations
};

ImageOptions parseCommandLine(int argc, char *argv[]) {
  ImageOptions opts;

  // Ensure there are enough command-line arguments
  if (argc <= 1) {
    std::cerr << "Not enough arguments.\n";
    throw std::invalid_argument("Not enough arguments.");
    help();
  }

  // Store the arguments inside a vector
  std::vector<std::string> args{argv + 1, argv + argc};

  // Iterate through the command-line arguments
  for (auto it = args.begin(); it != args.end(); it++) {
    std::cout << *it << std::endl;
    if (*it == "-h" || *it == "--help") {
      help();
      exit(0);
    } else if (*it == "-H" || *it == "--http") {
      opts.useHTTP = true;
    } else if (*it == "-a" || *it == "--address") {
      const char *next_arg = std::next(it)->c_str();
      opts.Addr = next_arg;
      it++;
    } else if (*it == "-p" || *it == "--port") {
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
    } else if (*it == "resize" || *it == "crop") {
      if (std::next(it) == args.end()) {
        std::cerr << *it << " needs dimension to operate.\n";
        help();
        throw std::invalid_argument(*it + " needs dimension to operate.");
      }
      const char *next_arg = std::next(it)->c_str();
      // Store the operation and its argument in operations and operationArgs
      opts.operations.push_back(*it);
      opts.operationArgs[*it] = next_arg;
      it++;
    } else {
      std::cerr << "Unknown option '" << *it << "'!\n";
      help();
      throw std::invalid_argument("Unknown option.");
    }
  }

  return opts;
}

// Map operations from cmdline args or HTTP Request to their functions
std::unordered_map<std::string, void (*)(Magick::Image &, const std::string &)>
    imageOperations = {
        {"resize", ImageManipulator::resize},
        {"crop", ImageManipulator::crop},
};

int main(int argc, char **argv) {
  Magick::InitializeMagick(*argv);
  Magick::Image image;

  //  Parse command-line arguments
  ImageOptions opts = parseCommandLine(argc, argv);

  // Start crow web server if opts.useHTTP is true
  if (opts.useHTTP) {
    std::cout << "Starting HTTP server at " << opts.Addr << ":" << opts.Port
              << std::endl;
    crow::SimpleApp app;
    CROW_ROUTE(app, "/test")
    ([](const crow::request &req) {
      crow::multipart::message msg(req);
      CROW_LOG_INFO << "first part's body" << msg.parts[0].body;
      return "Yatta";
    });
    app.bindaddr(opts.Addr).port(opts.Port).run();
  }
  // Do local image manipulation if HTTP is not used
  else {
    image.read(opts.imagePath);

    // Apply image processing operations based on the operation arguments
    // This operation will follow the order of operations. So if we resize
    // then crop, the result will differ if we crop then resize.
    for (const auto &arg : opts.operations) {
      auto it = imageOperations.find(arg);
      if (it != imageOperations.end()) {
        it->second(image, opts.operationArgs[arg]);
      }
    }

    image.write(opts.outputPath);
  }

  return 0;
}