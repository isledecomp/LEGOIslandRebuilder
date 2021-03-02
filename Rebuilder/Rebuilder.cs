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
    public class Rebuilder
    {
        MusicInjector music_injector = new MusicInjector();

        string jukebox_output;

        List<Process> processes = new List<Process>();

        private enum Version
        {
            kUnknown = -1,
            kEnglish10,
            kEnglish11,
            kGerman11,
            kDanish11,
            kSpanish11
        }

        // These must correspond to the `Version` enum above
        private static string[] VersionHashes = {
            "58FCF0F6500614E9F743712D1DD4D340088123DE",
            "BBE289E89E5A39949D272174162711EA5CFF522C",
            "96A6BAE8345AA04C21F1B319A632CAECFEE22443",
            "8DFD3E5FDDE8C95C61013069795171163C9A4821",
            "47EE50FC1EC5F6C54F465EB296D2F1B7CA25D5D2"
        };
        
        public enum FPSLimitType
        {
            Default,
            Uncapped,
            Limited
        };

        public enum ModelQualityType
        {
            Infinite,
            High,
            Medium,
            Low
        }

        public class PatchList {
            bool use_wasd = false;
            [Category("Controls")]
            [DisplayName("Use WASD")]
            [Description("Enables the use of WASD keys for movement rather than the arrow keys. NOTE: When using Debug Mode, this patch will re-map the conflicting debug keys to the arrow keys.")]
            [DefaultValue(false)]
            public bool UseWASD
            {
                get { return use_wasd; }
                set { use_wasd = value; }
            }

            float turn_max_speed = 20.0F;
            [Category("Controls")]
            [DisplayName("Turning: Max Speed")]
            [Description("Set the maximum turning speed. (Default = 20.0)")]
            [DefaultValue(20.0F)]
            public float TurnMaxSpeed
            {
                get { return turn_max_speed; }
                set { turn_max_speed = value; }
            }

            float turn_max_acceleration = 30.0F;
            [Category("Controls")]
            [DisplayName("Turning: Max Acceleration")]
            [Description("Set the speed at which turning accelerates (requires 'Turning: Enable Velocity') (Default = 30.0)")]
            [DefaultValue(30.0F)]
            public float TurnMaxAcceleration
            {
                get { return turn_max_acceleration; }
                set { turn_max_acceleration = value; }
            }

            float turn_min_acceleration = 15.0F;
            [Category("Controls")]
            [DisplayName("Turning: Min Acceleration")]
            [Description("Set the speed at which turning accelerates (requires 'Turning: Enable Velocity') (Default = 15.0)")]
            [DefaultValue(15.0F)]
            public float TurnMinAcceleration
            {
                get { return turn_min_acceleration; }
                set { turn_min_acceleration = value; }
            }

            float turn_deceleration = 50.0F;
            [Category("Controls")]
            [DisplayName("Turning: Deceleration")]
            [Description("Set the speed at which turning decelerates (requires 'Turning: Enable Velocity') (Default = 50.0)")]
            [DefaultValue(50.0F)]
            public float TurnDeceleration
            {
                get { return turn_deceleration; }
                set { turn_deceleration = value; }
            }

            bool turn_use_velocity = false;
            [Category("Controls")]
            [DisplayName("Turning: Enable Velocity")]
            [Description("By default, LEGO Island ignores the turning acceleration/deceleration values. Set this to TRUE to utilize them (Default = FALSE)")]
            [DefaultValue(false)]
            public bool TurnUseVelocity
            {
                get { return turn_use_velocity; }
                set { turn_use_velocity = value; }
            }

            float movement_max_speed = 40.0F;
            [Category("Controls")]
            [DisplayName("Movement: Max Speed")]
            [Description("Set the movement maximum speed. (Default = 40.0)")]
            [DefaultValue(40.0F)]
            public float MovementMaxSpeed
            {
                get { return movement_max_speed; }
                set { movement_max_speed = value; }
            }

            float movement_max_acceleration = 15.0F;
            [Category("Controls")]
            [DisplayName("Movement: Max Acceleration")]
            [Description("Set the movement acceleration speed (i.e. how long it takes to go from not moving to top speed) (Default = 15.0)")]
            [DefaultValue(15.0F)]
            public float MovementMaxAcceleration
            {
                get { return movement_max_acceleration; }
                set { movement_max_acceleration = value; }
            }

            float movement_min_acceleration = 4.0F;
            [Category("Controls")]
            [DisplayName("Movement: Min Acceleration")]
            [Description("Set the movement acceleration speed (i.e. how long it takes to go from not moving to top speed) (Default = 4.0)")]
            [DefaultValue(4.0F)]
            public float MovementMinAcceleration
            {
                get { return movement_min_acceleration; }
                set { movement_min_acceleration = value; }
            }

            float movement_deceleration = 50.0F;
            [Category("Controls")]
            [DisplayName("Movement: Deceleration")]
            [Description("Set the movement deceleration speed (i.e. how long it takes to slow to a stop after releasing the controls). " +
                "Increase this value to stop faster, decrease it to stop slower. " +
                "Usually this is set to a very high value making deceleration almost instant. (Default = 50.0)")]
            [DefaultValue(50.0F)]
            public float MovementDeceleration
            {
                get { return movement_deceleration; }
                set { movement_deceleration = value; }
            }

            int mouse_deadzone = 40;
            [Category("Controls")]
            [DisplayName("Mouse Deadzone")]
            [Description("Sets the radius from the center of the screen where the mouse will do nothing (40 = default).")]
            [DefaultValue(40)]
            public int MouseDeadzone
            {
                get { return mouse_deadzone; }
                set { mouse_deadzone = value; }
            }

            bool unhook_turn_speed = false;
            [Category("Controls")]
            [DisplayName("Turning: Unhook From Frame Rate")]
            [Description("LEGO Island contains a bug where the turning speed is influenced by the frame rate. Enable this to make the turn speed independent of the frame rate.")]
            [DefaultValue(false)]
            public bool UnhookTurnSpeed
            {
                get { return unhook_turn_speed; }
                set { unhook_turn_speed = value; }
            }

            bool use_joystick = false;
            [Category("Controls")]
            [DisplayName("Use Joystick")]
            [Description("Enables Joystick functionality.")]
            [DefaultValue(false)]
            public bool UseJoystick
            {
                get { return use_joystick; }
                set { use_joystick = value; }
            }

            bool full_screen = true;
            [Category("Graphics")]
            [DisplayName("Run in Full Screen")]
            [Description("Allows you to change modes without administrator privileges and registry editing. NOTE: Windowed mode is NOT compatible with \"Flip Video Memory Pages\".")]
            [DefaultValue(true)]
            public bool FullScreen
            {
                get { return full_screen; }
                set { full_screen = value; }
            }

            bool draw_cursor = false;
            [Category("Graphics")]
            [DisplayName("Draw Cursor")]
            [Description("Renders an in-game cursor, rather than a standard Windows pointer.")]
            [DefaultValue(false)]
            public bool DrawCursor
            {
                get { return draw_cursor; }
                set { draw_cursor = value; }
            }

            bool multiple_instances = false;
            [Category("System")]
            [DisplayName("Allow Multiple Instances")]
            [Description("By default, LEGO Island will allow only one instance of itself to run. " +
                "This patch allows infinite instances of LEGO Island to run.")]
            [DefaultValue(false)]
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
            [DefaultValue(false)]
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
            [DefaultValue(true)]
            public bool RedirectSaveData
            {
                get { return redirect_save_data; }
                set { redirect_save_data = value; }
            }

            FPSLimitType fps_limit_type = FPSLimitType.Default;
            [Category("Graphics")]
            [DisplayName("FPS Cap")]
            [Description("Modify LEGO Island's frame rate cap")]
            [DefaultValue(FPSLimitType.Default)]
            public FPSLimitType FPSLimit
            {
                get { return fps_limit_type; }
                set { fps_limit_type = value; }
            }

            ModelQualityType model_quality = ModelQualityType.Medium;
            [Category("Graphics")]
            [DisplayName("Model Quality")]
            [Description("Change LEGO Island's default model quality")]
            [DefaultValue(ModelQualityType.Medium)]
            public ModelQualityType ModelQuality
            {
                get { return model_quality; }
                set { model_quality = value; }
            }

            float custom_fps_limit = 24.0F;
            [Category("Graphics")]
            [DisplayName("FPS Cap - Custom Limit")]
            [Description("Is 'FPS Cap' is set to 'Limited', this will be the frame rate used.")]
            [DefaultValue(24.0F)]
            public float CustomFPS
            {
                get { return custom_fps_limit; }
                set { custom_fps_limit = value; }
            }

            bool override_resolution = false;
            [Category("Experimental (Use at your own risk)")]
            [DisplayName("Override Resolution")]
            [Description("Override LEGO Island's hardcoded 640x480 resolution with a custom resolution. " +
                "NOTE: This patch is currently incomplete and buggy.")]
            [DefaultValue(false)]
            public bool OverrideResolution
            {
                get { return override_resolution; }
                set { override_resolution = value; }
            }

            int resolution_width = 640;
            [Category("Experimental (Use at your own risk)")]
            [DisplayName("Override Resolution - Width:")]
            [Description("If 'Override Resolution' is enabled, this is the screen resolution width to use instead.")]
            [DefaultValue(640)]
            public int ResolutionWidth
            {
                get { return resolution_width; }
                set { resolution_width = value; }
            }

            int resolution_height = 480;
            [Category("Experimental (Use at your own risk)")]
            [DisplayName("Override Resolution - Height:")]
            [Description("If 'Override Resolution' is enabled, this is the screen resolution height to use instead.")]
            [DefaultValue(480)]
            public int ResolutionHeight
            {
                get { return resolution_height; }
                set { resolution_height = value; }
            }

            bool upscale_bitmaps = false;
            [Category("Experimental (Use at your own risk)")]
            [DisplayName("Override Resolution - Bitmap Upscale")]
            [Description("WARNING: This doesn't upscale the bitmaps' hitboxes yet and can make 2D areas like the Information Center difficult to navigate.")]
            [DefaultValue(false)]
            public bool UpscaleBitmaps
            {
                get { return upscale_bitmaps; }
                set { upscale_bitmaps = value; }
            }

            bool disable_autofinish_building = false;
            [Category("Gameplay")]
            [DisplayName("Disable Auto-Finish Building Section")]
            [Description("In LEGO Island v1.1, placing the last block when building will automatically end the building section. While convenient, " +
                "this prevents players from making any further changes after placing the last brick. It also notably defies what Bill Ding says - you " +
                "don't hit the triangle when you're finished building.\n\nThis patch restores the functionality in v1.0 where placing the last block " +
                "will not automatically finish the build section.")]
            [DefaultValue(false)]
            public bool DisableAutoFinishBuilding
            {
                get { return disable_autofinish_building; }
                set { disable_autofinish_building = value; }
            }

            bool debug_toggle = false;
            [Category("Gameplay")]
            [DisplayName("Debug Mode")]
            [Description("Enables the in-game debug mode automatically without the need to type OGEL.")]
            [DefaultValue(false)]
            public bool DebugToggle
            {
                get { return debug_toggle; }
                set { debug_toggle = value; }
            }

            bool music_toggle = true;
            [Category("Gameplay")]
            [DisplayName("Play Music")]
            [Description("Turns in-game music on or off.")]
            [DefaultValue(true)]
            public bool MusicToggle
            {
                get { return music_toggle; }
                set { music_toggle = value; }
            }

            float fov_multiplier = 0.1F;
            [Category("Graphics")]
            [DisplayName("Field of View Adjustment")]
            [Description("Globally adjusts the field of view by a multiplier\n\n" +
                "0.1 = Default (smaller than 0.1 is more zoomed in, larger than 0.1 is more zoomed out")]
            [DefaultValue(0.1F)]
            public float FOVMultiplier
            {
                get { return fov_multiplier; }
                set { fov_multiplier = value; }
            }
        }

        private PatchList patch_config;

        public class ConfigForm : Form
        {
            private LinkLabel update;

            private TabControl tabs;
            private PropertyGrid patch_view;

            public Button run_button;
            public Button run_additional_button;

            public const string run_button_run = "Run";
            public const string run_button_kill = "Kill";

            public TabPage music_page;

            public ConfigForm(PatchList working_config, MusicInjector music_injector)
            {
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
                patch_view.SelectedObject = working_config;

                // Set up tabs
                tabs = new TabControl();
                tabs.Dock = DockStyle.Fill;

                TabPage patches_page = new TabPage("Patches");
                patches_page.Controls.Add(patch_view);
                tabs.Controls.Add(patches_page);

                music_page = new TabPage("Music");
                music_page.Controls.Add(music_injector);
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
                run_button.Font = new Font(run_button.Font, FontStyle.Bold);
                run_btns.Controls.Add(run_button, 0, 0);

                run_additional_button = new Button();
                run_additional_button.Visible = false;
                run_additional_button.Text = "Run Additional";
                run_additional_button.Anchor = AnchorStyles.Left | AnchorStyles.Right;
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
            }

            private void AuthorLinkClick(object sender, LinkLabelLinkClickedEventArgs e)
            {
                Process.Start("https://www.legoisland.org/");
            }

            private void UpdateLinkClick(object sender, LinkLabelLinkClickedEventArgs e)
            {
                Process.Start("https://www.legoisland.org/rebuilder");
            }
        }

        public ConfigForm form;

        Rebuilder(bool enable_ui) {
            LoadConfig();

            if (enable_ui)
            {
                form = new ConfigForm(patch_config, music_injector);
                form.run_button.Click += new System.EventHandler(this.Run);
                form.run_additional_button.Click += new System.EventHandler(this.RunAdditional);
                form.music_page.Enter += new EventHandler(this.ShowMusicInjectorForm);
                form.FormClosing += new FormClosingEventHandler(this.OnClosing);
            }
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

        public static RegistryKey GetGameRegistryKey()
        {
            RegistryKey src = Registry.LocalMachine.OpenSubKey("SOFTWARE\\Mindscape\\LEGO Island", false);
            
            if (src == null)
            {
                src = Registry.LocalMachine.OpenSubKey("SOFTWARE\\WOW6432Node\\Mindscape\\LEGO Island", false);
            }

            return src;
        }

        public static string GetRegistryEntry(string key)
        {
            using (RegistryKey reg = GetGameRegistryKey())
            {
                if (reg != null)
                {
                    object o = reg.GetValue(key);

                    if (o != null)
                    {
                        return o.ToString();
                    }
                }
            }

            return null;
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
                    Log("Unknown version: " + final_hash);

                    if (MessageBox.Show("The version of LEGO Island you have installed is unknown to Rebuilder. This may result in unpredictable behavior. Would you like to continue?\n\n"
                        + "Your version is: " + final_hash,
                        "Unknown Version",
                        MessageBoxButtons.YesNo,
                        MessageBoxIcon.Warning) == DialogResult.Yes)
                    {
                        return Version.kEnglish11;
                    }
                }

                return v;
            }
        }

        private bool Patch(string source_dir, string dir)
        {
            string incompatibilities = "";

            string isleexe_url = Path.Combine(dir, "ISLE.EXE");
            string lego1dll_url = Path.Combine(dir, "LEGO1.DLL");

            Version version = DetermineVersion(lego1dll_url);

            Log("Found version: " + version);

            if (version == Version.kUnknown)
            {
                return false;
            }

            using (FileStream lego1dll = File.Open(lego1dll_url, FileMode.Open, FileAccess.ReadWrite))
            using (FileStream isleexe = File.Open(isleexe_url, FileMode.Open, FileAccess.ReadWrite))
            {
                long nav_offset, fov_offset_1, fov_offset_2, turn_speed_routine_loc, dsoundoffs1,
                     dsoundoffs2, dsoundoffs3, remove_fps_limit, jukebox_path_offset, model_quality_offset,
                     debug_toggle_offset;

                switch (version) {
                case Version.kEnglish10:
                    nav_offset = 0xF2C28;
                    fov_offset_1 = 0xA1D67;
                    fov_offset_2 = 0xA1D32;
                    turn_speed_routine_loc = 0x54258;
                    dsoundoffs1 = 0xB48FB;
                    dsoundoffs2 = 0xB48F1;
                    dsoundoffs3 = 0xAD7D3;
                    remove_fps_limit = 0x7A68B;
                    jukebox_path_offset = 0xD28F6;
                    model_quality_offset = 0xFF028;
                    debug_toggle_offset = 0x54C1F;
                    break;
                case Version.kEnglish11:
                default:
                    nav_offset = 0xF3228;
                    fov_offset_1 = 0xA22D7;
                    fov_offset_2 = 0xA22A2;
                    turn_speed_routine_loc = 0x544F8;
                    dsoundoffs1 = 0xB120B;
                    dsoundoffs2 = 0xB1201;
                    dsoundoffs3 = 0xADD43;
                    remove_fps_limit = 0x7ABAB;
                    jukebox_path_offset = 0xD2E66;
                    model_quality_offset = 0xFF648;
                    debug_toggle_offset = 0x54EBF;
                    break;
                case Version.kGerman11:
                    nav_offset = 0xF3428;
                    fov_offset_1 = 0xA2517;
                    fov_offset_2 = 0xA24E2;
                    turn_speed_routine_loc = 0x544F8;
                    dsoundoffs1 = 0xB144B;
                    dsoundoffs2 = 0xB1441;
                    dsoundoffs3 = 0xADF83;
                    remove_fps_limit = 0x7AD9B;
                    jukebox_path_offset = 0xD30A6;
                    model_quality_offset = 0xFF878;
                    debug_toggle_offset = 0x54EBF;
                    break;
                case Version.kDanish11:
                    nav_offset = 0xF3428;
                    fov_offset_1 = 0xA24C7;
                    fov_offset_2 = 0xA2492;
                    turn_speed_routine_loc = 0x544F8;
                    dsoundoffs1 = 0xB13FB;
                    dsoundoffs2 = 0xB13F1;
                    dsoundoffs3 = 0xADF33;
                    remove_fps_limit = 0x7AD5B;
                    jukebox_path_offset = 0xD3056;
                    model_quality_offset = 0xFF868;
                    debug_toggle_offset = 0x54EBF;
                    break;
                case Version.kSpanish11:
                    nav_offset = 0xF3228;
                    fov_offset_1 = 0xA2407;
                    fov_offset_2 = 0xA23D2;
                    turn_speed_routine_loc = 0x544F8;
                    dsoundoffs1 = 0xB133B;
                    dsoundoffs2 = 0xB1331;
                    dsoundoffs3 = 0xADE73;
                    remove_fps_limit = 0x7ACBB;
                    jukebox_path_offset = 0xD2F96;
                    model_quality_offset = 0xFF658;
                    debug_toggle_offset = 0x54EBF;
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
                    // Write routine to use frame delta time to adjust the turn speed
                    Write(lego1dll, new byte[] { 0xD9, 0x46, 0x24, 0xD8, 0x4C, 0x24, 0x14, 0xD8, 0x4E, 0x34 }, turn_speed_routine_loc);

                    // Frees up 26 bytes
                    WriteManyBytes(lego1dll, 0x90, 26);
                }

                if (patch_config.StayActiveWhenDefocused)
                {
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

                    Uri uri2 = new Uri(Path.Combine(source_dir, "ISLE.EXE"));
                    Uri relative = uri2.MakeRelativeUri(uri1);
                    string jukebox_path = "\\" + Uri.UnescapeDataString(relative.ToString()).Replace("/", "\\");

                    switch (version) {
                    case Version.kEnglish10:
                        WriteByte(lego1dll, 0xF6, 0x51EF5);
                        WriteByte(lego1dll, 0x34);
                        WriteByte(lego1dll, 0x0D);
                        WriteByte(lego1dll, 0x10);
                        break;
                    case Version.kEnglish11:
                    default:
                        WriteByte(lego1dll, 0x66, 0x52195);
                        WriteByte(lego1dll, 0x3A);
                        WriteByte(lego1dll, 0x0D);
                        WriteByte(lego1dll, 0x10);
                        break;
                    case Version.kGerman11:
                        WriteByte(lego1dll, 0xA6, 0x52195);
                        WriteByte(lego1dll, 0x3C);
                        WriteByte(lego1dll, 0x0D);
                        WriteByte(lego1dll, 0x10);
                        break;
                    case Version.kDanish11:
                        WriteByte(lego1dll, 0x56, 0x52195);
                        WriteByte(lego1dll, 0x3C);
                        WriteByte(lego1dll, 0x0D);
                        WriteByte(lego1dll, 0x10);
                        break;
                    case Version.kSpanish11:
                        WriteByte(lego1dll, 0x96, 0x52195);
                        WriteByte(lego1dll, 0x3B);
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
                    Int32 delay = (Int32) Math.Round(1000.0F / patch_config.CustomFPS);

                    WriteInt32(isleexe, delay, 0x4B4);
                }
                if (patch_config.FPSLimit != FPSLimitType.Default)
                {
                    // Disables 30 FPS limit in Information Center when using software mode
                    WriteManyBytes(lego1dll, 0x90, 8, remove_fps_limit);
                }

                switch (patch_config.ModelQuality) {
                case ModelQualityType.Low:
                    WriteFloat(lego1dll, 0.0f, model_quality_offset);
                    break;
                case ModelQualityType.Medium:
                    WriteFloat(lego1dll, 3.6f, model_quality_offset);
                    break;
                case ModelQualityType.High:
                    WriteFloat(lego1dll, 5.0f, model_quality_offset);
                    break;
                case ModelQualityType.Infinite:
                    WriteFloat(lego1dll, float.MaxValue, model_quality_offset);
                    break;
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

                if (patch_config.UseWASD) // WASD Keycodes Patch
                {
                    switch (version)
                    {
                        case Version.kEnglish10:
                            WriteByte(lego1dll, 0x02, 0x55B20); // We write to the debug mode table here to prevent the keys from colliding
                            lego1dll.Position += 1;
                            WriteByte(lego1dll, 0x04);
                            WriteByte(lego1dll, 0x0F);
                            lego1dll.Position += 24;
                            WriteByte(lego1dll, 0x18);
                            lego1dll.Position += 2;
                            WriteByte(lego1dll, 0x18);
                            lego1dll.Position += 14;
                            WriteByte(lego1dll, 0x18);
                            WriteByte(lego1dll, 0x41, 0x5B2EF); // Actual movement keycodes patch starts here
                            lego1dll.Position += 38;
                            WriteByte(lego1dll, 0x44);
                            lego1dll.Position += 18;
                            WriteByte(lego1dll, 0xA6);
                            WriteByte(lego1dll, 0x00);
                            lego1dll.Position += 25;
                            WriteByte(lego1dll, 0xB4);
                            WriteByte(lego1dll, 0x00);
                            lego1dll.Position += 17;
                            WriteByte(lego1dll, 0xB3);
                            WriteByte(lego1dll, 0x00);
                            lego1dll.Position += 17;
                            WriteByte(lego1dll, 0xB5);
                            WriteByte(lego1dll, 0x00);
                            break;
                        default: // German, Spanish, Danish and English 1.1 have identical offsets for this patch
                            WriteByte(lego1dll, 0x02, 0x55DC0);
                            lego1dll.Position += 1;
                            WriteByte(lego1dll, 0x04);
                            WriteByte(lego1dll, 0x0F);
                            lego1dll.Position += 24;
                            WriteByte(lego1dll, 0x18);
                            lego1dll.Position += 2;
                            WriteByte(lego1dll, 0x18);
                            lego1dll.Position += 14;
                            WriteByte(lego1dll, 0x18);
                            WriteByte(lego1dll, 0x41, 0x5B58F);
                            lego1dll.Position += 38;
                            WriteByte(lego1dll, 0x44);
                            lego1dll.Position += 18;
                            WriteByte(lego1dll, 0xA6);
                            WriteByte(lego1dll, 0x00);
                            lego1dll.Position += 25;
                            WriteByte(lego1dll, 0xB4);
                            WriteByte(lego1dll, 0x00);
                            lego1dll.Position += 17;
                            WriteByte(lego1dll, 0xB3);
                            WriteByte(lego1dll, 0x00);
                            lego1dll.Position += 17;
                            WriteByte(lego1dll, 0xB5);
                            WriteByte(lego1dll, 0x00);
                            break;
                    }
                }

                if (patch_config.DebugToggle)
                {
                WriteByte(lego1dll, 0xEB, debug_toggle_offset);
                }

                if (patch_config.DisableAutoFinishBuilding)
                {
                    if (version == Version.kEnglish10)
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

            using (RegistryKey src = GetGameRegistryKey())
            {
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

                        // Set draw cursor value
                        dst.SetValue("Draw Cursor", patch_config.DrawCursor ? "YES" : "NO");

                        // Set Joystick toggle
                        dst.SetValue("UseJoystick", patch_config.UseJoystick ? "YES" : "NO");

                        // Toggle music on or off
                        dst.SetValue("Music", patch_config.MusicToggle ? "YES" : "NO");

                        // Redirect save path
                        if (patch_config.RedirectSaveData)
                        {
                            string new_save_dir = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + "\\LEGO Island\\";
                            Directory.CreateDirectory(new_save_dir);
                            dst.SetValue("savepath", new_save_dir);
                        }

                        // Make sure to warn the user when running windowed mode and Direct3D HAL together
                        if (!patch_config.FullScreen)
                        {
                            if ((string)dst.GetValue("Flip Surfaces") == "YES")
                            {
                                return (MessageBox.Show("It looks like LEGO Island is configured to Flip Video Memory Pages. This setting is incompatible with LEGO Island's windowed mode. You must either uncheck this setting in the LEGO Island configuration, or switch to a purely software based mode (like RGB Emulation) to use windowed mode.", "Invalid Settings Detected", MessageBoxButtons.OK, MessageBoxIcon.Warning) == DialogResult.Yes);
                            }
                        }
                    }
                }
            }

            return string.IsNullOrEmpty(incompatibilities) || IncompatibleBuildMessage(incompatibilities);
        }

        private bool IsValidDir(string dir)
        {
            return (File.Exists(Path.Combine(dir, "ISLE.EXE")) && File.Exists(Path.Combine(dir, "LEGO1.DLL")));
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

        static public void Log(string text)
        {
            string log_path = Path.Combine(Path.GetTempPath(), "legoislandrebuilder.log");

            using (StreamWriter log = new StreamWriter(log_path, true))
            {
                log.WriteLine("[" + DateTime.Now + "] " + text);
                log.Close();
            }
        }

        public void LaunchGame()
        {
            // If the game is already running, we assume this button is a "Kill" button
            if (processes.Count > 0)
            {
                foreach (Process p in processes)
                {
                    p.Kill();
                }
                processes.Clear();
                return;
            }

            // Create temp path
            string temp_path = Path.Combine(Path.GetTempPath(), "LEGOIslandRebuilder");

            Log("Using working directory: " + temp_path);

            if (Directory.Exists(temp_path))
            {
                Log("Working directory already exists, no need to create");
            }
            else
            {
                try
                {
                    Directory.CreateDirectory(temp_path);

                    Log("Working directory created successfully");
                }
                catch (Exception e)
                {
                    Log("Failed to create working directory: " + e.ToString());

                    MessageBox.Show(
                        "Failed to create temporary path: " + e.ToString(),
                        "Failed to patch files",
                        MessageBoxButtons.OK,
                        MessageBoxIcon.Error
                    );

                    return;
                }
            }

            // Search for game directory
            Log("Searching for game directory...");

            // Create directory string
            string dir = "";

            // Check registry for disk path ()
            if (string.IsNullOrEmpty(dir))
            {
                dir = GetRegistryEntry("diskpath");
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
                            Log("Failed to find game directory, user cancelled prompt.");
                            return;
                        }
                    }
                }
            }

            Log("Found game directory: " + dir);

            try
            {
                string[] dest_files = Directory.GetFiles(temp_path);
                for (int i = 0; i < dest_files.Length; i++)
                {
                    Log("Deleting existing file: " + dest_files[i]);
                    File.SetAttributes(dest_files[i], FileAttributes.Normal);
                    File.Delete(dest_files[i]);
                }
            }
            catch (Exception e)
            {
                Log("Failed to delete old files: " + e.ToString());
                MessageBox.Show("Failed to delete old files: " + e.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            try
            {
                string[] src_files = Directory.GetFiles(dir);
                for (int i=0;i< src_files.Length;i++)
                {
                    Log("Copying game file: " + Path.GetFileName(src_files[i]));
                    File.Copy(src_files[i], Path.Combine(temp_path, Path.GetFileName(src_files[i])), true);
                }
            }
            catch (Exception e)
            {
                Log("Failed to copy files for patching: " + e.ToString());
                MessageBox.Show("Failed to copy files for patching: " + e.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            // Perform music insertion if necessary
            if (music_injector.ReplaceCount() > 0)
            {
                jukebox_output = Path.Combine(dir, "LEGO/Scripts/REJUKEBOX.SI");

                Log("Attempting to create injected jukebox file: " + jukebox_output);

                try
                {
                    using (FileStream test_fs = new FileStream(jukebox_output, FileMode.Create, FileAccess.Write))
                    {
                        test_fs.Close();
                    }
                }
                catch
                {
                    jukebox_output = Path.Combine(Path.GetTempPath(), "REJUKEBOX.SI");
                    Log("Unable to use previous jukebox path, using alternative: " + jukebox_output);
                }

                music_injector.Insert(jukebox_output);
            }

            Log("Patching files");
            if (!Patch(dir, temp_path)) return;

            // Set new EXE's compatibility mode to 256-colors
            Log("Creating compatibility registry keys");
            using (RegistryKey key = Registry.CurrentUser.OpenSubKey("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers", true))
            {
                if (key != null)
                {
                    string compat_string = "DWM8And16BitMitigation";
                    
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

            Log("Launching game...");
            ProcessStartInfo start_info = new ProcessStartInfo(Path.Combine(temp_path, "ISLE.EXE"));
            start_info.WorkingDirectory = dir;

            try
            {
                Process p = Process.Start(start_info);
                p.EnableRaisingEvents = true;
                p.Exited += new EventHandler(ProcessExit);
                processes.Add(p);

                form.run_button.Text = ConfigForm.run_button_kill;
                if (patch_config.MultipleInstances)
                {
                    form.run_additional_button.Visible = true;
                }

                Log("Game launched successfully");
            }
            catch (Exception e)
            {
                Log("Failed to launch LEGO Island: " + e.ToString());
                MessageBox.Show(
                    "Failed to launch LEGO Island: " + e.ToString(),
                    "Failed to launch",
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Error
                );
                return;
            }
        }

        private void Run(object sender, EventArgs e)
        {
            LaunchGame();
        }

        private void ProcessExit(object sender, EventArgs e)
        {
            form.run_button.BeginInvoke((MethodInvoker)delegate () {
                form.run_button.Text = ConfigForm.run_button_run;
                form.run_additional_button.Visible = false;
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

        private string GetSettingsDir()
        {
            string settings_path = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "LEGOIslandRebuilder");

            Directory.CreateDirectory(settings_path);

            return settings_path;
        }

        private string GetSettingsPath()
        {
            return Path.Combine(GetSettingsDir(), "settings.xml");
        }

        private string GetMusicSettingsPath()
        {
            return Path.Combine(GetSettingsDir(), "music.xml");
        }

        private void LoadConfig()
        {
            string settings_path = GetSettingsPath();

            bool config_loaded = false;

            // Load patch data
            if (File.Exists(settings_path))
            {
                try
                {
                    XmlSerializer serializer = new XmlSerializer(typeof(PatchList));
                    TextReader reader = new StreamReader(settings_path);
                    patch_config = (PatchList)serializer.Deserialize(reader);
                    config_loaded = true;
                    reader.Close();
                }
                catch (InvalidOperationException) { }
            }

            if (!config_loaded)
            {
                patch_config = new PatchList();
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

            bool autorun = false;
            
            // Parse command line options
            string[] arguments = Environment.GetCommandLineArgs();
            foreach (string a in arguments)
            {
                string lower_arg = a.ToLower();

                switch (a)
                {
                    case "--help":
                    case "-h":
                        MessageBox.Show(
                            "LEGO Island Rebuilder\n\nSupported arguments:\n\n--help, -h\nDisplays this help page.\n\n--run, -r\nRun LEGO Island immediately with the last used configuration.",
                            "Command Line Argument Help",
                            MessageBoxButtons.OK,
                            MessageBoxIcon.Information
                        );
                        
                        Environment.Exit(0);
                        break;
                    case "--run":
                    case "-r":
                        autorun = true;
                        break;
                }
            }

            Rebuilder instance = new Rebuilder(!autorun);

            if (autorun)
            {
                instance.LaunchGame();
            }
            else
            {
                Application.Run(instance.form);
            }
        }
    }
}
