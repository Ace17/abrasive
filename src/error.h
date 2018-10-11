#pragma once

#include "stdarg.h"
#include "stdio.h"

static
void Fail(const char* fmt, ...)
{
  va_list va;
  va_start(va, fmt);
  char buffer[4096] {};
  vsnprintf(buffer, sizeof buffer - 1, fmt, va);
  fprintf(stderr, "Fatal: %s\n", buffer);
  exit(1);
}

