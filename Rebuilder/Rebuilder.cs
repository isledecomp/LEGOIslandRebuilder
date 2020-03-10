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
using System.Security.Cryptography;

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

        private enum Version
        {
            kUnknown = -1,
            kEnglishv10,
            kEnglishv11
        }

        // These must correspond to the `Version` enum above
        private static string[] VersionHashes = {
            "58FCF0F6500614E9F743712D1DD4D340088123DE",
            "BBE289E89E5A39949D272174162711EA5CFF522C"
        };

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
            decimal turn_max_speed = 20.0M;
            [Category("Controls")]
            [DisplayName("Turning: Max Speed")]
            [Description("Set the maximum turning speed. (Default = 20.0)")]
            public decimal TurnMaxSpeed
            {
                get { return turn_max_speed; }
                set { turn_max_speed = value; }
            }

            decimal turn_max_acceleration = 30.0M;
            [Category("Controls")]
            [DisplayName("Turning: Max Acceleration")]
            [Description("Set the speed at which turning accelerates (requires 'Turning: Enable Velocity') (Default = 30.0)")]
            public decimal TurnMaxAcceleration
            {
                get { return turn_max_acceleration; }
                set { turn_max_acceleration = value; }
            }

            decimal turn_min_acceleration = 15.0M;
            [Category("Controls")]
            [DisplayName("Turning: Min Acceleration")]
            [Description("Set the speed at which turning accelerates (requires 'Turning: Enable Velocity') (Default = 30.0)")]
            public decimal TurnMinAcceleration
            {
                get { return turn_min_acceleration; }
                set { turn_min_acceleration = value; }
            }

            decimal turn_deceleration = 50.0M;
            [Category("Controls")]
            [DisplayName("Turning: Deceleration")]
            [Description("Set the speed at which turning decelerates (requires 'Turning: Enable Velocity') (Default = 50.0)")]
            public decimal TurnDeceleration
            {
                get { return turn_deceleration; }
                set { turn_deceleration = value; }
            }

            bool turn_use_velocity = false;
            [Category("Controls")]
            [DisplayName("Turning: Enable Velocity")]
            [Description("By default, LEGO Island ignores the turning acceleration/deceleration values. Set this to TRUE to utilize them (Default = FALSE)")]
            public bool TurnUseVelocity
            {
                get { return turn_use_velocity; }
                set { turn_use_velocity = value; }
            }

            decimal movement_max_speed = 40.0M;
            [Category("Controls")]
            [DisplayName("Movement: Max Speed")]
            [Description("Set the movement maximum speed. (Default = 40.0)")]
            public decimal MovementMaxSpeed
            {
                get { return movement_max_speed; }
                set { movement_max_speed = value; }
            }

            decimal movement_max_acceleration = 15.0M;
            [Category("Controls")]
            [DisplayName("Movement: Max Acceleration")]
            [Description("Set the movement acceleration speed (i.e. how long it takes to go from not moving to top speed) (Default = 15.0)")]
            public decimal MovementMaxAcceleration
            {
                get { return movement_max_acceleration; }
                set { movement_max_acceleration = value; }
            }

            decimal movement_min_acceleration = 4.0M;
            [Category("Controls")]
            [DisplayName("Movement: Min Acceleration")]
            [Description("Set the movement acceleration speed (i.e. how long it takes to go from not moving to top speed) (Default = 15.0)")]
            public decimal MovementMinAcceleration
            {
                get { return movement_min_acceleration; }
                set { movement_min_acceleration = value; }
            }

            decimal movement_deceleration = 50.0M;
            [Category("Controls")]
            [DisplayName("Movement: Deceleration")]
            [Description("Set the movement deceleration speed (i.e. how long it takes to slow to a stop after releasing the controls). " +
                "Increase this value to stop faster, decrease it to stop slower. " +
                "Usually this is set to a very high value making deceleration almost instant. (Default = 50.0)")]
            public decimal MovementDeceleration
            {
                get { return movement_deceleration; }
                set { movement_deceleration = value; }
            }

            int mouse_deadzone = 40;
            [Category("Controls")]
            [DisplayName("Mouse Deadzone")]
            [Description("Sets the radius from the center of the screen where the mouse will do nothing (40 = default).")]
            public int MouseDeadzone
            {
                get { return mouse_deadzone; }
                set { mouse_deadzone = value; }
            }

            bool unhook_turn_speed = false;
            [Category("Controls")]
            [DisplayName("Turning: Unhook From Frame Rate")]
            [Description("LEGO Island contains a bug where the turning speed is influenced by the frame rate. Enable this to make the turn speed independent of the frame rate.")]
            public bool UnhookTurnSpeed
            {
                get { return unhook_turn_speed; }
                set { unhook_turn_speed = value; }
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

            bool disable_autofinish_building = false;
            [Category("Gameplay")]
            [DisplayName("Disable Auto-Finish Building Section")]
            [Description("In LEGO Island v1.1, placing the last block when building will automatically end the building section. While convenient, " +
                "this prevents players from making any further changes placing the last brick. It also notably defies what Bill Ding says - you " +
                "don't hit the triangle when you're finished building.\n\nThis patch restores the functionality in v1.0 where placing the last block " +
                "will not automatically finish the build section.")]
            public bool DisableAutoFinishBuilding
            {
                get { return disable_autofinish_building; }
                set { disable_autofinish_building = value; }
            }

            decimal fov_multiplier = 0.1M;
            [Category("Graphics")]
            [DisplayName("Field of View Adjustment")]
            [Description("Globally adjusts the field of view by a multiplier\n\n" +
                "0.1 = Default (smaller than 0.1 is more zoomed in, larger than 0.1 is more zoomed out")]
            public decimal FOVMultiplier
            {
                get { return fov_multiplier; }
                set { fov_multiplier = value; }
            }
        }

        PatchList patch_config = new PatchList();

        LinkLabel update;

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

            update = new LinkLabel();
            update.Visible = false;
            update.Anchor = AnchorStyles.Left | AnchorStyles.Right;
            update.Text = "An update is available!";
            update.TextAlign = ContentAlignment.MiddleCenter;
            update.LinkClicked += new LinkLabelLinkClickedEventHandler(AuthorLinkClick);
            grid.Controls.Add(update, 0, 2);

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

            grid.Controls.Add(tabs, 0, 3);

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

            grid.Controls.Add(run_btns, 0, 4);

            grid.RowStyles.Clear();
            grid.RowStyles.Add(new RowStyle(SizeType.Absolute, title.Height));
            grid.RowStyles.Add(new RowStyle(SizeType.Absolute, subtitle.Height));
            grid.RowStyles.Add(new RowStyle(SizeType.Absolute, update.Height));
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
            return ((DisplayNameAttribute)typeof(PatchList).GetProperty(property).GetCustomAttributes(typeof(DisplayNameAttribute), true)[0]).DisplayName;
        }

        private static Version DetermineVersion(string lego1dll_url)
        {
            using (FileStream fs = new FileStream(lego1dll_url, FileMode.Open, FileAccess.Read))
            using (BufferedStream bs = new BufferedStream(fs))
            using (SHA1Managed sha1 = new SHA1Managed())
            {
                byte[] hash = sha1.ComputeHash(bs);
                StringBuilder formatted = new StringBuilder(2 * hash.Length);
                foreach (byte b in hash)
                {
                    formatted.AppendFormat("{0:X2}", b);
                }

                string final_hash = formatted.ToString();

                Version v = (Version) Array.IndexOf(VersionHashes, final_hash);

                if (v == Version.kUnknown) {
                    if (MessageBox.Show("The version of LEGO Island you have installed is unknown to Rebuilder. This may result in unpredictable behavior. Would you like to continue?\n\n"
                        + "Your version is: " + final_hash,
                        "Unknown Version",
                        MessageBoxButtons.YesNo,
                        MessageBoxIcon.Warning) == DialogResult.Yes)
                    {
                        return Version.kEnglishv11;
                    }
                }

                return v;
            }
        }

        private bool Patch(string source_dir, string dir)
        {
            string incompatibilities = "";

            string isleexe_url = dir + "/ISLE.EXE";
            string lego1dll_url = dir + "/LEGO1.DLL";

            Version version = DetermineVersion(lego1dll_url);

            if (version == Version.kUnknown)
            {
                return false;
            }

            using (FileStream lego1dll = File.Open(lego1dll_url, FileMode.Open, FileAccess.ReadWrite))
            using (FileStream isleexe = File.Open(isleexe_url, FileMode.Open, FileAccess.ReadWrite))
            {
                long nav_offset, fov_offset_1, fov_offset_2;

                switch (version) {
                case Version.kEnglishv10:
                    nav_offset = 0xF2C28;
                    fov_offset_1 = 0xA1D67;
                    fov_offset_2 = 0xA1D32;
                    break;
                case Version.kEnglishv11:
                default:
                    nav_offset = 0xF3228;
                    fov_offset_1 = 0xA22D7;
                    fov_offset_2 = 0xA22A2;
                    break;
                }

                WriteInt32(lego1dll, (Int32)patch_config.MouseDeadzone, nav_offset);

                // Skip zero threshold
                lego1dll.Position += 4;

                WriteFloat(lego1dll, (float)patch_config.MovementMaxSpeed);

                WriteFloat(lego1dll, (float)patch_config.TurnMaxSpeed);

                WriteFloat(lego1dll, (float)patch_config.MovementMaxAcceleration);

                WriteFloat(lego1dll, (float)patch_config.TurnMaxAcceleration);

                WriteFloat(lego1dll, (float)patch_config.MovementMinAcceleration);

                WriteFloat(lego1dll, (float)patch_config.TurnMinAcceleration);

                WriteFloat(lego1dll, (float)patch_config.MovementDeceleration);

                WriteFloat(lego1dll, (float)patch_config.TurnDeceleration);

                // Skip 0.4 value that we don't know yet
                lego1dll.Position += 4;

                WriteInt32(lego1dll, Convert.ToInt32(patch_config.TurnUseVelocity));

                // Patch EXE to read from HKCU instead of HKLM
                WriteByte(isleexe, 0x01, 0x1B5F);

                if (patch_config.UnhookTurnSpeed)
                {
                    // Write turn speed unhook routine
                    long turn_speed_routine_loc;

                    switch (version) {
                    case Version.kEnglishv10:
                        turn_speed_routine_loc = 0x54258;
                        break;
                    case Version.kEnglishv11:
                    default:
                        turn_speed_routine_loc = 0x544F8;
                        break;
                    }
                    
                    // Write routine to use frame delta time to adjust the turn speed
                    Write(lego1dll, new byte[] { 0xD9, 0x46, 0x24, 0xD8, 0x4C, 0x24, 0x14, 0xD8, 0x4E, 0x34 }, turn_speed_routine_loc);

                    // Frees up 26 bytes
                    WriteManyBytes(lego1dll, 0x90, 26);
                }

                if (patch_config.StayActiveWhenDefocused)
                {
                    long dsoundoffs1, dsoundoffs2, dsoundoffs3;

                    switch (version) {
                    case Version.kEnglishv10:
                        dsoundoffs1 = 0xB48FB;
                        dsoundoffs2 = 0xB48F1;
                        dsoundoffs3 = 0xAD7D3;
                        break;
                    case Version.kEnglishv11:
                    default:
                        dsoundoffs1 = 0xB120B;
                        dsoundoffs2 = 0xB1201;
                        dsoundoffs3 = 0xADD43;
                        break;
                    }

                    // Remove code that writes focus value to memory, effectively keeping it always true - frees up 3 bytes
                    Write(isleexe, new byte[] { 0x90, 0x90, 0x90 }, 0x1363);

                    // Write DirectSound flags to allow audio to play while the window is defocused
                    WriteByte(lego1dll, 0x80, dsoundoffs1);
                    WriteByte(lego1dll, 0x80, 0x5B96);
                    WriteByte(lego1dll, 0x80, dsoundoffs2);
                    WriteByte(lego1dll, 0x80, dsoundoffs3);
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

                    long jukebox_path_offset;

                    switch (version) {
                    case Version.kEnglishv10:
                        jukebox_path_offset = 0xD28F6;

                        WriteByte(lego1dll, 0xF6, 0x51EF5);
                        WriteByte(lego1dll, 0x34);
                        WriteByte(lego1dll, 0x0D);
                        WriteByte(lego1dll, 0x10);
                        break;
                    case Version.kEnglishv11:
                    default:
                        jukebox_path_offset = 0xD2E66;

                        WriteByte(lego1dll, 0x66, 0x52195);
                        WriteByte(lego1dll, 0x3A);
                        WriteByte(lego1dll, 0x0D);
                        WriteByte(lego1dll, 0x10);
                        break;
                    }

                    WriteString(lego1dll, jukebox_path, jukebox_path_offset);
                }

                // FOV Patch
                WriteByte(lego1dll, 0xEB, fov_offset_1);
                WriteByte(lego1dll, 0xC9);
                //WriteByte(lego1dll, 0x90);
                //WriteByte(lego1dll, 0x90);

                WriteByte(lego1dll, 0x68, fov_offset_2);
                WriteFloat(lego1dll, (float)patch_config.FOVMultiplier);
                WriteByte(lego1dll, 0xD8);
                WriteByte(lego1dll, 0x0C);
                WriteByte(lego1dll, 0x24);
                WriteByte(lego1dll, 0x5E);
                WriteByte(lego1dll, 0xEB);
                WriteByte(lego1dll, 0x2E);

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
                    long remove_fps_limit;

                    switch (version) {
                    case Version.kEnglishv10:
                        remove_fps_limit = 0x7A68B;
                        break;
                    case Version.kEnglishv11:
                    default:
                        remove_fps_limit = 0x7ABAB;
                        break;
                    }

                    // Disables 30 FPS limit in Information Center when using software mode
                    WriteManyBytes(lego1dll, 0x90, 8, remove_fps_limit);
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

                if (version == Version.kEnglishv10 && patch_config.RedirectSaveData)
                {
                    incompatibilities += "- " + GetDisplayNameOfProperty("RedirectSaveData") + "\n";
                    patch_config.RedirectSaveData = false;
                }

                if (patch_config.DisableAutoFinishBuilding)
                {
                    if (version == Version.kEnglishv10)
                    {
                        incompatibilities += "- " + GetDisplayNameOfProperty("DisableAutoFinishBuilding") + "\n";
                    }
                    else
                    {
                        // Disables cutscene/exit code
                        WriteManyBytes(lego1dll, 0x90, 5, 0x22C0B);

                        // Disables flag that freezes the UI on completion
                        WriteManyBytes(lego1dll, 0x90, 7, 0x22C6A);
                    }
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

                    if (!patch_config.RedirectSaveData)
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

        private void UpdateLinkClick(object sender, LinkLabelLinkClickedEventArgs e)
        {
            Process.Start("https://www.legoisland.org/rebuilder");
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
