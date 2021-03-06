/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details:
 *
 * Copyright (C) 2008 Novell, Inc.
 * Copyright (C) 2009 - 2012 Red Hat, Inc.
 * Copyright (C) 2011 - 2012 Google, Inc.
 */

#include "mm-error-helpers.h"
#include "mm-log.h"

#include <ctype.h>

typedef struct {
    guint code;
    const gchar *error;  /* lowercase, and stripped of special chars and whitespace */
    const gchar *message;
} ErrorTable;

/* --- Connection errors --- */

GError *
mm_connection_error_for_code (MMConnectionError code)
{
    const gchar *msg;

    switch (code) {
    case MM_CONNECTION_ERROR_UNKNOWN:
        msg = "Unknown";
        break;
    case MM_CONNECTION_ERROR_NO_CARRIER:
        msg = "No carrier";
        break;
    case MM_CONNECTION_ERROR_NO_DIALTONE:
        msg = "No dialtone";
        break;
    case MM_CONNECTION_ERROR_BUSY:
        msg = "Busy";
        break;
    case MM_CONNECTION_ERROR_NO_ANSWER:
        msg = "No answer";
        break;

    default:
        mm_dbg ("Invalid connection error code: %u", code);
        /* uhm... make something up (yes, ok, lie!). */
        code = MM_CONNECTION_ERROR_NO_CARRIER;
        msg = "No carrier";
    }

    return g_error_new_literal (MM_CONNECTION_ERROR, code, msg);
}

/* --- Mobile equipment errors --- */

