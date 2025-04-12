# Mp3Combiner

Mp3Combiner is a tool for downloading audio feeds from Broadcastify, combining multiple audio files, and optionally transcribing the audio.

## Features
- Download audio archives from Broadcastify using a feed URL.
- Combine multiple audio files into a single output file.
- Transcribe audio files into text.

## Usage
Run the program from the command line with the following syntax:

```
Mp3Combiner.exe <broadcastify_feed_url> <output_file> [file1 file2 ...] [--transcribe]
```

### Arguments
- `<broadcastify_feed_url>`: The URL of the Broadcastify feed to download.
- `<output_file>`: The name of the output file for combined audio.
- `[file1 file2 ...]`: (Optional) A list of audio files to combine.
- `--transcribe`: (Optional) Transcribe the combined audio file into text.

### Examples
1. **Download and combine files:**
   ```
   Mp3Combiner.exe "https://www.broadcastify.com/archives/feed/39972" "output.mp3" "file1.mp3" "file2.mp3"
   ```

2. **Download, combine, and transcribe:**
   ```
   Mp3Combiner.exe "https://www.broadcastify.com/archives/feed/39972" "output.mp3" "file1.mp3" "file2.mp3" --transcribe
   ```

## Building the Project
1. Install dependencies (e.g., CMake, Visual Studio).
2. Configure the project:
   ```
   cmake -S z:/Github/FeedFetcher -B z:/Github/FeedFetcher/build
   ```
3. Build the project:
   ```
   cmake --build z:/Github/FeedFetcher/build --config Release
   ```

## Future Enhancements
- Add a graphical user interface (GUI) for easier usage.
- Support additional transcription services.
