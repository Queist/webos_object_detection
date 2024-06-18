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

    std::string url;
    bool od;
    int gl_effect;
    if (!parse_ls_message(message, url, od, gl_effect))
        return true;

    std::thread([parsed_url]() { objectDetectionPipeline(parsed_url, od, gl_effect); }).detach();

    /**
     * JSON create test
     */
    jvalue_ref jobj = {0};

    jobj = jobject_create();
    if (jis_null(jobj)) {
        j_release(&jobj);
        return true;
    }

    jvalue_ref jreturnValue = {0};
    jreturnValue = jboolean_create(TRUE);
    jobject_set(jobj, j_cstr_to_buffer("returnValue"), jreturnValue);

    LSMessageReply(sh, message, jvalue_tostring_simple(jobj), &lserror);

    j_release(&parsed);
    return true;
}

bool effect_to_camera(LSHandle* sh, LSMessage* message, void* data) {
    /* TODO */
    return true;
}

bool stop_pipeline(LSHandle *sh, LSMessage *message, void *data) {
    /* TODO */
    return true;
}
