#pragma once

#include <json-c/json.h>

#define EXIT_LIBCOLLD_SUCCESS				0
#define EXIT_LIBCOLLD_NOT_INITIALIZED		1
#define EXIT_LIBCOLLD_NO_DEVICE_FOUND		2

int libcolld_init();
int libcolld_list_metrics(struct json_object* result);
int libcolld_start_polling(struct json_object* result, struct json_object* devices);
