import socket
import numpy as np
from faster_whisper import WhisperModel
import time
import requests
import json
from elevenlabs.client import ElevenLabs
import pygame
import os

# ==========================================
# 1. KONFIGURASI AI (WHISPER & ELEVENLABS)
# ==========================================
print("[Sistem] Memuat model Whisper 'small' (~241MB)...")
model = WhisperModel("small", device="cpu", compute_type="int8")

# Konfigurasi ElevenLabs
API_KEY = "sk_dad06c95cec521ec6cae0c5054841229e3d0012699db0920" 
client_eleven = ElevenLabs(api_key=API_KEY)
VOICE_ID = "N2lVS1w4EtoT3dr4eOWO" # ID Callum

print("[Sistem] Seluruh Model AI SIAP!")

# ==========================================
# 2. KONFIGURASI JARINGAN UDP & VAD
# ==========================================
UDP_IP = "0.0.0.0" 
UDP_PORT = 5005    

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))
print(f"\n[Sistem] Server Python SIAP! Menunggu suara di port {UDP_PORT}...")
print("-" * 50)

CHUNK_SIZE = 4096
SAMPLE_RATE = 16000
SILENCE_THRESHOLD = 0.1 
MAX_SILENCE_DURATION = 1.5 
MIN_RECORD_DURATION = 1.0 

audio_buffer = bytearray()
is_speaking = False
silence_start_time = 0

# ==========================================
# 3. FUNGSI OTAK (QWEN)
# ==========================================
def tanya_qwen(prompt_user):
    url = "http://localhost:11434/api/generate"
    payload = {
        "model": "mori",
        "prompt": prompt_user,
        "stream": False   
    }
    try:
        print("[Ollama] Mori sedang berpikir...")
        response = requests.post(url, json=payload)
        response.raise_for_status() 
        hasil = response.json()
        return hasil.get("response", "")
    except Exception as e:
        return f"Maaf, sepertinya koneksi ke otakku terputus. Error: {e}"

# ==========================================
# 4. FUNGSI PITA SUARA (ELEVENLABS)
# ==========================================
def buat_dan_putar_suara(teks):
    print(f"[TTS] Memproses suara Callum...")
    OUTPUT_FILE = "suara_mori_eleven.mp3"
    
    try:
        audio_generator = client_eleven.text_to_speech.convert(
            text=teks,
            voice_id=VOICE_ID,
            model_id="eleven_multilingual_v2",
            output_format="mp3_44100_128"
        )
        
        with open(OUTPUT_FILE, "wb") as f:
            for chunk in audio_generator:
                if chunk:
                    f.write(chunk)
                    
        pygame.mixer.init()
        pygame.mixer.music.load(OUTPUT_FILE)
        pygame.mixer.music.play()
        
        while pygame.mixer.music.get_busy():
            pygame.time.Clock().tick(10)
            
        pygame.mixer.quit()
        os.remove(OUTPUT_FILE)
        
    except Exception as e:
        print(f"[Error TTS] Gagal memutar suara: {e}")

# ==========================================
# 5. LOOP UTAMA
# ==========================================
while True:
    try:
        data, addr = sock.recvfrom(CHUNK_SIZE)
        audio_chunk = np.frombuffer(data, dtype=np.int16).astype(np.float32) / 32768.0
        volume = np.sqrt(np.mean(audio_chunk**2))
        
        if volume > SILENCE_THRESHOLD:
            if not is_speaking:
                print("\n[Mori] (Mendengarkan...)")
            is_speaking = True
            silence_start_time = time.time()
            audio_buffer.extend(data)
            
        elif is_speaking:
            audio_buffer.extend(data)
            
            if time.time() - silence_start_time > MAX_SILENCE_DURATION:
                duration_seconds = len(audio_buffer) / (SAMPLE_RATE * 2)
                
                if duration_seconds < MIN_RECORD_DURATION:
                    print(f"[Sistem] Mengabaikan suara pendek ({duration_seconds:.1f} dtk)")
                else:
                    print(f"[Sistem] Memproses suara ({duration_seconds:.1f} dtk)...")
                    audio_np = np.frombuffer(audio_buffer, dtype=np.int16).astype(np.float32) / 32768.0
                    
                    segments, info = model.transcribe(
                        audio_np, 
                        beam_size=5, 
                        language="id",
                        condition_on_previous_text=False,
                        initial_prompt="Berikut adalah percakapan santai dalam bahasa Indonesia.",
                        vad_filter=True, 
                        vad_parameters=dict(min_silence_duration_ms=500)
                    )
                    
                    teks_lengkap = "".join([segment.text for segment in segments]).strip()
                    
                    if teks_lengkap != "":
                        teks_kecil = teks_lengkap.lower()
                        
                        # FIREWALL WAKE WORD
                        if "mori" in teks_kecil:
                            print(f"\n[+] VALID COMMAND: {teks_lengkap}")
                            
                            # 1. Panggil Qwen
                            jawaban = tanya_qwen(teks_lengkap)
                            print(f"\nO O MORI MENJAWAB:\n{jawaban}\n")
                            
                            # 2. Putar Suara
                            buat_dan_putar_suara(jawaban)
                            
                        else:
                            print(f"[-] Mengabaikan (Tidak ada panggilan Mori): {teks_lengkap}")
                
                audio_buffer.clear()
                is_speaking = False
                
    except KeyboardInterrupt:
        print("\n[Sistem] Server dimatikan oleh pengguna.")
        break