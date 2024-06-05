#ifndef __UTIL_H__
#define __UTIL_H__

#include <luna-service2/lunaservice.h>
#include <pbnjson.h>
#include <PmLog.h>
#include "util.h"

bool parse_ls_message(LSMessage *message, const char *url, bool *od, int *gl_effect);
static PmLogContext getPmLogContext();

#endif // __UTIL_H__