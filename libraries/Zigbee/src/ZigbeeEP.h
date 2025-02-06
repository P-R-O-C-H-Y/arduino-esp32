/* Common Class for Zigbee End point */

#pragma once

#include "ZigbeeCore.h"
#if SOC_IEEE802154_SUPPORTED && CONFIG_ZB_ENABLED

#include <Arduino.h>

/* Useful defines */
#define ZB_CMD_TIMEOUT 10000  // 10 seconds
#define OTA_UPGRADE_QUERY_INTERVAL (1 * 60) // 1 minutes

#define ZB_ARRAY_LENTH(arr) (sizeof(arr) / sizeof(arr[0]))
#define XYZ_TO_RGB(X, Y, Z, r, g, b)                                \
  {                                                                 \
    r = (float)(3.240479 * (X) - 1.537150 * (Y) - 0.498535 * (Z));  \
    g = (float)(-0.969256 * (X) + 1.875992 * (Y) + 0.041556 * (Z)); \
    b = (float)(0.055648 * (X) - 0.204043 * (Y) + 1.057311 * (Z));  \
    if (r > 1) {                                                    \
      r = 1;                                                        \
    }                                                               \
    if (g > 1) {                                                    \
      g = 1;                                                        \
    }                                                               \
    if (b > 1) {                                                    \
      b = 1;                                                        \
    }                                                               \
  }

#define RGB_TO_XYZ(r, g, b, X, Y, Z)                               \
  {                                                                \
    X = (float)(0.412453 * (r) + 0.357580 * (g) + 0.180423 * (b)); \
    Y = (float)(0.212671 * (r) + 0.715160 * (g) + 0.072169 * (b)); \
    Z = (float)(0.019334 * (r) + 0.119193 * (g) + 0.950227 * (b)); \
  }

typedef struct zbstring_s {
  uint8_t len;
  char data[];
} ESP_ZB_PACKED_STRUCT zbstring_t;

typedef struct zb_device_params_s {
  esp_zb_ieee_addr_t ieee_addr;
  uint8_t endpoint;
  uint16_t short_addr;
} zb_device_params_t;

typedef enum {
  ZB_POWER_SOURCE_UNKNOWN = 0x00,
  ZB_POWER_SOURCE_MAINS = 0x01,
  ZB_POWER_SOURCE_BATTERY = 0x03,
} zb_power_source_t;

/* Zigbee End Device Class */
class ZigbeeEP {
public:
  ZigbeeEP(uint8_t endpoint = 10);
  ~ZigbeeEP() {}

  // Set ep config and cluster list
  void setEpConfig(esp_zb_endpoint_config_t ep_config, esp_zb_cluster_list_t *cluster_list) {
    _ep_config = ep_config;
    _cluster_list = cluster_list;
  }

  void setVersion(uint8_t version);
  uint8_t getEndpoint() {
    return _endpoint;
  }

  void printBoundDevices();
  void printBoundDevices(Print &print);

  std::list<zb_device_params_t *> getBoundDevices() const {
    return _bound_devices;
  }

  static bool bound() {
    return _is_bound;
  }
  static void allowMultipleBinding(bool bind) {
    _allow_multiple_binding = bind;
  }

  // Set Manufacturer name and model
  void setManufacturerAndModel(const char *name, const char *model);

  // Methods to read manufacturer and model name from selected endpoint and short address
  char *readManufacturer(uint8_t endpoint, uint16_t short_addr, esp_zb_ieee_addr_t ieee_addr);
  char *readModel(uint8_t endpoint, uint16_t short_addr, esp_zb_ieee_addr_t ieee_addr);

  // Set Power source and battery percentage for battery powered devices
  void setPowerSource(zb_power_source_t power_source, uint8_t percentage = 255);
  void setBatteryPercentage(uint8_t percentage);
  void reportBatteryPercentage();

  // Set time
  void addTimeCluster(tm time = {0}, int32_t gmt_offset = 0);  // gmt offset in seconds
  void setTime(tm time);
  void setTimezone(int32_t gmt_offset);

  // Get time from Coordinator or specific endpoint (blocking until response)
  struct tm getTime(uint8_t endpoint = 1, int32_t short_addr = 0x0000, esp_zb_ieee_addr_t ieee_addr = {0});
  int32_t getTimezone(uint8_t endpoint = 1, int32_t short_addr = 0x0000, esp_zb_ieee_addr_t ieee_addr = {0});  // gmt offset in seconds

  bool epAllowMultipleBinding() {
    return _allow_multiple_binding;
  }

  void addOTAClient(uint32_t file_version = 0x01010101, uint16_t manufacturer = 0x1001, uint16_t image_type = 0x1011, 
    uint32_t downloaded_file_ver = 0x01010101, uint16_t hw_version = 0x0101, uint8_t max_data_size = 223);
  void requestOTAUpdate();

  // findEndpoind may be implemented by EPs to find and bind devices
  virtual void findEndpoint(esp_zb_zdo_match_desc_req_param_t *cmd_req) {};

  //list of all handlers function calls, to be override by EPs implementation
  virtual void zbAttributeSet(const esp_zb_zcl_set_attr_value_message_t *message) {};
  virtual void zbAttributeRead(uint16_t cluster_id, const esp_zb_zcl_attribute_t *attribute) {};
  virtual void zbReadBasicCluster(const esp_zb_zcl_attribute_t *attribute);  //already implemented
  virtual void zbIdentify(const esp_zb_zcl_set_attr_value_message_t *message);
  virtual void zbWindowCoveringMovementCmd(const esp_zb_zcl_window_covering_movement_message_t *message) {};
  virtual void zbReadTimeCluster(const esp_zb_zcl_attribute_t *attribute);  //already implemented
  virtual void zbIASZoneStatusChangeNotification(const esp_zb_zcl_ias_zone_status_change_notification_message_t *message) {};
  virtual void zbIASZoneEnrollResponse(const esp_zb_zcl_ias_zone_enroll_response_message_t *message) {};

  virtual void addBoundDevice(zb_device_params_t *device) {
    _bound_devices.push_back(device);
    _is_bound = true;
  }

  void onIdentify(void (*callback)(uint16_t)) {
    _on_identify = callback;
  }

private:
  char *_read_manufacturer;
  char *_read_model;
  void (*_on_identify)(uint16_t time);
  time_t _read_time;
  int32_t _read_timezone;

protected:
  uint8_t _endpoint;
  esp_zb_ha_standard_devices_t _device_id;
  esp_zb_endpoint_config_t _ep_config;
  esp_zb_cluster_list_t *_cluster_list;
  static bool _is_bound;
  static bool _allow_multiple_binding;
  std::list<zb_device_params_t *> _bound_devices;
  SemaphoreHandle_t lock;
  zb_power_source_t _power_source;

  friend class ZigbeeCore;
};

#endif  //SOC_IEEE802154_SUPPORTED && CONFIG_ZB_ENABLED
