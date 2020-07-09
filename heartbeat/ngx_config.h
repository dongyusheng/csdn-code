
#ifndef _NGX_CONFIG_H_INCLUDED_
#define _NGX_CONFIG_H_INCLUDED_

# ifndef offsetof
#  define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
# endif

#define __WORDSIZE 64

#if __WORDSIZE == 64
# ifndef __intptr_t_defined
typedef long int		intptr_t;
#  define __intptr_t_defined
# endif
typedef unsigned long int	uintptr_t;
#else
# ifndef __intptr_t_defined
typedef int			intptr_t;
#  define __intptr_t_defined
# endif
typedef unsigned int		uintptr_t;
#endif


typedef unsigned short     ngx_short_t;
typedef unsigned char      ngx_char_t;
typedef unsigned long long ngx_long_t;


typedef intptr_t           ngx_int_t;
typedef uintptr_t          ngx_uint_t;
typedef intptr_t           ngx_flag_t;


/* TODO: auto_conf: ngx_inline   inline __inline __inline__ */
#ifndef ngx_inline
#define ngx_inline      inline
#endif

#endif

