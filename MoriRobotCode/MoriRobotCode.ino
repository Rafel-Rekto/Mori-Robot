#include <WiFi.h>
#include <WiFiUdp.h> // Tambahan untuk menerima sinyal saraf motorik

const char* ssid = "BAWAH_Plus";
const char* password = "Nabil123";

// IP Address Statis ESP32
IPAddress local_IP(192, 168, 100, 200);
IPAddress gateway(192, 168, 100, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);

// Buka port khusus untuk menerima status dari Python
WiFiUDP udpState;
const int statePort = 5006; 

unsigned long lastBlink = 0;
int currentState = 0; // 0=Idle, 1=Mendengar, 2=Berpikir, 3=Bicara

void setup() {
  Serial.begin(115200);

  siapkanLayar();
  drawIdle();
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);
  delay(1000);

  WiFi.config(local_IP, gateway, subnet, primaryDNS);
  WiFi.begin(ssid, password);
  
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi");
  Serial.print("IP Address ESP32: ");
  Serial.println(WiFi.localIP());

  siapkanMic(); 
  
  // Mulai dengarkan sinyal status dari Python
  udpState.begin(statePort); 
  Serial.println("[Sistem] Telinga & Saraf Motorik Mori Aktif!");
}

void loop() {
  // 1. CEK SINYAL STATUS DARI PYTHON
  int packetSize = udpState.parsePacket();
  if (packetSize) {
    char incomingPacket[10]; // Buffer kecil karena hanya menerima angka 0-3
    int len = udpState.read(incomingPacket, 10);
    if (len > 0) incomingPacket[len] = 0;
    
    String command = String(incomingPacket);
    command.trim();
    int newState = command.toInt();
    
    // Hanya ubah dan gambar ulang wajah jika statusnya berbeda (mencegah kedip berulang)
    if (newState != currentState) {
      currentState = newState;
      lastBlink = millis(); // Reset timer kedip
      
      // Gambar ekspresi awal sesuai status baru
      if (currentState == 0) drawIdle();
      else if (currentState == 1) drawListening(); // Mata membesar (Tanpa Delay)
      else if (currentState == 2) drawThinkingEyes(); // Mata menyipit
    }
  }

  // 2. JALANKAN ANIMASI LOOPING (Jika diperlukan)
  if (currentState == 0) {
    // Mode Idle: Kedip setiap 5 detik
    if (millis() - lastBlink > 5000) {
      blink();
      lastBlink = millis();
      drawIdle();
    }
  } else if (currentState == 2) {
     // Mode Berpikir: Animasi titik (Python sedang memproses Ollama, jadi aman pakai delay)
     animateThinking(); 
  } else if (currentState == 3) {
     // Mode Bicara: Animasi mulut/mata bergerak (Python sedang memutar suara)
     animateTalking();
  }

  // 3. SELALU KIRIM SUARA KE LAPTOP
  kirimSuaraUDP();
}