# 👾 Phần cứng

## 🛠 Linh kiện sử dụng

- **ESP8266 (NodeMCU)**
- **Cảm biến khí gas MQ-2**
- **Nguồn cấp 5V**
- **Dây cắm**
- **Broad cắm**

## 🔌 Sơ đồ nối dây

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
  │       │       ├──> Có → gửi IP, port server và tiếp tục
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
```

## 💾 Lưu đồ phần mềm (Firmware ESP8266)

```plaintext
Bắt đầu
  │
  ├──> Khởi tạo Serial Monitor (115200 baud)
  │
  ├──> Kết nối WiFi
  │       │
  │       ├──> Thành công?  
  │       │       ├──> Có → Hiển thị địa chỉ IP, port server và tiếp tục
  │       │       └──> Không → Khởi động lại sau 20 giây
  │
  ├──> Khởi tạo Web Server trên ESP8266
  │
  ├──> Lắng nghe request từ trình duyệt
  │
  ├──> Khi nhận request từ "/data":
  │       ├──> Đọc giá trị cảm biến MQ-2 (A0)
  │       ├──> Chuyển thành JSON và gửi về trình duyệt
  │       ├──> Nếu giá trị vượt ngưỡng → Gửi cảnh báo Discord
  │
  ├──> Khi nhận request từ "/":
  │       ├──> Gửi trang HTML để hiển thị biểu đồ
  │
  ├──> Cập nhật dữ liệu mỗi 2 giây
  │
  └──> Lặp lại vòng lặp chính
```

## 🚨 Lưu đồ gửi cảnh báo Discord

```plaintext
ESP8266 nhận dữ liệu từ cảm biến
  │
  ├──> Giá trị khí gas vượt ngưỡng? (lớn hơn 200 - có thể thay đổi ở phần #define GAS_THRESHOLD)
  │       │
  │       ├──> Không → Không gửi cảnh báo
  │       │
  │       ├──> Có → Chuẩn bị dữ liệu JSON
  │               │
  │               ├──> Kết nối tới Discord Webhook
  │               │
  │               ├──> Gửi HTTP POST với nội dung cảnh báo
  │               │
  │               ├──> Nhận phản hồi từ server
  │               │       ├──> Thành công?  
  │               │       │       ├──> Có → In "Gửi thành công"
  │               │       │       ├──> Không → In lỗi lên Serial Monitor
  │
  └──> Hoàn tất, chờ lần đo tiếp theo
```

## 🐱‍🏍 Sơ đồ khối hệ thống Webhook Discord

```plaintext
  +-------------+         +--------------------+         +-----------------------+
  |  MQ-2 Gas   | ----->  |  ESP8266 (Client)  | ----->  |  Discord Webhook API  |
  |  Sensor     |         |  - Đọc giá trị MQ-2|         |  - Nhận HTTP POST     |
  |  (A0)       |         |  - Kiểm tra ngưỡng |         |  - Hiển thị cảnh báo  |
  +-------------+         |  - Gửi HTTP POST   |         |  - Gửi thông báo      |
                          +--------------------+         +-----------------------+
```

## 📩 Dữ liệu JSON gửi lên Discord

```json
{
  "embeds": [
    {
      "title": "🚨 Cảnh báo khí gas vượt ngưỡng an toàn! 🚨",
      "description": "Nồng độ khí gas cao: 250",
      "color": 16711680,
      "footer": {
        "text": "text",
        "icon_url": "icon_url"
      }
    }
  ]
}
```

## 🔗 Dữ liệu request gửi đến Discord

```
POST api/webhooks/SERVER_ID/WEBHOOK_TOKEN HTTP/1.1
Host: discord.com
Content-Type: application/json
Content-Length: 211
Connection: close

{"embeds":[{"title":"🚨 Cảnh báo khí gas vượt ngưỡng an toàn! 🚨","description":"Nồng độ khí gas cao: 250","color":16711680,"footer":{"text":"text","icon_url":"icon_url"}}]}
```

