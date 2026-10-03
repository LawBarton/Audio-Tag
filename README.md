Audio Tag # Audio Tag Creator

Audio Tag Creator is a Windows desktop application written in C++ using the Win32 API. It allows users to either import local media files or download audio from a video URL, edit the song's metadata, optionally add album artwork, and save the finished MP3.

## Features:
- Import local audio or video files
- Download audio from supported video websites using yt-dlp
- Edit MP3 metadata
  - Song Title
  - Artist
  - Album
- Add album cover artwork
- Preview selected filenames and artwork
- Save the finished MP3 anywhere on your computer
- Simple Windows GUI built with the Win32 API
## Requirements
The following files must be in the same folder as the executable:
- AudioTagCreator.exe
- ffmpeg.exe
- yt-dlp.exe
The program uses:
- Win32 API
- GDI+
- FFmpeg
- yt-dlp

---

# How It Works
The application has two different workflows.
## Option 1 — Import a Local File
1. Launch the program.
2. Click **Drag File**.
3. Select a supported media file.
Supported formats include:
- MP3
- MP4
- WAV
- M4A
- AVI
- MKV
4. The selected filename will appear in the application.
5. Enter the desired:
   - Song Title
   - Artist
   - Album
6. (Optional) Click **Choose Cover** to select album artwork.
7. Click **Download MP3**.
8. Choose where to save the finished file.
The program will:
- Convert the media to MP3 (if necessary)
- Apply your metadata
- Embed the cover artwork (if selected)


## Option 2 — Download From a Video URL
1. Launch the program.
2. Click “Link File”.
3. Paste a supported video URL.
4. Click “Enter”.
5. The application checks the URL and displays the detected video title.
6. Click “Accept” if it is the correct video.
7. Enter:
   - Song Title
   - Artist
   - Album
8. (Optional) Choose album artwork.
9. Click **Download MP3**.
10. Choose where to save the file.
The application uses “yt-dlp” to:
- Download the audio
- Convert it to MP3
- Apply metadata using FFmpeg

If cover art is selected, FFmpeg embeds the image into the MP3.

---

# Metadata
The following tags can be written into the MP3:

- Title
- Artist
- Album
- Album Cover
# Supported Cover Formats
- JPG
- JPEG
- PNG
- BMP
# Supported Local Media Formats
- MP3
- MP4
- WAV
- M4A
- AVI
- MKV

# User Interface
The application contains several screens:

## Main Menu
Choose whether to:
- Drag a local file
- Import from a video URL

## Link Review
After entering a URL, the application displays the detected video title.

Options:

- Accept
- Cancel
## Audio Information

Enter:

- Song Title
- Artist
- Album

You may also select album artwork before exporting.

# External Tools
## yt-dlp
Used for:
- Downloading video audio
- Extracting audio as MP3

## FFmpeg
Used for:
- Audio conversion
- Writing metadata
- Embedding album artwork
---
#Compiling
Compile using MinGW:
```bash
g++ localFile.cpp -o AudioTagCreator -mwindows
```
---

# Notes

- `ffmpeg.exe` and `yt-dlp.exe` must be located beside the executable.
- Internet access is required when downloading from a URL.
- Album artwork is optional.
- The program allows the user to choose the save location before exporting.

---

# Author
Lawson Barton
University of Oklahoma
Software Engineering
