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
  std::regex time_regex("\\w{3} \\w{3}  \\d \\d{2}:\\d{2}:\\d{2} \\d{4}");
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

TEST_F(LoggerTest, empty_filename_exception)
{
  EXPECT_ANY_THROW(logger logger("", output_type::file));
}
