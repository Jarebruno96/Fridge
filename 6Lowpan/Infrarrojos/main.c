#include <stdio.h>
#include <contiki.h>
#include <contiki-net.h>
#include "sys/clock.h"
#include "dev/leds.h"
#include "letmecreate/core/network.h"
#include "letmecreate/core/debug.h"
#include "letmecreate/core/common.h"
#include "adc2.h"
#include <pic32_gpio.h>
#include <letmecreate/core/spi.h>
#include <math.h>

#define SERVER_IP_ADDR "fe80::19:f5ff:fe89:1fac"

#define SERVER_PORT 3000
#define CLIENT_PORT 3001

#define BUFFER_SIZE 64

PROCESS(main_process, "Main process");
AUTOSTART_PROCESSES(&main_process);


/*---------------------------------------------------------------------------*/
PROCESS_THREAD(main_process, ev, data)
{

  PROCESS_BEGIN();
  INIT_NETWORK_DEBUG();
  {
        // Due to the way Contiki protothreads work this needs to be static,
        // otherwise the data will be lost when switching to a different thread
    static struct uip_udp_conn * conn;
    static char buffer[BUFFER_SIZE];

    PRINTF("===START===\n");

    conn = udp_new_connection(CLIENT_PORT, SERVER_PORT, SERVER_IP_ADDR);

    spi_init();
    adc_enable();
    while(1)
    {
      float value;
      adc_get_measure(&value);
      

      int major = (int)value;
      int minor = (int)((value - (float)major) * 100.0f);
      //float v=(value/1023.0)*3.3;
      //float dist = 16.2537 * pow(v,4) - 129.893 * pow(v,3) + 382.268 * pow(v,2) - 512.611 * v + 301.439;

      float dist = (1.0 / (value / 13.15)) - 0.35;
      value = 12.08*pow(value, -1.058);



      sprintf(buffer,"%i.%02i , %f, %f ", major, minor, value, dist);
      udp_packet_send(conn, buffer, strlen(buffer));
      PROCESS_WAIT_UDP_SENT();


      static struct etimer timer;
      etimer_set(&timer, 2 * CLOCK_SECOND);
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));

    }
    adc_disable();
    spi_release();

  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
