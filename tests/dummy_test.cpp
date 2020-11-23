#include <base/logging.h>
#include <base/i18n/icu_util.h>
#include <base/command_line.h>
#include <base/threading/platform_thread.h>
#include <base/base_switches.h>
#include <base/feature_list.h>
#include <base/at_exit.h>
#include <base/message_loop/message_loop.h>

#include <base/bind.h>
#include <base/test/launcher/unit_test_launcher.h>
#include <base/test/test_suite.h>
#include <build/build_config.h>

#include <memory>
#include <string>
#include <iostream>

int main(int argc, char* argv[]) {
  std::cout << "HI from " << FROM_HERE.function_name();
  std::unique_ptr<std::string> hello
    = std::make_unique<std::string>("Hi!");
  std::cout << *hello;
  //delete hello;
  return 0;
}
