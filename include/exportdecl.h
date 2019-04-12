/*
 *  libautomusic (Library for Image-based Algorithmic Musical Composition)
 *  Copyright (C) 2018, automusic.
 *
 *  THIS PROJECT IS FREE SOFTWARE; YOU CAN REDISTRIBUTE IT AND/OR
 *  MODIFY IT UNDER THE TERMS OF THE GNU LESSER GENERAL PUBLIC LICENSE(GPL)
 *  AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION; EITHER VERSION 2.1
 *  OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
 *
 *  THIS PROJECT IS DISTRIBUTED IN THE HOPE THAT IT WILL BE USEFUL,
 *  BUT WITHOUT ANY WARRANTY; WITHOUT EVEN THE IMPLIED WARRANTY OF
 *  MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  SEE THE GNU
 *  LESSER GENERAL PUBLIC LICENSE FOR MORE DETAILS.
 */
#ifndef EXPORT_DECL_H
#define EXPORT_DECL_H

#define LIBAM_CALL __stdcall

#ifdef DLL_EXPORT
/* Export library functions */
# if defined _WIN32 || defined __CYGWIN__ || defined(__MINGW__)
#  ifdef __GNUC__ 
#   define LIBAM_EXPORT(proto) LIBAM_CALL __attribute__((dllexport)) proto
#  else
#   define LIBAM_EXPORT(proto) LIBAM_CALL __declspec(dllexport) proto
#  endif
# else
#  if __GNUC__ >= 4
#   define LIBAM_EXPORT(proto) LIBAM_CALL __attribute__ ((visibility("default"))) proto
#  else
#   define LIBAM_EXPORT(proto) LIBAM_CALL proto
#  endif
# endif
#elif !defined(LIBAM_STATIC_LINK)
/* Import library functions */
# ifdef __GNUC__
#  define LIBAM_EXPORT(proto) LIBAM_CALL __attribute__((dllimport)) proto
# else
#  define LIBAM_EXPORT(proto) LIBAM_CALL __declspec(dllimport) proto
# endif
#else
# define LIBAM_EXPORT(proto) LIBAM_CALL proto
#endif

/** @def BEGIN_C_DECLS
 * Used to start a block of function declarations which are shared
 * between C and C++ program.
 */
#if defined(__cplusplus)
# define C_DECLS extern "C" {
#else
# define C_DECLS
#endif

/** @def END_C_DECLS
 * Used to end a block of function declarations which are shared
 * between C and C++ program.
 */
#if defined(__cplusplus)
# define END_C_DECLS   }
#else
# define END_C_DECLS
#endif

#endif //!defined(EXPORT_DECL_H)
