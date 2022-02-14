#ifndef __DPRINT_H__
#define __DPRINT_H__


  extern int _dprint(const char *format, ...);
  #define dprint  _dprint

#if 0
#define __DEBUG

////=============================================
#if 1
#ifdef __DEBUG
  extern int _dprint(const char *format, ...);
  #define dprint	_dprint
#else
  extern int _dprint_dummy(const char *format, ...);
  #define dprint	_dprint_dummy
#endif
#endif
 #endif 
////=============================================
#endif ////__DPRINTK_H__



	
