//
// system_executor.h
// ~~~~~~~~~~~~~~~~~
// System executor implementation.
//
// Copyright (c) 2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EXECUTORS_EXPERIMENTAL_BITS_SYSTEM_EXECUTOR_H
#define EXECUTORS_EXPERIMENTAL_BITS_SYSTEM_EXECUTOR_H

#include <cassert>
#include <cstddef>
#include <type_traits>
#include <thread>
#include <vector>
#include <experimental/bits/scheduler.h>

namespace std {
namespace experimental {

class __system_executor_impl
  : public execution_context
{
public:
  __system_executor_impl()
  {
    _M_scheduler._Work_started();
    std::size_t __n = thread::hardware_concurrency();
    for (size_t __i = 0; __i < __n; ++__i)
      _M_threads.emplace_back([this](){ _M_scheduler._Run(); });
  }

  ~__system_executor_impl()
  {
    _M_scheduler._Work_finished();
    _M_scheduler._Stop();
    for (auto& __t: _M_threads)
      __t.join();
    shutdown_context();
  }

  static __system_executor_impl& _Instance()
  {
    static __system_executor_impl __e;
    return __e;
  }

  template <class _F> void _Post(_F&& __f)
  {
    _M_scheduler._Post(forward<_F>(__f));
  }

  template <class _F> void _Defer(_F&& __f)
  {
    _M_scheduler._Defer(forward<_F>(__f));
  }

private:
  __scheduler _M_scheduler;
  vector<thread> _M_threads;
};

inline execution_context& system_executor::context()
{
  return __system_executor_impl::_Instance();
}

inline void system_executor::work_started() noexcept
{
  // No-op.
}

inline void system_executor::work_finished() noexcept
{
  // No-op.
}

template <class _Func, class _Alloc>
void system_executor::dispatch(_Func&& __f, const _Alloc&)
{
  typename decay<_Func>::type tmp(forward<_Func>(__f));
  std::move(tmp)();
}

template <class _Func, class _Alloc>
inline void system_executor::post(_Func&& __f, const _Alloc&)
{
  __system_executor_impl::_Instance()._Post(forward<_Func>(__f));
}

template <class _Func, class _Alloc>
inline void system_executor::defer(_Func&& __f, const _Alloc&)
{
  __system_executor_impl::_Instance()._Defer(forward<_Func>(__f));
}

template <class _Func>
inline auto system_executor::wrap(_Func&& __f) const
{
  return (wrap_with_executor)(forward<_Func>(__f), *this);
}

} // namespace experimental
} // namespace std

#endif
