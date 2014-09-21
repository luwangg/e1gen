#ifndef E1GEN_PARAMS__H
#define E1GEN_PARAMS__H

/** \file  params.h
 *  \brief Parameters of E1 generator. Almost all parameters 
 *         are controlled by command-line options 
 */

#include "e1.h"

/** Program parameters. There is one global instance of this structure.
 */
typedef struct prog_params {
  e1_struct_t structure; /**< pcm30/pcm31/unstr \sa e1_struct */
  e1_framing_t framing;  /**< double-frame or multiframe \sa e1_framing */
  int verbose;           /**< print verbose information to stderr */
  int crc4;              /**< non-zero means that CRC4 bits are injected 
                              into SYNC timeslot (0th). valid when
                              multiframe is on */
  unsigned char silence_byte; /**< byte which is transmitted into unused timeslots */
  unsigned int max_frames; /**< how many frames to generate */
} prog_params_t;

extern prog_params_t gl_params;

void set_params_to_default( prog_params_t* p );
int parse_args( int argc, char *argv[], prog_params_t *p );

#endif // E1GEN_PARAMS__H 

