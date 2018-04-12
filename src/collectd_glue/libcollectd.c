#include "../low-collector.h"

#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>

// FIXME: It compile without these lines, but KDevelop complains about pthread_t being undeclared.
#ifndef _BITS_PTHREADTYPES
typedef unsigned long int pthread_t;
#endif

#include "libcollectd.h"

extern struct afb_event on_nfc_target_add_event;
extern struct afb_event on_nfc_target_remove_event;

#define MAX_NFC_DEVICE_COUNT 8
#define MAX_NFC_MODULATIONS 8
#define MAX_NFC_BAUDRATES 8
#define POLL_NUMBER 0x1
#define POLL_PERIOD 0x7


typedef struct libcolld_metrics_tag
{
    pthread_t poller;
    char *name;
    /*
    nfc_device *device;
    nfc_connstring name;

    nfc_modulation *modulations;
    size_t modulations_count;
    */
} libcolld_metrics;

typedef struct libcolld_context_tag
{
    void * context;
    //    nfc_context *context;
    libcolld_metrics *metrics;
    size_t metrics_count;
    struct json_object *last_target;
} libcolld_context;

static libcolld_context libcollectd;

void libcolld_polling_error(int code)
{
    /*
    switch (code)
    {
    case NFC_EIO:
        AFB_ERROR("libcollectd: polling failed with NFC_EIO (%d) code: Input / output error, device may not be usable anymore without re-open it!", code);
        break;
    case NFC_EINVARG:
        AFB_ERROR("libcollectd: polling failed with NFC_EINVARG (%d) code: Invalid argument(s)!", code);
        break;
    case NFC_EDEVNOTSUPP:
        AFB_ERROR("libcollectd: polling failed with NFC_EDEVNOTSUPP (%d) code: Operation not supported by device!", code);
        break;
    case NFC_ENOTSUCHDEV:
        AFB_ERROR("libcollectd: polling failed with NFC_ENOTSUCHDEV (%d) code: No such device!", code);
        break;
    case NFC_EOVFLOW:
        AFB_ERROR("libcollectd: polling failed with NFC_EOVFLOW (%d) code: Buffer overflow!", code);
        break;
    case NFC_ETIMEOUT:
        AFB_ERROR("libcollectd: polling failed with NFC_ETIMEOUT (%d) code: Operation timed out!", code);
        break;
    case NFC_EOPABORTED:
        AFB_ERROR("libcollectd: polling failed with NFC_EOPABORTED (%d) code: Operation aborted (by user)!", code);
        break;
    case NFC_ENOTIMPL:
        AFB_ERROR("libcollectd: polling failed with NFC_ENOTIMPL (%d) code: Not (yet) implemented!", code);
        break;
    case NFC_ETGRELEASED:
        AFB_ERROR("libcollectd: polling failed with NFC_ETGRELEASED (%d) code: Target released!", code);
        break;
    case NFC_ERFTRANS:
        AFB_ERROR("libcollectd: polling failed with NFC_ERFTRANS (%d) code: Error while RF transmission!", code);
        break;
    case NFC_EMFCAUTHFAIL:
        AFB_ERROR("libcollectd: polling failed with NFC_EMFCAUTHFAIL (%d) code: MIFARE Classic: authentication failed!", code);
        break;
    case NFC_ESOFT:
        AFB_ERROR("libcollectd: polling failed with NFC_ESOFT (%d) code: Software error (allocation, file/pipe creation, etc.)!", code);
        break;
    case NFC_ECHIP:
        //AFB_ERROR("libcollectd: polling failed with NFC_ECHIP (%d) code: Device's internal chip error!", code);
        break;
    default:
        AFB_ERROR("libcollectd: polling failed with unknown code: %d!", code);
        break;
    }
    */
}

/*
struct json_object *read_target(const nfc_target *target)
{
    struct json_object *result;
    const char *mt;

    if (!target)
    {
        AFB_WARNING("libcollectd: No target to read!");
        return NULL;
    }

    result = json_object_new_object();

    mt = str_nfc_modulation_type(target->nm.nmt);
    json_object_object_add(result, "Type", json_object_new_string(mt));

    switch (target->nm.nmt)
    {
    case NMT_ISO14443A:
        add_nfc_field(result, "ATQA", target->nti.nai.abtAtqa, 2);
        add_nfc_field(result, "SAK", &target->nti.nai.btSak, 1);
        add_nfc_field(result, "UID", target->nti.nai.abtUid, target->nti.nai.szUidLen);
        add_nfc_field(result, "ATS", target->nti.nai.abtAts, target->nti.nai.szAtsLen);

        break;
    case NMT_ISO14443B:
        add_nfc_field(result, "PUPI", target->nti.nbi.abtPupi, 4);
        add_nfc_field(result, "Application Data", target->nti.nbi.abtApplicationData, 4);
        add_nfc_field(result, "Protocol Info", target->nti.nbi.abtProtocolInfo, 3);
        add_nfc_field(result, "Card Id", &target->nti.nbi.ui8CardIdentifier, 1);

        break;
    default:
        AFB_WARNING("libcollectd: unsupported modulation type: %s.", mt);
        json_object_object_add(result, "error", json_object_new_string("unsupported tag type"));
        break;
    }
    return result;
}
*/

