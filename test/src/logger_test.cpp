#include <limits.h>
#include "gtest/gtest.h"
#include "logger.hpp"
#include <regex>


class LoggerTest : public ::testing::Test
{
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

TEST_F(LoggerTest, get_time_format_test)
{
  std::string time = log_inst.getTime();
  std::regex time_regex("\\w{3} \\w{3}[ ]{1,2}\\d{1,2} \\d{2}:\\d{2}:\\d{2} \\d{4}");

  EXPECT_TRUE(std::regex_match(time ,time_regex));
}

TEST_F(LoggerTest, header_with_debug)
{
  std::string header = log_inst.getHeader(severity_type::debug);
  std::regex debug_header (".*<DEBUG>.*");

  EXPECT_TRUE(std::regex_match(header ,debug_header));
}

TEST_F(LoggerTest, header_with_warning)
{
  std::string header = log_inst.getHeader(severity_type::warning);
  std::regex warning_header (".*<WARN>.*");

  EXPECT_TRUE(std::regex_match(header ,warning_header));
}

TEST_F(LoggerTest, header_with_error)
{
  std::string header = log_inst.getHeader(severity_type::error);
  std::regex error_header (".*<ERROR>.*");

  EXPECT_TRUE(std::regex_match(header ,error_header));
}

TEST_F(LoggerTest, empty_filename_write_exception)
{
  Logger logger("", output_type::file);

  EXPECT_ANY_THROW(logger.print("msg", severity_type::debug));
}

TEST_F(LoggerTest, before_writing_file_does_not_exists)
{
  std::string file_name = "test.txt";

  Logger logger(file_name, output_type::file);
  std::ifstream f(file_name.c_str());

  EXPECT_FALSE(f.good());
}

TEST_F(LoggerTest, writing_creates_file_with_given_name)
{
  std::string file_name = "test.txt";

  Logger logger(file_name, output_type::file);
  logger.print("msg", severity_type::debug);
  std::ifstream f(file_name.c_str());
  bool state = f.good();
  f.close();
  remove(file_name.c_str());

  EXPECT_TRUE(state);
}
