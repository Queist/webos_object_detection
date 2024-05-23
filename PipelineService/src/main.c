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
#include "LSCallBack.h"
#include "util.h"

// This service name
#define SERVICE_NAME "com.balance.app.service"
#define BUF_SIZE 64

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
    {"camera", effect_to_camera},
    {"stop", stop_pipeline},
};

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
