# 👾 Phần cứng

### 🛠 Linh kiện sử dụng
- **ESP8266 (NodeMCU)**
- **Cảm biến khí gas MQ-2**
- **Nguồn cấp 5V**
- **Dây cắm**
- **Broad cắm**

### 🔌 Sơ đồ nối dây
| MQ-2 Pin | ESP8266 Pin |
|----------|------------|
| VCC      | 3.3V       |
| GND      | GND        |
| A0       | A0         |
| D0       | D2 (GPIO4) |

---

## 📜 Lưu đồ phần cứng
```plaintext
Bắt đầu
  │
  ├──> Cấp nguồn cho ESP8266 và cảm biến MQ-2
  │
  ├──> ESP8266 kết nối tới Wifi
  │       │
  │       ├──> Kết nối thành công? 
  │       │       ├──> Có →gửi ip port server và tiếp tục
  │       │       └──> Không → Thử lại hoặc khởi động lại
  │
  ├──> MQ-2 đọc giá trị khí gas (Analog từ A0)
  │
  ├──> Gửi giá trị đến ESP8266 để xử lý
  │
  ├──> Hiển thị dữ liệu lên Web UI (trang HTML)
  │
  ├──> Nếu giá trị vượt ngưỡng → Gửi cảnh báo lên Discord
  │
  └──> Lặp lại quá trình đo mỗi 2 giây
