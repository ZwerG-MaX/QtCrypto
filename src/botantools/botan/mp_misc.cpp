/*************************************************
* MP Misc Functions Source File                  *
* (C) 1999-2004 The Botan Project                *
*************************************************/

#include <botan/mp_core.h>
#include <botan/mp_madd.h>

namespace Botan {

/*************************************************
* Core Division Operation                        *
*************************************************/
u32bit bigint_divcore(word q, word y1, word y2,
                      word x1, word x2, word x3)
   {
   word y0 = 0;
   bigint_madd(q, y2, 0, 0, &y2, &y0);
   bigint_madd(q, y1, y0, 0, &y1, &y0);

   if(y0 > x1) return 1;
   if(y0 < x1) return 0;
   if(y1 > x2)  return 1;
   if(y1 < x2)  return 0;
   if(y2 > x3)  return 1;
   if(y2 < x3)  return 0;
   return 0;
   }

/*************************************************
* Compare two MP integers                        *
*************************************************/
s32bit bigint_cmp(const word x[], u32bit x_size,
                  const word y[], u32bit y_size)
   {
   if(x_size < y_size) { return (-bigint_cmp(y, y_size, x, x_size)); }

   while(x_size > y_size)
      {
      if(x[x_size-1])
         return 1;
      x_size--;
      }
   for(u32bit j = x_size; j > 0; j--)
      {
      if(x[j-1] > y[j-1]) return 1;
      if(x[j-1] < y[j-1]) return -1;
      }
   return 0;
   }

/*************************************************
* Do a 2-word/1-word Division                    *
*************************************************/
word bigint_divop(word n1, word n0, word d)
   {
   word high = n1 % d;
   word quotient = 0;
   for(u32bit j = 0; j != MP_WORD_BITS; j++)
      {
      const word mask = (word)1 << (MP_WORD_BITS-1-j);
      const bool high_top_bit = (high & MP_WORD_TOP_BIT) ? true : false;

      high = (high << 1) | ((n0 & mask) >> (MP_WORD_BITS-1-j));

      if(high_top_bit || high >= d)
         {
         high -= d;
         quotient |= mask;
         }
      }
   return quotient;
   }

/*************************************************
* Do a 2-word/1-word Modulo                      *
*************************************************/
word bigint_modop(word n1, word n0, word d)
   {
   word z0 = n1 / d, z1 = bigint_divop(n1, n0, d);
   word carry = 0;
   bigint_madd(z1, d,     0, 0, &z1, &carry);
   bigint_madd(z0, d, carry, 0, &z0, &carry);
   return (n0-z1);
   }

}
