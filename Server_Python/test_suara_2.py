from elevenlabs.client import ElevenLabs
import pygame
import os

# Masukkan API Key rahasiamu di sini
API_KEY = "sk_dad06c95cec521ec6cae0c5054841229e3d0012699db0920"

# Inisialisasi klien ElevenLabs (SINTAKS BARU)
client = ElevenLabs(api_key=API_KEY)

# ID Suara "Charlie" (suara pria muda natural)
VOICE_ID = "N2lVS1w4EtoT3dr4eOWO" 

def buat_dan_putar_suara(teks):
    print(f"[TTS] Memproses suara natural: '{teks}'")
    OUTPUT_FILE = "suara_mori_eleven.mp3"
    
    # 1. Menghasilkan audio dari ElevenLabs (SINTAKS BARU)
    audio_generator = client.text_to_speech.convert(
        text=teks,
        voice_id=VOICE_ID,
        model_id="eleven_multilingual_v2",
        output_format="mp3_44100_128"
    )
    
    # 2. Menyimpan audio ke file MP3
    with open(OUTPUT_FILE, "wb") as f:
        for chunk in audio_generator:
            if chunk:
                f.write(chunk)
            
    print("[TTS] File suara berhasil diunduh!")

    # 3. Memutar file MP3
    pygame.mixer.init()
    pygame.mixer.music.load(OUTPUT_FILE)
    pygame.mixer.music.play()
    
    while pygame.mixer.music.get_busy():
        pygame.time.Clock().tick(10)
        
    pygame.mixer.quit()
    os.remove(OUTPUT_FILE)
    print("[TTS] Selesai berbicara.")

# Teks pengujian
teks_jawaban = "Wah, gila sih. Suaraku sekarang udah jauh lebih asyik kan, Rafel? Nggak kaku kayak robot lagi."

if __name__ == "__main__":
    buat_dan_putar_suara(teks_jawaban)