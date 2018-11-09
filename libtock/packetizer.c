#include "packetizer.h"

typedef struct {
    char* label;
    uint8_t len;
} json_field_t;

struct Packetizer {
    // pointer to array of fields (which are char * themselves)
    json_field_t* fields;
    uint8_t num;
};

static SerializationMode mode = SER_MODE_UNINITIALIZED;
static struct Packetizer packetizer = { .fields =  NULL, .num = 0};


PacketizerStatus packetizer_init(SerializationMode mode) {
    // free the memory used if it exists
    // deinit call most likely which goes thru the struct frees it all

    if(mode != SER_MODE_JSON){
        printf("Only JSON mode is currently supported by packetizer!");
        while(1) {};
    }
}
PacketizerStatus packetizer_add_sensor(packet_sensor_t* sensor, const char * label, DataType data_type){
    // allocate for the label
    uint8_t len = strlen(label);
    char * new_label = malloc(len);
    memcpy(new_label, label, len);


    // allocate a new array for the json_fields
    json_field_t* new_array = malloc(sizeof(json_field_t) * ++packetizer.num);

    for(uint8_t i=0; i < packetizer.num; i++){
        if (i == packetizer.num-1){
            // 
            new_array[i].label = new_label;
            new_array[i].len = len;
        } else {
            // copy over the old fields to the new block
             memcpy((new_array + i), &packetizer.fields[i], sizeof(json_field_t));
        }
    }
    packetizer.fields = new_array;
};

PacketStatus packet_add_data(packet_sensor_t* sensor, void* data) {};

int packetizer_debug(void){
     for(uint8_t i=0; i < packetizer.num; i++){

        uart_write(0, packetizer.fields[i].label, packetizer.fields[i].len);
    }
};