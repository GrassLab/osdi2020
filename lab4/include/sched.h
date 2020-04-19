#pragma once

struct task_struct {
  unsigned long pid;

};

struct task_struct task_pool[64];

int privilege_task_create(void(*func)());
