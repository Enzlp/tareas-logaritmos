#ifndef BLOCK_SIZE_H
#define BLOCK_SIZE_H

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

inline long get_block_size() {
  std::string cmd;

#ifdef __APPLE__
  cmd = "stat -f \"%k\" .";
#else
  cmd = "stat -f --format=\"%S\" .";
#endif

  FILE *pipe = popen(cmd.c_str(), "r");
  if (!pipe)
    return -1;

  char buffer[128];
  if (fgets(buffer, sizeof(buffer), pipe) == nullptr) {
    pclose(pipe);
    return -1;
  }

  pclose(pipe);
  return std::strtol(buffer, nullptr, 10);
}

#endif // BLOCK_SIZE_H
