#include <driver/i2s.h>
#include <WiFiUdp.h>

// Definisi Pin Mic INMP441
#define MIC_WS 14
#define MIC_SD 32
#define MIC_SCK 27
#define I2S_MIC_PORT I2S_NUM_0

// Konfigurasi Target UDP (Laptopmu)
WiFiUDP udpAudio;
const char* udpAddress = "192.168.100.212"; // IP Laptopmu
const int udpPort = 5005;                   // Port yang dibuka oleh Python

void siapkanMic() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 16000, // Kecepatan sampel 16kHz
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT, // Baca mentah di 32-bit
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 256, // Buffer kecil agar latensi super rendah
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = MIC_SCK,
    .ws_io_num = MIC_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = MIC_SD
  };

  i2s_driver_install(I2S_MIC_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_MIC_PORT, &pin_config);
}

// Fungsi utama penembak suara ke jaringan
void kirimSuaraUDP() {
  int32_t sample_buffer[256]; // Wadah data I2S 32-bit
  int16_t send_buffer[256];   // Wadah data 16-bit untuk dikirim ke Whisper
  size_t bytes_read;
  
  // Ambil suara dari mic
  i2s_read(I2S_MIC_PORT, sample_buffer, sizeof(sample_buffer), &bytes_read, portMAX_DELAY);
  
  int samples_read = bytes_read / 4; // 1 sample 32-bit = 4 byte
  
  // Konversi dari 32-bit ke 16-bit (syarat mutlak AI Whisper)
  for (int i = 0; i < samples_read; i++) {
     send_buffer[i] = sample_buffer[i] >> 14; 
  }
  
  // Tembakkan ke laptop via UDP
  if (samples_read > 0) {
    udpAudio.beginPacket(udpAddress, udpPort);
    udpAudio.write((uint8_t*)send_buffer, samples_read * 2);
    udpAudio.endPacket();
  }
}