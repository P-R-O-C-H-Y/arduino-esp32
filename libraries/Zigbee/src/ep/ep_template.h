/* Class of Zigbee On/Off Light endpoint inherited from common EP class */

#pragma once

#include "Zigbee_ep.h"
#include "ha/esp_zigbee_ha_standard.h"

class ZigbeeDevice : public Zigbee_EP {
  public:
    ZigbeeDevice(uint8_t endpoint);
    ~ZigbeeDevice();

    void find_endpoint(esp_zb_zdo_match_desc_req_param_t *cmd_req);
};