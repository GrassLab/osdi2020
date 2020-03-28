int
strcmp (const char *cs, const char *ct)
{
  unsigned char c1, c2;

  while (1)
    {
      c1 = *cs++;
      c2 = *ct++;
      if (c1 != c2)
	return c1 < c2 ? -1 : 1;
      if (!c1)
	break;
    }
  return 0;
}
