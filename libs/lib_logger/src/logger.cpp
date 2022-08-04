#include "logger.h"

#include <fstream>
#include <iostream>
#include <mutex>

namespace
{

  std::string get_time()
  {
    static char buffer[20];
    struct tm *time_struct;

    time_t now = time(0);
    time_struct = localtime(&now);
    strftime(buffer, sizeof(buffer), "%d-%m-%Y_%H-%M-%S", time_struct);

    return buffer;
  }

  std::string get_thread_id()
  {
    const auto thread_id = std::this_thread::get_id();

    std::ostringstream oss;
    oss << thread_id;

    return oss.str();
  }

} // namespace

namespace logger
{

  class Logger
  {
  public:
    static Logger &get_instance()
    {
      static Logger instance;
      return instance;
    }

    void set_path(const std::filesystem::path &path)
    {
      std::lock_guard<std::mutex> lock(mutex_);
      path_ = path;
    }

    void write_info(const std::string &message,
                    const std::string &function_prefix)
    {
      do_writing(get_time() + " [info] (" + function_prefix + ") (" +
                 get_thread_id() + ") " + message + "\n");
    }

    void write_warning(const std::string &message,
                       const std::string &function_prefix)
    {
      do_writing(get_time() + " [warning] (" + function_prefix + ") (" +
                 get_thread_id() + ") " + message + "\n");
    }

    void write_error(const std::string &message,
                     const std::string &function_prefix)
    {
      do_writing(get_time() + " [error] (" + function_prefix + ") (" +
                 get_thread_id() + ") " + message + "\n");
    }

  private:
    Logger() = default;
    ~Logger()
    {
      std::lock_guard<std::mutex> lock(mutex_);

      if (file_.is_open())
        file_.close();
    }

    Logger(const Logger &) = delete;
    Logger(Logger &&) = delete;
    Logger &operator=(Logger &) = delete;

    void do_writing(const std::string &message)
    {
      std::lock_guard<std::mutex> lock(mutex_);

      std::cout << message << std::flush;

      if (path_.empty())
        return;

      if (file_creating_time_.empty())
        file_creating_time_ = get_time();

      std::string filename(path_.c_str() + std::string("/") +
                           file_creating_time_ + std::string(".log"));
      file_.open(filename, std::ios::out | std::ios::app);
      if (!file_.is_open())
      {
        std::cout << "Logger error: Can't open file \""
                  << "\"" << std::endl;
      }
      else
      {
        file_.write(message.c_str(), message.size());

        if (file_.is_open())
          file_.close();
      }
    }

  private:
    std::mutex mutex_;

    std::string file_creating_time_;
    std::filesystem::path path_;
    std::fstream file_;
  };

  void info(const std::string &message, const std::string function_prefix)
  {
    Logger::get_instance().write_info(message, function_prefix);
  }

  void warning(const std::string &message, const std::string function_prefix)
  {
    Logger::get_instance().write_warning(message, function_prefix);
  }

  void error(const std::string &message, const std::string function_prefix)
  {
    Logger::get_instance().write_error(message, function_prefix);
  }

  void set_path(const std::filesystem::path &path)
  {
    Logger::get_instance().set_path(path);
  }

} // namespace log
