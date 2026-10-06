#include "contiki.h"
#include "net/routing/routing.h"
#include "net/ipv6/simple-udp.h"
#include "sys/log.h"

#define UDP_PORT 8765
static struct simple_udp_connection udp_conn;

typedef struct {
  uint32_t seq_no;
  uint32_t send_timestamp;
  uint16_t sender_id;
} app_packet_t;

PROCESS(udp_sink_process, "UDP Sink Process");
AUTOSTART_PROCESSES(&udp_sink_process);

static void udp_rx_callback(struct simple_udp_connection *c,
                            const uip_ipaddr_t *sender_addr,
                            uint16_t sender_port,
                            const uip_ipaddr_t *receiver_addr,
                            uint16_t receiver_port,
                            const uint8_t *data,
                            uint16_t datalen)
{
  if(datalen == sizeof(app_packet_t)) {
    app_packet_t *pkt = (app_packet_t *)data;
    uint32_t recv_time = (uint32_t)clock_time();
    uint32_t delay_ticks = (recv_time >= pkt->send_timestamp) ? (recv_time - pkt->send_timestamp) : 0;
    uint32_t delay_ms = (delay_ticks * 1000) / CLOCK_SECOND;

    /* Log chuẩn hóa: Sink nhận gói */
    printf("[DATA_RECV] TS=%lu | SRC=%u | SEQ=%lu | DELAY_MS=%lu\n",
           (unsigned long)recv_time, pkt->sender_id, (unsigned long)pkt->seq_no, (unsigned long)delay_ms);
  }
}

PROCESS_THREAD(udp_sink_process, ev, data)
{
  PROCESS_BEGIN();
  NETSTACK_ROUTING.root_start();
  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_rx_callback);
  PROCESS_END();
}
