#ifndef LIBAUTOMUSIC_H
#define LIBAUTOMUSIC_H

#include "exportdecl.h"

C_DECLS

/**@def RC_
 * @brief Definitions of Error codes
 */
#define RC_OK (0)
#define RC_FAILED (1)
#define RC_OPENFILE (2)
#define RC_PARSE_DATABASE (3)
#define RC_WRITE_FILE (4)
#define RC_PARAMETERS_UNGENERATED (5)
#define RC_LOADING_SAMPLES (6)
#define RC_UNSUPPORTED (7)

typedef struct am_context_s am_context_t;

/*
 * Exported functions
 */
/**
 * @brief Validate the compatibility of the required version.
 * @param version Required version code.
 * @return 0 if succeeded. otherwise failed.
 */
int LIBAM_EXPORT(libam_require_version)(int major, int minor, int revsion);

/**
 * @brief Create a context handle stores all the data needed by libautomusic.
 * @return a pointer to the context memory.
 */
am_context_t *LIBAM_EXPORT(libam_create_context)(void);

/**
 * @brief Start a process of composition by giving a image file.
 * @param context Handle, a pointer to the context memory.
 * @param form_template_index Index of template of musical structure form.
 * @param beats Beats per bar.
 * @param filename Path and filename of the image file.
 * @return status code. @see RC_*
 */
int LIBAM_EXPORT(libam_composite_by_image)(am_context_t *context, int form_template_index, int beats, const char *filename);

/**
 * @brief Output the result of composition to target file.
 * @param context Handle, a pointer to the context memory.
 * @param filetype Indicates the type of output file, 0 = MIDI, 1 = PCM RAW.
 * @param filename Path and filename of target file.
 * @return status code. @see RC_*
 */
int LIBAM_EXPORT(libam_output_file)(am_context_t *context, int filetype, const char *filename);

/**
 * @brief Release the context memory allocated by libam_create_context()
 * @param Target context Handle, a pointer to the context memory.
 */
void LIBAM_EXPORT(libam_free_context)(am_context_t *context);

END_C_DECLS

#endif
