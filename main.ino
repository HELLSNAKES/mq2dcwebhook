#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

// Thông tin WiFi
const char* ssid = "";
const char* password = "";

// Webhook để gửi cảnh báo
const char* discordWebhookHost = "discord.com";
const int discordWebhookPort = 443;
const char* discordWebhookPath = "";///api/webhooks/

// URL của icon trong server Discord
const char* footerIconUrl = "https://cdn.discordapp.com/emojis/1308611656355545179.webp?size=48&name=emoji_21";

// ----  Cấu hình Supabase ----
const char* supabaseHost = "";  // Thay bằng host Supabase của bạn
const int supabasePort = 443;
const char* supabaseApiKey = "";              // Thay bằng API key của bạn
const char* supabasePath = "/rest/v1/gasdata";                     // Thay bằng đường dẫn đến bảng dữ liệu của bạn
// ----------------------------------------------

#define MQ2_PIN A0
#define GAS_THRESHOLD 600

ESP8266WebServer server(80);

void setup() {
    Serial.begin(115200);
    Serial.println("\n🚀 ESP8266 Starting...");

    WiFi.begin(ssid, password);
    Serial.print("🔌 Connecting to WiFi ");
    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        if (millis() - startAttemptTime > 20000) {
            Serial.println("\n⛔ WiFi Connection Failed! Restarting...");
            ESP.restart();
        }
    }
    Serial.println("\n✅ WiFi Connected!");
    Serial.print("🌍 IP Address: ");
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html", getHtmlUI());
    });
    
    server.on("/data", HTTP_GET, []() {
        int gasValue = analogRead(MQ2_PIN);
        StaticJsonDocument<200> jsonDoc;
        jsonDoc["gas_level"] = gasValue;
        String jsonResponse;
        serializeJson(jsonDoc, jsonResponse);
        server.send(200, "application/json", jsonResponse);
    });
    
    server.begin();
    Serial.println("🌐 Web server started!");
}

void loop() {
    server.handleClient();
    delay(2000);
    int gasValue = analogRead(MQ2_PIN);
    Serial.print("Giá trị khí gas đo được: ");
    Serial.println(gasValue);
    
    // ---- Gửi dữ liệu lên Supabase ----
    sendToSupabase(gasValue);
    // ---------------------------------------------------

    if (gasValue > GAS_THRESHOLD) {
        sendToDiscord(gasValue);
    }
}

void sendToDiscord(int gasValue) {
    WiFiClientSecure client;
    client.setInsecure();

    if (!client.connect(discordWebhookHost, discordWebhookPort)) {
        Serial.println("❌ Kết nối đến Discord thất bại!");
        return;
    }

    String payload = "{\"embeds\":[{\"title\":\"🚨 Cảnh báo khí gas vượt ngưỡng an toàn! 🚨\",\"description\":\"Nồng độ khí gas cao: " + String(gasValue) + "\",\"color\":16711680,\"footer\":{\"text\":\"By HELLSNAKE",\"icon_url\":\"" + String(footerIconUrl) + "\"}}]}";
    String request = String("POST ") + discordWebhookPath + " HTTP/1.1\r\n" +
                     "Host: " + discordWebhookHost + "\r\n" +
                     "Content-Type: application/json\r\n" +
                     "Content-Length: " + payload.length() + "\r\n" +
                     "Connection: close\r\n\r\n" +
                     payload;

    client.print(request);
    while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") {
            break;
        }
    }
    Serial.println("✅ Phản hồi từ Discord: " + client.readString());
    client.stop();
}

// ----  Hàm gửi dữ liệu lên Supabase ----
void sendToSupabase(int gasValue) {
    WiFiClientSecure client;
    client.setInsecure();

    if (!client.connect(supabaseHost, supabasePort)) {
        Serial.println("❌ Kết nối đến Supabase thất bại!");
        return;
    }
    
    // payload JSON chứa giá trị khí gas
    String payload = "{\"gas_value\":" + String(gasValue) + "}";
    
    String request = String("POST ") + supabasePath + " HTTP/1.1\r\n" +
                     "Host: " + supabaseHost + "\r\n" +
                     "Content-Type: application/json\r\n" +
                     "apikey: " + supabaseApiKey + "\r\n" +
                     "Authorization: Bearer " + supabaseApiKey + "\r\n" +
                     "Content-Length: " + payload.length() + "\r\n" +
                     "Connection: close\r\n\r\n" +
                     payload;
    
    client.print(request);
    while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") {
            break;
        }
    }
    Serial.println("✅ Phản hồi từ Supabase: " + client.readString());
    client.stop();
}

