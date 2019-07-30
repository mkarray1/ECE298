#include <stdint.h>

void setUltrasonicFuncPtr( void (*getUltrasonicTimestamp)(uint16_t timestamp) );
void setRpmFuncPtr( void (*getRpmTimestamp)(uint16_t timestamp, int caller) );
void setMicFuncPtr( void (*getMicStatus)(void) );
