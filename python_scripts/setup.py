import os
import subprocess
import sys

def install_dependencies():
    # Install required Python packages
    print("Installing required Python packages...")
    subprocess.check_call([sys.executable, "-m", "pip", "install", "--upgrade", "pip"])
    subprocess.check_call([sys.executable, "-m", "pip", "install", "vosk"])

def download_vosk_model():
    # Download the Vosk model if it doesn't already exist
    model_dir = "vosk-model"
    if not os.path.exists(model_dir):
        print("Downloading Vosk model...")
        os.makedirs(model_dir, exist_ok=True)
        model_url = "https://alphacephei.com/vosk/models/vosk-model-small-en-us-0.15.zip"
        zip_file = "vosk-model.zip"
        subprocess.check_call(["curl", "-L", model_url, "-o", zip_file])
        subprocess.check_call(["tar", "-xf", zip_file, "-C", model_dir, "--strip-components=1"])
        os.remove(zip_file)
    else:
        print("Vosk model already exists.")

if __name__ == "__main__":
    try:
        install_dependencies()
        download_vosk_model()
        print("Setup complete.")
    except Exception as e:
        print(f"Setup failed: {e}")
        sys.exit(1)
