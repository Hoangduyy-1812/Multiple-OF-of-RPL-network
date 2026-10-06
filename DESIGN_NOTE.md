# TECHNICAL DESIGN NOTE: RPL OF SWITCHING MECHANISM
**Phase 1 Handover (Người 1 -> Người 2)**
**Môn học / Đề tài:** Nghiên cứu chuyển đổi OF0 & MRHOF trên cùng một node (RPL-Classic)
**Thời gian bàn giao:** 05/10/2026

---

### 1. Kiến trúc mạng & Topology phân biệt (DGRM)
* **Mô hình:** Topology hình thoi 5 node (file `rpl_switch_dgrm.csc`).
  * `Node 1`: Root / DAG Sink (Địa chỉ `fe80::201:1:1:1`).
  * `Node 2`: Intermediate Parent nhánh trái (1 hop tới Sink).
  * `Node 3 & Node 4`: Intermediate Parents nhánh phải (Node 4 cách Sink 2 hop qua Node 3).
  * `Node 5`: Testing Sender (Đáy hình thoi, gửi gói tin chu kỳ 10s về Node 1).
* **Ma trận kênh DGRM (Asymmetric Channel Matrix):**
  * `Node 2 -> Node 1`: RX Ratio = **50.0%** (Link rớt gói nặng để kích hoạt cơ chế phạt của MRHOF).
  * `Node 1 -> Node 2`: RX Ratio = **100.0%** (Nhận DIO ổn định).
  * Tất cả các liên kết còn lại giữa các cặp node: RX Ratio = **100.0%**.

---

### 2. Dữ liệu Baseline đã xác minh (Ground Truth)
Hai kịch bản chạy độc lập đã được thực nghiệm và lưu log tại thư mục `logs/`:
1. **Baseline OF0 (`logs/baseline_of0.log`):**
   * *Đặc tính:* Chuẩn Hop Count thuần túy (đã cấu hình `RPL_OF0_FIXED_SR = 1` và chuẩn hóa `parent_path_cost` trong `rpl-of0.c`).
   * *Hành vi:* Node 5 kiên quyết giữ **`PRNT=2`**, **`RANK=768`**, **`PCHG=1`** suốt toàn bộ thời gian chạy, phớt lờ việc link `2 -> 1` rớt 50%.
2. **Baseline MRHOF (`logs/baseline_mrhof.log`):**
   * *Đặc tính:* Động thái né link xấu theo ETX.
   * *Hành vi:* Node 2 bị đo đạc ETX phạt Rank tăng vọt lên 1280 – 2048. Tại mốc ~02:10, Node 5 tự động chuyển sang đi đường vòng qua **`PRNT=4`**, Rank ổn định ở 1024, **`PCHG=2`**.

---

### 3. Quy chuẩn cấu trúc Log hệ thống
Toàn bộ log được chuẩn hóa theo định dạng parse tự động (chu kỳ 10 giây):
```text
[NODE_STAT] TS=<ms> | ID=<id> | OF=<0/1> | PRNT=<parent_id> | RANK=<rank> | PCHG=<count> | CPU=<tick> | LPM=<tick> | TX=<tick> | RX=<tick>
[DATA_SEND] TS=<ms> | ID=<id> | SEQ=<seq>
[DATA_RECV] TS=<ms> | SRC=<id> | SEQ=<seq> | DELAY_MS=<delay>
