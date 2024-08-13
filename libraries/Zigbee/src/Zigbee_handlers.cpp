/* Zigbee Common Functions */
#include "Zigbee_core.h"
#include "Arduino.h"

// forward declaration of all implemented handlers
static esp_err_t zb_attribute_set_handler(const esp_zb_zcl_set_attr_value_message_t *message);
static esp_err_t zb_attribute_reporting_handler(const esp_zb_zcl_report_attr_message_t *message);
static esp_err_t zb_cmd_read_attr_resp_handler(const esp_zb_zcl_cmd_read_attr_resp_message_t *message);
static esp_err_t zb_cmd_default_resp_handler(esp_zb_zcl_cmd_default_resp_message_t *message);

// Zigbee action handlers
static esp_err_t zb_action_handler(esp_zb_core_action_callback_id_t callback_id, const void *message) {
  esp_err_t ret = ESP_OK;
  /* TODO: 
    Implement handlers for different Zigbee actions (callback_id's) here and call the respective EP's method
  */
  switch (callback_id) {
    case ESP_ZB_CORE_SET_ATTR_VALUE_CB_ID:          ret = zb_attribute_set_handler((esp_zb_zcl_set_attr_value_message_t *)message); break;
    case ESP_ZB_CORE_REPORT_ATTR_CB_ID:             ret = zb_attribute_reporting_handler((esp_zb_zcl_report_attr_message_t *)message); break;
    case ESP_ZB_CORE_CMD_READ_ATTR_RESP_CB_ID:      ret = zb_cmd_read_attr_resp_handler((esp_zb_zcl_cmd_read_attr_resp_message_t *)message); break;
    // case ESP_ZB_CORE_CMD_REPORT_CONFIG_RESP_CB_ID:  ret = zb_configure_report_resp_handler((esp_zb_zcl_cmd_config_report_resp_message_t *)message); break;
    case ESP_ZB_CORE_CMD_DEFAULT_RESP_CB_ID:        ret = zb_cmd_default_resp_handler((esp_zb_zcl_cmd_default_resp_message_t *)message); break;
    default:                                 log_w("Receive unhandled Zigbee action(0x%x) callback", callback_id); break;
  }
  return ret;
}

static esp_err_t zb_attribute_set_handler(const esp_zb_zcl_set_attr_value_message_t *message) {
  if (!message) {
    log_e("Empty message");
  }
  if (message->info.status != ESP_ZB_ZCL_STATUS_SUCCESS) {
    log_e("Received message: error status(%d)", message->info.status);
  }

  log_v(
    "Received message: endpoint(%d), cluster(0x%x), attribute(0x%x), data size(%d)", message->info.dst_endpoint, message->info.cluster, message->attribute.id,
    message->attribute.data.size
  );

  // List through all Zigbee EPs and call the callback function, with the message
  for (std::list<Zigbee_EP*>::iterator it = Zigbee.ep_objects.begin(); it != Zigbee.ep_objects.end(); ++it) {
    if (message->info.dst_endpoint == (*it)->_endpoint) {
      (*it)->attribute_set(message); //method zb_attribute_set_handler in the EP
    }
  }
  return ESP_OK;
}

static esp_err_t zb_attribute_reporting_handler(const esp_zb_zcl_report_attr_message_t *message) {
  if (!message) {
    log_e("Empty message");
  }
  if (message->status != ESP_ZB_ZCL_STATUS_SUCCESS) {
    log_e("Received message: error status(%d)", message->status);
  }
  log_v(
    "Received report from address(0x%x) src endpoint(%d) to dst endpoint(%d) cluster(0x%x)", message->src_address.u.short_addr, message->src_endpoint,
    message->dst_endpoint, message->cluster
  );
    // List through all Zigbee EPs and call the callback function, with the message
  for (std::list<Zigbee_EP*>::iterator it = Zigbee.ep_objects.begin(); it != Zigbee.ep_objects.end(); ++it) {
    if (message->dst_endpoint == (*it)->_endpoint) {
      //TODO: implement attribute reporting handler
    }
  }
  return ESP_OK;
}

static esp_err_t zb_cmd_read_attr_resp_handler(const esp_zb_zcl_cmd_read_attr_resp_message_t *message) {
  if (!message) {
    log_e("Empty message");
  }
  if (message->info.status != ESP_ZB_ZCL_STATUS_SUCCESS) {
    log_e("Received message: error status(%d)", message->info.status);
  }
  log_v(
    "Read attribute response: from address(0x%x) src endpoint(%d) to dst endpoint(%d) cluster(0x%x)", message->info.src_address.u.short_addr,
    message->info.src_endpoint, message->info.dst_endpoint, message->info.cluster
  );

  for (std::list<Zigbee_EP*>::iterator it = Zigbee.ep_objects.begin(); it != Zigbee.ep_objects.end(); ++it) {
    if (message->info.dst_endpoint == (*it)->_endpoint) {
      esp_zb_zcl_read_attr_resp_variable_t *variable = message->variables;
      while (variable) {
        log_v(
          "Read attribute response: status(%d), cluster(0x%x), attribute(0x%x), type(0x%x), value(%d)", variable->status, message->info.cluster,
          variable->attribute.id, variable->attribute.data.type, variable->attribute.data.value ? *(uint8_t *)variable->attribute.data.value : 0
        );
        if (variable->status == ESP_ZB_ZCL_STATUS_SUCCESS) {
          //esp_app_zb_attribute_handler(message->info.cluster, &variable->attribute);
          (*it)->attribute_read_cmd(message->info.cluster, &variable->attribute);
        }
        variable = variable->next;
      }
    }
  }
  return ESP_OK;
}

static esp_err_t zb_cmd_default_resp_handler(esp_zb_zcl_cmd_default_resp_message_t *message) {
  if (!message) {
    log_e("Empty message");
  }
  if (message->info.status != ESP_ZB_ZCL_STATUS_SUCCESS) {
    log_e("Received message: error status(%d)", message->info.status);
  }
  log_v(
    "Received default response: from address(0x%x), src_endpoint(%d) to dst_endpoint(%d), cluster(0x%x) with status 0x%x", message->info.src_address.u.short_addr,
    message->info.src_endpoint, message->info.dst_endpoint, message->info.cluster, message->status_code
  );
  return ESP_OK;
}
