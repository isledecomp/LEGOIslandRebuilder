using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Drawing;
using System.Diagnostics;
using Microsoft.Win32;

namespace Rebuilder
{
    class Rebuilder : Form
    {
        NumericUpDown turn_speed_control;
        NumericUpDown movement_speed_control;
        CheckBox redirect_saves;
        CheckBox run_fullscreen;
        CheckBox stay_active_when_window_is_defocused;
        Button run_button;

        Process p = null;

        string run_button_run = "Run";
        string run_button_kill = "Kill";

        Rebuilder() {
            Text = "Lego Island Rebuilder";
            MaximizeBox = false;
            Icon = Icon.ExtractAssociatedIcon(System.Reflection.Assembly.GetExecutingAssembly().Location);

            TableLayoutPanel grid = new TableLayoutPanel();
            grid.Dock = DockStyle.Fill;
            grid.ColumnCount = 2;
            grid.AutoSize = true;

            // Create automatic evenly spaced layout
            float f = 100 / grid.ColumnCount;
            for (int i=0;i<grid.ColumnCount;i++)
            {
                grid.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, f));
            }

            grid.SuspendLayout();

            int row = 0;

            Label title = new Label();
            title.Anchor = AnchorStyles.Left | AnchorStyles.Right;
            title.Text = "Lego Island Rebuilder";
            title.Font = new Font(title.Font, FontStyle.Bold);
            title.TextAlign = ContentAlignment.MiddleCenter;
            grid.Controls.Add(title, 0, row);
            grid.SetColumnSpan(title, 2);

            row++;

            LinkLabel subtitle = new LinkLabel();
            subtitle.Anchor = AnchorStyles.Left | AnchorStyles.Right;
            subtitle.Text = "by MattKC (itsmattkc.com)";
            subtitle.TextAlign = ContentAlignment.MiddleCenter;
            subtitle.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(AuthorLinkClick);
            grid.Controls.Add(subtitle, 0, row);
            grid.SetColumnSpan(subtitle, 2);

            row++;

            Label turn_speed_lbl = new Label();
            turn_speed_lbl.Text = "Turn Speed:";
            turn_speed_lbl.Anchor = AnchorStyles.Left | AnchorStyles.Bottom;
            grid.Controls.Add(turn_speed_lbl, 0, row);

            turn_speed_control = new NumericUpDown();
            turn_speed_control.Minimum = 0;
            turn_speed_control.Value = 20;
            turn_speed_control.Anchor = AnchorStyles.Left | AnchorStyles.Right;
            grid.Controls.Add(turn_speed_control, 1, row);

            row++;

            Label movement_speed_lbl = new Label();
            movement_speed_lbl.Text = "Movement Speed:";
            movement_speed_lbl.Anchor = AnchorStyles.Left | AnchorStyles.Bottom;
            grid.Controls.Add(movement_speed_lbl, 0, row);

            movement_speed_control = new NumericUpDown();
            movement_speed_control.Minimum = 0;
            movement_speed_control.Value = 20;
            movement_speed_control.Anchor = AnchorStyles.Left | AnchorStyles.Right;
            grid.Controls.Add(movement_speed_control, 1, row);

            row++;

            run_fullscreen = new CheckBox();
            run_fullscreen.Checked = true;
            run_fullscreen.Anchor = AnchorStyles.Left | AnchorStyles.Right;
            run_fullscreen.Text = "Run in full screen";
            grid.Controls.Add(run_fullscreen, 0, row);
            grid.SetColumnSpan(run_fullscreen, 2);

            row++;

            stay_active_when_window_is_defocused = new CheckBox();
            stay_active_when_window_is_defocused.Checked = false;
            stay_active_when_window_is_defocused.Anchor = AnchorStyles.Left | AnchorStyles.Right;
            stay_active_when_window_is_defocused.Text = "Stay active when window is defocused";
            grid.Controls.Add(stay_active_when_window_is_defocused, 0, row);
            grid.SetColumnSpan(stay_active_when_window_is_defocused, 2);

            row++;

            redirect_saves = new CheckBox();
            redirect_saves.Checked = true;
            redirect_saves.Anchor = AnchorStyles.Left | AnchorStyles.Right;
            redirect_saves.Text = "Redirect save files to %APPDATA%";
            grid.Controls.Add(redirect_saves, 0, row);
            grid.SetColumnSpan(redirect_saves, 2);

            row++;

            run_button = new Button();
            run_button.Text = run_button_run;
            run_button.Anchor = AnchorStyles.Left | AnchorStyles.Right;
            run_button.Click += new System.EventHandler(this.Run);
            grid.Controls.Add(run_button, 0, row);
            grid.SetColumnSpan(run_button, 2);

