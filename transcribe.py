import sys
import json
from vosk import Model, KaldiRecognizer
import wave

def transcribe(input_file, output_file):
    model = Model("path/to/vosk-model")  # Replace with the path to your Vosk model
    wf = wave.open(input_file, "rb")

    if wf.getnchannels() != 1 or wf.getsampwidth() != 2 or wf.getframerate() not in [8000, 16000]:
        raise ValueError("Audio file must be WAV format mono PCM.")

    rec = KaldiRecognizer(model, wf.getframerate())
    result = []

    while True:
        data = wf.readframes(4000)
        if len(data) == 0:
            break
        if rec.AcceptWaveform(data):
            result.append(json.loads(rec.Result())["text"])

    wf.close()

    with open(output_file, "w") as f:
        f.write(" ".join(result))

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python transcribe.py <input_file> <output_file>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]
    transcribe(input_file, output_file)
