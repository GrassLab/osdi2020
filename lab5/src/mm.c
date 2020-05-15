void memzero(char *b, char *e){
  while(b < e) *b = 0, b++;
}