void *libcolld_reader_main(void *arg)
{
    /*
    libcolld_metrics *metrics;
    nfc_target nt;
    int polled_target_count;
    nfc_modulation mods[MAX_NFC_MODULATIONS];
    struct json_object *result;
    size_t i, j;

    metrics = (libcolld_metrics *)arg;

    memset(mods, 0, sizeof(nfc_modulation) * MAX_NFC_MODULATIONS);
    for (i = 0, j = 0; i < metrics->modulations_count; ++i, ++j)
    {
        switch (metrics->modulations[i].nmt)
        {
#if defined(LIBCOLLD_POLL_ALL) || defined(LIBCOLLD_POLL_NMT_ISO14443A)
        case NMT_ISO14443A:
#endif
#if defined(LIBCOLLD_POLL_ALL) || defined(LIBCOLLD_POLL_NMT_ISOJEWEL)
        case NMT_JEWEL:
#endif
#if defined(LIBCOLLD_POLL_ALL) || defined(LIBCOLLD_POLL_NMT_ISO14443B)
        case NMT_ISO14443B:
#endif
#if defined(LIBCOLLD_POLL_ALL) || defined(LIBCOLLD_POLL_NMT_ISO14443BI)
        case NMT_ISO14443BI:
#endif
#if defined(LIBCOLLD_POLL_ALL) || defined(LIBCOLLD_POLL_NMT_ISO14443B2SR)
        case NMT_ISO14443B2SR:
#endif
#if defined(LIBCOLLD_POLL_ALL) || defined(LIBCOLLD_POLL_NMT_ISO14443B2CT)
        case NMT_ISO14443B2CT:
#endif
#if defined(LIBCOLLD_POLL_ALL) || defined(LIBCOLLD_POLL_NMT_FELICA)
        case NMT_FELICA:
#endif
            mods[j] = metrics->modulations[i];
            AFB_NOTICE("libcollectd: polling for %s at %s is ENABLED.", str_nfc_modulation_type(metrics->modulations[i].nmt), str_nfc_baud_rate(metrics->modulations[i].nbr));
            break;
        default:
            --j;
            // NMT_DEP is always disabled because it can't be polled
            AFB_NOTICE("libcollectd: polling for %s at %s is DISABLED.", str_nfc_modulation_type(metrics->modulations[i].nmt), str_nfc_baud_rate(metrics->modulations[i].nbr));
            break;
        }
    }

    while (metrics->metrics)
    {
        polled_target_count = nfc_initiator_poll_target(
            metrics->metrics,
            mods,
            j,
            POLL_NUMBER,
            POLL_PERIOD,
            &nt);

        switch (polled_target_count)
        {
        case 0:
            // No target detected
            AFB_INFO("libcollectd: polling done with no result.");
            if (libcollectd.last_target)
            {
                AFB_NOTICE("libcollectd: tag removed = %s", json_object_to_json_string(libcollectd.last_target));
                afb_event_push(on_nfc_target_remove_event, libcollectd.last_target);
                libcollectd.last_target = NULL;
            }
            break;

        case 1:
            AFB_INFO("libcollectd: polling done with one result.");
            // One target detected
            result = read_target(&nt);

            if (libcollectd.last_target)
            {
                if (strcmp(json_object_to_json_string(result), json_object_to_json_string(libcollectd.last_target)))
                {
                    AFB_NOTICE("libcollectd: tag removed = %s", json_object_to_json_string(libcollectd.last_target));
                    afb_event_push(on_nfc_target_remove_event, libcollectd.last_target);
                    libcollectd.last_target = NULL;
                }
            }

            if (!libcollectd.last_target)
            {
                json_object_get(result);
                libcollectd.last_target = result;

                AFB_NOTICE("libcollectd: tag added = %s", json_object_to_json_string(result));
                afb_event_push(on_nfc_target_add_event, result);
            }
            break;

        default:
            if (polled_target_count < 0)
                libcolld_polling_error(polled_target_count);
            else
                AFB_WARNING("libcollectd: polling done with unsupported result count: %d.", polled_target_count);

            // Consider target is removed
            if (libcollectd.last_target)
            {
                AFB_NOTICE("libcollectd: tag removed = %s", json_object_to_json_string(libcollectd.last_target));
                afb_event_push(on_nfc_target_remove_event, libcollectd.last_target);
                libcollectd.last_target = NULL;
            }
            break;
        }
    }
    */
    return NULL;
}

