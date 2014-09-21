#ifndef E1__H
#define E1__H

/** \file e1.h
 *  \brief E1 stream (2.048 Mbit/s) parameters and functions
 *         corresponding to ITU-T G.704 recommendation
 *  \sa http://www.itu.int/rec/T-REC-G.704-199810-I/en
 */

// number of timeslots in E1 stream
#define E1_TS_NUM 32

// mask for index creating
#define E1_TS_MASK (E1_TS_NUM - 1)

// number of frames in double-frame 
#define E1_DOUBLEFRAME_SIZE  2

// number of frames in multi-frame 
#define E1_MULTIFRAME_SIZE  16

// Frame containing the frame alignment signal (Table5A/G.704).
// Si bit should be set if multiframe enabled.
#define E1_TS0_FAS_WORD 0x1B

// Frame not containing the frame alignment signal (Table5A/G.704).
// Si bit should be set if multiframe enabled.
// A bit is not set. Sa4..Sa8 is not set.
#define E1_TS0_NFAS_WORD 0x40

// The bit of NFAS/FAS word, which is used in multistream
// See G.704 Table 5B
#define E1_TS0_Si_BIT(x)   ((unsigned char)(x)<<7)

typedef enum e1_framing {
  E1_FRAMING_DOUBLEFRAME = 0,
  E1_FRAMING_MULTIFRAME = 1,
} e1_framing_t;

const char* e1_framing_to_string( e1_framing_t framing );

typedef enum e1_struct {
  E1_STRUCT_INVALID = -1,
  E1_STRUCT_UNSTRUCTURED = 0, // w/o SYNC (0th timeslot)
  E1_STRUCT_PCM31 = 1,        // with SYNC(0th timeslot), w/o CAS (16th timeslot)
  E1_STRUCT_PCM30 = 2         // with CAS (16th timeslot)
} e1_struct_t;

e1_struct_t e1_struct_from_string( const char *s );
const char* e1_struct_to_string( e1_struct_t st );

#endif // E1__H

