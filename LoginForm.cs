using System;
using System.Windows.Forms;

namespace FeedFetcher
{
    public partial class LoginForm : Form
    {
        public string Username => txtUsername.Text;
        public string Password => txtPassword.Text;

        private TextBox txtUsername;
        private TextBox txtPassword;
        private Button btnOK;
        private Button btnCancel;

        public LoginForm()
        {
            InitializeComponent();
        }

        private void InitializeComponent()
        {
            this.Text = "Login";
            this.Width = 300;
            this.Height = 180;
            this.FormBorderStyle = FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;

            var lblUser = new Label { Text = "Username:", Left = 10, Top = 20, Width = 80 };
            txtUsername = new TextBox { Left = 100, Top = 20, Width = 160 };
            var lblPass = new Label { Text = "Password:", Left = 10, Top = 60, Width = 80 };
            txtPassword = new TextBox { Left = 100, Top = 60, Width = 160, PasswordChar = '*' };
            btnOK = new Button { Text = "OK", Left = 60, Top = 100, Width = 80 };
            btnCancel = new Button { Text = "Cancel", Left = 150, Top = 100, Width = 80 };

            btnOK.Click += (s, e) => this.DialogResult = DialogResult.OK;
            btnCancel.Click += (s, e) => this.DialogResult = DialogResult.Cancel;

            this.Controls.Add(lblUser);
            this.Controls.Add(txtUsername);
            this.Controls.Add(lblPass);
            this.Controls.Add(txtPassword);
            this.Controls.Add(btnOK);
            this.Controls.Add(btnCancel);
        }
    }
}
