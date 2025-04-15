
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

#endif  // XIDE_MINMAX_H
