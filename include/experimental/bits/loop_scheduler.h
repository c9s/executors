//
// loop_scheduler.h
// ~~~~~~~~~~~~~~~~
// Loop scheduler implementation.
//
// Copyright (c) 2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EXECUTORS_EXPERIMENTAL_BITS_LOOP_SCHEDULER_H
#define EXECUTORS_EXPERIMENTAL_BITS_LOOP_SCHEDULER_H

#include <cassert>

namespace std {
namespace experimental {

inline loop_scheduler::loop_scheduler()
{
}

inline loop_scheduler::loop_scheduler(size_t __concurrency_hint)
  : __scheduler(__concurrency_hint)
{
}

inline loop_scheduler::~loop_scheduler()
{
}

inline loop_scheduler::executor_type loop_scheduler::get_executor() const noexcept
{
  return executor_type(const_cast<loop_scheduler*>(this));
}

inline size_t loop_scheduler::run()
{
  return _Run();
}

inline size_t loop_scheduler::run_one()
{
  return _Run_one();
}

template <class _Rep, class _Period>
size_t loop_scheduler::run_for(
  const chrono::duration<_Rep, _Period>& __rel_time)
{
  return this->_Run_for(__rel_time);
}

template <class _Clock, class _Duration>
size_t loop_scheduler::run_until(
  const chrono::time_point<_Clock, _Duration>& __abs_time)
{
  return this->_Run_until(__abs_time);
}

inline size_t loop_scheduler::poll()
{
  return _Poll();
}

inline size_t loop_scheduler::poll_one()
{
  return _Poll_one();
}

inline void loop_scheduler::stop()
{
  _Stop();
}

inline bool loop_scheduler::stopped() const
{
  return _Stopped();
}

inline void loop_scheduler::reset()
{
  _Reset();
}

inline loop_scheduler::executor_type::executor_type(
  const loop_scheduler::executor_type& __e) noexcept
    : _M_scheduler(__e._M_scheduler)
{
}

inline loop_scheduler::executor_type&
  loop_scheduler::executor_type::operator=(const executor_type& __e) noexcept
{
  _M_scheduler = __e._M_scheduler;
  return *this;
}

inline loop_scheduler::executor_type::~executor_type()
{
}

inline execution_context& loop_scheduler::executor_type::context()
{
  return *_M_scheduler;
}

inline void loop_scheduler::executor_type::work_started() noexcept
{
  _M_scheduler->_Work_started();
}

inline void loop_scheduler::executor_type::work_finished() noexcept
{
  _M_scheduler->_Work_finished();
}

template <class _Func, class _Alloc>
void loop_scheduler::executor_type::dispatch(_Func&& __f, const _Alloc&)
{
  _M_scheduler->_Dispatch(forward<_Func>(__f));
}

template <class _Func, class _Alloc>
void loop_scheduler::executor_type::post(_Func&& __f, const _Alloc&)
{
  _M_scheduler->_Post(forward<_Func>(__f));
}

template <class _Func, class _Alloc>
void loop_scheduler::executor_type::defer(_Func&& __f, const _Alloc&)
{
  _M_scheduler->_Defer(forward<_Func>(__f));
}

template <class _Func>
inline auto loop_scheduler::executor_type::wrap(_Func&& __f) const
{
  return (wrap_with_executor)(forward<_Func>(__f), *this);
}

} // namespace experimental
} // namespace std

#endif
