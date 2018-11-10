#include <packetizer.h>

packet_sensor_t pressure;
packet_sensor_t temperature;

int main(void){

    packetizer_init(SER_MODE_JSON);
    packetizer_add_sensor(&pressure, "pressure", UINT);
    packetizer_add_sensor(&temperature, "temperature", INT);
    packetizer_debug();

    uint32_t pressure_data = 1231249343;
    int32_t temperature_data = 2343;
    packet_add_data(&pressure, &pressure_data);
    packet_add_data(&temperature, &temperature_data);

    packet_t * packet = packet_assemble();
    (void) packet;
}