static ErrorTable me_errors[] = {
    { MM_MOBILE_EQUIPMENT_ERROR_PHONE_FAILURE,                      "phonefailure",                              "Phone failure" },
    { MM_MOBILE_EQUIPMENT_ERROR_NO_CONNECTION,                      "noconnectiontophone",                       "No connection to phone" },
    { MM_MOBILE_EQUIPMENT_ERROR_LINK_RESERVED,                      "phoneadapterlinkreserved",                  "Phone-adaptor link reserved" },
    { MM_MOBILE_EQUIPMENT_ERROR_NOT_ALLOWED,                        "operationnotallowed",                       "Operation not allowed" },
    { MM_MOBILE_EQUIPMENT_ERROR_NOT_SUPPORTED,                      "operationnotsupported",                     "Operation not supported" },
    { MM_MOBILE_EQUIPMENT_ERROR_PH_SIM_PIN,                         "phsimpinrequired",                          "PH-SIM PIN required" },
    { MM_MOBILE_EQUIPMENT_ERROR_PH_FSIM_PIN,                        "phfsimpinrequired",                         "PH-FSIM PIN required" },
    { MM_MOBILE_EQUIPMENT_ERROR_PH_FSIM_PUK,                        "phfsimpukrequired",                         "PH-FSIM PUK required" },
    { MM_MOBILE_EQUIPMENT_ERROR_SIM_NOT_INSERTED,                   "simnotinserted",                            "SIM not inserted" },
    { MM_MOBILE_EQUIPMENT_ERROR_SIM_PIN,                            "simpinrequired",                            "SIM PIN required" },
    { MM_MOBILE_EQUIPMENT_ERROR_SIM_PUK,                            "simpukrequired",                            "SIM PUK required" },
    { MM_MOBILE_EQUIPMENT_ERROR_SIM_FAILURE,                        "simfailure",                                "SIM failure" },
    { MM_MOBILE_EQUIPMENT_ERROR_SIM_BUSY,                           "simbusy",                                   "SIM busy" },
    { MM_MOBILE_EQUIPMENT_ERROR_SIM_WRONG,                          "simwrong",                                  "SIM wrong" },
    { MM_MOBILE_EQUIPMENT_ERROR_INCORRECT_PASSWORD,                 "incorrectpassword",                         "Incorrect password" },
    { MM_MOBILE_EQUIPMENT_ERROR_SIM_PIN2,                           "simpin2required",                           "SIM PIN2 required" },
    { MM_MOBILE_EQUIPMENT_ERROR_SIM_PUK2,                           "simpuk2required",                           "SIM PUK2 required" },
    { MM_MOBILE_EQUIPMENT_ERROR_MEMORY_FULL,                        "memoryfull",                                "Memory full" },
    { MM_MOBILE_EQUIPMENT_ERROR_INVALID_INDEX,                      "invalidindex",                              "Invalid index" },
    { MM_MOBILE_EQUIPMENT_ERROR_NOT_FOUND,                          "notfound",                                  "Not found" },
    { MM_MOBILE_EQUIPMENT_ERROR_MEMORY_FAILURE,                     "memoryfailure",                             "Memory failure" },
    { MM_MOBILE_EQUIPMENT_ERROR_TEXT_TOO_LONG,                      "textstringtoolong",                         "Text string too long" },
    { MM_MOBILE_EQUIPMENT_ERROR_INVALID_CHARS,                      "invalidcharactersintextstring",             "Invalid characters in text string" },
    { MM_MOBILE_EQUIPMENT_ERROR_DIAL_STRING_TOO_LONG,               "dialstringtoolong",                         "Dial string too long" },
    { MM_MOBILE_EQUIPMENT_ERROR_DIAL_STRING_INVALID,                "invalidcharactersindialstring",             "Invalid characters in dial string" },
    { MM_MOBILE_EQUIPMENT_ERROR_NO_NETWORK,                         "nonetworkservice",                          "No network service" },
    { MM_MOBILE_EQUIPMENT_ERROR_NETWORK_TIMEOUT,                    "networktimeout",                            "Network timeout" },
    { MM_MOBILE_EQUIPMENT_ERROR_NETWORK_NOT_ALLOWED,                "networknotallowedemergencycallsonly",       "Network not allowed - emergency calls only" },
    { MM_MOBILE_EQUIPMENT_ERROR_NETWORK_PIN,                        "networkpersonalizationpinrequired",         "Network personalization PIN required" },
    { MM_MOBILE_EQUIPMENT_ERROR_NETWORK_PUK,                        "networkpersonalizationpukrequired",         "Network personalization PUK required" },
    { MM_MOBILE_EQUIPMENT_ERROR_NETWORK_SUBSET_PIN,                 "networksubsetpersonalizationpinrequired",   "Network subset personalization PIN required" },
    { MM_MOBILE_EQUIPMENT_ERROR_NETWORK_SUBSET_PUK,                 "networksubsetpersonalizationpukrequired",   "Network subset personalization PUK required" },
    { MM_MOBILE_EQUIPMENT_ERROR_SERVICE_PIN,                        "serviceproviderpersonalizationpinrequired", "Service provider personalization PIN required" },
    { MM_MOBILE_EQUIPMENT_ERROR_SERVICE_PUK,                        "serviceproviderpersonalizationpukrequired", "Service provider personalization PUK required" },
    { MM_MOBILE_EQUIPMENT_ERROR_CORP_PIN,                           "corporatepersonalizationpinrequired",       "Corporate personalization PIN required" },
    { MM_MOBILE_EQUIPMENT_ERROR_CORP_PUK,                           "corporatepersonalizationpukrequired",       "Corporate personalization PUK required" },
    { MM_MOBILE_EQUIPMENT_ERROR_UNKNOWN,                            "unknownerror",                              "Unknown error" },
    { MM_MOBILE_EQUIPMENT_ERROR_GPRS_ILLEGAL_MS,                    "illegalms",                                 "Illegal MS" },
    { MM_MOBILE_EQUIPMENT_ERROR_GPRS_ILLEGAL_ME,                    "illegalme",                                 "Illegal ME" },
    { MM_MOBILE_EQUIPMENT_ERROR_GPRS_SERVICE_NOT_ALLOWED,           "gprsservicesnotallowed",                    "GPRS services not allowed" },
    { MM_MOBILE_EQUIPMENT_ERROR_GPRS_PLMN_NOT_ALLOWED,              "plmnnotallowed",                            "PLMN not allowed" },
    { MM_MOBILE_EQUIPMENT_ERROR_GPRS_LOCATION_NOT_ALLOWED,          "locationareanotallowed",                    "Location area not allowed" },
    { MM_MOBILE_EQUIPMENT_ERROR_GPRS_ROAMING_NOT_ALLOWED,           "roamingnotallowedinthislocationarea",       "Roaming not allowed in this location area" },
    { MM_MOBILE_EQUIPMENT_ERROR_GPRS_SERVICE_OPTION_NOT_SUPPORTED,  "serviceoperationnotsupported",              "Service option not supported" },
    { MM_MOBILE_EQUIPMENT_ERROR_GPRS_SERVICE_OPTION_NOT_SUBSCRIBED, "requestedserviceoptionnotsubscribed",       "Requested service option not subscribed" },
    { MM_MOBILE_EQUIPMENT_ERROR_GPRS_SERVICE_OPTION_OUT_OF_ORDER,   "serviceoptiontemporarilyoutoforder",        "Service option temporarily out of order" },
    { MM_MOBILE_EQUIPMENT_ERROR_GPRS_UNKNOWN,                       "unspecifiedgprserror",                      "Unspecified GPRS error" },
    { MM_MOBILE_EQUIPMENT_ERROR_GPRS_PDP_AUTH_FAILURE,              "pdpauthenticationfailure",                  "PDP authentication failure" },
    { MM_MOBILE_EQUIPMENT_ERROR_GPRS_INVALID_MOBILE_CLASS,          "invalidmobileclass",                        "Invalid mobile class" },
};

