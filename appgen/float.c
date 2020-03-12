/*
 * FLOAT.C
 *   IEEE-754 uint64_t encoding/decoding, plus conversion into/from network byte order
 *
 *  Uses encoder and decoder from: https://beej.us/guide/bgnet/examples/ieee754.c
 *    uint64_t = 1-bit Sign (1=-), 11-bit Biased Exponent (BE), 52-bit Normalised Mantisa (NM)
 *    e.g., 85.125 = 1.010101001 x 2^6 => S=0, BE=(bias=1023)+(6)=10000000101, NM=010101001000000...
 *
 *  Adds conversion of encoded (host-byte ordered) double to and from big_endian network format
 */


#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <arpa/inet.h>
 
#include "float.h"

#define pack754_32(f)   (pack754((f),   32, 8))
#define pack754_64(f)   (pack754((f),   64, 11))
#define unpack754_32(i) (unpack754((i), 32, 8))
#define unpack754_64(i) (unpack754((i), 64, 11))

/* Encoding into IEEE-754 encoded double */
uint64_t pack754(long double f, unsigned bits, unsigned expbits)
{
  long double fnorm;
  int shift;
  long long sign, exp, significand;
  unsigned significandbits = bits - expbits - 1; // -1 for sign bit

  if (f == 0.0) return 0; // get this special case out of the way

  // check sign and begin normalization
  if (f < 0) { sign = 1; fnorm = -f; }
  else { sign = 0; fnorm = f; }

  // get the normalized form of f and track the exponent
  shift = 0;
  while(fnorm >= 2.0) { fnorm /= 2.0; shift++; }
  while(fnorm < 1.0) { fnorm *= 2.0; shift--; }
  fnorm = fnorm - 1.0;

  // calculate the binary form (non-float) of the significand data
  significand = fnorm * ((1LL<<significandbits) + 0.5f);

  // get the biased exponent
  exp = shift + ((1<<(expbits-1)) - 1); // shift + bias

  // return the final answer
  return (sign<<(bits-1)) | (exp<<(bits-expbits-1)) | significand;
}

/* Decoding from IEEE-754 encoded double */
long double unpack754(uint64_t i, unsigned bits, unsigned expbits)
{
  long double result;
  long long shift;
  unsigned bias;
  unsigned significandbits = bits - expbits - 1; // -1 for sign bit

  if (i == 0) return 0.0;

  // pull the significand
  result = (i&((1LL<<significandbits)-1)); // mask
  result /= (1LL<<significandbits); // convert back to float
  result += 1.0f; // add the one back on

  // deal with the exponent
  bias = (1<<(expbits-1)) - 1;
  shift = ((i>>significandbits)&((1LL<<expbits)-1)) - bias;
  while(shift > 0) { result *= 2.0; shift--; }
  while(shift < 0) { result /= 2.0; shift++; }

  // sign it
  result *= (i>>(bits-1))&1? -1.0: 1.0;

  return result;
}

/* Adds conversion (of IEEE-754 encoded double) into Network Big-Endian byte ordering */
uint64_t pack754_be(long double f) {
  uint64_t h = pack754_64(f);
  return ((htonll(h)));
}

/* Adds conversion (of IEEE-754 encoded double) from Network Big-Endian byte ordering */
long double unpack754_be(uint64_t i) {
  uint64_t h = (ntohll(i));
  return (unpack754_64(h));
}

/* testing */
int main(void)
{
  float f = 3.1415926, f2;
  double d = 3.14159265358979323, d2;
  uint32_t fi;
  uint64_t di, ni;

  fi = pack754_32(f);
  f2 = unpack754_32(fi);

  di = pack754_64(d);
  d2 = unpack754_64(di);
  
  printf("float before : %.7f\n", f);
  printf("float encoded: 0x%08" PRIx32 "\n", fi);
  printf("float after  : %.7f\n\n", f2);

  printf("double before : %.20lf\n", d);
  printf("double encoded: 0x%016" PRIx64 "\n", di);
  printf("double after  : %.20lf\n", d2);
  
  ni = pack754_be(d);
  printf("double encoded big endian: 0x%016" PRIx64 "\n", ni);
  printf("double from big endian encoding: %.20lf\n", (double) unpack754_be(ni));

  return 0;
}