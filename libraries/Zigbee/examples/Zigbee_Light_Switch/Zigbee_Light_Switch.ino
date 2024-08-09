// Copyright 2023 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @brief This example demonstrates simple Zigbee light switch.
 *
 * The example demonstrates how to use ESP Zigbee stack to control a light bulb.
 * The light bulb is a Zigbee end device, which is controlled by a Zigbee coordinator.
 * Button switch and Zigbee runs in separate tasks.
 *
 * Proper Zigbee mode must be selected in Tools->Zigbee mode
 * and also the correct partition scheme must be selected in Tools->Partition Scheme.
 *
 * Please check the README.md for instructions and more detailed description.
 */

#ifndef ZIGBEE_MODE_ZCZR
#error "Zigbee coordinator mode is not selected in Tools->Zigbee mode"
#endif

#include "Zigbee_core.h"
#include "ep/ep_on_off_switch.h"

#include "esp_zigbee_core.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ha/esp_zigbee_ha_standard.h"

#define SWITCH_ENDPOINT_NUMBER 5

/* Switch configuration */
#define GPIO_INPUT_IO_TOGGLE_SWITCH GPIO_NUM_9
#define PAIR_SIZE(TYPE_STR_PAIR)    (sizeof(TYPE_STR_PAIR) / sizeof(TYPE_STR_PAIR[0]))

typedef enum {
  SWITCH_ON_CONTROL,
  SWITCH_OFF_CONTROL,
  SWITCH_ONOFF_TOGGLE_CONTROL,
  SWITCH_LEVEL_UP_CONTROL,
  SWITCH_LEVEL_DOWN_CONTROL,
  SWITCH_LEVEL_CYCLE_CONTROL,
  SWITCH_COLOR_CONTROL,
} switch_func_t;

typedef struct {
  uint8_t pin;
  switch_func_t func;
} switch_func_pair_t;

typedef enum {
  SWITCH_IDLE,
  SWITCH_PRESS_ARMED,
  SWITCH_PRESS_DETECTED,
  SWITCH_PRESSED,
  SWITCH_RELEASE_DETECTED,
} switch_state_t;

static switch_func_pair_t button_func_pair[] = {{GPIO_INPUT_IO_TOGGLE_SWITCH, SWITCH_ONOFF_TOGGLE_CONTROL}};

/********************* Zigbee functions **************************/
static void esp_zb_buttons_handler(switch_func_pair_t *button_func_pair) {
  if (button_func_pair->func == SWITCH_ONOFF_TOGGLE_CONTROL) {
    /* implemented light switch toggle functionality */
    esp_zb_zcl_on_off_cmd_t cmd_req;
    cmd_req.zcl_basic_cmd.src_endpoint = SWITCH_ENDPOINT_NUMBER;
    cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_DST_ADDR_ENDP_NOT_PRESENT;
    cmd_req.on_off_cmd_id = ESP_ZB_ZCL_CMD_ON_OFF_TOGGLE_ID;
    log_i("Send 'on_off toggle' command");
    esp_zb_zcl_on_off_cmd_req(&cmd_req);
  }
}

/********************* GPIO functions **************************/
static QueueHandle_t gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void *arg) {
  xQueueSendFromISR(gpio_evt_queue, (switch_func_pair_t *)arg, NULL);
}

static void switch_gpios_intr_enabled(bool enabled) {
  for (int i = 0; i < PAIR_SIZE(button_func_pair); ++i) {
    if (enabled) {
      enableInterrupt((button_func_pair[i]).pin);
    } else {
      disableInterrupt((button_func_pair[i]).pin);
    }
  }
}

ZigbeeSwitch zbSwitch = ZigbeeSwitch(SWITCH_ENDPOINT_NUMBER);

/********************* Arduino functions **************************/
void setup() {
  
  Serial.begin(115200);

  //Add endpoint to Zigbee Core
  log_d("Adding ZigbeeSwitch endpoint to Zigbee Core");
  Zigbee.addEndpoint(&zbSwitch);

  //Open network for 180 seconds after boot
  Zigbee.setRebootOpenNetwork(180);
  
  // Init button switch
  for (int i = 0; i < PAIR_SIZE(button_func_pair); i++) {
    pinMode(button_func_pair[i].pin, INPUT_PULLUP);
    /* create a queue to handle gpio event from isr */
    gpio_evt_queue = xQueueCreate(10, sizeof(switch_func_pair_t));
    if (gpio_evt_queue == 0) {
      log_e("Queue was not created and must not be used");
      while (1);
    }
    attachInterruptArg(button_func_pair[i].pin, gpio_isr_handler, (void *)(button_func_pair + i), FALLING);
  }

  // When all EPs are registered, start Zigbee with ZIGBEE_COORDINATOR mode
  log_d("Calling Zigbee.begin()");
  Zigbee.begin(ZIGBEE_COORDINATOR);
  
  Serial.println("Waiting for Light to bound to the switch");
  //Wait for switch to bound to a light:
  while(!zbSwitch.is_bound()) 
  {
    Serial.printf(".");
    delay(500);
  }
  Serial.println();
}

void loop() {
  // Handle button switch in loop()
  uint8_t pin = 0;
  switch_func_pair_t button_func_pair;
  static switch_state_t switch_state = SWITCH_IDLE;
  bool evt_flag = false;
  

  /* check if there is any queue received, if yes read out the button_func_pair */
  if (xQueueReceive(gpio_evt_queue, &button_func_pair, portMAX_DELAY)) {
    pin = button_func_pair.pin;
    switch_gpios_intr_enabled(false);
    evt_flag = true;
  }
  while (evt_flag) {
    bool value = digitalRead(pin);
    switch (switch_state) {
      case SWITCH_IDLE:           switch_state = (value == LOW) ? SWITCH_PRESS_DETECTED : SWITCH_IDLE; break;
      case SWITCH_PRESS_DETECTED: switch_state = (value == LOW) ? SWITCH_PRESS_DETECTED : SWITCH_RELEASE_DETECTED; break;
      case SWITCH_RELEASE_DETECTED:
        switch_state = SWITCH_IDLE;
        /* callback to button_handler */
        (*esp_zb_buttons_handler)(&button_func_pair);
        break;
      default: break;
    }
    if (switch_state == SWITCH_IDLE) {
      switch_gpios_intr_enabled(true);
      evt_flag = false;
      break;
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
