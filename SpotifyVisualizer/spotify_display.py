#!/usr/bin/env python3
"""
Spotify Now Playing Display
Shows the current track, artist, and album cover in a simple window with audio visualizer.
Requires: spotipy, pillow, numpy
Install: pip3 install --user spotipy pillow numpy
"""

import sys
import tkinter as tk
from tkinter import ttk
from PIL import Image
import spotipy
from spotipy.oauth2 import SpotifyOAuth
import urllib.request
import io
import time
import base64
import subprocess
import threading
import numpy as np
import math
import signal
import os
from datetime import datetime

class SpotifyDisplay:
    def __init__(self, root):
        self.root = root
        self.root.title("Spotify Now Playing")
        self.root.geometry("550x200")  # Fixed compact layout
        self.root.minsize(550, 200)
        self.root.maxsize(550, 200)
        self.root.configure(bg='#05020b')
        
        # Make window always on top (picture-in-picture mode)
        self.root.attributes('-topmost', True)
        
        # Lock window size for consistent styling
        self.root.resizable(False, False)
        
        # Track tracking
        self.current_track_id = None
        self.progress_ms = 0
        self.duration_ms = 1
        self.last_update_time = time.time()
        self.is_playing = False
        self.history_path = os.path.expanduser("~/Documents/Scripts/PlaylistTracker/history.txt")
        self.artist_genre_cache = {}
        self.current_album = ""
        self.current_artists = ""
        
        # Audio capture variables
        self.audio_running = False
        self.running = True
        self.spectrum = np.zeros(50)
        self.bar_heights = [0.1 for _ in range(50)]
        
        # Initialize Spotify client
        try:
            self.sp = spotipy.Spotify(auth_manager=SpotifyOAuth(
                client_id="fd0a7f7a24384a628f9ced97761c493a",
                client_secret="58789811b226494eb5f2319996d8712b",
                redirect_uri="http://127.0.0.1:8888/callback",
                scope="user-read-currently-playing user-read-playback-state user-modify-playback-state",
                open_browser=True,
                cache_path=".spotify_cache"
            ))
        except Exception as e:
            self.show_error(f"Failed to authenticate with Spotify:\n{str(e)}")
            return
        
        # Create UI elements
        self.create_widgets()
        
        # Setup audio capture
        self.setup_audio_capture()
        
        # Start updating
        self.update_track()
    
    def create_widgets(self):
        # Background canvas handles the visualizer and progress bar
        self.canvas = tk.Canvas(
            self.root,
            bg='#05020b',
            highlightthickness=0
        )
        self.canvas.place(x=0, y=0, relwidth=1, relheight=1)
        
        # Fixed left panel for album art + metadata
        left_frame = tk.Frame(self.root, bg='#05020b')
        left_frame.place(x=12, y=10, width=210, height=180)
        
        cover_wrapper = tk.Frame(left_frame, bg='#0d0619', bd=0, highlightthickness=0)
        cover_wrapper.pack(pady=(0, 6))
        
        self.cover_label = tk.Label(cover_wrapper, bg='#0d0619', borderwidth=0)
        self.cover_label.pack(padx=4, pady=4)
        
        self.info_label = tk.Label(
            left_frame,
            text="No track",
            font=('DejaVu Sans', 13, 'bold'),
            fg='#ff5ec6',
            bg='#05020b',
            anchor='center'
        )
        self.info_label.pack(pady=(0, 4), padx=5, fill='x')
        self.info_label.config(width=26)
        
        self.meta_label = tk.Label(
            left_frame,
            text="Waiting for playback",
            font=('DejaVu Sans', 9),
            fg='#b892f8',
            bg='#05020b',
            wraplength=190,
            justify='center'
        )
        self.meta_label.pack(padx=6, fill='x')
        
        self.root.protocol("WM_DELETE_WINDOW", self.on_close)
    

    
    def setup_audio_capture(self):
        # Setup audio capture using PulseAudio parec (no sudo needed)
        try:
            self.CHUNK = 2048
            self.RATE = 44100
            
            # Try to start parec subprocess to capture monitor audio
            try:
                # Find monitor source - prefer bluez (Bluetooth headset)
                try:
                    sources = subprocess.check_output(['pactl', 'list', 'short', 'sources'], text=True)
                    
                    monitor_source = None
                    monitor_sources = []
                    for line in sources.split('\n'):
                        if 'monitor' in line.lower() and line.strip():
                            source_name = line.split()[1]
                            monitor_sources.append(source_name)
                            # Prefer Bluetooth headset monitor
                            if 'bluez' in source_name:
                                monitor_source = source_name
                                break
                    
                    # If no Bluetooth, use first monitor found
                    if not monitor_source and monitor_sources:
                        monitor_source = monitor_sources[0]
                except:
                    monitor_source = None
                
                # Start parec with monitor source if found
                if monitor_source:
                    self.audio_process = subprocess.Popen(
                        ['parec', '--device=' + monitor_source, '--format=s16le', '--rate=44100', '--channels=1'],
                        stdout=subprocess.PIPE,
                        stderr=subprocess.PIPE,
                        bufsize=self.CHUNK * 2
                    )
                else:
                    # Try default
                    self.audio_process = subprocess.Popen(
                        ['parec', '--format=s16le', '--rate=44100', '--channels=1'],
                        stdout=subprocess.PIPE,
                        stderr=subprocess.PIPE,
                        bufsize=self.CHUNK * 2
                    )
                
                # Start audio processing thread
                self.audio_thread = threading.Thread(target=self.audio_loop, daemon=True)
                self.audio_thread.start()
                self.audio_running = True
            except Exception as e:
                self.audio_running = False
        except Exception as e:
            self.audio_running = False
    
    def audio_loop(self):
        # Continuously read and process audio data
        while self.running:
            try:
                # Read audio data from parec
                raw_data = self.audio_process.stdout.read(self.CHUNK * 2)  # 2 bytes per sample
                if not raw_data or len(raw_data) < self.CHUNK * 2:
                    time.sleep(0.01)
                    continue
                
                # Convert bytes to numpy array
                audio_data = np.frombuffer(raw_data, dtype=np.int16)
                
                # Apply FFT to get frequency spectrum
                fft = np.fft.rfft(audio_data)
                fft = np.abs(fft)
                
                # Get frequency bins (50 bars) with logarithmic spacing
                num_bars = 50
                
                # Calculate logarithmic frequency bins starting from a higher frequency to skip infra bass
                # Start from ~40Hz instead of 0Hz to avoid wasting bars on infra bass
                min_freq_bin = max(2, int(len(fft) * 40 / 22050))  # ~40Hz
                freq_bins = np.logspace(np.log10(min_freq_bin), np.log10(len(fft)), num_bars + 1, dtype=int)
                
                new_spectrum = []
                for i in range(num_bars):
                    start = freq_bins[i]
                    end = min(freq_bins[i + 1], len(fft))
                    if end > start:
                        # Average the FFT bins for this bar, with moderate bass emphasis
                        bar_value = np.mean(fft[start:end])
                        # Moderate boost for bass frequencies (first 25% of bars)
                        if i < num_bars * 0.25:
                            bar_value *= 1.3  # 30% boost for bass (reduced from 80%)
                        new_spectrum.append(bar_value)
                    else:
                        new_spectrum.append(0)
                
                # Normalize and smooth
                if new_spectrum:
                    max_val = max(new_spectrum) if max(new_spectrum) > 0 else 1
                    self.spectrum = np.array([v / max_val for v in new_spectrum])
                
            except Exception as e:
                break
    
    def on_close(self):
        # Cleanup
        self.running = False
        self.audio_running = False
        if hasattr(self, 'audio_process'):
            try:
                self.audio_process.terminate()
                self.audio_process.wait(timeout=1)
            except:
                self.audio_process.kill()
        self.root.quit()
        self.root.destroy()
        sys.exit(0)
    
    def show_error(self, message):
        error_label = tk.Label(
            self.root,
            text=message,
            font=('Helvetica', 12),
            fg='#FF0000',
            bg='#191414',
            wraplength=450
        )
        error_label.pack(pady=50)

    def set_marquee_text(self, track_name, artists, album_name):
        headline = (track_name or "No track").strip() or "No track"
        if len(headline) > 32:
            headline = headline[:29] + '...'
        self.info_label.config(text=headline)
        details = ' · '.join(filter(None, [artists.strip() if artists else '', album_name.strip() if album_name else '']))
        self.meta_label.config(text=details or "Waiting for playback")

    def log_track(self, track_name, artists, genres):
        """Append track info with timestamp and genres to history file"""
        try:
            os.makedirs(os.path.dirname(self.history_path), exist_ok=True)
            timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            genres_text = ', '.join(genres) if genres else 'Unknown genre'
            line = f"{timestamp} - {track_name} - {artists} - {genres_text}\n"
            with open(self.history_path, 'a', encoding='utf-8') as history_file:
                history_file.write(line)
        except Exception:
            pass

    def get_genres(self, artist_ids):
        """Fetch genres for the first artist (cache results)"""
        if not artist_ids:
            return []
        primary_id = artist_ids[0]
        if primary_id in self.artist_genre_cache:
            return self.artist_genre_cache[primary_id]
        try:
            artist_info = self.sp.artist(primary_id)
            genres = artist_info.get('genres', []) or []
            self.artist_genre_cache[primary_id] = genres
            return genres
        except Exception:
            self.artist_genre_cache[primary_id] = []
            return []

    def get_progress_ratio(self):
        """Estimate current progress ratio between 0 and 1"""
        if self.duration_ms <= 0:
            return 0.0
        progress = self.progress_ms
        if self.is_playing:
            elapsed = (time.time() - self.last_update_time) * 1000
            progress = min(self.duration_ms, self.progress_ms + elapsed)
        ratio = progress / self.duration_ms
        return max(0.0, min(1.0, ratio))

    def format_time(self, ms_value):
        minutes = int(ms_value // 60000)
        seconds = int((ms_value % 60000) // 1000)
        return f"{minutes}:{seconds:02d}"

    def draw_progress_bar(self, left_margin, width, top, bottom):
        if not self.current_track_id or self.duration_ms <= 0:
            return
        bar_height = max(4, bottom - top)
        y = top
        x1 = left_margin + 14
        x2 = max(x1 + 60, width - 16)
        if x2 <= x1:
            return
        ratio = self.get_progress_ratio()
        progress_width = x1 + (x2 - x1) * ratio
        # Background bar
        self.canvas.create_rectangle(
            x1, y, x2, y + bar_height,
            fill='#140b26',
            outline='#281546',
            width=1
        )
        # Filled portion
        self.canvas.create_rectangle(
            x1, y, progress_width, y + bar_height,
            fill='#ff5ec6',
            outline=''
        )
        # Progress text
        if self.is_playing:
            elapsed = (time.time() - self.last_update_time) * 1000
            progress_ms = min(self.duration_ms, self.progress_ms + elapsed)
        else:
            progress_ms = self.progress_ms
        progress_text = f"{self.format_time(progress_ms)} / {self.format_time(self.duration_ms)}"
        self.canvas.create_text(
            (x1 + x2) / 2,
            y + bar_height / 2,
            text=progress_text,
            fill='#12071f',
            font=('JetBrains Mono', 8, 'bold')
        )
    
    def update_track(self):
        try:
            current = self.sp.current_playback()
            
            if current and current.get('item'):
                track = current['item']
                track_name = track['name']
                artists = ', '.join([artist['name'] for artist in track['artists']])
                album_name = track['album']['name'] if track.get('album') else ""
                artist_ids = [artist.get('id') for artist in track['artists'] if artist.get('id')]
                genres = self.get_genres(artist_ids)
                track_id = track['id']
                
                # Update track ID and print track name when it changes
                if track_id != self.current_track_id:
                    self.current_track_id = track_id
                    print(f"♫ Now playing: {track_name} - {artists}")
                    self.log_track(track_name, artists, genres)
                
                # Update playback progress
                self.progress_ms = current.get('progress_ms', 0)
                self.duration_ms = track.get('duration_ms', 1)
                self.is_playing = current.get('is_playing', False)
                self.last_update_time = time.time()
                self.current_album = album_name
                self.current_artists = artists
                
                # Get album cover
                images = track['album']['images']
                if images:
                    cover_url = images[0]['url']  # Get largest image
                    self.load_album_cover(cover_url)
                
                # Update labels
                self.set_marquee_text(track_name, artists, album_name)
            else:
                self.set_marquee_text("No track playing", "Start playback", "")
                self.cover_label.config(image='')
                self.current_track_id = None
                self.is_playing = False
        
        except Exception as e:
            pass
        
        # Update every 2 seconds
        self.root.after(2000, self.update_track)
    
    def load_album_cover(self, url):
        try:
            with urllib.request.urlopen(url) as u:
                raw_data = u.read()
            
            image = Image.open(io.BytesIO(raw_data))
            
            # Fixed size for compact left panel
            image = image.resize((120, 120), Image.LANCZOS)
            
            # Convert to PPM format (tkinter native)
            with io.BytesIO() as output:
                image.save(output, format='PPM')
                ppm_data = output.getvalue()
            
            photo = tk.PhotoImage(data=ppm_data)
            
            self.cover_label.config(image=photo)
            self.cover_label.image = photo  # Keep a reference
        except Exception as e:
            print(f"Failed to load album cover: {e}")
    
    def draw_visualizer(self):
        """Draw audio visualizer bars"""
        try:
            width = self.canvas.winfo_width()
            height = self.canvas.winfo_height()
            
            if width <= 1 or height <= 1:
                self.root.after(33, self.draw_visualizer)
                return
            
            self.canvas.delete('all')
            # Smooth vertical gradient background each frame
            gradient_steps = 48
            top_color = (23, 5, 41)
            bottom_color = (5, 2, 11)
            step_height = height / gradient_steps
            for i in range(gradient_steps):
                ratio = i / max(1, gradient_steps - 1)
                r = int(top_color[0] + (bottom_color[0] - top_color[0]) * ratio)
                g = int(top_color[1] + (bottom_color[1] - top_color[1]) * ratio)
                b = int(top_color[2] + (bottom_color[2] - top_color[2]) * ratio)
                color = f'#{r:02x}{g:02x}{b:02x}'
                y1 = i * step_height
                y2 = y1 + step_height + 1
                self.canvas.create_rectangle(0, y1, width, y2, fill=color, outline='')
            
            # Reserve space for left panel (230px)
            left_margin = 230
            viz_width = max(40, width - left_margin - 12)
            top_margin = 15
            progress_reserved = 40  # space near bottom for progress bar
            viz_height = max(40, height - progress_reserved - top_margin)
            bar_base_y = top_margin + viz_height
            progress_top = max(top_margin + 20, height - 24)
            progress_bottom = min(height - 12, progress_top + 8)
            if bar_base_y > progress_top - 6:
                bar_base_y = progress_top - 6
                viz_height = max(20, bar_base_y - top_margin)
            
            # Number of bars
            num_bars = min(50, max(20, viz_width // 12))
            bar_width = viz_width / num_bars
            
            # Initialize bar heights if needed
            if len(self.bar_heights) != num_bars:
                self.bar_heights = [0.1 for _ in range(num_bars)]
            
            # Use real-time spectrum if available
            if self.audio_running and len(self.spectrum) > 0:
                # Map spectrum to bars
                spectrum_per_bar = len(self.spectrum) // num_bars
                
                for i in range(num_bars):
                    # Get average spectrum value for this bar
                    start_idx = i * spectrum_per_bar
                    end_idx = start_idx + spectrum_per_bar
                    if end_idx <= len(self.spectrum):
                        target = np.mean(self.spectrum[start_idx:end_idx])
                    else:
                        target = 0.1
                    
                    # Clamp and add minimum height
                    target = max(0.05, min(0.95, target * 0.8 + 0.1))
                    
                    # Smooth transition with slower response (less sensitive)
                    self.bar_heights[i] += (target - self.bar_heights[i]) * 0.25
                    
                    # Progressive color gradient: violet to neon pink
                    freq_pos = i / num_bars
                    r = int(132 + (255 - 132) * freq_pos)
                    g = int(94 + (110 - 94) * freq_pos)
                    b = int(247 + (198 - 247) * freq_pos)
                    color = f'#{r:02x}{g:02x}{b:02x}'
                    
                    bar_height = viz_height * self.bar_heights[i]
                    x1 = left_margin + i * bar_width
                    y1 = bar_base_y - bar_height
                    x2 = x1 + bar_width - 2
                    y2 = bar_base_y
                    
                    self.canvas.create_rectangle(
                        x1, y1, x2, y2,
                        fill=color,
                        outline='',
                        width=0
                    )
            
            else:
                # Fallback animation if no real audio (more dynamic)
                t = time.time() * 4
                for i in range(num_bars):
                    # Multiple overlapping waves for more interesting motion
                    phase1 = (i / num_bars) * math.pi * 3 + t
                    phase2 = (i / num_bars) * math.pi * 5 - t * 0.7
                    wave1 = (math.sin(phase1) + 1) / 2
                    wave2 = (math.sin(phase2) + 1) / 2
                    target = (wave1 * 0.6 + wave2 * 0.4) * 0.7 + 0.15
                    
                    self.bar_heights[i] += (target - self.bar_heights[i]) * 0.4
                    
                    # Progressive color gradient: violet to neon pink
                    freq_pos = i / num_bars
                    r = int(132 + (255 - 132) * freq_pos)
                    g = int(94 + (110 - 94) * freq_pos)
                    b = int(247 + (198 - 247) * freq_pos)
                    color = f'#{r:02x}{g:02x}{b:02x}'
                    
                    bar_height = viz_height * self.bar_heights[i]
                    x1 = left_margin + i * bar_width
                    y1 = bar_base_y - bar_height
                    x2 = x1 + bar_width - 2
                    y2 = bar_base_y
                    
                    self.canvas.create_rectangle(
                        x1, y1, x2, y2,
                        fill=color,
                        outline='',
                        width=0
                    )
            # Draw progress bar under visualizer (not at window bottom)
            self.draw_progress_bar(left_margin, width, progress_top, progress_bottom)
        except Exception as e:
            pass
        
        # Update at ~30 FPS for smooth animation
        self.root.after(33, self.draw_visualizer)

def main():
    root = tk.Tk()
    
    # Handle Ctrl+C gracefully
    def signal_handler(sig, frame):
        print('\nExiting...')
        root.quit()
        root.destroy()
        sys.exit(0)
    
    signal.signal(signal.SIGINT, signal_handler)
    
    app = SpotifyDisplay(root)
    app.draw_visualizer()  # Start visualizer animation
    
    try:
        root.mainloop()
    except KeyboardInterrupt:
        print('\nExiting...')
        sys.exit(0)

if __name__ == "__main__":
    main()
