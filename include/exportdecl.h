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
#else
/* Import library functions */
# ifdef __GNUC__
#  define LIBAM_EXPORT(proto) LIBAM_CALL __attribute__((dllimport)) proto
# else
#  define LIBAM_EXPORT(proto) LIBAM_CALL __declspec(dllimport) proto
# endif
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
