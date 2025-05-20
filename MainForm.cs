using System;
using System.Windows.Forms;

namespace FeedFetcher
{
    public partial class MainForm : Form
    {
        private ProgressBar progressBar;
        private Button btnDownload;
        private Button btnCombine;
        private Button btnTranscribe;
        private CheckBox chkTrimSilence;
        private TextBox txtFeedId;
        private ListBox lstDates;
        private Label lblDescription;

        public MainForm()
        {
            InitializeComponent();
        }

        private void InitializeComponent()
        {
            this.Text = "Feed Downloader";
            this.Width = 620;
            this.Height = 350;
            this.FormBorderStyle = FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;

            lblDescription = new Label { Text = "Enter the numerical feed ID:", Left = 10, Top = 10, Width = 580 };
            txtFeedId = new TextBox { Left = 10, Top = 40, Width = 580, Text = "https://www.broadcastify.com/archives/feed/" };
            lstDates = new ListBox { Left = 10, Top = 70, Width = 580, Height = 120 };
            btnDownload = new Button { Text = "Download", Left = 10, Top = 200, Width = 100 };
            btnTranscribe = new Button { Text = "Transcribe", Left = 120, Top = 200, Width = 100 };
            btnCombine = new Button { Text = "Combine Files", Left = 230, Top = 200, Width = 100 };
            chkTrimSilence = new CheckBox { Text = "Trim Silence", Left = 340, Top = 200, Width = 100 };
            progressBar = new ProgressBar { Left = 10, Top = 240, Width = 580, Height = 20, Style = ProgressBarStyle.Marquee };

            btnDownload.Click += BtnDownload_Click;
            btnTranscribe.Click += BtnTranscribe_Click;
            btnCombine.Click += BtnCombine_Click;

            this.Controls.Add(lblDescription);
            this.Controls.Add(txtFeedId);
            this.Controls.Add(lstDates);
            this.Controls.Add(btnDownload);
            this.Controls.Add(btnTranscribe);
            this.Controls.Add(btnCombine);
            this.Controls.Add(chkTrimSilence);
            this.Controls.Add(progressBar);
        }

        private void BtnDownload_Click(object sender, EventArgs e)
        {
            // Call Python or batch script to fetch feeds
            FeedDownloader.FetchFeeds(txtFeedId.Text);
        }

        private void BtnTranscribe_Click(object sender, EventArgs e)
        {
            // Call Python script for transcription
            PythonInterop.RunTranscribe("combined.mp3", chkTrimSilence.Checked);
        }

        private void BtnCombine_Click(object sender, EventArgs e)
        {
            // Combine files using FFmpeg
            AudioCombiner.CombineFiles(new string[] { "file1.mp3", "file2.mp3" }, "combined.mp3");
        }
    }
}