GError *
mm_mobile_equipment_error_for_code (MMMobileEquipmentError code)
{
    guint i;

    /* Look for the code */
    for (i = 0; i < G_N_ELEMENTS (me_errors); i++) {
        if (me_errors[i].code == code)
            return g_error_new_literal (MM_MOBILE_EQUIPMENT_ERROR,
                                        code,
                                        me_errors[i].message);
    }

    /* Not found? Then, default */
    mm_dbg ("Invalid mobile equipment error code: %u", (guint)code);
    return g_error_new (MM_MOBILE_EQUIPMENT_ERROR,
                        MM_MOBILE_EQUIPMENT_ERROR_UNKNOWN,
                        "Unknown error");
}

GError *
mm_mobile_equipment_error_for_string (const gchar *str)
{
    MMMobileEquipmentError code = MM_MOBILE_EQUIPMENT_ERROR_UNKNOWN;
    const gchar *msg = NULL;
    gchar *buf;
    guint i;
    guint j;

    g_return_val_if_fail (str != NULL, NULL);

    /* Normalize the error code by stripping whitespace and odd characters */
    buf = g_strdup (str);
    for (i = 0, j = 0; str[i]; i++) {
        if (isalnum (str[i]))
            buf[j++] = tolower (str[i]);
    }
    buf[j] = '\0';

    /* Look for the string */
    for (i = 0; i < G_N_ELEMENTS (me_errors); i++) {
        if (g_str_equal (me_errors[i].error, buf)) {
            code = me_errors[i].code;
            msg = me_errors[i].message;
            break;
        }
    }

    /* Not found? Then, default */
    if (!msg) {
        mm_dbg ("Invalid mobile equipment error string: '%s' (%s)", str, buf);
        code = MM_MOBILE_EQUIPMENT_ERROR_UNKNOWN;
        msg = "Unknown error";
    }

    g_free (buf);
    return g_error_new_literal (MM_MOBILE_EQUIPMENT_ERROR, code, msg);
}

/* --- Message errors --- */

