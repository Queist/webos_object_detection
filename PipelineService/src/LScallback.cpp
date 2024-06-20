#include <luna-service2/lunaservice.h>
#include <pbnjson.h>
#include <string.h>
#include <string>
#include <stdbool.h>
#include <thread>
#include "LScallback.h"
#include "pipeline/pipeline.h"
#include "util.h"

#define BUF_SIZE 64

bool effect_to_file(LSHandle *sh, LSMessage *message, void *data) {
    LSError lserror;
    LSErrorInit(&lserror);

    //TODO : handling error with LSError

    /**
     * JSON create test
     */
    jvalue_ref jobj = {0}, jreturnValue = {0};

    jobj = jobject_create();
    if (jis_null(jobj)) {
        j_release(&jobj);
        return true;
    }

    std::string url;
    bool od;
    int gl_effect;
    if (!parse_ls_message(message, url, od, gl_effect)) {
        jreturnValue = jboolean_create(FALSE);
        jobject_set(jobj, j_cstr_to_buffer("complete"), jreturnValue);

        LSMessageReply(sh, message, jvalue_tostring_simple(jobj), &lserror);
        return true;
    }

    std::thread([url, od, gl_effect]() { objectDetectionPipeline(url, od, gl_effect); }).detach();

    jreturnValue = jboolean_create(TRUE);
    jobject_set(jobj, j_cstr_to_buffer("complete"), jreturnValue);

    LSMessageReply(sh, message, jvalue_tostring_simple(jobj), &lserror);
    return true;
}

