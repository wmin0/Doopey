#include "common/TaskThread.h"

#include "common/Thread.h"
#include "common/Doopey.h"

using namespace Doopey;

TaskThread::TaskThread():
  Thread(TaskThread::threadFunc, TaskThread::threadStop),
  _task(NULL), _run(false), _free(true) {
  init();
}

TaskThread::TaskThread(void (*task)(void* obj, void* input, void* output)):
  Thread(TaskThread::threadFunc, TaskThread::threadStop),
  _task(task), _run(false), _free(true) {
  init();
}

TaskThread::~TaskThread() {
  pthread_mutex_destroy(&_task_lock);
  stop(this);
  log->debug("TaskThread stop %d\n", _thread);
}

void TaskThread::init() {
  // TODO: check init return type
  pthread_mutex_init(&_task_lock, NULL);
  start(this);
  log->debug("TaskThread start %d\n", _thread);
}

void TaskThread::threadFunc(void* obj) {
  TaskThread* thread = (TaskThread*)obj;
  thread->_run = true;
  pthread_mutex_lock(&(thread->_task_lock));
  thread->mainLoop();
}

void TaskThread::threadStop(void* obj) {
  TaskThread* thread = (TaskThread*)obj;
  thread->_run = false;
  pthread_mutex_unlock(&(thread->_task_lock));
}

void TaskThread::mainLoop() {
  while (_run) {
    pthread_mutex_lock(&_task_lock);
    if (!_run) {
      break;
    }
    _task(_task_obj, _task_input, _task_output);
    _free = true;
    _task_obj = NULL;
    _task_input = NULL;
    _task_output = NULL;
  }
}

bool TaskThread::setTask(void (*task)(void* obj, void* input, void* output)) {
  if (_free) {
    _task = task;
    return true;
  }
  return false;
}

bool TaskThread::runTask(void* obj, void* input, void* output) {
  if (_free && _run && NULL != _task) {
    // TODO: lock
    _free = false;
    _task_obj = obj;
    _task_input = input;
    _task_output = output;
    pthread_mutex_unlock(&_task_lock);
    return true;
  }
  return false;
}
