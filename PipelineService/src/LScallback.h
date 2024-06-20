#ifndef __LSCALLBACK_H__
#define __LSCALLBACK_H__

#include <luna-service2/lunaservice.h>
#include <stdbool.h>

// Declare of each method
// All method format must be : bool function(LSHandle*, LSMessage*, void*)
bool stop_pipeline(LSHandle *sh, LSMessage *message, void *data);

#endif // __LSCALLBACK_H__