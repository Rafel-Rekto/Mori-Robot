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

enum EkspresiMori {
  MORI_DIAM,
  MORI_LIRIK_KANAN,
  MORI_LIRIK_KIRI,
  MORI_BERPIKIR,
  MORI_TIDUR,
  MORI_BINGUNG,
  MORI_BAHAGIA
};

void siapkanLayar();
void updateWajahMori();
void setEkspresi(EkspresiMori ekspresiBaru);
void tanyaQwen(String prompt);

void setup() {
  Serial.begin(115200);

  siapkanLayar(); // Panggil fungsi untuk menyiapkan layar TFT
  
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

  updateWajahMori();
  
  // 2. MENDENGARKAN INPUT DARI SERIAL MONITOR
  if (Serial.available() > 0) {
    String userInput = Serial.readStringUntil('\n');
    userInput.trim(); // Membersihkan spasi atau karakter enter tak terlihat

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
    setEkspresi(MORI_BERPIKIR);

    // Menembak server dengan HTTP POST
    int httpResponseCode = http.POST(requestBody);

    // 4. MEMBEDAH JAWABAN
    if (httpResponseCode == 200) {
      String response = http.getString();
      
      JsonDocument responseDoc;
      DeserializationError error = deserializeJson(responseDoc, response);

      if (!error) {
        String reply = responseDoc["response"].as<String>();
        Serial.println("Mori: " + reply);
        setEkspresi(MORI_BAHAGIA);
      } else {
        Serial.print("[Error] Gagal membedah JSON: ");
        Serial.println(error.c_str());
      }
    } else {
      Serial.print("[Error] HTTP Request gagal. Kode: ");
      Serial.println(httpResponseCode);
      if(httpResponseCode == -11) {
         Serial.println("[Info] Timeout! Laptop butuh waktu lebih lama dari batas waktu ESP32.");
      }
    }
    
    // Selalu tutup koneksi agar memori tidak bocor
    http.end(); 
  } else {
    Serial.println("[Error] Koneksi WiFi terputus.");
  }
}

