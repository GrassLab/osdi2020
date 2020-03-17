#include "string_util.h"

int string_cmp(const char * string_a, const char * string_b)
{
  /* Return 0 if not the same, idx if the same */
  for(int idx = 0; ; ++idx)
  {
    if(string_a[idx] != string_b[idx])
    {
      return 0;
    }
    if(string_a[idx] == '\0') /* implicit string_b[idx] == '\0' */
    {
      return idx;
    }
  }
}

void string_strip_newline(char * string)
{
  int idx = 0;
  /* search to end of string */
  while(string[idx] != '\0')
  {
    ++idx;
  }
  idx -= 1;

  for(; idx >= 0; --idx)
  {
    if(string[idx] == '\n')
    {
      string[idx] = '\0';
    }
    else
    {
      break;
    }
  }
}

