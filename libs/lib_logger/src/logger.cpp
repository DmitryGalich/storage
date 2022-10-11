#include "logger.h"

#include <array>
#include <fstream>
#include <iostream>
#include <mutex>

namespace
{

  std::string get_time()
  {
    const uint kTimeArrayBufferSize(20);
    std::array<char, kTimeArrayBufferSize> buffer{};
    struct tm *time_struct = nullptr;

    time_t now = time(nullptr);
    time_struct = localtime(&now);
    strftime(buffer.data(), buffer.size(), "%d-%m-%Y_%H-%M-%S", time_struct);

    return buffer.data();
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
    Logger(const Logger &) = delete;
    Logger(Logger &&) = delete;
    Logger &operator=(Logger &) = delete;
    Logger &operator=(Logger &&) = delete;

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

    void clear_path() { path_.clear(); }

    void write_info(const std::string &message,
                    const std::string &function_prefix)
    {
      do_writing(message, "info", function_prefix);
    }

    void write_warning(const std::string &message,
                       const std::string &function_prefix)
    {
      do_writing(message, "warning", function_prefix);
    }

    void write_error(const std::string &message,
                     const std::string &function_prefix)
    {
      do_writing(message, "error", function_prefix);
    }

    void set_application_name(const std::string &app_name)
    {
      app_name_ = app_name;
    }

    void clear_application_name() { app_name_.clear(); }

  private:
    Logger() = default;
    ~Logger()
    {
      std::lock_guard<std::mutex> lock(mutex_);

      if (file_.is_open())
        file_.close();
    }

    void do_writing(const std::string &message, const std::string &type,
                    const std::string &function_prefix)
    {
      const std::string kLogLine(get_time() + " [" + type +
                                 "] (thread: " + get_thread_id() + ") (" +
                                 function_prefix + ") " + message + "\n");

      std::lock_guard<std::mutex> lock(mutex_);

      std::cout << kLogLine << std::flush;

      if (path_.empty())
        return;

      if (file_creating_time_.empty())
        file_creating_time_ = get_time();

      std::string filename(path_.c_str() + std::string("/") + app_name_ +
                           (app_name_.empty() ? "" : "_") + file_creating_time_ +
                           std::string(".log"));
      file_.open(filename, std::ios::out | std::ios::app);
      if (!file_.is_open())
      {
        std::cout << "Logger error: Can't open file \""
                  << "\"" << std::endl;
      }
      else
      {
        file_.write(kLogLine.c_str(), kLogLine.size());

        if (file_.is_open())
          file_.close();
      }
    }

  private:
    std::mutex mutex_;

    std::string file_creating_time_;
    std::string app_name_;
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

  void clear_path() { Logger::get_instance().clear_path(); }

  void set_application_name(const std::string &app_name)
  {
    Logger::get_instance().set_application_name(app_name);
  }

  void clear_application_name()
  {
    Logger::get_instance().clear_application_name();
  }

} // namespace logger
