#include <string.h>
#include "e1.h"


const char* e1_framing_to_string( e1_framing_t framing )
{
  if ( framing == E1_FRAMING_DOUBLEFRAME ) {
    return "double-frame";
  } else if ( framing == E1_FRAMING_MULTIFRAME ) {
    return "multiframe";
  } else {
    return "";
  }
}

/** converts string to e1_struct_t value.
 */
e1_struct_t e1_struct_from_string( const char *s )
{
  if ( strcmp( s, "unstr" ) == 0 ) {
    return E1_STRUCT_UNSTRUCTURED;
  } else if ( strcmp( s, "pcm31" ) == 0 ) {
    return E1_STRUCT_PCM31;
  } else if ( strcmp( s, "pcm30" ) == 0 ) {
    return E1_STRUCT_PCM30;
  } else return E1_STRUCT_INVALID;
}

const char* e1_struct_to_string( e1_struct_t st )
{
  switch ( st ) {
    case E1_STRUCT_UNSTRUCTURED: return "unstr";
    case E1_STRUCT_PCM31:        return "pcm31";
    case E1_STRUCT_PCM30:        return "pcm30";
    default:                     return "";
  }
}
