#include <luna-service2/lunaservice.h>
#include <pbnjson.hpp>
#include <PmLog.h>
#include <string>
#include "util.h"

bool parse_ls_message(LSMessage *message, std::string &url, bool &od, int &gl_effect) {
    JSchemaInfo schemaInfo;
    // Initialize schema
    jschema_info_init(&schemaInfo, jschema_all(), NULL, NULL);

    pbnjson::JValue request = pbnjson::JValue();

    pbnjson::JInput input(LSMessageGetPayload(message));
    pbnjson::JSchema schema = pbnjson::JSchemaFragment("{}");
    pbnjson::JDomParser parser;

    if (parser.parse(input, schema)) {
        request = parser.getDom();
    } else {
        return false;
    }

    if (!request["objectDetection"].isNull()) {
        od = request["objectDetection"].asBool();
    } else {
        return false;
    }

    if (request["glEffect"].isNumber()) {
        gl_effect = request["glEffect"].asNumber();
    } else {
        return false;
    }

    if (!request["url"].isNull()) {
        std::string tmp = request["url"].asString();
        url = tmp.substr(1, tmp.size() - 2);
    } else {
        return false;
    }

    return true;
}