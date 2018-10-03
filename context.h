#pragma once
#include <memory>
#include <thread>
#include <mutex>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iostream>
#include "bulk.h"
#include "bulk_storage.h"
#include "bulk_observer.h"
#include "async.h"

class Context
{
public:
  Context(std::size_t bulkSize,
          std::shared_ptr<ToConsolePrint> consolePrint,
          std::shared_ptr<ToFilePrint> filePrint) : _ptrBulkRead{BulkReadCmd::create(bulkSize)},
                                                    _consolePrint(consolePrint),
                                                    _filePrint(filePrint)
  {
    _ptrBulkRead->subscribe(_consolePrint);
    _ptrBulkRead->subscribe(_filePrint);
  }

  virtual ~Context()
  {
    _ptrBulkRead->flush();
  }
  void process(const char *data, std::size_t size)
  {
    std::lock_guard<std::mutex> locker(lock_mutex);
    dataStr += {data, size};

    while (true)
    {
      std::size_t endPos = dataStr.find('\n');
      if (endPos != std::string::npos)
      {
        std::string cmd(dataStr.begin(), dataStr.begin() + endPos);
        _ptrBulkRead->process(cmd);
        dataStr.erase(dataStr.begin(), dataStr.begin() + endPos + 1);
      }
      else
        break;
    }
  }

private:
  std::shared_ptr<BulkReadCmd> _ptrBulkRead{nullptr};
  std::shared_ptr<ToConsolePrint> _consolePrint{nullptr};
  std::shared_ptr<ToFilePrint> _filePrint{nullptr};
  std::mutex lock_mutex;
  std::string dataStr;
};

class ContextPool
{
public:
  static ContextPool &Instance(bool connect_only_console = false)
  {
    static ContextPool contextPool(connect_only_console);
    return contextPool;
  }

  async::handle_t make_context(std::size_t bulkSize)
  {
    std::lock_guard<std::mutex> lock_it(locker);
    auto context = std::make_shared<Context>(bulkSize, _consolePrint, _filePrint);
    _context.emplace_back(context);
    return reinterpret_cast<async::handle_t>(context.get());
  }

  void erase(async::handle_t &handle)
  {
    std::lock_guard<std::mutex> lock_it(locker);
    _context.erase(
        std::remove_if(_context.begin(), _context.end(),
                       [=](auto item) { return item.get() == handle; }),
        _context.end());
  }
  std::size_t get_context_size() const
  {
    if (!&_context)
      return 0;
    return _context.size();
  }

  Context *find(async::handle_t &handle)
  {
    std::lock_guard<std::mutex> lock_it(locker);
    if (!&_context)
      return nullptr;
    auto iter = std::find_if(_context.cbegin(), _context.cend(),
                             [=](auto item) { return item.get() == handle; });
    if (iter != _context.cend())
      return (*iter).get();
    return nullptr;
  }
  virtual ~ContextPool()
  {
    _consolePrint->stop();
    _filePrint->stop();
  }

private:
  ContextPool(bool connect_only_console = false) : _consolePrint(ToConsolePrint::create(std::cout))
  {
    if (!connect_only_console)
      _filePrint = ToFilePrint::create(std::thread::hardware_concurrency() ? std::thread::hardware_concurrency() : 1);
    else
      _filePrint = nullptr;
  }

  std::vector<std::shared_ptr<Context>> _context;
  std::shared_ptr<ToConsolePrint> _consolePrint;
  std::shared_ptr<ToFilePrint> _filePrint;
  std::mutex locker;
};