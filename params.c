#include <stdio.h>              /* for printf */
#include <string.h>              /* for printf */
#include <stdlib.h>             /* for exit */
#include <errno.h>
#include <getopt.h>
#include <libgen.h>
#include "params.h"
#include "e1ts.h"

prog_params_t gl_params;

void set_params_to_default( prog_params_t* p ) 
{
  p->verbose = 0;
  p->structure = E1_STRUCT_PCM30;
  p->framing = E1_FRAMING_DOUBLEFRAME;
  p->crc4 = 1; // enabled
  p->silence_byte = 0;
  p->max_frames = 0; // unlimited
}

static void print_help( const char* prog_name )
{
  printf( 
   "Usage: %s [OPTION]\n"
   "Options:\n"
   "  -s <struct>, --structure=<struct> set E1 structure.\n"
   "      Possible values are: PCM30, PCM31, UNSTR. Default = PCM30.\n" 
   "  -F <framing>, --framing=<framing> set E1 framing: double-frame or\n"
   "      multi-frame. Values are: 2 for double-frame, \n"
   "      16 for multi-frame. Default = 2\n"
   "  -c <on/off>, --crc4=<on/off> Enables of disables insertion of CRC-4\n"
   "       bits into SYNC timeslot. Valid only with framing=16.\n"
   "       Default = on.\n"
   "  -S <hexbyte>, --silence-byte=<hexbyte> Sets silence byte, which is\n" 
   "       transmitted into unused timeslots. Default = 0\n" 
   "  -n <num-of-frames>, --num-of-frames=<integer> Sets number of frames\n"
   "       to generate. Default value is 0 (unlimited). \n" 
   "  -t <tsnum:file>, --timeslot=<tsnum:file> Sets a file for a specified timeslot.\n"
   "       File will be read each time timeslot data is required. Example: -t 1:/dev/null\n"
   "       If file read will report EOF or error, specified timeslot will be switched\n"
   "       to filled with silence byte\n"
   "  -v, --verbose print more\n"
   "  -V, --version print version\n"
   "  -h, --help print this message\n",
   prog_name );
}

/** VERSION macros should be set for proper version displaying
 */
static void print_version( const char* prog_name )
{
  printf( "%s version: %s\n", basename( (char*)prog_name ), VERSION );
}

/** parses one parameter and saves parsed value to corresponding 
 * struct field.
 */
static int parse_param( char param, char* arg, prog_params_t* p )
{
  if ( param == 'v' ) {
    p->verbose = 1;
  } else {
#ifdef DEBUG    
    fprintf( stderr, "%c : %s\n", param, arg );
#endif
    switch ( param ) {
      case 'F':
        { 
          unsigned long f = strtoul( arg, NULL, 10 ); 
          if ( f == 2 ) { // double frame
            p->framing = E1_FRAMING_DOUBLEFRAME;
          } else if ( f == 16 ) { // multiframe
            p->framing = E1_FRAMING_MULTIFRAME;
          } else {
            fprintf( stderr, "invalid framing option (-F/--framing)\n" );
            return -EINVAL; 
          } 
        }
        break;

      case 's':
        p->structure = e1_struct_from_string( arg );
        if ( p->structure == E1_STRUCT_INVALID ) {
          fprintf( stderr, "invalid structure option (-s/--structure)\n" );
          return -EINVAL; 
        }
        break;
     
      case 'S':
        p->silence_byte = strtoul( arg, NULL, 16 );
        break;
     
      case 'n':
        p->max_frames = strtoul( arg, NULL, 10 );
        break;
    
      // -t/--timeslot 
      case 't': // we assume something like --timeslot=1:/dev/urandom
        {
          // splitting arg by ':'
          char *tofree, *optarg;
          tofree = optarg = strdup( arg ); 
          char *ts_num = strsep( &optarg, ":" );
          
          if ( ts_num == optarg ) { // delimiter not found
            fprintf( stderr, "invalid use of -t/--timeslot option\n" );
            free( tofree );
            return EINVAL;
          }
          
          unsigned char ts = strtoul( ts_num, NULL, 10 );
          if ( ts >= E1_TS_NUM ) {
            fprintf( stderr, "timeslot number %d is too big (max = 31)\n", ts );
            free( tofree );
            return EINVAL;
          }
          e1_tx_set_file_for_timeslot( ts, optarg );
          free( tofree );
        }
        break;
    
      // -c/--crc4 
      case 'c':
        if ( strcmp( arg, "on" ) == 0 ) {
          p->crc4 = 1;
        } else if ( strcmp( arg, "off" ) == 0 ) {
          p->crc4 = 0;
        } else {
          fprintf( stderr, "invalid crc4 option (-c/--crc4)\n" );
          return EINVAL; 
        }
        break;
    }
  }

  return 0;
}

int parse_args( int argc, char *argv[], prog_params_t *p )
{
  int c = 0;
  while( 1 ) {
    static struct option long_options[] = {
//             has param \    / param required
//                        |  |
//                        V  V
      { "structure",      1, 0, 's' },
      { "silence-byte",   1, 0, 'S' },
      { "num-of-frames",  1, 0, 'n' },
      { "framing",        1, 0, 'F' },
      { "crc4",           1, 0, 'c' },
      { "timeslot",       1, 0, 't' },
      { "verbose",        0, 0, 'v' },
      { "help",           0, 0, 'h' },
      { "version",        0, 0, 'V' },
      { 0, 0, 0, 0 }
    };

    c = getopt_long( argc, argv, "s:S:n:F:c:vhVt:",
                     long_options, NULL );
    if( c == -1 ) {
      break;
    } else {
#ifdef DEBUG      
      fprintf( stderr, "option '%c'", c );
      if ( optarg ) {
        fprintf( stderr, " with arg %s\n", optarg );
      } else {
        fprintf( stderr, "\n" );
      }
#endif      
    }

    switch ( c ) {
      case 'h':
        print_help( argv[0] );
        exit( EXIT_SUCCESS );
      case 'V':
        print_version( argv[0] );
        exit( EXIT_SUCCESS );
      case 's':
      case 'S':
      case 'F':
      case 'c':
      case 'v':
      case 'n':
      case 't':
        if ( parse_param( c, optarg, p ) != 0 ) {
          exit( EINVAL );
        }
        break;

      default:
        break; 
    }
  }

  if ( p->verbose ) {
    fprintf( stderr, "E1 frame structure = %s\n", 
                     e1_struct_to_string( p->structure ) );

    fprintf( stderr, "E1 framing type = %s\n", 
                     e1_framing_to_string( p->framing ) );

    fprintf( stderr, "Silence byte = %u\n", p->silence_byte );
    if ( p->framing != E1_FRAMING_DOUBLEFRAME ) {
      fprintf( stderr, "CRC-4 bits injection is %s\n", 
                     p->crc4 ? "on" : "off" );
    }
    
    if ( p->max_frames > 0 ) {
      fprintf( stderr, "will generate %d frames\n", p->max_frames );
    }
  }

  return 0;
}

