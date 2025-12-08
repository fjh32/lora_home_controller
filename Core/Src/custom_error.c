#include "custom_error.h"

void error_loop(const char *str)
{
  __disable_irq();
  uart_print("A fatal error has occurred....");
  uart_print(str);
  while (1);
}