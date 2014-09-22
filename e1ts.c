#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "e1.h"
#include "params.h"
#include "e1ts.h"

static e1_tx_timeslot_t gl_e1_tx_ts[ E1_TS_NUM ]; 

// frame counter (1 frame = 32 timeslots)
static unsigned gl_frame = 0;

// timeslot counter, 0..31
static unsigned char gl_ts = 0;

unsigned char e1_tx_from_file( e1_tx_timeslot_t *this, unsigned int frame );
unsigned char e1_tx_silence( e1_tx_timeslot_t *this, unsigned int frame );
unsigned char e1_tx_sync_timeslot( e1_tx_timeslot_t *this, unsigned int frame );
unsigned char e1_tx_doubleframe_timeslot( e1_tx_timeslot_t *this, unsigned int frame );
unsigned char e1_tx_multiframe_timeslot( e1_tx_timeslot_t *this, unsigned int frame );

/** \brief fills gl_e1_tx_ts with zeroes. should be called once at startup
 */ 
void e1_tx_timeslots_clear( void )
{
  memset( gl_e1_tx_ts, 0, sizeof( gl_e1_tx_ts ) );
}

/** initializes timeslots, opens all required files. 
 *  should be called after options parsing.
 *  
 *  If fname is NULL, timeslot is set up for sending the silence byte.
 *  If fname is non-zero, timeslot is set up for sending data from the file.
 */
int e1_tx_timeslots_init( void )
{
  unsigned i = 0;
  // opening all files, if any
  for ( i = 0; i < E1_TS_NUM; i++ ) { 
    e1_tx_timeslot_t *t = &gl_e1_tx_ts[ i ];
    t->num = i;

    if ( t->fname != NULL ) {
      if ( t->file == NULL ) {
        // file name is set but the file is not opened yet
        t->file = fopen( t->fname, "r" );
        if ( t->file == NULL ) {
          perror( t->fname );
          return errno; 
        }

        // lookup other timeslots with the same fname.
        // and set 'file' to t->file value.
        // this is done for consequtive reading of same file
        // when it is used for some timeslots (more then one)
        unsigned j = 0;
        for ( j = i + 1; j < E1_TS_NUM; j ++ ) {
          e1_tx_timeslot_t *_t = &gl_e1_tx_ts[ j ];
          if ( _t->fname && ( strcmp( _t->fname, t->fname ) == 0 ) ) {
            _t->file = t->file;
          }
        }
      }
    }
  }

  // assigning callbacks. file has highest priority even for 0th timeslot
  for ( i = 0; i < E1_TS_NUM; i++ ) { 
    e1_tx_timeslot_t *t = &gl_e1_tx_ts[ i ];

    if ( t->file ) {
      t->get_byte = e1_tx_from_file;
    } else if ( ( gl_params.structure != E1_STRUCT_UNSTRUCTURED ) && 
                ( i == 0 ) ) {
      t->get_byte = e1_tx_sync_timeslot;
    } else {
      t->get_byte = e1_tx_silence;      
    }
  }

  return 0;
}

/** \brief returns next byte which should be transmitted
 *  \details function counts frames and
 */
unsigned char e1_tx_get_next_byte( void )
{
  e1_tx_timeslot_t *t = &gl_e1_tx_ts[ gl_ts ];
  
  gl_ts ++;

  unsigned char byte = t->get_byte( t, gl_frame );

  if ( gl_ts == E1_TS_NUM ) {
    gl_frame ++;
    gl_ts = 0;
  }

  return byte; 
}

/** \brief Reads a file which corresponds to timeslot 
 */
unsigned char e1_tx_from_file( e1_tx_timeslot_t *this, unsigned int frame )
{
  unsigned char b = 0;
  int ret = fread( &b, 1, 1, this->file );  
  if ( ret == 1 ) {
    return b;
  } else { // some error or EOF
    fprintf( stderr, "can't read file '%s' for %d-th timeslot. sendind silence byte",
                      this->fname, this->num );
    fclose( this->file );
    this->file = NULL;
    this->fname = NULL;
    this->get_byte = e1_tx_silence;
    return this->get_byte( this, frame );
  }   
}

/** \brief just sending the silence byte 
 */
unsigned char e1_tx_silence( e1_tx_timeslot_t *this, unsigned int frame )
{
  return gl_params.silence_byte;
}

/** \brief generates 0th timeslot accordingly to G.704
 */
unsigned char e1_tx_sync_timeslot( e1_tx_timeslot_t *this, unsigned int frame )
{
  if ( gl_params.framing == E1_FRAMING_DOUBLEFRAME ) {
    return e1_tx_doubleframe_timeslot( this, frame );
  } else if ( gl_params.framing == E1_FRAMING_MULTIFRAME ) {
    return e1_tx_multiframe_timeslot( this, frame );
  } else {
    return 0;
  }  
}

/** \brief generates 0th timeslot without multiframe by CRC-4 structure.
 *  \param frame -- frame number
 *  \sa G.704 Table 5A
 */
unsigned char e1_tx_doubleframe_timeslot( e1_tx_timeslot_t *this, unsigned int frame )
{
  unsigned char ts0[2] = { E1_TS0_FAS_WORD, E1_TS0_NFAS_WORD };

  return ts0[ frame & 0x1 ];
}

/** \brief generates 0th timeslot with multiframe and optionally with CRC-4 structure.
 *  \param frame -- frame number
 *  \todo Add CRC-4 support
 *  \sa G.704 Table 5B
 */
unsigned char e1_tx_multiframe_timeslot( e1_tx_timeslot_t *this, unsigned int frame )
{
  unsigned char ts0[E1_DOUBLEFRAME_SIZE] = { E1_TS0_FAS_WORD, E1_TS0_NFAS_WORD };
 
  // please look at Table 5B in G.704.
  // CRC4 is not supported yet
  unsigned char Si[E1_MULTIFRAME_SIZE] = { 
   [0] = E1_TS0_Si_BIT( 0 ),  // C1
   [1] = E1_TS0_Si_BIT( 0 ),  // should be cleared
   [2] = E1_TS0_Si_BIT( 0 ),  // C2
   [3] = E1_TS0_Si_BIT( 0 ),  // should be cleared
   [4] = E1_TS0_Si_BIT( 0 ),  // C3
   [5] = E1_TS0_Si_BIT( 1 ),  // should be set
   [6] = E1_TS0_Si_BIT( 0 ),  // C4
   [7] = E1_TS0_Si_BIT( 0 ),  // should be cleared
   [8] = E1_TS0_Si_BIT( 0 ),  // C1 
   [9] = E1_TS0_Si_BIT( 1 ),  // should be set
   [10] = E1_TS0_Si_BIT( 0 ), // C2
   [11] = E1_TS0_Si_BIT( 1 ), // should be set
   [12] = E1_TS0_Si_BIT( 0 ), // C3
   [13] = E1_TS0_Si_BIT( 0 ), // E-bit
   [14] = E1_TS0_Si_BIT( 0 ), // C4
   [15] = E1_TS0_Si_BIT( 0 ), // E-bit
  };

  return ts0[ frame & 0x1 ] | Si[ frame & 0xf ];
}

/** \brief Associates a file with specified timeslot.
 */
void e1_tx_set_file_for_timeslot( unsigned char ts, const char *fname )
{
  assert( ts < E1_TS_NUM );

#ifdef DEBUG
  fprintf( stderr, "%s: ts%u = %s\n", __FUNCTION__, ts, fname );
#endif
  gl_e1_tx_ts[ ts ].fname = (const char*) strdup( fname );
}
