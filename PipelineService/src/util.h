#ifndef __UTIL_H__
#define __UTIL_H__

bool parse_ls_message(LSMessage *message, const char *url, bool *od, int *gl_effect);
static PmLogContext getPmLogContext();

#endif // __UTIL_H__