void exit_handler()
{
    size_t i;
    //nfc_device *dev;
    for (i = 0; i < libcollectd.metrics_count; ++i)
    {
        /*
        if (libcollectd.metrics[i].device)
        {
            dev = libcollectd.metrics[i].device;
            libcollectd.metrics[i].device = NULL;
            nfc_close(dev);
        }
     */
    }
    nfc_exit(libcollectd.context);
    libcollectd.context = NULL;
}

void sigterm_handler(int sig)
{
    if (sig == SIGTERM && libcollectd.context && libcollectd.metrics_count)
    {
        exit_handler();
    }
}

/// @brief Start the libcollectd context.
/// @return An exit code, @c EXIT_LIBCOLLD_SUCCESS (zero) on success.
int libcolld_init()
{
    plugin_init_all();

    plugin_register_init();

    return EXIT_LIBCOLLD_SUCCESS;
}

/// @brief List metrics founds by libcollectd.
/// @param[in] result A json object array into which found metrics are added.
/// @return An exit code, @c EXIT_LIBCOLLD_SUCCESS (zero) on success.
int libcolld_list_metrics(struct json_object *result)
{
    struct json_object *metrics;
    size_t i;

    for (i = 0; i < libcollectd.metrics_count; ++i)
    {
        metrics = json_object_new_object();
        json_object_object_add(metrics, "source", json_object_new_string("libcollectd"));
        json_object_object_add(metrics, "name", json_object_new_string(libcollectd.metrics[i].name));
        json_object_array_add(result, metrics);
    }

    return EXIT_LIBCOLLD_SUCCESS;
}

int libcolld_start_polling(struct json_object *result, struct json_object *devices)
{
/*
    struct json_object *device;
    size_t i;
    int r;

    for (i = 0; i < libcollectd.metrics_count; ++i)
    {
        device = json_object_new_object();
        json_object_object_add(device, "source", json_object_new_string("libcollectd"));
        json_object_object_add(device, "name", json_object_new_string(libcollectd.devices[i].name));
        if (libcollectd.devices[i].device)
        {
            json_object_object_add(device, "status", json_object_new_string("already polling"));
            AFB_NOTICE("libcollectd: Device '%s' is already polling.", libcollectd.devices[i].name);
        }
        else
        {
            libcollectd.devices[i].device = nfc_open(libcollectd.context, libcollectd.devices[i].name);
            if (libcollectd.devices[i].device)
            {
                if (nfc_initiator_init(libcollectd.devices[i].device) < 0)
                {
                    nfc_close(libcollectd.devices[i].device);
                    libcollectd.devices[i].device = NULL;
                    json_object_object_add(device, "status", json_object_new_string("failed to set initiator mode"));
                    AFB_ERROR("libcollectd: nfc_initiator_init failedfor device '%s'!", libcollectd.devices[i].name);
                }
                else
                {
                    r = pthread_create(&libcollectd.devices[i].poller, NULL, libcolld_reader_main, (void *)&libcollectd.devices[i]);
                    if (r)
                    {
                        nfc_close(libcollectd.devices[i].device);
                        libcollectd.devices[i].device = NULL;
                        json_object_object_add(device, "status", json_object_new_string("failed to create the polling thread"));
                        AFB_ERROR("libcollectd: pthread_create failed!");
                    }
                    else
                    {
                        json_object_object_add(device, "status", json_object_new_string("polling"));
                        AFB_NOTICE("libcollectd: Polling the device '%s'.", libcollectd.devices[i].name);
                    }
                }
            }
            else
            {
                json_object_object_add(device, "status", json_object_new_string("failed to open device"));
                AFB_ERROR("libcollectd: Failed to open device '%s'!", libcollectd.devices[i].name);
            }
        }
        json_object_array_add(result, device);
    }
*/
    return EXIT_LIBCOLLD_SUCCESS;
}

#if SEB
int plugin_register_init(const char *name, int (*callback)(void)) {
    return 0;
}

int plugin_register_config(const char *name,
                           int (*callback)(const char *key, const char *val),
                           const char **keys, int keys_num) {

}

int plugin_register_read(const char *name, int (*callback)(void)) {

}
#endif
