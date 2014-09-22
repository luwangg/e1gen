/** \file  e1gen.c
 *  \brief main for e1gen program. Parameters parsing and 
 *         top-level calls 
 */

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include "params.h"
#include "e1ts.h"

/** \brief parses command line options and then generates e1 stream
 */
int main( int argc, char *argv[] )
{
  set_params_to_default( &gl_params );
  e1_tx_timeslots_clear( );

  int err = parse_args( argc, argv, &gl_params );

  if ( !err ) {
    err = e1_tx_timeslots_init();
    if ( err ) {
      exit( err );
    }

    unsigned i = 0;
    for ( i = 0; 
          ( i < gl_params.max_frames*E1_TS_NUM ) || ( gl_params.max_frames == 0 ); 
          i++ ) {
      unsigned char b = e1_tx_get_next_byte();
      if ( 1 != fwrite( &b, 1, 1, stdout ) ) {
        fprintf( stderr, "write error!\n" );
        exit( EFAULT );
      }
    } 
  } else {
    exit( err );
  }

  return 0;
}