            ToolTip tip = new ToolTip();
            tip.SetToolTip(turn_speed_control, "Set the turn speed multiplier. Lego Island ties its turn speed to the frame rate which is too fast on modern PCs. Use this value to correct it.\n\n" +
                "0 = No turning at all\n" +
                "7 = Recommended for modern PCs\n" +
                "20 = Lego Island's default");
            tip.SetToolTip(movement_speed_control, "Set the movement speed multiplier. This value does not affect other racers so it can be used to cheat (or cripple) your chances in races.\n\n" +
                "0 = No movement at all\n" +
                "20 = Lego Island's default");
            tip.SetToolTip(run_fullscreen, "Override the registry check and run Lego Island either full screen or windowed. " +
                "Allows you to change modes without administrator privileges and registry editing.");
            tip.SetToolTip(redirect_saves, "Redirect save data to a folder that's writable so games can be saved without administrator privileges.\n\n" +
                "Saves will be stored in: " + Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + "\\LEGO Island");
            tip.SetToolTip(stay_active_when_window_is_defocused, "Lego Island's default behavior is to pause all operations when defocused. " +
                "This setting overrides that behavior and keeps Lego Island active even when unfocused.\n\n" +
                "NOTE: This currently only works in windowed mode.");

            grid.ResumeLayout(true);

            Controls.Add(grid);

            AutoSize = true;
            AutoSizeMode = AutoSizeMode.GrowAndShrink;

            ResumeLayout(true);

