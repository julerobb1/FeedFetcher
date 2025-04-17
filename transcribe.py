import sys
import os
from pydub import AudioSegment
from pydub.silence import detect_nonsilent

def trim_silence(input_file, output_file):
    print("Trimming silence...")
    audio = AudioSegment.from_file(input_file)
    nonsilent_ranges = detect_nonsilent(audio, min_silence_len=1000, silence_thresh=-40)
    trimmed_audio = sum([audio[start:end] for start, end in nonsilent_ranges])
    trimmed_audio.export(output_file, format="wav")
    return output_file

def transcribe_with_pocketsphinx(input_file):
    from pocketsphinx import AudioFile
    print("Using PocketSphinx for transcription...")
    config = {
        'audio_file': input_file,
        'hmm': 'model/en-us/en-us',  # Update with the PocketSphinx model path
        'lm': 'model/en-us/en-us.lm.bin',
        'dict': 'model/en-us/cmudict-en-us.dict'
    }
    audio = AudioFile(**config)
    for phrase in audio:
        print(phrase)

def transcribe_with_whisper(input_file):
    import whisper
    print("Using Whisper for transcription...")
    model = whisper.load_model("base")
    result = model.transcribe(input_file)
    print(result["text"])

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python transcribe.py <engine> <input_file> [--trim-silence]")
        sys.exit(1)

    engine = sys.argv[1].lower()
    input_file = sys.argv[2]
    trim_silence_flag = "--trim-silence" in sys.argv

    if not os.path.exists(input_file):
        print(f"Error: File '{input_file}' not found.")
        sys.exit(1)

    if trim_silence_flag:
        input_file = trim_silence(input_file, "trimmed_" + input_file)

    if engine == "pocketsphinx":
        transcribe_with_pocketsphinx(input_file)
    elif engine == "whisper":
        transcribe_with_whisper(input_file)
    else:
        print(f"Error: Unknown engine '{engine}'. Use 'pocketsphinx' or 'whisper'.")
