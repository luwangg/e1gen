#ifndef E1TS__H
#define E1TS__H 

/** \file e1ts.h
 *  \brief Types and functions for timeslot transmitting
 */
#include <stdio.h>

struct e1_tx_timeslot;
typedef struct e1_tx_timeslot e1_tx_timeslot_t;

struct e1_tx_timeslot {
  unsigned char num; /**< number of timeslot = 0..31 */
  const char *fname; /**< filename for reading data for timeslot */
  FILE *file; /**< file for reading data for timeslot */
  unsigned char (*get_byte)( e1_tx_timeslot_t *this, unsigned int frame );
                     /**< callback which should return byte for sending it to 
                          'num' timeslot */
};

void e1_tx_timeslots_clear( void );
int e1_tx_timeslots_init( void );
unsigned char e1_tx_get_next_byte( void );
void e1_tx_set_file_for_timeslot( unsigned char ts, const char *fname );

#endif // E1TS__H