            CenterToScreen();
        }

        private void Write(FileStream fs, byte[] bytes, long pos = -1)
        {
            if (pos > -1)
            {
                fs.Position = pos;
            }
            
            fs.Write(bytes, 0, bytes.Length);
        }

        private void WriteByte(FileStream fs, byte b, long pos = -1)
        {
            if (pos > -1)
            {
                fs.Position = pos;
            }

            fs.WriteByte(b);
        }
        
        private void WriteInt32(FileStream fs, Int32 integer, long pos = -1)
        {
            byte[] int_bytes = BitConverter.GetBytes(integer);
            Write(fs, int_bytes, pos);
        }

        private void WriteString(FileStream fs, string s, long pos = -1)
        {
            byte[] str_bytes = System.Text.Encoding.ASCII.GetBytes(s);
            Write(fs, str_bytes, pos);
        }

        private void Patch(string dir)
        {
            using (FileStream lego1dll = File.Open(dir + "/LEGO1.DLL", FileMode.Open, FileAccess.Write))
            using (FileStream isleexe = File.Open(dir + "/ISLE.EXE", FileMode.Open, FileAccess.Write))
            {
                // Write turn/movement speed hack (this frees up 12 bytes of code)
                Write(lego1dll, new byte[] { 0x7E, 0x04, 0x2B, 0xD1, 0xEB, 0x0C, 0x89, 0xC8, 0xF7, 0xD8, 0x39, 0xD0, 0x7E, 0x2F, 0x01, 0xCA, 0x29, 0xCE, 0x89, 0x54, 0x24, 0x04, 0xDB, 0x44, 0x24, 0x04, 0x89, 0x74, 0x24, 0x04, 0xDA, 0x74, 0x24, 0x04, 0x3D, 0xF0, 0x00, 0x00, 0x00, 0x74, 0x0A, 0xC7, 0x44, 0x24, 0x04 }, 0x54323);                
                WriteInt32(lego1dll, (Int32)turn_speed_control.Value);
                Write(lego1dll, new byte[] { 0xEB, 0x08, 0xC7, 0x44, 0x24, 0x04 });                
                WriteInt32(lego1dll, (Int32)movement_speed_control.Value);
                Write(lego1dll, new byte[] { 0xDA, 0x4C, 0x24, 0x04, 0xD9, 0x5C, 0x24, 0x04, 0xD9, 0x44, 0x24, 0x04, 0x5E, 0x83, 0xC4, 0x04, 0xC2, 0x0C, 0x00, 0xC7, 0x44, 0x24, 0x04, 0x00, 0x00, 0x00, 0x00, 0xD9, 0x44, 0x24, 0x04, 0x5E, 0x83, 0xC4, 0x04, 0xC2, 0x0C, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });

                if (redirect_saves.Checked)
                {
                    // Write patch to write saves to "AppData" instead of "Program Files"
                    Write(lego1dll, new byte[] { 0xE9, 0x61, 0x9B, 0x09, 0x00 }, 0x39300);
                    Write(lego1dll, new byte[] { 0x53, 0xBB, 0x79, 0x3A, 0x0D, 0x10, 0x89, 0x5C, 0x24, 0x08, 0x56, 0x8B, 0x01, 0x57, 0xE9, 0x8C, 0x64, 0xF6, 0xFF }, 0xD2E66);

                    // New directory to write in (defaults to "%AppData%/LEGO Island")
                    string new_save_dir = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + "\\LEGO Island\\";
                    Directory.CreateDirectory(new_save_dir);
                    WriteString(lego1dll, new_save_dir);
                }

                
                if (stay_active_when_window_is_defocused.Checked)
                {
                    // Remove code that writes focus value to memory, effectively keeping it always true - frees up 3 bytes
                    Write(isleexe, new byte[] { 0x90, 0x90, 0x90 }, 0x1363);

                    // Write DirectSound flags to allow audio to play while the window is defocused
                    WriteByte(lego1dll, 0x80, 0xB120B);
                    WriteByte(lego1dll, 0x80, 0x5B96);
                    WriteByte(lego1dll, 0x80, 0xB1201);
                    WriteByte(lego1dll, 0x80, 0xADD43);
                }

                // This operation skips the registry check so full screen/window mode can be set without entering the registry
                if (run_fullscreen.Checked)
                {
                    // Write all nops since full screen is enabled by default, frees up 13 bytes that were used to call the registry reading function
                    Write(isleexe, new byte[] { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }, 0x1E03);
                }
                else
                {
                    // Write 7 bytes to set the full screen value to 0, frees up 7 bytes of code that were used to call the registry reading function
                    Write(isleexe, new byte[] { 0xC7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }, 0x1E03);
                }
            }
        }

        private bool IsValidDir(string dir)
        {
            return (File.Exists(dir + "/ISLE.EXE") && File.Exists(dir + "/LEGO1.DLL"));
        }

        private void Run(object sender, EventArgs e)
        {
            if (p != null)
            {
                p.Kill();
                return;
            }

            string temp_path = Path.GetTempPath() + "lirebuild";

            Directory.CreateDirectory(temp_path);

            string[] possible_dirs = {
                "C:/Program Files (x86)/Lego Island",
                "C:/Program Files/Lego Island",
                "/Program Files (x86)/Lego Island",
                "/Program Files/Lego Island"
            };

            string dir = "";
            for (int i=0;i<possible_dirs.Length;i++)
            {
                if (IsValidDir(possible_dirs[i]))
                {
                    dir = possible_dirs[i];
                    break;
                }
            }

            if (string.IsNullOrEmpty(dir))
            {
                using (OpenFileDialog ofd = new OpenFileDialog())
                {
                    ofd.Filter = "ISLE.EXE|ISLE.EXE";
                    ofd.Title = "Where is Lego Island installed?";

                    while (true)
                    {
                        if (ofd.ShowDialog() == DialogResult.OK)
                        {
                            dir = Path.GetDirectoryName(ofd.FileName);

                            if (IsValidDir(dir))
                            {
                                break;
                            }
                            else
                            {
                                MessageBox.Show(
                                    "This directory does not contain ISLE.EXE and LEGO1.DLL.",
                                    "Failed to find critical files",
                                    MessageBoxButtons.OK,
                                    MessageBoxIcon.Error
                                );
                            }
                        }
                        else
                        {
                            return;
                        }
                    }
                }
            }

            try
            {
                string[] dest_files = Directory.GetFiles(temp_path);
                for (int i = 0; i < dest_files.Length; i++)
                {
                    File.Delete(dest_files[i]);
                }                    

                string[] src_files = Directory.GetFiles(dir);
                for (int i=0;i< src_files.Length;i++)
                {
                    File.Copy(src_files[i], temp_path + "/" + Path.GetFileName(src_files[i]), true);
                }
            }
            catch
            {
                MessageBox.Show("Failed to patch files", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            Patch(temp_path);

            /*
            // Set new EXE's compatibility mode to 256-colors
            using (RegistryKey key = Registry.LocalMachine.OpenSubKey("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers", true))
            {
                key.CreateSubKey(temp_path + "\\ISLE.EXE");
                key.SetValue(temp_path + "\\ISLE.EXE", "DWM8And16BitMitigation");
            }
            */

            ProcessStartInfo start_info = new ProcessStartInfo(temp_path + "/ISLE.EXE");
            start_info.WorkingDirectory = dir;
            p = Process.Start(start_info);
            p.EnableRaisingEvents = true;
            p.Exited += new EventHandler(ProcessExit);
            run_button.Text = run_button_kill;
        }

        private void ProcessExit(object sender, EventArgs e)
        {
            run_button.BeginInvoke((MethodInvoker)delegate () { run_button.Text = run_button_run; });
            p = null;
        }

        private void AuthorLinkClick(object sender, System.Windows.Forms.LinkLabelLinkClickedEventArgs e)
        {
            ((LinkLabel)sender).LinkVisited = true;
            Process.Start("http://www.itsmattkc.com/");
        }

        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Rebuilder());
        }
    }
}
