using System.Diagnostics;
using System.Windows.Forms;

namespace FeedFetcher
{
    public static class FeedDownloader
    {
        public static void FetchFeeds(string feedId)
        {
            // Call the Python or batch script to fetch feeds
            var psi = new ProcessStartInfo
            {
                FileName = "python",
                Arguments = "app_logic.py download_feed_archives",
                UseShellExecute = false,
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                CreateNoWindow = true
            };
            try
            {
                using (var process = Process.Start(psi))
                {
                    string output = process.StandardOutput.ReadToEnd();
                    string error = process.StandardError.ReadToEnd();
                    process.WaitForExit();
                    if (process.ExitCode != 0)
                        MessageBox.Show($"Error fetching feeds: {error}", "Error");
                    else
                        MessageBox.Show("Feeds downloaded successfully.", "Success");
                }
            }
            catch
            {
                MessageBox.Show("Failed to start Python process. Ensure Python is installed and in PATH.", "Error");
            }
        }
    }
}
