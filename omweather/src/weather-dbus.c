/* vim: set sw=4 ts=4 et: */
/*
 * This file is part of Other Maemo Weather(omweather)
 *
 * Copyright (C) 2006-2009 Vlad Vasiliev
 * Copyright (C) 2006-2009 Pavel Fialko
 * 	for the code
 *        
 * Copyright (C) 2008 Andrew Zhilin
 *		      az@pocketpcrussia.com 
 *	for default icon set (Glance)
 *
 * This software is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
  * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
*/
/*******************************************************************************/
#include "weather-common.h"
#include "weather-dbus.h"
/*******************************************************************************/
#define GCONF_KEY_CURRENT_CONNECTIVITY  "/system/osso/connectivity/IAP/current"
#define MCE_MATCH_RULE "type='signal',interface='" MCE_SIGNAL_IF \
                        "',member='" MCE_DEVICE_ORIENTATION_SIG "'"
/*******************************************************************************/
#ifdef USE_CONIC
    #include <conic/conic.h>
    #define USER_DATA_MAGIC 0xaadcaadc
#endif
/*******************************************************************************/
#if defined OS2009 || defined OS2008
    #include <mce/dbus-names.h>
    #include <mce/mode-names.h>
    #include "weather-portrait.h"
#endif
/*******************************************************************************/
void
weather_initialize_dbus(void){
    gchar   *tmp;
#if defined USE_DBUS && !defined OS2008 && !defined OS2009 
    gchar       *filter_string;
    DBusError   error;
#endif
#if defined USE_DBUS && !defined OS2008
    DBusError error;
#endif
    GConfClient *gconf_client = NULL;
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
    if(!app->dbus_is_initialize){
        /* Reseting values */
        app->iap_connecting = FALSE;
        app->iap_connected = FALSE;
        app->iap_connecting_timer = 0;
        check_current_connection();
#ifdef USE_CONIC
        app->connection = con_ic_connection_new();
        if(app->connection != NULL){
            g_object_set(app->connection, "automatic-connection-events",
                         TRUE, NULL);
            g_signal_connect(G_OBJECT(app->connection), "connection-event",
                             G_CALLBACK(connection_cb),
                             GINT_TO_POINTER(USER_DATA_MAGIC));
        }
#else
#ifndef NONMAEMO
        osso_iap_cb(iap_callback);
#endif
#endif

#ifdef USE_DBUS
        /* Add D-BUS signal handler for 'status_changed' */
        app->dbus_conn = dbus_bus_get(DBUS_BUS_SYSTEM, NULL);

#if !defined OS2008 && !defined OS2009 && !defined NONMAEMO
        filter_string =
            g_strdup_printf("interface=%s", ICD_DBUS_INTERFACE);
        /* add match */
        dbus_error_init (&error);
        dbus_bus_add_match(app->dbus_conn, filter_string, &error);
        if (dbus_error_is_set(&error)){
             fprintf(stderr,"dbus_bus_add_match failed: %s", error.message);
             dbus_error_free(&error);
        }
        g_free(filter_string);
        /* add the callback */
        dbus_connection_add_filter(app->dbus_conn,
                                   get_connection_status_signal_cb,
                                   NULL, NULL);

#endif

#if defined OS2009 && defined APPLICATION
        dbus_error_init (&error);
        dbus_bus_add_match(app->dbus_conn, MCE_MATCH_RULE, &error);
        if(dbus_error_is_set(&error)){
             fprintf(stderr,"dbus_bus_add_match failed: %s", error.message);
             dbus_error_free(&error);
        }
        if(!dbus_connection_add_filter(app->dbus_conn,
                                      get_mce_signal_cb, NULL, NULL)){
             fprintf(stderr,"Error dbus_connection_add_filter failed\n");
        }
#endif

#endif
/* For Debug on i386 */
#ifdef DEBUGCONNECT
        app->iap_connected = TRUE;
#endif
#if defined(NONMAEMO)
        app->iap_connected = TRUE;
#endif
        app->dbus_is_initialize = TRUE;
    }
#ifdef DEBUGFUNCTIONCALL
    END_FUNCTION;
#endif

}
/*******************************************************************************/
void
weather_deinitialize_dbus(void){
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
#ifdef USE_CONIC
  if(app->connection){
      g_object_unref(app->connection);
  }
#endif
    if (app->dbus_conn){
#if !defined OS2008 && !defined OS2009 && !defined NONMAEMO
         dbus_bus_remove_match(app->dbus_conn, ICD_DBUS_INTERFACE, NULL);
         dbus_connection_remove_filter(app->dbus_conn,
             get_connection_status_signal_cb);
#endif
         dbus_connection_close(app->dbus_conn);
         dbus_connection_unref(app->dbus_conn);
    }
#ifdef DEBUGFUNCTIONCALL
    END_FUNCTION;
#endif

}
/*******************************************************************************/
#if !defined OS2008 && !defined OS2009 && !defined NONMAEMO
static DBusHandlerResult
get_connection_status_signal_cb(DBusConnection *connection, DBusMessage *message,
                                                                void *user_data){

    gchar   *iap_name = NULL,
            *iap_nw_type = NULL,
            *iap_state = NULL;
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
    /* check signal */
    if(!dbus_message_is_signal(message, ICD_DBUS_INTERFACE, ICD_STATUS_CHANGED_SIG))
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

    if(!dbus_message_get_args(message, NULL, DBUS_TYPE_STRING, &iap_name,
                                               DBUS_TYPE_STRING, &iap_nw_type,
                                               DBUS_TYPE_STRING, &iap_state,
                                               DBUS_TYPE_INVALID))
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
#ifndef RELEASE
    fprintf(stderr,">>>>>>>>>>>>>>>>>>>>>OMWeather - iap_state = %s\n", iap_state);
#endif
    if(!strcmp(iap_state, "CONNECTED")){
        if(!app->iap_connected){
            app->iap_connected = TRUE;
            app->iap_connecting = FALSE;
            app->iap_connecting_timer = 0;
            if(app->config->downloading_after_connecting)
                add_current_time_event();
        }
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }
    if(!strcmp(iap_state, "CONNECTING")){
        app->iap_connected = FALSE;
        app->iap_connecting = TRUE;
        app->iap_connecting_timer = 0;
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }
    app->iap_connected = FALSE;     /* !!!!!!!!! Need Remove download */
    app->iap_connecting = FALSE;
    app->iap_connecting_timer = 0;
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}
#endif
/*******************************************************************************/
#ifdef USE_CONIC
void
connection_cb(ConIcConnection *connection, ConIcConnectionEvent *event,
                                                            gpointer user_data){
    gchar                   *iap_id,
                            *bearer;
    ConIcConnectionStatus   status;
    ConIcConnectionError    error;
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
    status = con_ic_connection_event_get_status(event);
    bearer = (gchar*)con_ic_event_get_bearer_type(CON_IC_EVENT(event));
    switch(status){
#ifdef OS2009
        case CON_IC_STATUS_NETWORK_UP:
            app->iap_connected = FALSE;
            app->iap_connecting = TRUE;
            app->iap_connecting_timer = 0;
            DEBUG_FUNCTION("\n>>>>>>>>>>>>>>>>>Connecting\n");
        break;
#endif
        case CON_IC_STATUS_CONNECTED:
            app->iap_connecting = FALSE;
            app->iap_connecting_timer = 0;
            DEBUG_FUNCTION("\n>>>>>>>>>>>>>>>>>Connected\n");
            if((bearer && !strncmp(bearer,"WLAN", 4) && app->config->update_wlan) ||
                (bearer && !strncmp(bearer,"DUN_GSM", 7) && app->config->update_gsm)||
                (bearer && !strncmp(bearer,"GPRS", 4) && app->config->update_gsm))
                app->iap_connected = TRUE;
            else
                app->iap_connected = FALSE;
            if((app->config->downloading_after_connecting) &&
              ((bearer && !strncmp(bearer,"WLAN", 4) && app->config->update_wlan) ||
              (bearer && !strncmp(bearer,"DUN_GSM", 7) && app->config->update_gsm) ||
              (bearer && !strncmp(bearer,"GPRS", 4) && app->config->update_gsm)))
                add_current_time_event();
        break;
        case CON_IC_STATUS_DISCONNECTED:
            DEBUG_FUNCTION("\n>>>>>>>>>>>>>>>>>Disconnected\n");
            error = con_ic_connection_event_get_error(event);
            switch(error){
                case CON_IC_CONNECTION_ERROR_NONE:
                    DEBUG_FUNCTION("Libconic thinks there was nothing wrong.");
                break;
                case CON_IC_CONNECTION_ERROR_INVALID_IAP:
                    DEBUG_FUNCTION("Invalid (non-existing?) IAP was requested.");
                break;
                case CON_IC_CONNECTION_ERROR_CONNECTION_FAILED:
                    DEBUG_FUNCTION("Connection just failed.");
                break;
                case CON_IC_CONNECTION_ERROR_USER_CANCELED:
                    DEBUG_FUNCTION("User canceled the connection attempt");
                break;
            }
            app->phase = ZERO_PHASE;
            app->iap_connected = FALSE;
            app->iap_connecting = FALSE;
            app->iap_connecting_timer = 0;
        break;
        case CON_IC_STATUS_DISCONNECTING:
            app->iap_connected = FALSE;
            app->iap_connecting = FALSE;
            app->iap_connecting_timer = 0;
            DEBUG_FUNCTION("\n>>>>>>>>>>>>>>>>>Disconnecting\n");
        break;
        default:
            app->iap_connected = FALSE;
            app->iap_connecting = FALSE;
            app->phase = ZERO_PHASE;
            DEBUG_FUNCTION("\n>>>>>>>>>>>>>>>>>Default\n");
        break;
    }
}
#else
/*******************************************************************************/
#ifndef NONMAEMO
void
iap_callback(struct iap_event_t *event, void *arg){
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
    app->iap_connecting = FALSE;
    switch(event->type){
        case OSSO_IAP_CONNECTED:
            app->iap_connected = TRUE;
            app->iap_connecting = FALSE;
            if(app->config->downloading_after_connecting)
                add_current_time_event();
        break;
        case OSSO_IAP_DISCONNECTED:
            app->iap_connected = FALSE;
            app->iap_connecting = FALSE;
        break;
        default:
        case OSSO_IAP_ERROR:
            app->iap_connected = FALSE;
            app->iap_connecting = FALSE;
        break;
    }
}
#endif
#endif
/*******************************************************************************/
void
check_current_connection(void){
    gchar *tmp = NULL;
    gchar *gconf_path = NULL;
    gchar *type_of_connection = NULL;
    GConfClient *gconf_client = NULL;
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
    /* Check current connection */
    gconf_client = gconf_client_get_default();
    if (gconf_client) {
        tmp = gconf_client_get_string(gconf_client,
                                      GCONF_KEY_CURRENT_CONNECTIVITY,
                                      NULL);
        if (tmp) {
            gconf_path = g_strdup_printf("/system/osso/connectivity/IAP/%s/type", tmp);
            type_of_connection = gconf_client_get_string(gconf_client,
                                      gconf_path,
                                      NULL);
            if ((type_of_connection && !strncmp(type_of_connection, "WLAN", 4) && app->config->update_wlan) ||
                (type_of_connection && !strncmp(type_of_connection, "DUN_GSM", 7) && app->config->update_gsm))
                app->iap_connected = TRUE;
            else
                app->iap_connected = FALSE;
            if (gconf_path)
                g_free(gconf_path);
            if (type_of_connection);
                g_free(type_of_connection);
            g_free(tmp);
        } else
            app->iap_connected = FALSE;
        gconf_client_clear_cache(gconf_client);
        g_object_unref(gconf_client);
    }
}
/*******************************************************************************/
