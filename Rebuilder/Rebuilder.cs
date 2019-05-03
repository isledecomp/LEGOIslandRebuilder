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
        TableLayoutPanel advanced_grid;

        NumericUpDown turn_speed_control;
        NumericUpDown movement_speed_control;
        CheckBox redirect_saves;
        CheckBox run_fullscreen;
        CheckBox stay_active_when_window_is_defocused;

        CheckBox override_resolution;
        NumericUpDown res_width;
        NumericUpDown res_height;

        Button run_button;
        CheckBox advanced_button;

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
            float f = 100f / grid.ColumnCount;
            for (int i=0;i<grid.ColumnCount;i++)
            {
                grid.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, f));
            }

            // Build standard layout
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
            turn_speed_control.Value = 1;
            turn_speed_control.DecimalPlaces = 2;
            turn_speed_control.Anchor = AnchorStyles.Left | AnchorStyles.Right;
            grid.Controls.Add(turn_speed_control, 1, row);

            row++;

            Label movement_speed_lbl = new Label();
            movement_speed_lbl.Text = "Movement Speed:";
            movement_speed_lbl.Anchor = AnchorStyles.Left | AnchorStyles.Bottom;
            grid.Controls.Add(movement_speed_lbl, 0, row);

            movement_speed_control = new NumericUpDown();
            movement_speed_control.Minimum = 0;
            movement_speed_control.Value = 1;
            movement_speed_control.DecimalPlaces = 2;
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
            run_button.Font = new Font(run_button.Font, FontStyle.Bold);
            grid.Controls.Add(run_button, 0, row);

            advanced_button = new CheckBox();
            advanced_button.Text = "Advanced";
            advanced_button.TextAlign = ContentAlignment.MiddleCenter;
            advanced_button.Appearance = System.Windows.Forms.Appearance.Button;
            advanced_button.Anchor = AnchorStyles.Left | AnchorStyles.Right;
            advanced_button.CheckedChanged += new EventHandler(ToggleAdvanced);
            grid.Controls.Add(advanced_button, 1, row);

            grid.ResumeLayout(true);

            // Build advanced layout
            // Create automatic evenly spaced layout
            advanced_grid = new TableLayoutPanel();
            advanced_grid.Visible = false;
            advanced_grid.Dock = DockStyle.Right;
            advanced_grid.ColumnCount = 2;
            advanced_grid.AutoSize = true;

            f = 100f / advanced_grid.ColumnCount;
            for (int i = 0; i < advanced_grid.ColumnCount; i++)
            {
                advanced_grid.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, f));
            }

            advanced_grid.SuspendLayout();

            row = 0;

            override_resolution = new CheckBox();
            override_resolution.Text = "Override Resolution";
            override_resolution.Anchor = AnchorStyles.Left | AnchorStyles.Right;
            override_resolution.CheckedChanged += new EventHandler(ToggleOverrideResolution);
            advanced_grid.Controls.Add(override_resolution, 0, row);

            row++;

            Label res_width_lbl = new Label();
            res_width_lbl.Text = "Width:";
            res_width_lbl.Anchor = AnchorStyles.Left | AnchorStyles.Bottom;
            advanced_grid.Controls.Add(res_width_lbl, 0, row);

            res_width = new NumericUpDown();
            res_width.Enabled = false;
            res_width.Minimum = 0;
            res_width.Maximum = int.MaxValue;
            res_width.Value = 640;            
            res_width.Anchor = AnchorStyles.Left | AnchorStyles.Right;
            advanced_grid.Controls.Add(res_width, 1, row);

            row++;

            Label res_height_lbl = new Label();
            res_height_lbl.Text = "Height:";
            res_height_lbl.Anchor = AnchorStyles.Left | AnchorStyles.Bottom;
            advanced_grid.Controls.Add(res_height_lbl, 0, row);

            res_height = new NumericUpDown();
            res_height.Enabled = false;
            res_height.Minimum = 0;
            res_height.Maximum = int.MaxValue;
            res_height.Value = 480;
            res_height.Anchor = AnchorStyles.Left | AnchorStyles.Right;
            advanced_grid.Controls.Add(res_height, 1, row);

            row++;

            Label advanced_warning_lbl = new Label();
            advanced_warning_lbl.Text = "WARNING: These features are experimental and often incomplete. Use at your own risk.";
            advanced_warning_lbl.Anchor = AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
            advanced_warning_lbl.Font = new Font(advanced_warning_lbl.Font, FontStyle.Bold);
            advanced_grid.Controls.Add(advanced_warning_lbl, 0, row);
            advanced_grid.SetColumnSpan(advanced_warning_lbl, 2);

            advanced_grid.ResumeLayout(true);

            // Set up tooltips
            ToolTip tip = new ToolTip();
            tip.SetToolTip(turn_speed_control, "Set the turn speed multiplier. Lego Island ties its turn speed to the frame rate which is too fast on modern PCs. Use this value to correct it.\n\n" +
                "0.00 = No turning at all\n" +
                "0.35 = Recommended for modern PCs\n" +
                "1.00 = Lego Island's default");
            tip.SetToolTip(movement_speed_control, "Set the movement speed multiplier. This value does not affect other racers so it can be used to cheat (or cripple) your chances in races.\n\n" +
                "0.00 = No movement at all\n" +
                "1.00 = Lego Island's default");
            tip.SetToolTip(run_fullscreen, "Override the registry check and run Lego Island either full screen or windowed. " +
                "Allows you to change modes without administrator privileges and registry editing.");
            tip.SetToolTip(redirect_saves, "Redirect save data to a folder that's writable so games can be saved without administrator privileges.\n\n" +
                "Saves will be stored in: " + Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + "\\LEGO Island");
            tip.SetToolTip(stay_active_when_window_is_defocused, "Lego Island's default behavior is to pause all operations when defocused. " +
                "This setting overrides that behavior and keeps Lego Island active even when unfocused.\n\n" +
                "NOTE: This currently only works in windowed mode.");

            Controls.Add(grid);
            Controls.Add(advanced_grid);

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

        private void WriteFloat(FileStream fs, float f, long pos = -1)
        {
            byte[] f_bytes = BitConverter.GetBytes(f);
            Write(fs, f_bytes, pos);
        }

        private void WriteString(FileStream fs, string s, long pos = -1)
        {
            byte[] str_bytes = System.Text.Encoding.ASCII.GetBytes(s);
            Write(fs, str_bytes, pos);
        }

        private bool ApproxEqual(float a, float b)
        {
            return Math.Abs(a - b) < 0.0001;
        }

        private bool IncompatibleBuildMessage(string incompatibilities)
        {
            return (MessageBox.Show("The following patches you've chosen are not compatible with this version of LEGO Island:\n\n" + incompatibilities + "\nContinue without them?", "Compatibility", MessageBoxButtons.YesNo, MessageBoxIcon.Warning) == DialogResult.Yes);
        }

        private bool Patch(string dir)
        {
            string incompatibilities = "";

            using (FileStream lego1dll = File.Open(dir + "/LEGO1.DLL", FileMode.Open, FileAccess.ReadWrite))
            using (FileStream isleexe = File.Open(dir + "/ISLE.EXE", FileMode.Open, FileAccess.ReadWrite))
            {
                // Crude check if the build is September or August                
                lego1dll.Position = 0x54083;
                bool aug_build = (lego1dll.ReadByte() == 0x7E);
                
                // Write turn/movement speed hack (this frees up 12 bytes of code)
                long turn_speed_offset = aug_build ? 0x54083 : 0x54323;                
                Write(lego1dll, new byte[] { 0x7E, 0x04, 0x2B, 0xD1, 0xEB, 0x0C, 0x89, 0xC8, 0xF7, 0xD8, 0x39, 0xD0, 0x7E, 0x2F, 0x01, 0xCA, 0x29, 0xCE, 0x89, 0x54, 0x24, 0x04, 0xDB, 0x44, 0x24, 0x04, 0x89, 0x74, 0x24, 0x04, 0xDA, 0x74, 0x24, 0x04, 0x3D, 0xF0, 0x00, 0x00, 0x00, 0x74, 0x0A, 0xC7, 0x44, 0x24, 0x04 }, turn_speed_offset);                
                WriteFloat(lego1dll, (float)turn_speed_control.Value);
                Write(lego1dll, new byte[] { 0xEB, 0x08, 0xC7, 0x44, 0x24, 0x04 });
                WriteFloat(lego1dll, (float)movement_speed_control.Value);
                Write(lego1dll, new byte[] { 0xD8, 0x4C, 0x24, 0x04, 0xD8, 0x4C, 0x24, 0x14, 0xD9, 0x5C, 0x24, 0x04, 0xD9, 0x44, 0x24, 0x04, 0x5E, 0x83, 0xC4, 0x04, 0xC2, 0x0C, 0x00, 0xC7, 0x44, 0x24, 0x04, 0x00, 0x00, 0x00, 0x00, 0xD9, 0x44, 0x24, 0x04, 0x5E, 0x83, 0xC4, 0x04, 0xC2, 0x0C, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
                
                // Patch EXE to read from HKCU instead of HKLM
                WriteByte(isleexe, 0x01, 0x1B5F);

                if (stay_active_when_window_is_defocused.Checked)
                {
                    // Remove code that writes focus value to memory, effectively keeping it always true - frees up 3 bytes
                    Write(isleexe, new byte[] { 0x90, 0x90, 0x90 }, 0x1363);

                    // Write DirectSound flags to allow audio to play while the window is defocused
                    WriteByte(lego1dll, 0x80, aug_build ? 0xB48FB : 0xB120B);
                    WriteByte(lego1dll, 0x80, 0x5B96);
                    WriteByte(lego1dll, 0x80, aug_build ? 0xB48F1 : 0xB1201);
                    WriteByte(lego1dll, 0x80, aug_build ? 0xAD7D3 : 0xADD43);
                }

                // INCOMPLETE: Resolution hack:
                if (override_resolution.Checked)
                {
                    // Changes window size
                    WriteInt32(isleexe, (Int32) res_width.Value, 0xE848);
                    WriteInt32(isleexe, (Int32) res_height.Value, 0xE84C);

                    // Changes D3D render size
                    WriteInt32(isleexe, (Int32)res_width.Value-1, 0x4D0);
                    WriteInt32(isleexe, (Int32)res_height.Value-1, 0x4D7);
                }

                if (aug_build && redirect_saves.Checked)
                {
                    incompatibilities += redirect_saves.Text + "\n";
                }
            }
            
            using (RegistryKey src = Registry.LocalMachine.OpenSubKey("SOFTWARE\\WOW6432Node\\Mindscape\\LEGO Island", false))
            using (RegistryKey dst = Registry.CurrentUser.CreateSubKey("Software\\Mindscape\\LEGO Island"))
            {
                // Copy config data from HKLM to HKCU
                CopyRegistryKey(src, dst);

                // Set full screen value
                dst.SetValue("Full Screen", run_fullscreen.Checked ? "YES" : "NO");

                // Redirect save path
                if (redirect_saves.Checked)
                {
                    string new_save_dir = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + "\\LEGO Island\\";
                    Directory.CreateDirectory(new_save_dir);
                    dst.SetValue("savepath", new_save_dir);
                }
            }

            return string.IsNullOrEmpty(incompatibilities) || IncompatibleBuildMessage(incompatibilities);
        }

        private bool IsValidDir(string dir)
        {
            return (File.Exists(dir + "/ISLE.EXE") && File.Exists(dir + "/LEGO1.DLL"));
        }

        private void ToggleAdvanced(object sender, EventArgs e)
        {
            advanced_grid.Visible = advanced_button.Checked;
            //CenterToScreen();
        }

        private void ToggleOverrideResolution(Object sender, EventArgs e)
        {
            res_width.Enabled = override_resolution.Checked;
            res_height.Enabled = override_resolution.Checked;
        }

        private void CopyRegistryKey(RegistryKey src, RegistryKey dst)
        {
            // copy the values
            foreach (var name in src.GetValueNames())
            {
                dst.SetValue(name, src.GetValue(name), src.GetValueKind(name));
            }

            // copy the subkeys
            foreach (var name in src.GetSubKeyNames())
            {
                using (var srcSubKey = src.OpenSubKey(name, false))
                {
                    var dstSubKey = dst.CreateSubKey(name);
                    CopyRegistryKey(srcSubKey, dstSubKey);
                }
            }
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

            if (!Patch(temp_path)) return;

            // Set new EXE's compatibility mode to 256-colors
            using (RegistryKey key = Registry.CurrentUser.OpenSubKey("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers", true))
            {
                key.CreateSubKey(temp_path + "\\ISLE.EXE");
                
                if (run_fullscreen.Checked)
                {
                    key.SetValue(temp_path + "\\ISLE.EXE", "HIGHDPIAWARE DWM8And16BitMitigation");
                }
                else
                {
                    key.SetValue(temp_path + "\\ISLE.EXE", "HIGHDPIAWARE DWM8And16BitMitigation 256COLOR");
                }
            }

            ProcessStartInfo start_info = new ProcessStartInfo(temp_path + "/ISLE.EXE");
            start_info.WorkingDirectory = dir;

            try
            {
                p = Process.Start(start_info);
            }
            catch
            {
                p = null;
                return;
            }
            
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
