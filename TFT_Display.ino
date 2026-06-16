#include <Adafruit_GFX.h>    // Library grafis inti
#include <Adafruit_ST7735.h> // Library driver layar (Ubah ke ILI9341 jika layarmu tipe ILI)
#include <SPI.h>

// Definisi Pin ESP32 yang kita sepakati di tabel
#define TFT_CS     5
#define TFT_RST    4
#define TFT_DC     2

// Inisialisasi objek layar TFT
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

int mLebar = 40;
int mTinggi = 50;
int mRadius = 20;

int mKiriX = 30;
int mKananX = 100;
int mY = 35;

int mz = 0;

// Fungsi buatan sendiri yang akan dipanggil oleh file utama nanti
void siapkanLayar() {
  // Inisialisasi layar ST7735R (tipe black tab/red tab umum)
  tft.initR(INITR_BLACKTAB); 
  
  tft.setRotation(1); // Mengubah mode layar menjadi Landscape (horizontal)
  tft.fillScreen(ST7735_BLACK); // Warnai latar belakang menjadi hitam murni
  }


void gambarMata(int xKiri, int xKanan, int y, int tinggi, int warna) {
  tft.fillRoundRect(xKiri, y, mLebar, tinggi, mRadius, warna);  // Mata Kiri
  tft.fillRoundRect(xKanan, y, mLebar, tinggi, mRadius, warna); // Mata Kanan
}

void ekspresiDiam(){
  gambarMata(mKiriX, mKananX, mY, mTinggi, ST7735_CYAN);
}

void lirikKanan(){
  for(int i = 0; i < 15; i++){
    gambarMata(mKiriX, mKananX, mY, mTinggi, ST7735_BLACK);//hapus mata 
    mKananX = mKananX + 1; //geser mata kanan ke kanan
    mKiriX = mKiriX + 1; //geser mata kiri ke kanan

    gambarMata(mKiriX, mKananX, mY, mTinggi, ST7735_CYAN); //gambar mata baru
    delay(10); //tunggu sebentar sebelum geser lagi
  }
}

void lirikKiri(){
  for (int i = 0; i < 15; i++){
    gambarMata(mKiriX, mKananX, mY, mTinggi, ST7735_BLACK); //hapus mata 
    mKananX = mKananX - 1; //geser mata kanan ke kiri
    mKiriX = mKiriX - 1; //geser mata kiri ke kiri

    gambarMata(mKiriX, mKananX, mY, mTinggi, ST7735_CYAN); //gambar mata baru
    delay(10); //tunggu sebentar sebelum geser lagi

  }
}

void ekspresiBerpikir() {
  // 1. Bersihkan layar dari mata normal
  tft.fillScreen(ST7735_BLACK);

  // 2. Bajak variabel ukuran! Buat mata jadi kecil/menyipit
  mLebar = 16;  // Aslinya 24
  int tinggiMikir = 20; // Aslinya 40 (kita pakai variabel lokal karena tinggi ada di dalam kurung)

  // 3. Set titik pusat mata yang baru (Mendekat & Naik)
  int pikirKiriX = 50;  // Geser ke kanan dari aslinya (35)
  int pikirKananX = 85; // Geser ke kiri dari aslinya (101)
  int pikirY = 23;      // Naik ke atas (Aslinya 44)

  // 4. Looping lirik kanan-kiri ala orang mikir (lakukan 3 kali)
  for(int i = 0; i < 3; i++) {
    
    // -- LIRIK KANAN ATAS --
    gambarMata(pikirKiriX + 10, pikirKananX + 10, pikirY, tinggiMikir, ST7735_CYAN);
    delay(500); // Tahan gaya ini setengah detik
    // Hapus mata posisi ini
    gambarMata(pikirKiriX + 10, pikirKananX + 10, pikirY, tinggiMikir, ST7735_BLACK); 

    // -- LIRIK KIRI ATAS --
    gambarMata(pikirKiriX - 10, pikirKananX - 10, pikirY, tinggiMikir, ST7735_CYAN);
    delay(500); 
    // Hapus mata posisi ini
    gambarMata(pikirKiriX - 10, pikirKananX - 10, pikirY, tinggiMikir, ST7735_BLACK);
  }

  // 5. KEMBALIKAN KE UKURAN NORMAL SETELAH SELESAI
  mLebar = 24; 
  tft.fillScreen(ST7735_BLACK); // Bersihkan sisa animasi
  ekspresiDiam(); // Panggil mata normal lagi
}

void ekspresiTidur() {
  tft.fillScreen(ST7735_BLACK); // Bersihkan layar
  
  int yTidur = 80;    // Posisikan jauh di bawah layar (aslinya 44)
  int tinggiTidur = 6; // Buat sangat tipis (aslinya 40)
  
  // Kita gambar manual karena ini bukan bentuk mata normal
  tft.fillRoundRect(mKiriX, yTidur, mLebar, tinggiTidur, 3, ST7735_CYAN);
  tft.fillRoundRect(mKananX, yTidur, mLebar, tinggiTidur, 3, ST7735_CYAN);
}

void ekspresiBingung() {
  tft.fillScreen(ST7735_BLACK);
  
  // Mata Kiri: Membesar (Lebar & Tinggi ditambah, posisi Y dinaikkan)
  tft.fillRoundRect(mKiriX - 5, mY - 10, mLebar + 10, mTinggi + 20, mRadius, ST7735_CYAN);
  
  // Mata Kanan: Mengecil (Lebar & Tinggi dikurangi, posisi Y diturunkan)
  tft.fillRoundRect(mKananX + 5, mY + 10, mLebar - 10, mTinggi - 20, mRadius, ST7735_CYAN);
}

void ekspresiBahagia() {
  tft.fillScreen(ST7735_BLACK);
  
  // 1. Gambar dua lingkaran utuh warna Hijau di posisi mata
  // (x, y, radius lingkaran, warna)
  int radiusBulat = 18;
  tft.fillCircle(mKiriX + (mLebar/2), mY + 15, radiusBulat, ST7735_CYAN);
  tft.fillCircle(mKananX + (mLebar/2), mY + 15, radiusBulat, ST7735_CYAN);
  
  // 2. Trik Masking! Timpa paruh bawah lingkaran dengan kotak Hitam
  // Kotak ini bertindak sebagai "penghapus" yang tidak terlihat
  tft.fillRect(mKiriX - 10, mY + 15, mLebar + 20, 30, ST7735_BLACK);
  tft.fillRect(mKananX - 10, mY + 15, mLebar + 20, 30, ST7735_BLACK);
}

void kananKiri (){
  lirikKanan(); // Gerakkan mata ke kanan
  delay(1000); // Tahan posisi mata selama 1 detik
  lirikKiri(); // Gerakkan mata ke kiri
  delay(1000); // Tahan posisi mata selama 1 detik
  lirikKiri(); // Gerakkan mata ke kiri lagi
  delay(1000); // Tahan posisi mata selama 1 detik
  lirikKanan(); // Gerakkan mata ke kanan lagi
  delay(1000); // Tahan posisi mata selama 1 detik
}