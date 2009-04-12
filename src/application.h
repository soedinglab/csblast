// Copyright 2009, Andreas Biegert

#ifndef SRC_APPLICATION_H_
#define SRC_APPLICATION_H_

#include <cstdio>
#include <cstdlib>

#include <string>

#include "globals.h"
#include "getopt_pp.h"

using namespace GetOpt;

namespace cs {

// Basic (abstract) application class.
// Defines the high level behavior of an application. A new application is
// written by deriving a class from Application and writing an implementation
// of the run() and maybe some other (like parse_options() etc.) methods.
class Application {
 public:
  // Register the application instance.
  Application();
  // Clean up the application settings, flush the diagnostic stream.
  virtual ~Application();

  // Main function (entry point) for the application.
  int main(int argc,                 // argc in a regular main
           char* argv[],             // argv in a regular main
           FILE* fout,               // output stream
           const std::string& name   // application name
           );

 protected:
  // Returns output stream for this application.
  FILE* stream() const { return output_fp_; }

 private:
  // Copyright string for usage output.
  static const char* kCopyright;

  // Runs the application. To be implemented by derived classes.
  virtual void run() = 0;
  // Parses command line options.
  virtual void parse_options(GetOpt_pp& /* options */) const { };
  // Prints options summary to stream.
  virtual void print_options() const { };
  // Prints options summary to stream.
  virtual void print_banner() const { };
  // Prints short application description.
  virtual void print_description() const { };
  // Prints copyright notification.
  void print_usage() const;

  static Application* instance_;       // current app. instance
  std::string         app_name_;       // application name
  std::string         log_level_;      // log reporting level
  std::string         log_file_name_;  // name of logfile
  FILE*               log_file_fp_;    // file pointer to logfile
  FILE*               output_fp_;      // file pointer to output stream
};

}  // namespace cs

#endif  // SRC_APPLICATION_H_