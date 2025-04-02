import subprocess

def download_feed_archives():
    script_path = r"z:\Github\FeedFetcher\scripts\fetch_feeds.bat"
    try:
        subprocess.run([script_path], check=True)
        print("Feed archives downloaded successfully.")
    except subprocess.CalledProcessError as e:
        print(f"Failed to download feed archives: {e}")

def login_to_broadcastify(username, password):
    login_url = "https://www.broadcastify.com/login"
    cookie_file = r"z:\Github\FeedFetcher\cookies.txt"
    curl_path = r"z:\Github\FeedFetcher\third_party\curl\x64\curl.exe"  # Adjust for architecture

    try:
        subprocess.run([
            curl_path, "-c", cookie_file, "-d", f"username={username}&password={password}", login_url
        ], check=True)
        print("Login successful.")
    except subprocess.CalledProcessError as e:
        print(f"Failed to log in: {e}")
