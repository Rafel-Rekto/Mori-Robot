#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

//Rafel ganteng 
const char* ssid = "BAWAH_Plus";
const char* password = "Nabil123";
const char* ollama_url = "http://192.168.100.212:11434/api/generate"; // Replace with your Ollama API endpoint

//ini untuk bypass biar gak lelet
IPAddress local_IP(192, 168, 100, 200); // IP Khusus untuk ESP32 Mori
IPAddress gateway(192, 168, 100, 1);    // Alamat Router rumah (biasanya diakhiri angka 1)
IPAddress subnet(255, 255, 255, 0);     // Subnet Mask standar
IPAddress primaryDNS(8, 8, 8, 8);       // DNS Google (Opsional tapi bagus agar koneksi stabil)

unsigned long lastBlink = 0;

void tanyaQwen(String prompt);

void setup() {
  Serial.begin(115200);

  //siapkanLayar(); // Inisialisasi layar TFT
  //drawIdle();
  // siapkanMic(); // Panggil fungsi mic
  // Serial.println("Mic Mori SIAP! Buka Serial Plotter sekarang.");
  siapkanSpeaker(); // Nyalakan mesin speaker
  Serial.println("Speaker Mori SIAP! Bersiaplah mendengar bunyi...");
  
  WiFi.mode(WIFI_STA);     // Memastikan ESP32 bertindak sebagai Client (bukan pemancar)
  WiFi.disconnect(true);   // Memaksa ESP32 melupakan sisa ingatan Wi-Fi yang nyangkut
  delay(1000);

  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS)) {
    Serial.println("[Error] Gagal mengatur Static IP!");
  }
  // --------------------------

  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi");
  Serial.print("IP Address ESP32: ");
  Serial.println(WiFi.localIP());
  Serial.println("Silahkan tanya, Mori siap membantu...");

}

void loop() {

  // if (millis() - lastBlink > 5000) {

  //   blink();

  //   lastBlink = millis();

  //   drawIdle();
  // }
  tesBunyiBeep();
  
  Serial.println("Beep selesai. Menunggu 3 detik...");
  delay(3000); // Jeda 3 detik sebelum bunyi lagi
  tesVisualSuara();

  if (Serial.available() > 0) {

    String userInput = Serial.readStringUntil('\n');

    userInput.trim();

    if (userInput.length() > 0) {

      Serial.println("\nKamu: " + userInput);

      tanyaQwen(userInput);
    }
  }
}

// 3. FUNGSI UTAMA MENEMBAK API
void tanyaQwen(String prompt) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(ollama_url);
    http.addHeader("Content-Type", "application/json");
    
    // Konfigurasi Timeout (Penting!)
    http.setTimeout(20000); 

    // Merakit paket JSON menggunakan ArduinoJson v7
    JsonDocument doc;
    doc["model"] = "mori";
    doc["prompt"] = prompt;
    doc["stream"] = false;

    String requestBody;
    serializeJson(doc, requestBody);

    Serial.println("[Sistem] Sebentar yah... Mori sedang berpikir...");
    animateThinking();

    // Menembak server dengan HTTP POST
    int httpResponseCode = http.POST(requestBody);

    // 4. MEMBEDAH JAWABAN
    if (httpResponseCode == 200) {
      String response = http.getString();
      
      JsonDocument responseDoc;
      DeserializationError error = deserializeJson(responseDoc, response);

      if (!error) {
        String reply = responseDoc["response"].as<String>();
        drawHappy();
        delay(400);
        animateTalking();
        Serial.println("Mori: " + reply);
        drawIdle();
        
      } else {
        Serial.print("[Error] Gagal membedah JSON: ");
        drawConfused();
        delay(1000);
        drawIdle();
        Serial.println(error.c_str());
      }
    } else {
      drawConfused();
      Serial.print("[Error] HTTP Request gagal. Kode: ");
      delay(1000);
      drawIdle();
      Serial.println(httpResponseCode);
      if(httpResponseCode == -11) {
         Serial.println("[Info] Timeout! Laptop butuh waktu lebih lama dari batas waktu ESP32.");
      }
    }
    
    // Selalu tutup koneksi agar memori tidak bocor
    http.end(); 
  } else {
    drawSleeping();
    Serial.println("[Error] Koneksi WiFi terputus.");
    delay(1000);
    drawIdle();
  }
}

