using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Drawing;
using System.Diagnostics;
using Microsoft.Win32;
using System.Xml;

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
        CheckBox upscale_bitmaps;

        Button run_button;
        CheckBox advanced_button;

        CheckBox multiple_instances;

        MusicInjector music_injector = new MusicInjector();
        Button music_replacement_btn = new Button();

        string jukebox_output;

        Process p = null;

        string run_button_run = "Run";
        string run_button_kill = "Kill";

        bool aug_build = false;

        public static string[] standard_hdd_dirs = {
                "C:/Program Files (x86)/LEGO Island",
                "C:/Program Files/LEGO Island",
                "/Program Files (x86)/LEGO Island",
                "/Program Files/LEGO Island"
            };

        Rebuilder() {

            Text = "LEGO Island Rebuilder";
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
            title.Text = "LEGO Island Rebuilder";
            title.Font = new Font(title.Font, FontStyle.Bold);
            title.TextAlign = ContentAlignment.MiddleCenter;
            grid.Controls.Add(title, 0, row);
            grid.SetColumnSpan(title, 2);

            row++;

            LinkLabel subtitle = new LinkLabel();
            subtitle.Anchor = AnchorStyles.Left | AnchorStyles.Right;
            subtitle.Text = "by MattKC (itsmattkc.com)";
            subtitle.TextAlign = ContentAlignment.MiddleCenter;
            subtitle.LinkClicked += new LinkLabelLinkClickedEventHandler(AuthorLinkClick);
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

            Label music_replacement_lbl = new Label();
            music_replacement_lbl.Text = "Music Injection:";
            music_replacement_lbl.Anchor = AnchorStyles.Left | AnchorStyles.Bottom;
            grid.Controls.Add(music_replacement_lbl, 0, row);

            music_replacement_btn.Anchor = AnchorStyles.Left | AnchorStyles.Right;
            UpdateMusicInjectorBtnText();
            music_replacement_btn.Click += new EventHandler(this.ShowMusicInjectorForm);
            grid.Controls.Add(music_replacement_btn, 1, row);

            row++;

            run_fullscreen = new CheckBox();
            run_fullscreen.Checked = true;
            run_fullscreen.Anchor = AnchorStyles.Left | AnchorStyles.Right;
            run_fullscreen.Text = "Run in full screen";
            grid.Controls.Add(run_fullscreen, 0, row);
            grid.SetColumnSpan(run_fullscreen, 2);

            row++;

            multiple_instances = new CheckBox();
            multiple_instances.Anchor = AnchorStyles.Left | AnchorStyles.Right;
            multiple_instances.Text = "Allow multiple instances";
            grid.Controls.Add(multiple_instances, 0, row);
            grid.SetColumnSpan(multiple_instances, 2);

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

            upscale_bitmaps = new CheckBox();
            upscale_bitmaps.Enabled = false;
            upscale_bitmaps.Anchor = AnchorStyles.Left | AnchorStyles.Right;
            upscale_bitmaps.Text = "Upscale Bitmaps";
            advanced_grid.Controls.Add(upscale_bitmaps, 1, row);

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
            tip.SetToolTip(turn_speed_control, "Set the turn speed multiplier. LEGO Island ties its turn speed to the frame rate which is too fast on modern PCs. Use this value to correct it.\n\n" +
                "0.00 = No turning at all\n" +
                "0.35 = Recommended for modern PCs\n" +
                "1.00 = LEGO Island's default");
            tip.SetToolTip(movement_speed_control, "Set the movement speed multiplier. This value does not affect other racers so it can be used to cheat (or cripple) your chances in races.\n\n" +
                "0.00 = No movement at all\n" +
                "1.00 = LEGO Island's default");
            tip.SetToolTip(run_fullscreen, "Override the registry check and run LEGO Island either full screen or windowed. " +
                "Allows you to change modes without administrator privileges and registry editing.");
            tip.SetToolTip(redirect_saves, "Redirect save data to a folder that's writable so games can be saved without administrator privileges.\n\n" +
                "Saves will be stored in: " + Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + "\\LEGO Island");
            tip.SetToolTip(stay_active_when_window_is_defocused, "LEGO Island's default behavior is to pause all operations when defocused. " +
                "This setting overrides that behavior and keeps LEGO Island active even when unfocused.\n\n" +
                "NOTE: This currently only works in windowed mode.");
            tip.SetToolTip(upscale_bitmaps, "WARNING: This doesn't upscale the bitmaps' hitboxes yet and can make 2D areas like the Information Center difficult to navigate");

            Controls.Add(grid);
            Controls.Add(advanced_grid);

            AutoSize = true;
            AutoSizeMode = AutoSizeMode.GrowAndShrink;

            ResumeLayout(true);

            CenterToScreen();

            Shown += new EventHandler(OnStartup);
            FormClosing += new FormClosingEventHandler(OnClosing);
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

        private void WriteManyBytes(FileStream fs, byte b, int count, long pos = -1)
        {
            if (pos > -1)
            {
                fs.Position = pos;
            }

            for (int i=0;i< count;i++)
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

        private bool Patch(string source_dir, string dir)
        {
            string incompatibilities = "";

            using (FileStream lego1dll = File.Open(dir + "/LEGO1.DLL", FileMode.Open, FileAccess.ReadWrite))
            using (FileStream isleexe = File.Open(dir + "/ISLE.EXE", FileMode.Open, FileAccess.ReadWrite))
            {
                // Crude check if the build is September or August                
                lego1dll.Position = 0x54083;
                aug_build = (lego1dll.ReadByte() == 0x7E);
                
                // Write turn/movement speed hack (this frees up 12 bytes of code)
                long turn_speed_offset = aug_build ? 0x54083 : 0x54323;              
                Write(lego1dll, new byte[] { 0x7E, 0x04, 0x2B, 0xD1, 0xEB, 0x0A, 0x89, 0xC8, 0xF7, 0xD8, 0x39, 0xD0, 0x7E, 0x44, 0x01, 0xCA, 0x29, 0xCE, 0x89, 0x54, 0x24, 0x04, 0xDB, 0x44, 0x24, 0x04, 0x89, 0x74, 0x24, 0x04, 0xDA, 0x74, 0x24, 0x04, 0x3D, 0xF0, 0x00, 0x00, 0x00, 0x74, 0x0A, 0xC7, 0x44, 0x24, 0x04 }, turn_speed_offset);                
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

                if (multiple_instances.Checked)
                {
                    // LEGO Island uses FindWindowA in user32.dll to determine if it's already running, here we replace the call with moving 0x0 into EAX, simulating a NULL response from FindWindowA
                    WriteByte(isleexe, 0xEB, 0x10B5);
                }

                // Redirect JUKEBOX.SI if we're inserting music
                if (music_injector.ReplaceCount() > 0)
                {
                    Uri uri1 = new Uri(jukebox_output.Substring(0, jukebox_output.LastIndexOf(".")));

                    Uri uri2 = new Uri(source_dir + "/ISLE.EXE");
                    Uri relative = uri2.MakeRelativeUri(uri1);
                    string jukebox_path = "\\" + Uri.UnescapeDataString(relative.ToString()).Replace("/", "\\");

                    if (aug_build)
                    {
                        WriteByte(lego1dll, 0xF6, 0x51EF5);
                        WriteByte(lego1dll, 0x34);
                        WriteByte(lego1dll, 0x0D);
                        WriteByte(lego1dll, 0x10);
                    }
                    else
                    {
                        WriteByte(lego1dll, 0x66, 0x52195);
                        WriteByte(lego1dll, 0x3A);
                        WriteByte(lego1dll, 0x0D);
                        WriteByte(lego1dll, 0x10);
                    }                    

                    WriteString(lego1dll, jukebox_path, aug_build ? 0xD28F6 : 0xD2E66);
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

                    // Write code to upscale the bitmaps
                    if (upscale_bitmaps.Checked)
                    {
                        Write(lego1dll, new byte[] { 0xE9, 0x2D, 0x01, 0x00, 0x00, 0x8B, 0x56, 0x1C, 0x6A, 0x00, 0x8D, 0x45, 0xE4, 0xF6, 0x42, 0x30, 0x08, 0x74, 0x07, 0x68, 0x00, 0x80, 0x00, 0x00, 0xEB, 0x02, 0x6A, 0x00, 0x8B, 0x3B, 0x50, 0x51, 0x8D, 0x4D, 0xD4, 0x51, 0x53, 0x53, 0x50, 0x68 }, 0xB20E9);

                        WriteFloat(lego1dll, (float) res_height.Value / 480.0f);

                        Int32 x_offset = (Int32)Math.Round((res_width.Value - (res_height.Value / 3 * 4))/2);
                        

                        Write(lego1dll, new byte[] { 0xDB, 0x45, 0xD4, 0xD8, 0x0C, 0x24, 0xDB, 0x5D, 0xD4, 0xDB, 0x45, 0xD8, 0xD8, 0x0C, 0x24, 0xDB, 0x5D, 0xD8, 0xDB, 0x45, 0xDC, 0xD8, 0x0C, 0x24, 0xDB, 0x5D, 0xDC, 0xDB, 0x45, 0xE0, 0xD8, 0x0C, 0x24, 0xDB, 0x5D, 0xE0, 0x58, 0x8B, 0x45, 0xD4, 0x05 });
                        WriteInt32(lego1dll, x_offset);
                        Write(lego1dll, new byte[] { 0x89, 0x45, 0xD4, 0x8B, 0x45, 0xDC, 0x05 });
                        WriteInt32(lego1dll, x_offset);
                        Write(lego1dll, new byte[] { 0x89, 0x45, 0xDC });

                        // Frees up 143 bytes of NOPs
                        WriteManyBytes(lego1dll, 0x90, 143);

                        Write(lego1dll, new byte[] { 0x58, 0x5B });

                        Write(lego1dll, new byte[] { 0xE9, 0xF6, 0xFD, 0xFF, 0xFF }, 0xB22F3);

                        // Frees up 19 bytes of NOPs
                        WriteManyBytes(lego1dll, 0x90, 19);
                    }
                }

                if (aug_build && redirect_saves.Checked)
                {
                    incompatibilities += redirect_saves.Text + "\n";
                }
            }

            RegistryKey src = Registry.LocalMachine.OpenSubKey("SOFTWARE\\Mindscape\\LEGO Island", false);
            if (src == null)
            {
                src = Registry.LocalMachine.OpenSubKey("SOFTWARE\\WOW6432Node\\Mindscape\\LEGO Island", false);
            }

            if (src == null)
            {
                if (MessageBox.Show("Failed to find LEGO Island's registry entries. Some patches may fail. Do you wish to continue?",
                    "Failed to find registry keys",
                    MessageBoxButtons.YesNo,
                    MessageBoxIcon.Warning) == DialogResult.No)
                {
                    return false;
                }
            }
            else
            {
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
            }
            

            return string.IsNullOrEmpty(incompatibilities) || IncompatibleBuildMessage(incompatibilities);
        }

        private bool IsValidDir(string dir)
        {
            return (File.Exists(dir + "/ISLE.EXE") && File.Exists(dir + "/LEGO1.DLL"));
        }

        private void ShowMusicInjectorForm(object sender, EventArgs e)
        {
            //music_changed = true;
            music_injector.ShowDialog();
            UpdateMusicInjectorBtnText();
        }

        private void UpdateMusicInjectorBtnText()
        {
            int count = music_injector.ReplaceCount();
            string btn_text = count + " song";
            if (count != 1)
            {
                btn_text += "s";
            }
            btn_text += " replaced";
            music_replacement_btn.Text = btn_text;
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
            upscale_bitmaps.Enabled = override_resolution.Checked;
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

            string temp_path = Path.GetTempPath() + "LEGOIslandRebuilder";
            Directory.CreateDirectory(temp_path);

            string dir = "";
            for (int i=0;i<standard_hdd_dirs.Length;i++)
            {
                if (IsValidDir(standard_hdd_dirs[i]))
                {
                    dir = standard_hdd_dirs[i];
                    break;
                }
            }

            if (string.IsNullOrEmpty(dir))
            {
                using (OpenFileDialog ofd = new OpenFileDialog())
                {
                    ofd.Filter = "ISLE.EXE|ISLE.EXE";
                    ofd.Title = "Where is LEGO Island installed?";

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

            // Perform music insertion if necessary
            if (music_injector.ReplaceCount() > 0)
            {
                jukebox_output = dir + "/LEGO/Scripts/REJUKEBOX.SI";

                try
                {
                    using (FileStream test_fs = new FileStream(jukebox_output, FileMode.Create, FileAccess.Write))
                    {

                    }
                }
                catch
                {
                    jukebox_output = Path.GetTempPath() + "REJUKEBOX.SI";
                }

                music_injector.Insert(jukebox_output);
            }

            if (!Patch(dir, temp_path)) return;

            // Set new EXE's compatibility mode to 256-colors
            using (RegistryKey key = Registry.CurrentUser.OpenSubKey("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers", true))
            {
                if (key != null)
                {
                    key.CreateSubKey(temp_path + "\\ISLE.EXE");

                    //string compat_string = "HIGHDPIAWARE";
                    string compat_string = "HIGHDPIAWARE DWM8And16BitMitigation";
                    
                    if (!run_fullscreen.Checked)
                    {
                        compat_string += " 256COLOR";
                    }

                    if (!redirect_saves.Checked || aug_build)
                    {
                        compat_string += " RUNASADMIN";
                    }

                    key.SetValue(temp_path + "\\ISLE.EXE", compat_string);
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

        private void AuthorLinkClick(object sender, LinkLabelLinkClickedEventArgs e)
        {
            Process.Start("http://www.itsmattkc.com/");
        }

        private string GetSettingsPath()
        {
            string settings_path = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + "/LEGOIslandRebuilder/settings.xml";

            Directory.CreateDirectory(Path.GetDirectoryName(settings_path));

            return settings_path;
        }

        private void OnStartup(object sender, EventArgs e)
        {
            // Load settings
            string settings_path = GetSettingsPath();

            if (File.Exists(settings_path))
            {
                XmlReader stream = XmlReader.Create(settings_path);

                while (stream.Read())
                {
                    if (stream.NodeType == XmlNodeType.Element && stream.IsStartElement())
                    {
                        if (stream.Name == "turnspeed")
                        {
                            stream.Read();
                            turn_speed_control.Value = decimal.Parse(stream.Value);
                        }
                        else if (stream.Name == "movespeed")
                        {
                            stream.Read();
                            movement_speed_control.Value = decimal.Parse(stream.Value);
                        }
                        else if (stream.Name == "fullscreen")
                        {
                            stream.Read();
                            run_fullscreen.Checked = bool.Parse(stream.Value);
                        }
                        else if (stream.Name == "stayactive")
                        {
                            stream.Read();
                            stay_active_when_window_is_defocused.Checked = bool.Parse(stream.Value);
                        }
                        else if (stream.Name == "redirecttoappdata")
                        {
                            stream.Read();
                            redirect_saves.Checked = bool.Parse(stream.Value);
                        }
                        else if (stream.Name == "showadvanced")
                        {
                            stream.Read();
                            advanced_button.Checked = bool.Parse(stream.Value);
                        }
                        else if (stream.Name == "overrideres")
                        {
                            stream.Read();
                            override_resolution.Checked = bool.Parse(stream.Value);
                        }
                        else if (stream.Name == "overridereswidth")
                        {
                            stream.Read();
                            res_width.Value = decimal.Parse(stream.Value);
                        }
                        else if (stream.Name == "overrideresheight")
                        {
                            stream.Read();
                            res_height.Value = decimal.Parse(stream.Value);
                        }
                        else if (stream.Name == "musicinjection")
                        {
                            music_injector.LoadData(stream);
                            UpdateMusicInjectorBtnText();
                        }
                        else if (stream.Name == "upscalebitmaps")
                        {
                            stream.Read();
                            upscale_bitmaps.Checked = bool.Parse(stream.Value);
                        }
                    }
                }

                stream.Close();
            }
        }

        private void OnClosing(object sender, FormClosingEventArgs e)
        {
            // Save settings

            string settings_path = GetSettingsPath();

            XmlWriter stream = XmlWriter.Create(settings_path);

            stream.WriteStartDocument();

            stream.WriteStartElement("settings");

            stream.WriteStartElement("turnspeed");
            stream.WriteString(turn_speed_control.Value.ToString());
            stream.WriteEndElement(); // turnspeed

            stream.WriteStartElement("movespeed");
            stream.WriteString(movement_speed_control.Value.ToString());
            stream.WriteEndElement(); // movespeed

            stream.WriteStartElement("fullscreen");
            stream.WriteString(run_fullscreen.Checked.ToString());
            stream.WriteEndElement(); // fullscreen

            stream.WriteStartElement("stayactive");
            stream.WriteString(stay_active_when_window_is_defocused.Checked.ToString());
            stream.WriteEndElement(); // stayactive

            stream.WriteStartElement("redirecttoappdata");
            stream.WriteString(redirect_saves.Checked.ToString());
            stream.WriteEndElement(); // redirecttoappdata

            stream.WriteStartElement("showadvanced");
            stream.WriteString(advanced_button.Checked.ToString());
            stream.WriteEndElement(); // showadvanced

            stream.WriteStartElement("overrideres");
            stream.WriteString(override_resolution.Checked.ToString());
            stream.WriteEndElement(); // overrideres

            stream.WriteStartElement("overridereswidth");
            stream.WriteString(res_width.Value.ToString());
            stream.WriteEndElement(); // overridereswidth

            stream.WriteStartElement("overrideresheight");
            stream.WriteString(res_height.Value.ToString());
            stream.WriteEndElement(); // overrideresheight

            stream.WriteStartElement("upscalebitmaps");
            stream.WriteString(upscale_bitmaps.Checked.ToString());
            stream.WriteEndElement();

            stream.WriteStartElement("musicinjection");
            music_injector.SaveData(stream);
            stream.WriteEndElement(); // musicinjection

            stream.WriteEndElement(); // settings

            stream.WriteEndDocument();

            stream.Close();
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
