/*
entropy calculator
calculates shannon, renyi, min entropy of bytes from a file.

written by seth hardy, shardy@aculei.net

license-ish thing:
you can use this code if you give credit where credit is due.
I assume no responsibility for what happens if you use it, though.

*/  


#include <stdio.h>
#include <math.h>

int main(int argc, char **argv) {

  FILE *in;
  long count[255];
  double prob[255];
  long total;
  int i;
  double foo;

  if( argc != 2 ) {
    printf("usage: %s filename\n\n", argv[0]);
    return 1;
  }

  for(i=0;i<255;i++) { 
    count[i] = 0;
    prob[i] = 0;
  }

  total = 0;

  in = fopen(argv[1],"r");
  while( !feof(in) ) {
    i = fgetc(in); 
    count[i]++;
    total++;
  } 

  fclose(in);

  for(i=0;i<255;i++) 
    prob[i] = (double)count[i] / total;

  // compute renyi entropy

  foo = 0;
  for(i=0;i<255;i++) foo += prob[i] * prob[i];
  foo = -1 * log(foo) / log(exp(1));
  printf("  renyi: %f\n",foo);

  // compute min entropy

  foo = 0;
  for(i=0;i<255;i++) 
    if( prob[i] > foo ) foo = prob[i];
  foo = -1 * log(foo) / log(exp(1));
  printf("    min: %f\n",foo);

  // compute shannon entropy

  foo = 0;
  for(i=0;i<255;i++) 
    if( prob[i] != 0) foo = foo + ( prob[i] * log(prob[i]) / log(exp(1)) );
  foo = -1 * foo;
  printf("shannon: %f\n",foo);

  return 0;

}
