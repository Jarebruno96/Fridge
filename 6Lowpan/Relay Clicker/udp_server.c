#include <stdio.h>
#include <contiki.h>
#include <contiki-net.h>
#include "letmecreate/core/network.h"
#include "letmecreate/click/relay2.h"
#include "dev/leds.h"
#include <pic32_gpio.h>
#include "letmecreate/core/common.h"
// Used for PRINT6ADDR function
#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"

#include "letmecreate/core/debug.h"

#define SERVER_PORT 5005
#define CLIENT_PORT 5006
#define SERVER_IP_ADDR "fe80:0000:0000:0000:28e9:3285:421c:bc82"
#define BUFFER_SIZE 4096

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
        static char network_data[BUFFER_SIZE];
        static int length;
        static struct uip_ip_hdr metadata;
        static struct etimer timer;

        PRINTF("===START===\n");

        // Making our IP address constant to match the other example
        if(ipv6_add_address(SERVER_IP_ADDR, NULL, 0) < 0)
        {
            PRINTF("Failed to set IPV6 address\n");
            return 1;
        }

        // Bind a new connection, needs to be called on both sides
        conn = udp_new_connection(SERVER_PORT, CLIENT_PORT, NULL);

        relay2_click_set_relay_state(MIKROBUS_1, RELAY2_CLICK_RELAY_1, 0);
        relay2_click_set_relay_state(MIKROBUS_1, RELAY2_CLICK_RELAY_2, 0);

        while(1)
        {
            PRINTF("Waiting for data...\n");

            PROCESS_WAIT_UDP_RECEIVED();
            length = udp_packet_receive(network_data, BUFFER_SIZE, &metadata);

            if(strcmp(network_data,"EL")){
                relay2_click_set_relay_state(MIKROBUS_1, RELAY2_CLICK_RELAY_2, 1);
            }else if(strcmp(network_data,"AL")){
                relay2_click_set_relay_state(MIKROBUS_1, RELAY2_CLICK_RELAY_2, 0);
            }else if(strcmp(network_data,"EA")){
                relay2_click_set_relay_state(MIKROBUS_1, RELAY2_CLICK_RELAY_1, 1);
            }else if(strcmp(network_data,"AA")){
                relay2_click_set_relay_state(MIKROBUS_1, RELAY2_CLICK_RELAY_1, 0);
            }


        }

    }

    PROCESS_END();
}

/*---------------------------------------------------------------------------*/
