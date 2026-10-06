#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_

/* 1. Ép sử dụng RPL Classic theo yêu cầu đề tài */
#define UIP_CONF_ROUTER                 1
#define ROUTING_CONF_RPL_CLASSIC        1

/* 2. Hỗ trợ cả 2 OF (Lõi Contiki-NG đã có sẵn khai báo extern) */
#define RPL_CONF_SUPPORTED_OFS          {&rpl_of0}

/* 3. OF mặc định ban đầu: Chạy OF0 trước */
#define RPL_CONF_OF                     rpl_of0
#define RPL_CONF_OF_OCP                 0

/* 4. Kích hoạt đo năng lượng ngầm cho Người 3 */
#define ENERGEST_CONF_ON                1

/* 5. Chu kỳ gửi gói tin */
#define SEND_INTERVAL                   (10 * CLOCK_SECOND)

/* 6. Tối ưu log để console chạy mượt */
#define LOG_CONF_LEVEL_RPL              LOG_LEVEL_INFO
#define LOG_CONF_LEVEL_TCPIP            LOG_LEVEL_WARN
#define LOG_CONF_LEVEL_IPV6             LOG_LEVEL_WARN

#endif /* PROJECT_CONF_H_ */
