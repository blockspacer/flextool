#pragma once

#include <base/files/file_path.h>
#include <base/macros.h>
#include <base/sequenced_task_runner.h>
#include <base/stl_util.h>
#include <base/strings/string_split.h>
#include <base/trace_event/trace_event.h>

#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/option.hpp>
#include <boost/utility/in_place_factory.hpp>

#include <string>
#include <vector>

namespace cmd {

class UnergisteredOption
{
public:
  UnergisteredOption(
    const std::string& key
    , const std::vector<std::string>& values);

  UnergisteredOption(UnergisteredOption&& other);

  const std::string key;

  const std::vector<std::string> values;

  /// \note combines key with SINGLE value
  std::string KVToSting(
    const size_t value_index
    // separator for key and value concatenation
    , const std::string& separator = "") const;

  // useful for debug purposes
  /// \note combines key with ALL values WITHOUT separator
  // USAGE:
  // auto res = static_cast<std::string>(unergisteredOption);
  operator std::string() const;

private:
  DISALLOW_COPY_AND_ASSIGN(UnergisteredOption);
};

/// \note must be as simple as possible,
/// it is interface to communicate with boost::program_options
class BoostCmd {
public:
  // type of boost::program_options::basic_option
  // and boost::program_options::basic_parsed_options
  using charType = char;

  BoostCmd();

  ~BoostCmd();

  bool init(int argc, char* argv[]);

  // may be used to print help for ALL registered command-line options
  std::string optionsToString();

  boost::program_options::options_description_easy_init&
  options();

  size_t count(const base::StringPiece& key);

  // get options not present in
  // boost::program_options::options_description_easy_init
  // i.e. cmd args passed by user, but unknown for program_options
  /// \note requires to set allow_unregistered()
  /// \note cache result of this function call
  /// if you use it in performance critical code
  std::vector<UnergisteredOption> unregisteredOptions();

  /// \note cache result of this function call
  /// if you use it in performance critical code
  template<typename T>
  const T& getAs(const base::StringPiece& key)
  {
    CHECK(!key.empty());
    CHECK(!vm_.empty());

    // Command-line argument can have alias.
    // We expect comma as delimiter.
    // Example: outdir,R
    std::vector<base::StringPiece> key_split
      = base::SplitStringPiece(
          key
          , ","
          , base::TRIM_WHITESPACE
          , base::SPLIT_WANT_NONEMPTY);
    boost::program_options::variables_map::const_iterator optIter;
    for(const base::StringPiece& part: key_split) {
      optIter = vm_.find(part.as_string());
      if(optIter != vm_.end()) {
        DVLOG(9)
            << "Found associated command-line argument: "
            << part
            << " for key "
            << key;
        break;
      } else {
        DVLOG(9)
            << "Unable to find associated command-line argument: "
            << part
            << " for key "
            << key;
      }
    }

    CHECK(optIter != vm_.end())
        << "Unable to find any of command-line arguments: "
        << key;

    CHECK(vm_.count((*optIter).first))
        << "Unable to count any of command-line arguments: "
        << key;

    return
      (*optIter).second.as<T>();
  }

private:
  boost::program_options::variables_map vm_{};

  std::unique_ptr<
    boost::program_options::basic_parsed_options<charType>
    > parsed_options_;

  boost::program_options::options_description desc_;

  boost::program_options::options_description_easy_init
    options_;

  SEQUENCE_CHECKER(sequence_checker_);

  DISALLOW_COPY_AND_ASSIGN(BoostCmd);
};

} // namespace cmd
