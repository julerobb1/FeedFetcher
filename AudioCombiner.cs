using System.Diagnostics;
using System.Windows.Forms;

namespace FeedFetcher
{
    public static class AudioCombiner
    {
        public static void CombineFiles(string[] inputFiles, string outputFile)
        {
            // Use FFmpeg to combine files
            string input = string.Join("|", inputFiles);
            string args = $"-y -i \"concat:{input}\" -c copy {outputFile}";
            var psi = new ProcessStartInfo
            {
                FileName = "ffmpeg",
                Arguments = args,
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
                        MessageBox.Show($"Error combining files: {error}", "Error");
                    else
                        MessageBox.Show("Files combined successfully.", "Success");
                }
            }
            catch
            {
                MessageBox.Show("Failed to start FFmpeg process. Ensure FFmpeg is installed and in PATH.", "Error");
            }
        }
    }
}
