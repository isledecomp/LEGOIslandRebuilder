using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Drawing;
using System.Diagnostics;
using System.ComponentModel;
using Microsoft.Win32;
using System.Xml;
using System.Xml.Serialization;

namespace Rebuilder
{
    public class Rebuilder : Form
    {
        Button run_button;
        Button run_additional_button;

        MusicInjector music_injector = new MusicInjector();

        TabControl tabs;
        PropertyGrid patch_view;

        string jukebox_output;

        List<Process> processes = new List<Process>();

        string run_button_run = "Run";
        string run_button_kill = "Kill";

        bool aug_build = false;

        public static string[] standard_hdd_dirs = {
                "C:/Program Files (x86)/LEGO Island",
                "C:/Program Files/LEGO Island",
                "/Program Files (x86)/LEGO Island",
                "/Program Files/LEGO Island"
            };

        public enum FPSLimitType
        {
            Default,
            Uncapped,
            Limited
        };

        public class PatchList {
            decimal turn_speed = 1.0M;
            [Category("Controls")]
            [DisplayName("Turn Speed")]
            [Description("Set the turn speed multiplier. LEGO Island ties its turn speed to the frame rate which " +
                "is too fast on modern PCs. Use this value to correct it. (" +
                "0.00 = No turning at all, " +
                "0.35 = Recommended for modern PCs, " +
                "1.00 = LEGO Island's default)")]
            public decimal TurnSpeed
            {
                get { return turn_speed; }
                set { turn_speed = value; }
            }

            decimal move_speed = 1.0M;
            [Category("Controls")]
            [DisplayName("Move Speed")]
            [Description("Set the movement speed multiplier. This value does not affect other racers so it can " +
                "be used to cheat (or cripple) your chances in races. (" +
                "0.00 = No movement at all, " +
                "1.00 = LEGO Island's default)")]
            public decimal MoveSpeed
            {
                get { return move_speed; }
                set { move_speed = value; }
            }

            bool full_screen = true;
            [Category("Graphics")]
            [DisplayName("Run in Full Screen")]
            [Description("Allows you to change modes without administrator privileges and registry editing.")]
            public bool FullScreen
            {
                get { return full_screen; }
                set { full_screen = value; }
            }

            bool multiple_instances = false;
            [Category("System")]
            [DisplayName("Allow Multiple Instances")]
            [Description("By default, LEGO Island will allow only one instance of itself to run. " +
                "This patch allows infinite instances of LEGO Island to run.")]
            public bool MultipleInstances
            {
                get { return multiple_instances; }
                set { multiple_instances = value; }
            }

            bool stay_active_when_defocused = false;
            [Category("System")]
            [DisplayName("Stay Active When Defocused")]
            [Description("By default, LEGO Island pauses when it's not the active window. " +
                "This patch prevents that behavior.")]
            public bool StayActiveWhenDefocused
            {
                get { return stay_active_when_defocused; }
                set { stay_active_when_defocused = value; }
            }

            bool redirect_save_data = true;
            [Category("System")]
            [DisplayName("Redirect Save Files to %APPDATA%")]
            [Description("By default LEGO Island saves its game data in its Program Files folder. In newer versions of " +
                "Windows, this folder is considered privileged access, necessitating running LEGO Island as administrator " +
                "to save here. This patch sets LEGO Island's save location to %APPDATA% instead, which is an accessible and " +
                "standard location that most modern games and apps save to.")]
            public bool RedirectSaveData
            {
                get { return redirect_save_data; }
                set { redirect_save_data = value; }
            }

            FPSLimitType fps_limit_type = FPSLimitType.Default;
            [Category("Graphics")]
            [DisplayName("FPS Cap")]
            [Description("Modify LEGO Island's frame rate cap")]
            public FPSLimitType FPSLimit
            {
                get { return fps_limit_type; }
                set { fps_limit_type = value; }
            }

            decimal custom_fps_limit = 24.0M;
            [Category("Graphics")]
            [DisplayName("FPS Cap - Custom Limit")]
            [Description("Is 'FPS Cap' is set to 'Limited', this will be the frame rate used.")]
            public decimal CustomFPS
            {
                get { return custom_fps_limit; }
                set { custom_fps_limit = value; }
            }

            bool override_resolution = false;
            [Category("Experimental (Use at your own risk)")]
            [DisplayName("Override Resolution")]
            [Description("Override LEGO Island's hardcoded 640x480 resolution with a custom resolution. " +
                "NOTE: This patch is currently incomplete and buggy.")]
            public bool OverrideResolution
            {
                get { return override_resolution; }
                set { override_resolution = value; }
            }

