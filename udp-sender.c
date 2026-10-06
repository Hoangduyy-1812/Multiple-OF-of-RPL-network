#include "contiki.h"
#include <stdio.h>
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "sys/energest.h"
#include "sys/log.h"

#if ROUTING_CONF_RPL_CLASSIC
#include "net/routing/rpl-classic/rpl.h"
#include "net/routing/rpl-classic/rpl-private.h"
#endif

#define LOG_MODULE "AppSender"
#define LOG_LEVEL LOG_LEVEL_INFO
#define UDP_PORT 8765

static struct simple_udp_connection udp_conn;
static uint32_t seq_id = 0;
static uint16_t parent_change_count = 0;
static uip_ipaddr_t last_parent_ip;

typedef struct {
  uint32_t seq_no;
  uint32_t send_timestamp;
  uint16_t sender_id;
} app_packet_t;

PROCESS(udp_sender_process, "UDP Sender Process");
AUTOSTART_PROCESSES(&udp_sender_process);

PROCESS_THREAD(udp_sender_process, ev, data)
{
  static struct etimer periodic_timer;
  uip_ipaddr_t dest_ip;

  PROCESS_BEGIN();

  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, NULL);
  etimer_set(&periodic_timer, SEND_INTERVAL);

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
    etimer_reset(&periodic_timer);
    /* Sử dụng đúng tên hàm get_root_ipaddr của Contiki-NG */
    if(NETSTACK_ROUTING.node_is_reachable() && NETSTACK_ROUTING.get_root_ipaddr(&dest_ip)) {
      rpl_instance_t *instance = rpl_get_default_instance();
      uint16_t current_rank = 0;
      uint8_t current_ocp = 0;
      uint16_t parent_id = 0;

      if(instance != NULL && instance->current_dag != NULL) {
        current_rank = instance->current_dag->rank;
        if(instance->of != NULL) {
          current_ocp = instance->of->ocp;
        }
        if(instance->current_dag->preferred_parent != NULL) {
          uip_ipaddr_t *p_ip = rpl_parent_get_ipaddr(instance->current_dag->preferred_parent);
          if(p_ip != NULL) {
            parent_id = p_ip->u8[15];
            if(!uip_ipaddr_cmp(&last_parent_ip, p_ip)) {
              parent_change_count++;
              uip_ipaddr_copy(&last_parent_ip, p_ip);
            }
          }
        }
      }

      energest_flush();
      uint32_t cpu_ticks = (uint32_t)energest_type_time(ENERGEST_TYPE_CPU);
      uint32_t lpm_ticks = (uint32_t)energest_type_time(ENERGEST_TYPE_LPM);
      uint32_t tx_ticks  = (uint32_t)energest_type_time(ENERGEST_TYPE_TRANSMIT);
      uint32_t rx_ticks  = (uint32_t)energest_type_time(ENERGEST_TYPE_LISTEN);

      printf("[NODE_STAT] TS=%lu | ID=%u | OF=%u | PRNT=%u | RANK=%u | PCHG=%u | CPU=%lu | LPM=%lu | TX=%lu | RX=%lu\n",
             (unsigned long)clock_time(), (unsigned int)linkaddr_node_addr.u8[LINKADDR_SIZE - 1],
             current_ocp, parent_id, current_rank, parent_change_count,
             (unsigned long)cpu_ticks, (unsigned long)lpm_ticks, (unsigned long)tx_ticks, (unsigned long)rx_ticks);

      app_packet_t pkt;
      pkt.seq_no = ++seq_id;
      pkt.send_timestamp = (uint32_t)clock_time();
      pkt.sender_id = linkaddr_node_addr.u8[LINKADDR_SIZE - 1];

      printf("[DATA_SEND] TS=%lu | ID=%u | SEQ=%lu\n",
             (unsigned long)clock_time(), (unsigned int)pkt.sender_id, (unsigned long)pkt.seq_no);

      simple_udp_sendto(&udp_conn, &pkt, sizeof(pkt), &dest_ip);
    }
  }

  PROCESS_END();
}
