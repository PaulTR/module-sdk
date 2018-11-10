#include "packetizer.h"
#include <stdlib.h>
#include <uart.h>

typedef struct {
    char* label;
    uint8_t len;
    DataType type;
} json_field_t;

struct Packetizer {
    // pointer to array of fields (which are char * themselves)
    json_field_t* fields;
    uint8_t num;
};

static SerializationMode mode = SER_MODE_UNINITIALIZED;
static struct Packetizer packetizer = { .fields =  NULL, .num = 0};


PacketizerStatus packetizer_init(SerializationMode mode_set) {
    // free the memory used if it exists
    // deinit call most likely which goes thru the struct frees it all
    mode = mode_set;

    if(mode != SER_MODE_JSON){
        printf("Only JSON mode is currently supported by packetizer!");
        while(1) {};
    }

    return PACKETIZER_OK;
}
PacketizerStatus packetizer_add_sensor(packet_sensor_t* sensor, const char * label, DataType data_type){
    // allocate for the label
    uint8_t len = strlen(label);
    char * new_label = malloc(len+1);
    memcpy(new_label, label, len+1);


    // allocate a new array for the json_fields
    json_field_t* new_array = malloc(sizeof(json_field_t) * ++packetizer.num);

    for(uint8_t i=0; i < packetizer.num; i++){
        if (i == packetizer.num-1){
            new_array[i].label = new_label;
            new_array[i].len = len;
            new_array[i].type = data_type;
            //printf("%u\r\n", new_array[i].type );
            sensor->index = i;
            sensor->type = data_type;
        } else {
            // copy over the old fields to the new block
             memcpy((new_array + i), &packetizer.fields[i], sizeof(json_field_t));
        }
    }
    packetizer.fields = new_array;
    return PACKETIZER_OK;
};

typedef struct  {
    char * data;
    int len;
} datapoint_t;

static datapoint_t* current_packet;

PacketStatus packet_add_data(packet_sensor_t* sensor, void* data) {
    PacketStatus ret = PACKET_OK;
    // if there is no active packet being assembled,
    // allocate one
    if(current_packet == NULL){
        current_packet = calloc(1, sizeof(datapoint_t) * packetizer.num);
    }

    // if there is already stuff here, free it
    if( current_packet[sensor->index].data !=NULL){
        free( current_packet[sensor->index].data );
        ret = PACKET_VALUE_OVERWRITTEN;
    }

    char * label = packetizer.fields[sensor->index].label;
    char * value;
    int bytes_required;
    switch(sensor->type){
        case UINT:
            bytes_required = 3 + packetizer.fields[sensor->index].len + 11 + 1;
            value = malloc(bytes_required*2);
            snprintf(value, bytes_required, "\"%s\":%u", label, *((uint*)data));
            if(value!=NULL){
                current_packet[sensor->index].data = value;
                current_packet[sensor->index].len = bytes_required-2;
            }
            break;
        case INT:
            bytes_required = 3 + packetizer.fields[sensor->index].len + 12 + 1;
            value = malloc(bytes_required*2);
            snprintf(value, bytes_required, "\"%s\":%i", label, *((int*)data));
            if(value!=NULL){
                current_packet[sensor->index].data = value;
                current_packet[sensor->index].len = bytes_required-2;
            }
            break;
        case FLOAT:
            break;
        case DOUBLE:
            break;
    }
    return ret;
};

void packetizer_debug(void){
     for(uint8_t i=0; i < packetizer.num; i++){
        uart_writestr(0, packetizer.fields[i].label);
        //uart_write(0, packetizer.fields[i].label, packetizer.fields[i].len);
        uart_writestr(0, "\r\n");
    }
};

packet_t* packet_assemble(void){
    int count = 0;
    char prefix[] = "payload:";
    int prefix_len = strlen(prefix);
    int packet_length = prefix_len;

    for(uint8_t i=0; i<packetizer.num; i++){
        if(current_packet[i].data!=NULL){
            packet_length += current_packet[i].len + 1; //length plus comma
        }
        printf("%s\r\n", current_packet[i].data);
    }
    char * payload = malloc(packet_length+1);
    memcpy(payload, &prefix, prefix_len);
    count += prefix_len;
    
    for(uint8_t i=0; i<packetizer.num; i++){
        memcpy(payload + count, current_packet[i].data, current_packet[i].len);
        count += current_packet[i].len +1 ;
        payload[count-1] =',';
    }

    *(payload+count)='\0';

    packet_t* packet = malloc(sizeof(packet_t));

    packet->data = (unsigned char*) payload;
    packet->len = count;

    return (packet_t*) packet;
};