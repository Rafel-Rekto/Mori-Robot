#include <driver/i2s.h>

// Definisi Pin Mic INMP441 yang sudah kita sepakati
#define MIC_WS 14
#define MIC_SD 32
#define MIC_SCK 27
#define I2S_MIC_PORT I2S_NUM_0

#define AMP_BCLK 26
#define AMP_LRC 25
#define AMP_DIN 22
#define I2S_SPEAKER_PORT I2S_NUM_1 // port I2S ke-2 agar tidak bentrok dengan Mic

// Fungsi untuk menyalakan telinga Mori
void siapkanMic() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX), // Mode Menerima (RX)
    .sample_rate = 16000, // Kualitas standar suara (16kHz)
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = MIC_SCK,
    .ws_io_num = MIC_WS,
    .data_out_num = I2S_PIN_NO_CHANGE, // Kita tidak mengirim suara ke mic
    .data_in_num = MIC_SD
  };

  // Terapkan konfigurasi ke ESP32
  i2s_driver_install(I2S_MIC_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_MIC_PORT, &pin_config);
}

// Fungsi untuk membaca suara dan mencetaknya ke grafik
void tesVisualSuara() {
  int32_t sample = 0;
  size_t bytes_read;
  
  // Baca data suara dari I2S
  i2s_read(I2S_MIC_PORT, &sample, sizeof(int32_t), &bytes_read, portMAX_DELAY);
  
  if (bytes_read > 0) {
    // Geser bit agar grafiknya tidak terlalu besar/kecil di layar
    Serial.println(sample >> 14); 
  }
}

// Fungsi untuk menyalakan Amplifier
void siapkanSpeaker() {
  i2s_config_t i2s_speaker_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX), // Mode Mengirim (TX)
    .sample_rate = 16000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // Standar audio 16-bit
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
  };

  i2s_pin_config_t speaker_pin_config = {
    .bck_io_num = AMP_BCLK,
    .ws_io_num = AMP_LRC,
    .data_out_num = AMP_DIN,
    .data_in_num = I2S_PIN_NO_CHANGE // Kita tidak menerima suara dari amp
  };

  i2s_driver_install(I2S_SPEAKER_PORT, &i2s_speaker_config, 0, NULL);
  i2s_set_pin(I2S_SPEAKER_PORT, &speaker_pin_config);
}

// Fungsi membuat nada Beep buatan (Gelombang Sinus)
void tesBunyiBeep() {
  int sample_rate = 16000;
  float frekuensi = 440.0; // Nada A4 (seperti garpu tala)
  float volume = 2000.0;   // Jangan di-set terlalu tinggi dulu agar telingamu aman!
  size_t bytes_written;

  // Membunyikan nada selama 1 detik (16000 sampel)
  for (int i = 0; i < sample_rate; i++) {
    // Rumus matematika gelombang suara
    int16_t sample = (int16_t)(volume * sin(2.0 * PI * frekuensi * i / sample_rate));
    
    // Kemas menjadi 32-bit (kiri dan kanan) agar bisa dikirim ke amplifier
    uint32_t sample32 = (sample << 16) | (sample & 0xffff); 
    
    i2s_write(I2S_SPEAKER_PORT, &sample32, sizeof(sample32), &bytes_written, portMAX_DELAY);
  }
}