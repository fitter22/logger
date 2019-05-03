#include <string>
#include <memory>
#include <fstream>
#include <thread>
#include <mutex>
#include <sstream>
#include <iostream>
#include <condition_variable>
#include <thread>

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



class log_logic_interface
{
  public:
    virtual void open_ostream(const std::string& name) = 0;
    virtual void close_ostream() = 0;
    virtual void write_thread(std::string& msg) = 0;
};

class log_logic : public log_logic_interface
{
  private:
    std::ofstream file_stream;
    std::string full_msg;
    std::condition_variable write_cond;
    void write(std::string msg);
    std::mutex write_mutex;
  public:
    log_logic() {}
    void open_ostream(const std::string& name);
    void close_ostream();
    void write_thread(std::string& msg);
    ~log_logic();
};

void log_logic::open_ostream(const std::string& name)
{
  file_stream.open(name.c_str(), std::ios_base::binary|std::ios_base::out);
  if(!file_stream.is_open())
  {
    throw(std::runtime_error("LOGGER: Unable to open an output stream"));
  }
}

void log_logic::close_ostream()
{
  if(file_stream)
  {
    file_stream.close();
  }
}

void log_logic::write(std::string full_msg)
{
  write_mutex.lock();
  file_stream<<full_msg<<std::endl;
  write_mutex.unlock();
}

void log_logic::write_thread(std::string& msg)
{
  std::thread t(&log_logic::write, this, msg);
  t.detach();
}

log_logic::~log_logic()
{
    close_ostream();
}


//------------------------------------------
// Main logger instance
//------------------------------------------

class logger
{
  private:
    std::string get_time();
    std::string get_header(severity_type severity);
    std::string log_stream;
    log_logic* policy;
    output_type output;

  public:
    logger(const std::string& name, output_type output_type);
    void print(const std::string& msg, severity_type severity);
    ~logger();
};


void logger::print(const std::string& msg, severity_type severity)
{
  std::string log_msg = get_header(severity) + log_stream + msg;

  if(output == output_type::file)
  {
    policy->write_thread(log_msg);
  }
  else if (output == output_type::standard_output)
  {
    std::cout<<log_msg<<std::endl;
  }
}


std::string logger::get_time()
{
    std::string time_str;
    time_t raw_time;
    time(& raw_time);
    time_str = ctime(&raw_time);
    return time_str.substr(0 , time_str.size() - 1); // delete newline character
}

std::string logger::get_header(severity_type severity)
{
    std::stringstream header;
    header.str("");
    header<<"<"<<get_time()<<" - ";
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

logger::logger(const std::string& name, output_type output_type)
{
  output = output_type;

  policy = new log_logic;
  if(!policy)
  {
    throw std::runtime_error("LOGGER: Unable to create the logger instance");
  }

  if(output == output_type::file)
  {
    policy->open_ostream(name);
  }
}

logger::~logger()
{
    if(policy)
    {
      if(output == output_type::file)
      {
        policy->close_ostream();
      }
      delete policy;
    }
}

// make static logger obj, so it lives until the program ends
static logger log_inst("execution.log", output_type::standard_output);
