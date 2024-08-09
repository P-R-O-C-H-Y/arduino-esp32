/* Class of Zigbee Temperature sensor endpoint inherited from common EP class */

#pragma once

#include "Zigbee_ep.h"
#include "ha/esp_zigbee_ha_standard.h"

//define the thermostat configuration to avoid narrowing conversion issue in zigbee-sdk
#define ZB_DEFAULT_THERMOSTAT_CONFIG()                                                                              \
    {                                                                                                               \
        .basic_cfg =                                                                                                \
            {                                                                                                       \
                .zcl_version = ESP_ZB_ZCL_BASIC_ZCL_VERSION_DEFAULT_VALUE,                                          \
                .power_source = ESP_ZB_ZCL_BASIC_POWER_SOURCE_DEFAULT_VALUE,                                        \
            },                                                                                                      \
        .identify_cfg =                                                                                             \
            {                                                                                                       \
                .identify_time = ESP_ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE,                                   \
            },                                                                                                      \
        .thermostat_cfg =                                                                                           \
            {                                                                                                       \
                .local_temperature = (int16_t)ESP_ZB_ZCL_THERMOSTAT_LOCAL_TEMPERATURE_DEFAULT_VALUE,                \
                .occupied_cooling_setpoint = ESP_ZB_ZCL_THERMOSTAT_OCCUPIED_COOLING_SETPOINT_DEFAULT_VALUE,         \
                .occupied_heating_setpoint = ESP_ZB_ZCL_THERMOSTAT_OCCUPIED_HEATING_SETPOINT_DEFAULT_VALUE,         \
                .control_sequence_of_operation = ESP_ZB_ZCL_THERMOSTAT_CONTROL_SEQ_OF_OPERATION_DEFAULT_VALUE,      \
                .system_mode = ESP_ZB_ZCL_THERMOSTAT_CONTROL_SYSTEM_MODE_DEFAULT_VALUE,                             \
            },                                                                                                      \
    }
class ZigbeeThermostat : public Zigbee_EP {
  public:
    ZigbeeThermostat(uint8_t endpoint);
    ~ZigbeeThermostat();

    void find_endpoint(esp_zb_zdo_match_desc_req_param_t *cmd_req);
    static void bind_cb(esp_zb_zdp_status_t zdo_status, void *user_ctx);
    static void find_cb(esp_zb_zdp_status_t zdo_status, uint16_t addr, uint8_t endpoint, void *user_ctx);

};