// Copyright (c) 2020 LG Electronics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <stdio.h>
#include <glib-object.h>
#include <luna-service2/lunaservice.h>
#include <pbnjson.h>
#include <PmLog.h>

// This service name
#define SERVICE_NAME "com.balance.app.service"
#define BUF_SIZE 64

static PmLogContext getPmLogContext() {
    static PmLogContext s_context = 0;
    if (0 == s_context) {
        PmLogGetContext("NativeService", &s_context);
    }
    return s_context;
}

// Main loop for aliving background service
GMainLoop *gmainLoop;

LSHandle  *sh = NULL;
LSMessage *message;

// Declare of each method
// All method format must be : bool function(LSHandle*, LSMessage*, void*)
bool effect_to_file(LSHandle *sh, LSMessage *message, void *data);
bool stop_pipeline(LSHandle *sh, LSMessage *message, void *data);

LSMethod sampleMethods[] = {
    {"file", effect_to_file},
    {"stop", stop_pipeline},
};

bool effect_to_file(LSHandle *sh, LSMessage *message, void *data) {
    PmLogInfo(getPmLogContext(), "HANDLE_HELLO", 0, "hello method called");
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
    PmLogInfo(getPmLogContext(), "HANDLE_HELLO", 0, "hello method called");

    objectDetectionPipeline(url);
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

bool stop_pipeline(LSHandle *sh, LSMessage *message, void *data) {
    /* TODO */
    return true;
}

// Register background service and initialize
int main(int argc, char* argv[]) {
    LSError lserror;
    LSHandle  *handle = NULL;
    bool bRetVal = FALSE;

    LSErrorInit(&lserror);

    // create a GMainLoop
    gmainLoop = g_main_loop_new(NULL, FALSE);

    bRetVal = LSRegister(SERVICE_NAME, &handle, &lserror);
    if (FALSE== bRetVal) {
        LSErrorFree( &lserror );
        return 0;
    }
    sh = LSMessageGetConnection(message);

    LSRegisterCategory(handle, "/", sampleMethods, NULL, NULL, &lserror);

    LSGmainAttach(handle, gmainLoop, &lserror);

    // run to check continuously for new events from each of the event sources
    g_main_loop_run(gmainLoop);
    // Decreases the reference count on a GMainLoop object by one
    g_main_loop_unref(gmainLoop);

    return 0;
}