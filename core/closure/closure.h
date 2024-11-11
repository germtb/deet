#ifndef C_PRINT_H_
#define C_PRINT_H_

#include "../array/array.h"

#include <stdlib.h>
#include <stdarg.h>

typedef struct Closure
{
  void *(*fn)(va_list *ap);
  Array *args;
} Closure;

#endif // C_PRINT_H_
