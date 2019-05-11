#include <string>
#include <memory>
#include <fstream>
#include <thread>
#include <mutex>
#include <sstream>
#include <iostream>

// logging library supports three severity levels
enum severity_type
{
   debug,
   error,
   warning
};

enum output_type
{
  standard_output,
  file
};

// wrap logging methods to make them better readable
#define LOG(msg) { log_inst.print(msg, severity_type::debug); }
#define LOG_ERR(msg) { log_inst.print(msg, severity_type::error); }
#define LOG_WARN(msg) { log_inst.print(msg, severity_type::warning); }


class LogLogic
{
  private:
    std::ofstream file_stream;
    std::string full_msg;
    output_type output;
    void writeToStdo(std::string &msg);
    void writeToFilestream(std::string msg);
    void writeThread(std::string& msg);
    void write(std::string &msg);
    void openStream(const std::string &name);
    void closeStream();
  public:
    LogLogic(const std::string &name, std::string &msg, output_type output_type);
    ~LogLogic() {};
};

void LogLogic::openStream(const std::string &name)
{
  file_stream.open(name.c_str(), std::ios_base::app|std::ios_base::out);
  if(!file_stream.is_open())
  {
    throw(std::runtime_error("LOGGER: Unable to open an output stream"));
  }
}

void LogLogic::closeStream()
{
  if(file_stream)
  {
    file_stream.close();
  }
}
void LogLogic::write(std::string &full_msg)
{
  if(output == output_type::file)
  {
    writeThread(full_msg);
  }
  else if (output == output_type::standard_output)
  {
    writeToStdo(full_msg);
  }
}

void LogLogic::writeToStdo(std::string &full_msg)
{
  std::cout<<full_msg<<std::endl;
}

void LogLogic::writeToFilestream(std::string full_msg)
{
  file_stream<<full_msg<<std::endl;
}

void LogLogic::writeThread(std::string &msg)
{
  std::thread t(&LogLogic::writeToFilestream, this, msg);
  t.detach();
}

LogLogic::LogLogic(const std::string &name, std::string &msg, output_type output_type)
{
  output = output_type;

  std::mutex write_mutex;
  std::lock_guard<std::mutex> lock(write_mutex);

  if (output == output_type::file)
  {
    openStream(name);
  }
  write(msg);
}

//------------------------------------------
// Main logger instance
//------------------------------------------

class Logger
{
  private:
    std::string log_stream;
    LogLogic *writer;
    output_type output;
    std::string file_name;

  public:
    std::string getTime();
    std::string getHeader(severity_type severity);
    Logger(const std::string &name, output_type output_type);
    void print(const std::string &msg, severity_type severity);
    ~Logger() {};
};

void Logger::print(const std::string &msg, severity_type severity)
{
  std::string log_msg = getHeader(severity) + log_stream + msg;

  writer = new LogLogic(file_name, log_msg, output);
  if(!writer)
  {
    throw std::runtime_error("LOGGER: Unable to create the logger instance");
  }
}

std::string Logger::getTime()
{
    std::string time_str;
    time_t raw_time;
    time(&raw_time);
    time_str = ctime(&raw_time);
    return time_str.substr(0 , time_str.size() - 1); // delete newline character
}

std::string Logger::getHeader(severity_type severity)
{
    std::stringstream header;
    header.str("");
    header<<"<"<<getTime()<<" - ";
    header.fill('0');
    header.width(7);
    header<<clock()<<"> "; // time in ms from the program start

    switch(severity)
    {
      case severity_type::debug:
        header<<"<DEBUG> ";
        break;
      case severity_type::warning:
        header<<"<WARN>  ";
        break;
      case severity_type::error:
        header<<"<ERROR> ";
        break;
    };

    return header.str();
}

Logger::Logger(const std::string &name, output_type output_type)
{
  output = output_type;
  file_name = name;
}


// make static logger obj, so it lives until the program ends
static Logger log_inst("execution.log", output_type::file);
