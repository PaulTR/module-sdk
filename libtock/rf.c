
#include "rf.h"
#include "tock.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RF_DRIVER           (0xCC1352)
#define ALLOW_NUM_W             (1)
#define ALLOW_NUM_C             (2)
#define SUBSCRIBE_TX            (1)

enum cmd {
  COMMAND_DRIVER_CHECK = 0,
  COMMAND_INITIALIZE = 1,
  COMMAND_RETURN_RADIO_STATUS = 2,
  COMMAND_STOP_RADIO_OPERATION = 3,
  COMMAND_FORCE_STOP_RADIO = 4,
  COMMAND_SET_DEVICE_CONFIG = 5,
  COMMAND_SEND = 6,
  COMMAND_SET_ADDRESS = 7,
};


typedef struct {
  const char * address;
  bool on;
} device_params;

unsigned char BUF_CFG[27];

// Internal callback for transmission
static int tx_result;

static void tx_done_callback(int result,
                             __attribute__ ((unused)) int arg2,
                             __attribute__ ((unused)) int arg3,
                             void* ud) {
  tx_result     = result;
  *((bool*) ud) = true;
}

int rf_driver_check(void) {
  return command(RF_DRIVER, COMMAND_DRIVER_CHECK, 0, 0);
}

int rf_init(void) {
  return command(RF_DRIVER, COMMAND_INITIALIZE, 0, 0);
}

int rf_set_address(unsigned char *address) {
  if (!address) return TOCK_EINVAL;
  int err = allow(RF_DRIVER, ALLOW_NUM_C, (void *) address, 10);
  if (err < 0) return err;
  return command(RF_DRIVER, COMMAND_SET_ADDRESS, 0, 0);
}

int rf_send(unsigned short addr,
                caut_type_t ftype,
                const char *payload,
                unsigned char len) {
  // Setup parameters in ALLOW_CFG and ALLOW_TX
  int err = allow(RF_DRIVER, ALLOW_NUM_C, (void *) BUF_CFG, 11);
  if (err < 0) return err;
  BUF_CFG[0] = ftype;
  err        = allow(RF_DRIVER, ALLOW_NUM_W, (void *) payload, len);
  if (err < 0) return err;

  // Subscribe to the transmit callback
  bool tx_done = false;
  err = subscribe(RF_DRIVER, SUBSCRIBE_TX,
                  tx_done_callback, (void *) &tx_done);
  if (err < 0) return err;

  // Issue the send command and wait for the transmission to be done.
  err = command(RF_DRIVER, COMMAND_SEND, (unsigned int) addr, 0);
  if (err < 0) return err;
  yield_for(&tx_done);
  if (tx_result != TOCK_SUCCESS) {
    return tx_result;
  }else {
    return TOCK_SUCCESS;
  }

}