            int resolution_width = 640;
            [Category("Experimental (Use at your own risk)")]
            [DisplayName("Override Resolution - Width:")]
            [Description("If 'Override Resolution' is enabled, this is the screen resolution width to use instead.")]
            public int ResolutionWidth
            {
                get { return resolution_width; }
                set { resolution_width = value; }
            }

            int resolution_height = 480;
            [Category("Experimental (Use at your own risk)")]
            [DisplayName("Override Resolution - Height:")]
            [Description("If 'Override Resolution' is enabled, this is the screen resolution height to use instead.")]
            public int ResolutionHeight
            {
                get { return resolution_height; }
                set { resolution_height = value; }
            }

            bool upscale_bitmaps = false;
            [Category("Experimental (Use at your own risk)")]
            [DisplayName("Override Resolution - Bitmap Upscale")]
            [Description("WARNING: This doesn't upscale the bitmaps' hitboxes yet and can make 2D areas like the Information Center difficult to navigate.")]
            public bool UpscaleBitmaps
            {
                get { return upscale_bitmaps; }
                set { upscale_bitmaps = value; }
            }
        };

        PatchList patch_config = new PatchList();

        Rebuilder() {
            Size = new Size(420, 420);
            Text = "LEGO Island Rebuilder";
            Icon = Icon.ExtractAssociatedIcon(System.Reflection.Assembly.GetExecutingAssembly().Location);

            TableLayoutPanel grid = new TableLayoutPanel();
            grid.Dock = DockStyle.Fill;

            // Build standard layout
            grid.SuspendLayout();
            
            Label title = new Label();
            title.Anchor = AnchorStyles.Left | AnchorStyles.Right;
            title.Text = "LEGO Island Rebuilder";
            title.Font = new Font(title.Font, FontStyle.Bold);
            title.TextAlign = ContentAlignment.MiddleCenter;
            grid.Controls.Add(title, 0, 0);

            LinkLabel subtitle = new LinkLabel();
            subtitle.Anchor = AnchorStyles.Left | AnchorStyles.Right;
            subtitle.Text = "by MattKC (www.legoisland.org)";
            subtitle.TextAlign = ContentAlignment.MiddleCenter;
            subtitle.LinkClicked += new LinkLabelLinkClickedEventHandler(AuthorLinkClick);
            grid.Controls.Add(subtitle, 0, 1);
            
            // Set up patch view
            patch_view = new PropertyGrid();
            patch_view.Dock = DockStyle.Fill;
            patch_view.SelectedObject = patch_config;

            // Set up tabs
            tabs = new TabControl();
            tabs.Dock = DockStyle.Fill;

            TabPage patches_page = new TabPage("Patches");
            patches_page.Controls.Add(patch_view);
            tabs.Controls.Add(patches_page);

            TabPage music_page = new TabPage("Music");
            music_page.Controls.Add(music_injector);
            music_page.Enter += new EventHandler(this.ShowMusicInjectorForm);
            tabs.Controls.Add(music_page);

            grid.Controls.Add(tabs, 0, 2);

            TableLayoutPanel run_btns = new TableLayoutPanel();
            run_btns.Dock = DockStyle.Fill;
            run_btns.Padding = new Padding(0);
            run_btns.Margin = new Padding(0);
            run_btns.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 0.5F));
            run_btns.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 0.5F));

            run_button = new Button();
            run_button.Text = run_button_run;
            run_button.Anchor = AnchorStyles.Left | AnchorStyles.Right;
            run_button.Click += new System.EventHandler(this.Run);
            run_button.Font = new Font(run_button.Font, FontStyle.Bold);
            run_btns.Controls.Add(run_button, 0, 0);

            run_additional_button = new Button();
            run_additional_button.Visible = false;
            run_additional_button.Text = "Run Additional";
            run_additional_button.Anchor = AnchorStyles.Left | AnchorStyles.Right;
            run_additional_button.Click += new System.EventHandler(this.RunAdditional);
            run_btns.Controls.Add(run_additional_button, 1, 0);

            grid.Controls.Add(run_btns, 0, 3);

            grid.RowStyles.Clear();
            grid.RowStyles.Add(new RowStyle(SizeType.Absolute, title.Height));
            grid.RowStyles.Add(new RowStyle(SizeType.Absolute, subtitle.Height));
            grid.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
            grid.RowStyles.Add(new RowStyle(SizeType.Absolute, run_button.Height + run_button.Margin.Top + run_button.Margin.Bottom));

            grid.ResumeLayout(true);

            Controls.Add(grid);

            ResumeLayout(true);

            CenterToScreen();

            Shown += new EventHandler(this.OnStartup);
            FormClosing += new FormClosingEventHandler(this.OnClosing);
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

        private static string GetDisplayNameOfProperty(string property)
        {
            return ((DisplayNameAttribute)typeof(PatchList).GetProperty("RedirectSaveData").GetCustomAttributes(typeof(DisplayNameAttribute), true)[0]).DisplayName;
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
                Write(lego1dll, new byte[] { 0x7E, 0x04, 0x2B, 0xD1, 0xEB, 0x0A, 0x89, 0xC8, 0xF7, 0xD8, 0x39, 0xD0, 0x7E, 0x50, 0x01, 0xCA, 0x29, 0xCE, 0x89, 0x54, 0x24, 0x04, 0xDB, 0x44, 0x24, 0x04, 0x89, 0x74, 0x24, 0x04, 0xDA, 0x74, 0x24, 0x04, 0x3D, 0xF0, 0x00, 0x00, 0x00, 0x74, 0x0A, 0xC7, 0x44, 0x24, 0x04 }, turn_speed_offset);                
                WriteFloat(lego1dll, (float)patch_config.TurnSpeed);
                Write(lego1dll, new byte[] { 0xEB, 0x08, 0xC7, 0x44, 0x24, 0x04 });
                WriteFloat(lego1dll, (float)patch_config.MoveSpeed);
                Write(lego1dll, new byte[] { 0xD8, 0x4C, 0x24, 0x04, 0xD8, 0x4C, 0x24, 0x14, 0xD9, 0x5C, 0x24, 0x04, 0xD9, 0x44, 0x24, 0x04, 0xEB, 0x19 });
                WriteManyBytes(lego1dll, 0x90, 17);
                Write(lego1dll, new byte[] { 0x89, 0x7C, 0x24, 0x04, 0xD9, 0x44, 0x24, 0x04, 0x5E, 0x83, 0xC4, 0x04, 0xC2, 0x0C, 0x00 });


                // Patch EXE to read from HKCU instead of HKLM
                WriteByte(isleexe, 0x01, 0x1B5F);

                if (patch_config.StayActiveWhenDefocused)
                {
                    // Remove code that writes focus value to memory, effectively keeping it always true - frees up 3 bytes
                    Write(isleexe, new byte[] { 0x90, 0x90, 0x90 }, 0x1363);

                    // Write DirectSound flags to allow audio to play while the window is defocused
                    WriteByte(lego1dll, 0x80, aug_build ? 0xB48FB : 0xB120B);
                    WriteByte(lego1dll, 0x80, 0x5B96);
                    WriteByte(lego1dll, 0x80, aug_build ? 0xB48F1 : 0xB1201);
                    WriteByte(lego1dll, 0x80, aug_build ? 0xAD7D3 : 0xADD43);
                }

                if (patch_config.MultipleInstances)
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

                // FPS Patch
                
                if (patch_config.FPSLimit == FPSLimitType.Uncapped)
                {
                    // Write zero frame delay resulting in uncapped frame rate
                    WriteInt32(isleexe, 0, 0x4B4);
                }
                else if (patch_config.FPSLimit == FPSLimitType.Limited)
                {
                    // Calculate frame delay and write new limit
                    Int32 delay = (Int32) Math.Round(1000.0M / patch_config.CustomFPS);

                    WriteInt32(isleexe, delay, 0x4B4);
                }
                if (patch_config.FPSLimit != FPSLimitType.Default)
                {
                    // Disables 30 FPS limit in Information Center when using software mode
                    WriteManyBytes(lego1dll, 0x90, 8, aug_build ? 0x7A68B : 0x7ABAB);
                }

                // INCOMPLETE: Resolution hack:
                if (patch_config.OverrideResolution)
                {
                    // Changes window size
                    WriteInt32(isleexe, (Int32)patch_config.ResolutionWidth, 0xE848);
                    WriteInt32(isleexe, (Int32)patch_config.ResolutionHeight, 0xE84C);

                    // Changes D3D render size
                    WriteInt32(isleexe, (Int32)patch_config.ResolutionWidth - 1, 0x4D0);
                    WriteInt32(isleexe, (Int32)patch_config.ResolutionHeight - 1, 0x4D7);

                    // Write code to upscale the bitmaps
                    if (patch_config.UpscaleBitmaps)
                    {
                        Write(lego1dll, new byte[] { 0xE9, 0x2D, 0x01, 0x00, 0x00, 0x8B, 0x56, 0x1C, 0x6A, 0x00, 0x8D, 0x45, 0xE4, 0xF6, 0x42, 0x30, 0x08, 0x74, 0x07, 0x68, 0x00, 0x80, 0x00, 0x00, 0xEB, 0x02, 0x6A, 0x00, 0x8B, 0x3B, 0x50, 0x51, 0x8D, 0x4D, 0xD4, 0x51, 0x53, 0x53, 0x50, 0x68 }, 0xB20E9);

                        WriteFloat(lego1dll, (float)patch_config.ResolutionHeight / 480.0f);

                        Int32 x_offset = (Int32)Math.Round((patch_config.ResolutionWidth - (patch_config.ResolutionHeight / 3.0 * 4.0))/2.0);
                        

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

                if (aug_build && patch_config.RedirectSaveData)
                {
                    incompatibilities += "- " + GetDisplayNameOfProperty("RedirectSaveData") + "\n";
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
                    dst.SetValue("Full Screen", patch_config.FullScreen ? "YES" : "NO");

                    // Redirect save path
                    if (patch_config.RedirectSaveData)
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
            if (!music_injector.Prepare())
            {
            }
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

        private void RunAdditional(object sender, EventArgs e)
        {
            Process p = Process.Start(processes[0].StartInfo);
            p.EnableRaisingEvents = true;
            p.Exited += new EventHandler(ProcessExit);
            processes.Add(p);
        }

        private void Run(object sender, EventArgs e)
        {
            if (processes.Count > 0)
            {
                foreach (Process p in processes)
                {
                    p.Kill();
                }
                processes.Clear();
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
                    
                    if (!patch_config.FullScreen)
                    {
                        if (System.Environment.OSVersion.Version.Major < 8)
                        {
                            // Use 16-bit on Windows 8+
                            compat_string += " 16BITCOLOR";
                        }
                        else
                        {
                            // Older versions only have 256 color support
                            compat_string += " 256COLOR";
                        }
                    }

                    if (!patch_config.RedirectSaveData || aug_build)
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
                Process p = Process.Start(start_info);
                p.EnableRaisingEvents = true;
                p.Exited += new EventHandler(ProcessExit);
                processes.Add(p);

                run_button.Text = run_button_kill;
                if (patch_config.MultipleInstances)
                {
                    run_additional_button.Visible = true;
                }
            }
            catch
            {
                return;
            }
        }

        private void ProcessExit(object sender, EventArgs e)
        {
            run_button.BeginInvoke((MethodInvoker)delegate () {
                run_button.Text = run_button_run;
                run_additional_button.Visible = false;
            });

            for (int i=0;i<processes.Count;i++)
            {
                if (processes[i] == sender)
                {
                    processes.RemoveAt(i);
                    break;
                }
            }
        }

        private void AuthorLinkClick(object sender, LinkLabelLinkClickedEventArgs e)
        {
            Process.Start("https://www.legoisland.org/");
        }

        private string GetSettingsDir()
        {
            string settings_path = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + "/LEGOIslandRebuilder";

            Directory.CreateDirectory(settings_path);

            return settings_path;
        }

        private string GetSettingsPath()
        {
            return GetSettingsDir() + "/settings.xml";
        }

        private string GetMusicSettingsPath()
        {
            return GetSettingsDir() + "/music.xml";
        }

        private void OnStartup(object sender, EventArgs e)
        {
            string settings_path = GetSettingsPath();

            // Load patch data
            if (File.Exists(settings_path))
            {
                try
                {
                    XmlSerializer serializer = new XmlSerializer(typeof(PatchList));
                    TextReader reader = new StreamReader(settings_path);
                    patch_config = (PatchList)serializer.Deserialize(reader);
                    patch_view.SelectedObject = patch_config;
                    reader.Close();
                }
                catch (InvalidOperationException) { }
            }

            settings_path = GetMusicSettingsPath();

            // Load music patch data
            if (File.Exists(settings_path))
            {
                XmlReader stream = XmlReader.Create(settings_path);

                while (stream.Read())
                {
                    if (stream.NodeType == XmlNodeType.Element && stream.IsStartElement() && stream.Name == "music")
                    {
                        music_injector.LoadData(stream);
                        break;
                    }
                }

                stream.Close();
            }
        }

        private void OnClosing(object sender, FormClosingEventArgs e)
        {
            // Load patch data
            XmlSerializer serializer = new XmlSerializer(typeof(PatchList));
            TextWriter writer = new StreamWriter(GetSettingsPath());
            serializer.Serialize(writer, patch_config);
            writer.Close();

            // Load music injection data
            XmlWriter music_writer = XmlWriter.Create(GetMusicSettingsPath());
            music_writer.WriteStartDocument();
            music_writer.WriteStartElement("music");
            music_injector.SaveData(music_writer);
            music_writer.WriteEndElement(); // music
            music_writer.WriteEndDocument();
            music_writer.Close();
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
