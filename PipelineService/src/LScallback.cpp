#include <luna-service2/lunaservice.h>
#include <pbnjson.h>
#include <string.h>
#include <stdbool.h>
#include "LScallback.h"
#include "pipeline/pipeline.h"

#define BUF_SIZE 64

bool effect_to_file(LSHandle *sh, LSMessage *message, void *data) {
    LSError lserror;
    JSchemaInfo schemaInfo;
    jvalue_ref parsed = {0}, value = {0};
    jvalue_ref jobj = {0}, jreturnValue = {0};
    const char *url = NULL;
    char buf[BUF_SIZE] = {0, };

    LSErrorInit(&lserror);

    // Initialize schema
    jschema_info_init (&schemaInfo, jschema_all(), NULL, NULL);

    // get message from LS2 and parsing to make object
    parsed = jdom_parse(j_cstr_to_buffer(LSMessageGetPayload(message)), DOMOPT_NOOPT, &schemaInfo);

    if (jis_null(parsed)) {
        j_release(&parsed);
        return true;
    }

    // Get value from payload.input
    value = jobject_get(parsed, j_cstr_to_buffer("url"));

    // JSON Object to string without schema validation check
    url = jvalue_tostring_simple(value);

    objectDetectionPipeline(url, true, 20);
    /**
     * JSON create test
     */
    jobj = jobject_create();
    if (jis_null(jobj)) {
        j_release(&jobj);
        return true;
    }

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
