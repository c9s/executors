#include <experimental/strand>
#include <experimental/executor>
#include <experimental/future>
#include <experimental/loop_scheduler>
#include <atomic>
#include <cassert>

std::atomic<int> function_count(0);
std::atomic<int> handler_count(0);

int function1()
{
  return ++function_count;
}

struct function2
{
  function2() {}
  int operator()() { return ++function_count; }
};

struct function3
{
  function3() {}
  function3(const function3&) = delete;
  function3(function3&&) {}
  int operator()() { return ++function_count; }
};

void handler1(int)
{
  ++handler_count;
}

struct handler2
{
  handler2() {}
  void operator()(int) { ++handler_count; }
};

struct handler3
{
  handler3() {}
  handler3(const handler3&) = delete;
  handler3(handler3&&) {}
  void operator()(int) && { ++handler_count; }
};

int main()
{
  function2 f2;
  const function2 cf2;
  function3 f3;

  handler2 h2;
  const handler2 ch2;
  handler3 h3;

  std::experimental::loop_scheduler scheduler;
  auto ex = make_strand(scheduler.get_executor());
  std::experimental::executor_work<decltype(ex)> w(ex);
  std::thread t([&](){ scheduler.run(); });

  std::experimental::post(ex.wrap(function1), handler1);
  std::experimental::post(ex.wrap(function1), &handler1);
  std::experimental::post(ex.wrap(function1), handler2());
  std::experimental::post(ex.wrap(function1), h2);
  std::experimental::post(ex.wrap(function1), ch2);
  std::experimental::post(ex.wrap(function1), handler3());
  std::experimental::post(ex.wrap(function1), std::move(h3));
  std::future<int> fut1 = std::experimental::post(ex.wrap(function1), std::experimental::use_future);
  fut1.get();

  std::experimental::post(ex.wrap(&function1), handler1);
  std::experimental::post(ex.wrap(&function1), &handler1);
  std::experimental::post(ex.wrap(&function1), handler2());
  std::experimental::post(ex.wrap(&function1), h2);
  std::experimental::post(ex.wrap(&function1), ch2);
  std::experimental::post(ex.wrap(&function1), handler3());
  std::experimental::post(ex.wrap(&function1), std::move(h3));
  std::future<int> fut2 = std::experimental::post(ex.wrap(&function1), std::experimental::use_future);
  fut2.get();

  std::experimental::post(ex.wrap(function2()), handler1);
  std::experimental::post(ex.wrap(function2()), &handler1);
  std::experimental::post(ex.wrap(function2()), handler2());
  std::experimental::post(ex.wrap(function2()), h2);
  std::experimental::post(ex.wrap(function2()), ch2);
  std::experimental::post(ex.wrap(function2()), handler3());
  std::experimental::post(ex.wrap(function2()), std::move(h3));
  std::future<int> fut3 = std::experimental::post(ex.wrap(function2()), std::experimental::use_future);
  fut3.get();

  std::experimental::post(ex.wrap(f2), handler1);
  std::experimental::post(ex.wrap(f2), &handler1);
  std::experimental::post(ex.wrap(f2), handler2());
  std::experimental::post(ex.wrap(f2), h2);
  std::experimental::post(ex.wrap(f2), ch2);
  std::experimental::post(ex.wrap(f2), handler3());
  std::experimental::post(ex.wrap(f2), std::move(h3));
  std::future<int> fut4 = std::experimental::post(ex.wrap(f2), std::experimental::use_future);
  fut4.get();

  std::experimental::post(ex.wrap(cf2), handler1);
  std::experimental::post(ex.wrap(cf2), &handler1);
  std::experimental::post(ex.wrap(cf2), handler2());
  std::experimental::post(ex.wrap(cf2), h2);
  std::experimental::post(ex.wrap(cf2), ch2);
  std::experimental::post(ex.wrap(cf2), handler3());
  std::experimental::post(ex.wrap(cf2), std::move(h3));
  std::future<int> fut5 = std::experimental::post(ex.wrap(cf2), std::experimental::use_future);
  fut5.get();

  std::experimental::post(ex.wrap(function3()), handler1);
  std::experimental::post(ex.wrap(function3()), &handler1);
  std::experimental::post(ex.wrap(function3()), handler2());
  std::experimental::post(ex.wrap(function3()), h2);
  std::experimental::post(ex.wrap(function3()), ch2);
  std::experimental::post(ex.wrap(function3()), handler3());
  std::experimental::post(ex.wrap(function3()), std::move(h3));
  std::future<int> fut6 = std::experimental::post(ex.wrap(function3()), std::experimental::use_future);
  fut6.get();

  std::experimental::post(ex.wrap(std::move(f3)), handler1);
  std::experimental::post(ex.wrap(std::move(f3)), &handler1);
  std::experimental::post(ex.wrap(std::move(f3)), handler2());
  std::experimental::post(ex.wrap(std::move(f3)), h2);
  std::experimental::post(ex.wrap(std::move(f3)), ch2);
  std::experimental::post(ex.wrap(std::move(f3)), handler3());
  std::experimental::post(ex.wrap(std::move(f3)), std::move(h3));
  std::future<int> fut7 = std::experimental::post(ex.wrap(std::move(f3)), std::experimental::use_future);
  fut7.get();

  w.reset();
  t.join();

  assert(function_count == 56);
  assert(handler_count == 49);
}
