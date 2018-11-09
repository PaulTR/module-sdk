#include <packetizer.h>

packet_sensor_t pressure;
packet_sensor_t temperature;

int main(void){

    packetizer_init(SER_MODE_JSON);
    packetizer_add_sensor(&pressure, "pressure", UINT32_T);

    packetizer_debug();

    packetizer_add_sensor(&temperature, "temperature", FLOAT);

    while(1){
      uint32_t pressure_data = 1231249343;
      float temperature_data = 23.43;
      packet_add_data(&pressure, &pressure_data);
      packet_add_data(&temperature, &temperature_data);

      packet_t * packet = packet_assemble();
      (void) packet;
      //packet_debug(&packet); // prints out the packet in human readable format
      //helium_send(&packet);  
    }

}