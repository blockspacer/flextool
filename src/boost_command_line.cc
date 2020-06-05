#include "flextool/boost_command_line.hpp" // IWYU pragma: associated

namespace po = boost::program_options;

namespace {

/// \todo code repeat
template<class T>
std::ostream& operator<<(
  std::ostream& stream, const std::vector<T>& data)
{
  std::copy(data.begin(), data.end(),
    std::ostream_iterator<T>(stream, " "));
  return stream;
}

} // namespace

namespace cmd {

UnergisteredOption::
  operator std::string() const
{
  //CHECK(key);
  //CHECK(values);
  std::string result = /***/key;
  for (const auto &piece : /***/values) {
    result += piece;
  }
  return result;
}

UnergisteredOption::UnergisteredOption(
  const std::string /***/key
  , const std::vector<std::string> /***/values)
  : key(key)
  , values(values)
{}

UnergisteredOption::UnergisteredOption(
  UnergisteredOption &&other)
  : key(other.key)
  , values(other.values)
{}

std::string
UnergisteredOption::KVToSting(
    const size_t value_index
    , const std::string &separator) const
{
  //CHECK(key);
  //CHECK(values);
  CHECK(value_index >=0 && value_index < (/***/values).size());
  VLOG(9)
    << "KVToSting. key: "
    << key
    << " separator: "
    << separator
    << " values: "
    << values
    << " value: "
    << (/***/values)[value_index];
  return /***/key + separator + (/***/values)[value_index];
}

BoostCmd::BoostCmd()
  : desc_("Allowed options")
  , options_(desc_.add_options())
{
  DETACH_FROM_SEQUENCE(sequence_checker_);
}

BoostCmd::~BoostCmd()
{
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
}

size_t BoostCmd::count(const base::StringPiece &key)
{
  CHECK(!key.empty());
  CHECK(key.find_first_of(',')
        == base::StringPiece::npos);
  return vm_.count(key.as_string());
}

boost::program_options::options_description_easy_init&
  BoostCmd::options()
{
  return options_;
}

std::string BoostCmd::optionsToString()
{
  std::stringstream stream;
  desc_.print(stream);
  return stream.str();
}

std::vector<UnergisteredOption>
  BoostCmd::unregisteredOptions()
{
  CHECK(parsed_options_);
  CHECK(!vm_.empty());

  std::vector<UnergisteredOption> result;

  for (const boost::program_options::basic_option<charType>& opt
       : parsed_options_->options)
  {
    CHECK(opt.string_key.find_first_of(',')
          == base::StringPiece::npos);
    const bool isOptionSpecified
        = vm_.find(opt.string_key) != vm_.end();
    if (!isOptionSpecified)
    {
      VLOG(9)
        << "found UnergisteredOption with key: "
        << opt.string_key
        << " and values: "
        << opt.value;
      result.push_back(
        UnergisteredOption(
          opt.string_key
          , opt.value
        )
      );
    }
  }

  return result;
}

bool BoostCmd::init(int argc, char* argv[])
{
  DCHECK(argc > 0);

  DCHECK(!desc_.options().empty())
    << "you must register some command line options";

  try {
    // see https://theboostcpplibraries.com/boost.program_options
    parsed_options_ = std::make_unique<po::basic_parsed_options<char>>(
      po::command_line_parser(argc, argv).
        options(desc_)
        // see https://www.boost.org/doc/libs/1_73_0/doc/html/boost/program_options/command_line_style/style_t.html
        .style(po::command_line_style::default_style
           // Allow "--long_name" style.
           | po::command_line_style::allow_long
           // Allow "-<single character" style.
           | po::command_line_style::allow_short
           // Ignore the difference in case for all options.
           | po::command_line_style::case_insensitive
           // Allow "-" in short options.
           | po::command_line_style::allow_dash_for_short
           // Allow long options with single option starting character, e.g -foo=10
           | po::command_line_style::allow_long_disguise)
        .allow_unregistered()
        .run()
    );

    po::store(*parsed_options_, vm_);

    po::notify(vm_);

    DCHECK(!vm_.empty());
  }
  catch(std::exception& e) {
    // log all command-line arguments
    {
      LOG(ERROR)
        << "app arguments:";
      for(int i = 0; i < argc; ++i) {
        LOG(ERROR)
          << " "
          << argv[i]
          << " ";
      }
    }
    LOG(ERROR)
      << "ERROR: "
      << e.what();
    return false;
  }
  catch(...) {
    LOG(ERROR) << "ERROR: Exception of unknown type!";
    return false;
  }

  return true;
}

} // namespace cmd
