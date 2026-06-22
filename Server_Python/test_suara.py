import asyncio
import edge_tts
import pygame
import os

# Konfigurasi Suara (Ardi = Pria Indonesia)
VOICE = "id-ID-ArdiNeural"
OUTPUT_FILE = "suara_mori.mp3"

async def buat_dan_putar_suara(teks):
    print(f"[TTS] Memproses suara: '{teks}'")
    
    # 1. Mengubah teks menjadi file MP3
    communicate = edge_tts.Communicate(teks, VOICE)
    await communicate.save(OUTPUT_FILE)
    print("[TTS] File suara berhasil dibuat!")
    
    # 2. Memutar file MP3 menggunakan Pygame
    pygame.mixer.init()
    pygame.mixer.music.load(OUTPUT_FILE)
    pygame.mixer.music.play()
    
    # Tunggu sampai suara selesai diputar
    while pygame.mixer.music.get_busy():
        pygame.time.Clock().tick(10)
        
    # Bersihkan file MP3 agar tidak menumpuk di laptop
    pygame.mixer.quit()
    os.remove(OUTPUT_FILE)
    print("[TTS] Selesai berbicara.")

# Teks pengujian (Anggap ini jawaban dari Qwen)
teks_jawaban = "Halo Rafel. Telingaku sudah tajam, otakku sudah jalan, dan sekarang suaraku sudah terdengar jelas."

# Jalankan fungsi
if __name__ == "__main__":
    asyncio.run(buat_dan_putar_suara(teks_jawaban))