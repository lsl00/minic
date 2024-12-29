#pragma once

#include <cstdio>
#include <iostream>
#define unreachable(err_msg)                                                   \
  {                                                                            \
    std::cerr << __FILE__ << " : " << "At line" << __LINE__ << " : "           \
              << err_msg;                                                      \
    exit(1);                                                                   \
  }