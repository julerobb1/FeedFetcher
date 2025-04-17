# FeedFetcher

FeedFetcher is a tool for downloading, combining, and transcribing audio feeds. It supports silence trimming and dynamic transcription engine selection based on system specifications.

## Features
- **Download and Combine Audio Feeds**: Merge multiple audio files into a single output.
- **Transcription**: Automatically transcribe audio using either PocketSphinx or Whisper.
- **Silence Trimming**: Optionally detect and remove silence from audio files before transcription.
- **Dynamic Engine Selection**: Automatically selects the transcription engine based on system specifications (e.g., GPU availability).

---

## Requirements
1. **Python**: Ensure Python is installed on your system.
2. **FFmpeg**: Required for combining audio files and converting formats.
3. **Python Libraries**:
   - `pydub`
   - `pocketsphinx`
   - `whisper`
   - `ffmpeg` (Python wrapper for FFmpeg)

Install the required Python libraries using:
```bash
pip install pydub pocketsphinx openai-whisper ffmpeg-python
```

---

## Adding Python to the System PATH
If Python is not already added to your system PATH, follow these steps:

### Windows
1. Open the Start Menu and search for "Environment Variables."
2. Click on "Edit the system environment variables."
3. In the System Properties window, click on the **Environment Variables** button.
4. Under "System variables," find the `Path` variable and click **Edit**.
5. Click **New** and add the path to your Python installation (e.g., `C:\Python39` or `C:\Users\<YourUsername>\AppData\Local\Programs\Python\Python39`).
6. Click **OK** to save the changes.

### Verify Python PATH
To verify that Python is correctly added to the PATH, open a Command Prompt and type:
```bash
python --version
```
You should see the Python version displayed.

---

## Usage
### Running the Application
1. Build the project using CMake.
2. Run the executable and use the graphical interface to:
   - Fetch and download audio feeds.
   - Combine multiple audio files.
   - Transcribe audio with optional silence trimming.

### Command-Line Options
You can also run the transcription script directly:
```bash
python transcribe.py <engine> <input_file> [--trim-silence]
```
- `<engine>`: Choose `pocketsphinx` or `whisper`.
- `<input_file>`: Path to the audio file.
- `--trim-silence`: Optional flag to remove silence before transcription.

---

## Notes
- Whisper requires a GPU for optimal performance. If no GPU is detected, PocketSphinx will be used by default.
- Please ensure FFmpeg is installed and added to your system PATH for audio processing. - In some cases this step has been required, other times its not typically needed. But in general, make sure FFmpeg is installed. 

- Program functionality is limited at this time, more features will be added as others contribute to it. 
If you find or encounter any issues, please open an issue on the GitHub repository.