static ErrorTable msg_errors[] = {
    { MM_MESSAGE_ERROR_ME_FAILURE,             "mefailure",             "ME failure" },
    { MM_MESSAGE_ERROR_SMS_SERVICE_RESERVED,   "smsservicereserved",    "SMS service reserved" },
    { MM_MESSAGE_ERROR_NOT_ALLOWED,            "operationnotallowed",   "Operation not allowed" },
    { MM_MESSAGE_ERROR_NOT_SUPPORTED,          "operationnotsupported", "Operation not supported" },
    { MM_MESSAGE_ERROR_INVALID_PDU_PARAMETER,  "invalidpduparameter",   "Invalid PDU mode parameter" },
    { MM_MESSAGE_ERROR_INVALID_TEXT_PARAMETER, "invalidtextparameter",  "Invalid text mode parameter" },
    { MM_MESSAGE_ERROR_SIM_NOT_INSERTED,       "simnotinserted",        "SIM not inserted" },
    { MM_MESSAGE_ERROR_SIM_PIN,                "simpinrequired",        "SIM PIN required" },
    { MM_MESSAGE_ERROR_PH_SIM_PIN,             "phsimpinrequired",      "PH-SIM PIN required" },
    { MM_MESSAGE_ERROR_SIM_FAILURE,            "simfailure",            "SIM failure" },
    { MM_MESSAGE_ERROR_SIM_BUSY,               "simbusy",               "SIM busy" },
    { MM_MESSAGE_ERROR_SIM_WRONG,              "simwrong",              "SIM wrong" },
    { MM_MESSAGE_ERROR_SIM_PUK,                "simpukrequired",        "SIM PUK required" },
    { MM_MESSAGE_ERROR_SIM_PIN2,               "simpin2required",       "SIM PIN2 required" },
    { MM_MESSAGE_ERROR_SIM_PUK2,               "simpuk2required",       "SIM PUK2 required" },
    { MM_MESSAGE_ERROR_MEMORY_FAILURE,         "memoryfailure",         "Memory failure" },
    { MM_MESSAGE_ERROR_INVALID_INDEX,          "invalidindex",          "Invalid index" },
    { MM_MESSAGE_ERROR_MEMORY_FULL,            "memoryfull",            "Memory full" },
    { MM_MESSAGE_ERROR_SMSC_ADDRESS_UNKNOWN,   "smscaddressunknown",    "SMSC address unknown" },
    { MM_MESSAGE_ERROR_NO_NETWORK,             "nonetwork",             "No network" },
    { MM_MESSAGE_ERROR_NETWORK_TIMEOUT,        "networktimeout",        "Network timeout" },
    { MM_MESSAGE_ERROR_NO_CNMA_ACK_EXPECTED,   "nocnmaackexpected",     "No CNMA acknowledgement expected" },
    { MM_MESSAGE_ERROR_UNKNOWN,                "unknown",               "Unknown" }
};

GError *
mm_message_error_for_code (MMMessageError code)
{
    guint i;

    /* Look for the code */
    for (i = 0; i < G_N_ELEMENTS (msg_errors); i++) {
        if (msg_errors[i].code == code)
            return g_error_new_literal (MM_MESSAGE_ERROR,
                                        code,
                                        msg_errors[i].message);
    }

    /* Not found? Then, default */
    mm_dbg ("Invalid message error code: %u", (guint)code);
    return g_error_new (MM_MESSAGE_ERROR,
                        MM_MESSAGE_ERROR_UNKNOWN,
                        "Unknown error");
}

GError *
mm_message_error_for_string (const gchar *str)
{
    MMMessageError code = MM_MESSAGE_ERROR_UNKNOWN;
    const gchar *msg = NULL;
    gchar *buf;
    guint i;
    guint j;

    g_return_val_if_fail (str != NULL, NULL);

    /* Normalize the error code by stripping whitespace and odd characters */
    buf = g_strdup (str);
    for (i = 0, j = 0; str[i]; i++) {
        if (isalnum (str[i]))
            buf[j++] = tolower (str[i]);
    }
    buf[j] = '\0';

    /* Look for the string */
    for (i = 0; i < G_N_ELEMENTS (msg_errors); i++) {
        if (g_str_equal (msg_errors[i].error, buf)) {
            code = msg_errors[i].code;
            msg = msg_errors[i].message;
            break;
        }
    }

    /* Not found? Then, default */
    if (!msg) {
        mm_dbg ("Invalid message error string: '%s' (%s)", str, buf);
        code = MM_MESSAGE_ERROR_UNKNOWN;
        msg = "Unknown error";
    }

    g_free (buf);
    return g_error_new_literal (MM_MESSAGE_ERROR, code, msg);
}
