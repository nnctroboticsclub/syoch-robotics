#include <functional>
#include "../logger/logger.hpp"
#include "../platform/thread.hpp"
#include "../platform/timer.hpp"

namespace robotics::experimental::scheduler {

logger::Logger logger{"sch.app", "Scheduler"};

enum TimingType { kLater, kRepeatUntil, kRepeatInterval };

template <typename Time, typename Duration>
class Task {
 public:
  TimingType type;
  Time scheduled_at;

  Time stop_at;  // -1: forever
  Duration interval;
  int count_max;
  const char* task_name;

  std::function<void()> task;

  static Task* Later(const char* task_name, Time scheduled_at, Duration delay,
                     std::function<void()> task) {
    auto task_obj = new Task;
    task_obj->type = kLater;
    task_obj->task = task;
    task_obj->task_name = task_name;

    logger.Info("Task %s: %ld", task_name, scheduled_at);
    task_obj->scheduled_at = scheduled_at;
    task_obj->interval = delay;
    return task_obj;
  }

  static Task* RepeatInterval(const char* task_name, Time scheduled_at,
                              Duration interval, std::function<void()> task) {
    auto task_obj = new Task;
    task_obj->type = kRepeatInterval;
    task_obj->task_name = task_name;
    task_obj->task = task;

    logger.Info("Task %s: %ld", task_name, scheduled_at);
    task_obj->scheduled_at = scheduled_at;
    task_obj->interval = interval;
    return task_obj;
  }

  static Task* RepeatUntil(const char* task_name, Time scheduled_at,
                           Duration interval, int count_max,
                           std::function<void()> task) {
    auto task_obj = new Task;
    task_obj->type = kRepeatUntil;
    task_obj->task_name = task_name;
    task_obj->task = task;

    logger.Info("Task %s: %ld", task_name, scheduled_at.count());
    task_obj->scheduled_at = scheduled_at;
    task_obj->interval = interval;
    task_obj->count_max = count_max;
    return task_obj;
  }

 private:
  int count_current_;

  Time ScheduledRunAt() {
    switch (type) {
      case kLater:
        return this->scheduled_at + this->interval;
      case kRepeatUntil:
        return this->scheduled_at + this->interval * count_current_;
      case kRepeatInterval:
        return this->scheduled_at + this->interval * count_current_;
      default:
        return this->scheduled_at;
    }
  }

 public:
  bool NeededToRun(Time current_time) {
    return ScheduledRunAt() <= current_time;
  }

  void Run() {
    switch (type) {
      case kLater:
        break;
      case kRepeatUntil:
        count_current_++;
        break;
      case kRepeatInterval:
        count_current_++;
        break;
    }

    this->task();
  }

  bool NeedDispose() {
    switch (type) {
      case kLater:
        return true;
      case kRepeatUntil:
        return count_current_ >= count_max;
      case kRepeatInterval:
        return false;
      default:
        return false;
    }
  }

  void Dispose() { this->task = nullptr; }
};

template <typename Time, typename Duration>
class Scheduler {
  system::Timer timer;
  system::Thread main_thread;

  std::vector<Task<Time, Duration>*> tasks;

  void Tick() {
    const Time current_time =
        (Time)std::chrono::duration_cast<Time>(timer.ElapsedTime());
    std::vector<size_t> to_remove;
    int i = 0;
    logger.Trace("Tick at %d", current_time);
    for (auto& task : tasks) {
      logger.Trace("Task %s scheduled at %d", task->task_name,
                   task->scheduled_at);
      if (task->NeededToRun(current_time)) {
        logger.Info("Task %s runned", task->task_name);
        task->Run();

        if (task->NeedDispose()) {
          logger.Info("Task %s disposed", task->task_name);
          task->Dispose();
          to_remove.push_back(i);
        }
      }
      i++;
    }

    for (auto i : to_remove) {
      tasks.erase(tasks.begin() + i);
    }
  }

  void Init() {
    timer.Reset();
    timer.Start();
  }

  void MainTask() {
    using namespace std::chrono_literals;

    Init();
    while (true) {
      Tick();

      ThisThread::sleep_for(1s);
    }
  }

 public:
  Scheduler() {}

  void AddTask(Task<Time, Duration>* task) { tasks.emplace_back(task); }

  void Start() {
    main_thread = system::Thread();
    main_thread.SetStackSize(4096);
    main_thread.SetThreadName("Scheduler");
    main_thread.Start([this]() { MainTask(); });
  }
};
}  // namespace robotics::experimental::scheduler