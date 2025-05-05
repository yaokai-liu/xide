
#ifndef XIDE_MINMAX_H
#define XIDE_MINMAX_H

#ifndef __cplusplus
  #ifndef NOMINMAX
    #ifndef max
      #define max(a, b) (((a) > (b)) ? (a) : (b))
    #endif

    #ifndef min
      #define min(a, b) (((a) < (b)) ? (a) : (b))
    #endif
  #endif
#endif

#define minmax(a, b, c) min(max(a, min(b, c)), max(b, c))
#define maxmin(a, b, c) max(min(a, max(b, c)), min(b, c))

#endif  // XIDE_MINMAX_H
