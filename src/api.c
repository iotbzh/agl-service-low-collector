#include "low-collector.h"

/*
static const struct afb_auth collector_cpu_auths[] = {
};
*/

static const struct afb_verb_v2 collector_cpu_verbs[] = {
    // SEB
	// {
	// 	.verb = "subscribe",
	// 	.callback = verb_subscribe,
	// 	.auth = NULL,
	// 	.info = NULL,
	// 	.session = AFB_SESSION_NONE_V2
	// },
	// {
	// 	.verb = "unsubscribe",
	// 	.callback = verb_unsubscribe,
	// 	.auth = NULL,
	// 	.info = NULL,
	// 	.session = AFB_SESSION_NONE_V2
	// },
	{
		.verb = "list-metrics",
		.callback = verb_list_metrics,
		.auth = NULL,
		.info = NULL,
		.session = AFB_SESSION_NONE_V2
	},
	{
		.verb = "start",
		.callback = verb_start,
		.auth = NULL,
		.info = NULL,
		.session = AFB_SESSION_NONE_V2
	},
	{
		.verb = "stop",
		.callback = verb_stop,
		.auth = NULL,
		.info = NULL,
		.session = AFB_SESSION_NONE_V2
	},
	{ .verb = NULL }
};

const struct afb_binding afbBindingV2 = {
	.api = "collector-controller",
	.specification = NULL,
	.info = NULL,
	.verbs = collector_cpu_verbs,
	.preinit = NULL,
	.init = init,
	.onevent = NULL,
	.noconcurrency = 0
};
