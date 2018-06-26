/*******************************************************************************
    Copyright (c) The Taichi Authors (2016- ). All Rights Reserved.
    The use of this software is governed by the LICENSE file.
*******************************************************************************/

#pragma once

#include <vector>
#include <string>
#include "interface.h"

TC_NAMESPACE_BEGIN

class Task : public Unit {
 public:
  virtual void run(const std::vector<std::string> &parameters) {
    assert_info(parameters.size() == 0, "No parameters supported.");
    this->run();
  }
  virtual void run() {
    this->run(std::vector<std::string>());
  }
};

TC_INTERFACE(Task)

#define TC_REGISTER_TASK(task)                                      \
  class Task_##task : public taichi::Task {                         \
    void run(const std::vector<std::string> &parameters) override { \
      task(parameters);                                             \
    }                                                               \
  };                                                                \
  TC_IMPLEMENTATION(Task, Task_##task, #task)

TC_NAMESPACE_END
