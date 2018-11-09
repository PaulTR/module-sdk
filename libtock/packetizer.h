
#pragma once

#include "tock.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SER_MODE_UNINITIALIZED,
    SER_MODE_JSON
} SerializationMode;

typedef enum {
    PACKETIZER_OK = 0,
    PACKETIZER_NO_MEMORY
} PacketizerStatus;

typedef enum {
    UINT8_T,
    UINT16_T,
    UINT32_T,
    INT8_T,
    INT16_T,
    INT32_T,
    FLOAT,
    DOUBLE
} DataType;

typedef struct {
    char * label;
    uint8_t index;
    DataType type;
} packet_sensor_t;

typedef enum {
    PACKET_OK = 0,
    PACKET_NO_MEMORY,
    PACKET_VALUE_OVERWRITTEN,
} PacketStatus;

PacketizerStatus packetizer_init(SerializationMode mode);
PacketizerStatus packetizer_add_sensor(packet_sensor_t* sensor, const char * label, DataType data_type);

PacketStatus packet_add_data(packet_sensor_t* sensor, void* data);

typedef struct{
    uint8_t * data;
    uint32_t len;
}packet_t ;

packet_t* packet_assemble();

int packetizer_debug(void);

#ifdef __cplusplus
}
#endif
