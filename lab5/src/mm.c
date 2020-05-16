void memzero(char *b, char *e){
  while(b < e) *b = 0, b++;
}

void msetzero(char *b, unsigned long size){
  while(size--) *b = 0, b++;
}
