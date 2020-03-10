using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Drawing;
using System.Media;
using System.Xml;
using Microsoft.Win32;

namespace Rebuilder
{
    public class MusicInjector : DataGridView
    {
        string jukebox_si_path = "";
        string temp_extract_path = "";

        List<string> filenames = new List<string>();

        SoundPlayer sound_player = new SoundPlayer();
        int last_played_row = -1;

        List<string> replace_src = new List<string>();
        List<string> replace_dst = new List<string>();

        public MusicInjector()
        {
            /*Text = "Music Injector";
            Icon = Icon.ExtractAssociatedIcon(System.Reflection.Assembly.GetExecutingAssembly().Location);
            Visible = false;
            Width = 720;
            Height = 540;
            CenterToScreen();*/

            CellClick += new DataGridViewCellEventHandler(TableCellClicked);

            Columns.Add("name", "Name");
            Columns.Add("filename", "Filename");

            // Create Extract button column
            DataGridViewButtonColumn play_col = new DataGridViewButtonColumn();
            play_col.AutoSizeMode = DataGridViewAutoSizeColumnMode.AllCells;
            play_col.HeaderText = "Play";
            play_col.Text = "Play";
            play_col.Name = "play";
            play_col.UseColumnTextForButtonValue = true;
            Columns.Add(play_col);

            // Create Extract button column
            DataGridViewButtonColumn extract_col = new DataGridViewButtonColumn();
            extract_col.AutoSizeMode = DataGridViewAutoSizeColumnMode.AllCells;
            extract_col.HeaderText = "Extract";
            extract_col.Text = "Extract";
            extract_col.Name = "extract";
            extract_col.UseColumnTextForButtonValue = true;
            Columns.Add(extract_col);

            // Create Replace button column
            DataGridViewButtonColumn replace_col = new DataGridViewButtonColumn();
            replace_col.AutoSizeMode = DataGridViewAutoSizeColumnMode.AllCells;
            replace_col.HeaderText = "Replace";
            replace_col.Text = "Replace";
            replace_col.Name = "replace";
            replace_col.UseColumnTextForButtonValue = true;
            Columns.Add(replace_col);

            foreach (DataGridViewColumn col in Columns)
            {
                col.SortMode = DataGridViewColumnSortMode.NotSortable;
            }

            Dock = DockStyle.Fill;

            ReadOnly = true;
            RowHeadersVisible = false;
            AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill;
            AllowUserToResizeRows = false;
        }

        public bool Prepare()
        {
            if (string.IsNullOrEmpty(jukebox_si_path))
            {
                FindJukeboxSI();

                if (string.IsNullOrEmpty(jukebox_si_path))
                {
                    MessageBox.Show(
                                        "Rebuilder needs access to JUKEBOX.SI to inject music.",
                                        "Failed to find critical files",
                                        MessageBoxButtons.OK,
                                        MessageBoxIcon.Error
                                    );
                    return false;
                }
                else
                {
                    Extract();
                }
            }
            return true;
        }

        private void FindJukeboxSI()
        {
            // Test standard directories
            List<string> test_dirs = new List<string>();

            test_dirs.AddRange(Rebuilder.standard_hdd_dirs);

            // Check registry for CD path
            {
                string p = Rebuilder.GetRegistryEntry("diskpath");

                if (!string.IsNullOrEmpty(p))
                {
                    test_dirs.Add(p);
                }

                p = Rebuilder.GetRegistryEntry("cdpath");

                if (!string.IsNullOrEmpty(p))
                {
                    test_dirs.Add(p);
                }
            }

            // Loop through drive letters
            for (int i=65;i<91;i++)
            {
                string drive_letter = ((char)i).ToString() + ":";
                test_dirs.Add(drive_letter);
            }

            for (int i=0;i<test_dirs.Count;i++)
            {
                string test_fn = test_dirs[i] + "/LEGO/Scripts/ISLE/JUKEBOX.SI";
                if (File.Exists(test_fn))
                {
                    jukebox_si_path = test_fn;                    
                    return;
                }
            }

            // Fallback to asking
            using (OpenFileDialog ofd = new OpenFileDialog())
            {
                ofd.Filter = "JUKEBOX.SI|JUKEBOX.SI";
                ofd.Title = "Where is JUKEBOX.SI?";

                if (ofd.ShowDialog() == DialogResult.OK)
                {
                    jukebox_si_path = ofd.FileName;
                }
            }
        }

        private bool SeekUntil(FileStream stream, string str, long limit = -1)
        {
            byte[] b = new byte[str.Length];

            int read_byte;

            // Read until 
            while ((read_byte = stream.ReadByte()) != -1 && (limit == -1 || stream.Position < limit))
            {
                // Push bytes along
                for (int i = 1; i < b.Length; i++)
                {
                    b[i - 1] = b[i];
                }

                // Set last byte to the newest byte
                b[b.Length - 1] = (byte)read_byte;

                // See if byte array matches string
                if (System.Text.Encoding.ASCII.GetString(b) == str)
                {
                    return true;
                }
            }

            return false;
        }

        private string BacktrackToFindWAVFilename(FileStream stream)
        {
            string wav_fn = "";
            long current_pos = stream.Position;
            do
            {
                stream.Position -= 2;
            } while ((char)stream.ReadByte() != '\\');

            int wav_fn_char;
            while ((wav_fn_char = stream.ReadByte()) != 0)
            {
                wav_fn += (char)wav_fn_char;
            }
            stream.Position = current_pos;
            return wav_fn;
        }

        private void TableCellClicked(object sender, DataGridViewCellEventArgs e)
        {
            if (e.RowIndex < 0 || e.RowIndex >= filenames.Count)
            {
                return;
            }

            string fn = filenames[e.RowIndex];
            string full_path = temp_extract_path + "/" + fn;

            switch (e.ColumnIndex)
            {
                case 2: // PLAY
                    if (last_played_row == e.RowIndex)
                    {
                        sound_player.Stop();
                        last_played_row = -1;
                    }
                    else
                    {
                        last_played_row = e.RowIndex;

                        string cell_fn = Rows[e.RowIndex].Cells[1].Value.ToString();
                        string play_fn;
                        if (cell_fn == fn)
                        {
                            play_fn = full_path;
                        }
                        else
                        {
                            play_fn = cell_fn;
                        }

                        sound_player.SoundLocation = play_fn;
                        sound_player.Play();                        
                    }
                    break;
                case 3: // EXTRACT
                    using (SaveFileDialog sfd = new SaveFileDialog())
                    {
                        sfd.Title = "Save \"" + fn + "\"";
                        sfd.Filter = "WAVE Audio (*.wav)|*.wav";
                        sfd.FileName = fn;

                        if (sfd.ShowDialog() == DialogResult.OK)
                        {
                            try
                            {
                                File.Copy(full_path, sfd.FileName);
                            }
                            catch
                            {
                                MessageBox.Show(
                                        "Failed to save file. You may not have permission to save to this location.",
                                        "Failed to save",
                                        MessageBoxButtons.OK,
                                        MessageBoxIcon.Error
                                    );
                            }
                        }
                    }
                    break;
                case 4: // REPLACE

                    bool replace = (Rows[e.RowIndex].Cells[1].Value.ToString() == filenames[e.RowIndex]);

                    if (last_played_row == e.RowIndex)
                    {
                        sound_player.Stop();
                        last_played_row = -1;
                    }

                    if (!replace)
                    {
                        if (MessageBox.Show("This audio has already been replaced. Do you want to restore the original audio?",
                            "Replace or Restore",
                            MessageBoxButtons.YesNo,
                            MessageBoxIcon.Question) == DialogResult.No)
                        {
                            replace = true;
                        }
                    }

                    if (replace)
                    {
                        using (OpenFileDialog ofd = new OpenFileDialog())
                        {
                            ofd.Title = "Replace \"" + fn + "\"";
                            ofd.Filter = "WAVE Audio (*.wav)|*.wav";
                            if (ofd.ShowDialog() == DialogResult.OK)
                            {
                                SetCellBold(Rows[e.RowIndex].Cells[0]);
                                SetCellBold(Rows[e.RowIndex].Cells[1]);

                                Rows[e.RowIndex].Cells[1].Value = ofd.FileName;
                            }
                        }
                    }
                    else
                    {
                        SetCellNormal(Rows[e.RowIndex].Cells[0]);
                        SetCellNormal(Rows[e.RowIndex].Cells[1]);

                        Rows[e.RowIndex].Cells[1].Value = filenames[e.RowIndex];
                    }

                    ConvertTableToReplaceData();
                    
                    break;
            }
        }

        private void Extract()
        {
            int file_count = 0;
            temp_extract_path = Path.GetTempPath() + "LEGOIslandRebuilder/jukebox";
            Directory.CreateDirectory(temp_extract_path);

            using (FileStream stream = new FileStream(jukebox_si_path, FileMode.Open, FileAccess.Read))
            {
                byte[] b = new byte[4];

                while (SeekUntil(stream, " WAV"))
                {

                    long pos_before_find_id = stream.Position;

                    // Backtrack to find the filename
                    string wav_fn = BacktrackToFindWAVFilename(stream);

                    file_count++;

                    string wav_path = temp_extract_path + "/" + wav_fn;

                    // Find the ID of the WAV
                    stream.Position = pos_before_find_id;

                    do
                    {
                        stream.Position -= (b.Length + 1);
                        stream.Read(b, 0, b.Length);
                    } while (System.Text.Encoding.ASCII.GetString(b) != "MxOb");
                    stream.Position += 11;
                                        
                    List<byte> wav_name_bytes = new List<byte>();
                    int wav_nbyte;
                    while ((wav_nbyte = stream.ReadByte()) != 0x00) {
                        wav_name_bytes.Add((byte) wav_nbyte);
                    }
                    string wav_name = System.Text.Encoding.ASCII.GetString(wav_name_bytes.ToArray());

                    stream.Read(b, 0, b.Length);
                    Int32 id = BitConverter.ToInt32(b, 0);
                    stream.Position = pos_before_find_id;

                    // Find first LIST which contains the size of this WAVE section
                    SeekUntil(stream, "LIST");
                    stream.Read(b, 0, b.Length);
                    Int32 list_size = BitConverter.ToInt32(b, 0);
                    long list_data_start = stream.Position;
                    long list_end_pos = list_data_start + list_size;

                    // Get WAVE-compatible header position
                    long wave_header_pos = -1;

                    pos_before_find_id = stream.Position;
                    while (true)
                    {
                        // Find first MxCh which contains the WAVE header information
                        SeekUntil(stream, "MxCh");

                        // Check ID
                        stream.Position += 6;
                        stream.Read(b, 0, b.Length);
                        if (BitConverter.ToInt32(b, 0) == id)
                        {
                            wave_header_pos = stream.Position + 8;
                            break;
                        }
                    }
                    stream.Position = pos_before_find_id;

                    

                    //Console.WriteLine("Extracting \"" + wav_fn + "\"");

                    // Get WAVE-compatible header
                    stream.Position = wave_header_pos;
                    byte[] copied_bytes = new byte[16];
                    stream.Read(copied_bytes, 0, 16);

                    Rows.Add(wav_name, wav_fn);
                    filenames.Add(wav_fn);

                    using (FileStream wav_out = new FileStream(wav_path, FileMode.Create, FileAccess.Write))
                    {
                        wav_out.Write(System.Text.Encoding.ASCII.GetBytes("RIFF"), 0, 4);

                        // 4 bytes for file size
                        for (int i = 0; i < 4; i++)
                        {
                            wav_out.WriteByte(0);
                        }

                        wav_out.Write(System.Text.Encoding.ASCII.GetBytes("WAVEfmt "), 0, 8);

                        // Subchunk size (always 16)
                        wav_out.Write(BitConverter.GetBytes(16), 0, 4);

                        // Copy bytes from WAVE header
                        wav_out.Write(copied_bytes, 0, 16);

                        // Data chunk header
                        wav_out.Write(System.Text.Encoding.ASCII.GetBytes("data"), 0, 4);

                        // 4 bytes for data chunk size
                        for (int i = 0; i < 4; i++)
                        {
                            wav_out.WriteByte(0);
                        }

                        // Start reading chunks
                        Int32 data_chunk_size = 0;

                        int chunk_counter = 0;

                        while (SeekUntil(stream, "MxCh", list_end_pos))
                        {
                            // Read chunk size
                            stream.Read(b, 0, b.Length);

                            Int32 mxch_size = BitConverter.ToInt32(b, 0) - 14;

                            // Weird 16-bit thing
                            stream.Read(b, 0, 2);
                            Int16 v1 = BitConverter.ToInt16(b, 0);

                            // ID?
                            stream.Read(b, 0, 4);
                            Int32 v2 = BitConverter.ToInt32(b, 0);

                            // Millis
                            stream.Read(b, 0, 4);
                            Int32 millis = BitConverter.ToInt32(b, 0);

                            // Bytes
                            stream.Read(b, 0, 4);
                            Int32 bytes = BitConverter.ToInt32(b, 0);

                            if (mxch_size > 0 && v2 == id)
                            {
                                byte[] chunk_data = new byte[mxch_size];
                                stream.Read(chunk_data, 0, mxch_size);
                                wav_out.Write(chunk_data, 0, mxch_size);

                                chunk_counter++;

                                data_chunk_size += mxch_size;
                            }
                        }

                        // Fill in file size
                        wav_out.Position = 4;
                        wav_out.Write(BitConverter.GetBytes(wav_out.Length - 8), 0, 4);

                        // Fill in data chunk size
                        wav_out.Position = 40;
                        wav_out.Write(BitConverter.GetBytes(data_chunk_size), 0, 4);
                    }
                }
            }

            // Convert replace data to table
            for (int i=0;i<replace_src.Count;i++)
            {
                int index = filenames.IndexOf(replace_dst[i]);

                Rows[index].Cells[1].Value = replace_src[i];

                SetCellBold(Rows[index].Cells[0]);
                SetCellBold(Rows[index].Cells[1]);
            }

            //Console.WriteLine("\nDone! Extracted " + file_count + " files\n");
        }

        static void SetCellNormal(DataGridViewCell cell)
        {
            cell.Style.Font = new Font(Control.DefaultFont, FontStyle.Regular);
        }

        static void SetCellBold(DataGridViewCell cell)
        {            
            cell.Style.Font = new Font(Control.DefaultFont, FontStyle.Bold);
        }

        static void WriteString(FileStream stream, string text)
        {
            byte[] b = System.Text.Encoding.ASCII.GetBytes(text);
            stream.Write(b, 0, b.Length);
        }

        static void WriteZeroes(FileStream stream, int count)
        {
            for (int i = 0; i < count; i++)
            {
                stream.WriteByte(0);
            }
        }

        static void WriteMxChHeader(FileStream stream, Int32 chunk_size, Int16 flags, Int32 id, UInt32 millis, Int32 actual_data_size)
        {
            // MxChs have to fall on even numbers
            if (stream.Position % 2 == 1)
            {
                stream.WriteByte(0);
            }

            // Write chunk header
            WriteString(stream, "MxCh");

            // Chunk total size
            byte[] write_bytes = BitConverter.GetBytes(chunk_size);
            stream.Write(write_bytes, 0, write_bytes.Length);

            // Write flags
            write_bytes = BitConverter.GetBytes(flags);
            stream.Write(write_bytes, 0, write_bytes.Length);

            // Write ID
            write_bytes = BitConverter.GetBytes(id);
            stream.Write(write_bytes, 0, write_bytes.Length);

            // Write milliseconds
            write_bytes = BitConverter.GetBytes(millis);
            stream.Write(write_bytes, 0, write_bytes.Length);

            // Write total data size
            write_bytes = BitConverter.GetBytes(actual_data_size);
            stream.Write(write_bytes, 0, write_bytes.Length);
        }

        private void ConvertTableToReplaceData()
        {
            replace_src.Clear();
            replace_dst.Clear();

            for (int i = 0; i < filenames.Count; i++)
            {
                if (Rows[i].Cells[1].Value.ToString() != filenames[i])
                {
                    replace_src.Add(Rows[i].Cells[1].Value.ToString());
                    replace_dst.Add(filenames[i]);
                }
            }
        }

        public void Insert(string output_fn)
        {
            if (Prepare())
            {
                Reconstruct(output_fn);
            }
        }

        private void Reconstruct(string output_fn)
        {
            // For inserting, we reconstruct the SI file from scratch

            using (FileStream in_stream = new FileStream(jukebox_si_path, FileMode.Open, FileAccess.Read))
            using (FileStream out_stream = new FileStream(output_fn, FileMode.Create, FileAccess.Write))
            {
                // First we copy the SI header up until the first MxSt since that will be largely identical (though a lot of the variables will change)
                byte[] si_header = new byte[312];
                in_stream.Read(si_header, 0, si_header.Length);
                out_stream.Write(si_header, 0, si_header.Length);

                byte[] b = new byte[4];

                // Next we loop over the in_stream finding "MxSt"s til the end of the file
                while (SeekUntil(in_stream, "MxSt"))
                {
                    // Find this MxSt's original offset location
                    long in_mxst_loc = in_stream.Position - 4;
                    in_stream.Position = 0x2C;
                    long offset_index_loc = -1;
                    while (in_stream.Position < 0x12C)
                    {
                        in_stream.Read(b, 0, b.Length);

                        if (BitConverter.ToInt32(b, 0) == in_mxst_loc)
                        {
                            offset_index_loc = in_stream.Position - 4;
                            break;
                        }
                    }
                    in_stream.Position = in_mxst_loc + 4;

                    // Ensure we got an MxOf index
                    if (offset_index_loc == -1)
                    {
                        Console.WriteLine("FAILED TO FIND MXOF ENTRY for MxSt at " + in_mxst_loc);
                        return;
                    }

                    // Store the new offset
                    long out_mxst_loc = out_stream.Position;
                    out_stream.Position = offset_index_loc;
                    byte[] new_offset_bytes = BitConverter.GetBytes((Int32)out_mxst_loc);
                    out_stream.Write(new_offset_bytes, 0, new_offset_bytes.Length);
                    out_stream.Position = out_mxst_loc;

                    // Write an MxSt in the output stream
                    WriteString(out_stream, "MxSt");

                    // Write 4-bytes to fill in later for the stream length
                    long mxst_length_position = out_stream.Position;
                    WriteZeroes(out_stream, 4);

                    // We expect an MxOb here so we'll write that
                    WriteString(out_stream, "MxOb");

                    // Skip original's MxSt length and MxOb since we don't need it
                    in_stream.Position += 8;

                    // Read MxOb length since we'll be copying that verbatim and copy it to the file
                    in_stream.Read(b, 0, b.Length);
                    Int32 mxob_len = BitConverter.ToInt32(b, 0);
                    out_stream.Write(b, 0, b.Length);

                    // Copy MxOb data
                    long mxob_output_start = out_stream.Position;
                    byte[] mxob_data = new byte[mxob_len];
                    in_stream.Read(mxob_data, 0, mxob_len);
                    out_stream.Write(mxob_data, 0, mxob_len);

                    // Find ID in MxOb data
                    List<string> types = new List<string>();
                    List<Int32> ids = new List<Int32>();
                    List<List<byte[]>> data = new List<List<byte[]>>();
                    Int32 wav_id = -1;
                    int mxob_wav_ms_pos = -1;
                    long wav_ms_pos = -1;
                    Int32 flc_id = -1;
                    int flc_write_count = 1;

                    for (int i = 0; i < mxob_len - 4; i++)
                    {
                        // If this MxOb has sub MxObs, we need to find the WAV one
                        if (System.Text.Encoding.ASCII.GetString(mxob_data, i, 4) == "MxOb" || i == 0)
                        {
                            string t = "";
                            Int32 id = -1;

                            // Find ID
                            for (i += 15; i < mxob_len; i++)
                            {
                                if (mxob_data[i] == 0)
                                {
                                    i++;
                                    id = BitConverter.ToInt32(mxob_data, i);
                                    break;
                                }
                            }

                            i += 12;

                            int ms_pos = i;

                            // Find type
                            for (; i < mxob_len - 4; i++)
                            {
                                string here = System.Text.Encoding.ASCII.GetString(mxob_data, i, 4);
                                if (here == "LIST" || here == " WAV" || here == " FLC")
                                {
                                    if (here == " WAV")
                                    {
                                        wav_id = id;

                                        mxob_wav_ms_pos = ms_pos;
                                        wav_ms_pos = mxob_output_start + ms_pos;
                                    }
                                    else if (here == " FLC")
                                    {
                                        flc_id = id;
                                    }

                                    t = here;
                                    break;
                                }
                            }

                            types.Add(t);
                            ids.Add(id);
                            data.Add(new List<byte[]>());
                        }
                    }

                    // Sanity checking that we found the ID
                    if (ids.Count == 0 || wav_id == -1)
                    {
                        Console.WriteLine("Failed to find MxSt ID");
                        return;
                    }

                    // Sometimes a header has an odd length, but the files are automatically aligned to 2-bytes so we'll write an extra byte here
                    if (mxob_len % 2 == 1)
                    {
                        out_stream.WriteByte((byte)in_stream.ReadByte());
                    }

                    // Find the WAVE specified in in_stream
                    string wav_fn = BacktrackToFindWAVFilename(in_stream);

                    // Here we expect a "LIST" followed by the first "MxCh" which contains the wave header. The LIST contains a size that we'll need to modify
                    WriteString(out_stream, "LIST");
                    in_stream.Position += 4;

                    // LIST size
                    long list_size_pos = out_stream.Position;
                    WriteZeroes(out_stream, 4);

                    // Read file's LIST size so we can quickly skip over it later
                    in_stream.Read(b, 0, b.Length);
                    long end_list_pos = in_stream.Position + BitConverter.ToInt32(b, 0);

                    // Write leading MxDa
                    in_stream.Position += 4;
                    WriteString(out_stream, "MxDa");

                    long wave_header_pos = -1;

                    // Each sub-object will need a leading chunk
                    for (int i = 0; i < ids.Count; i++)
                    {
                        // LIST's leading chunk was part of the MxOb header so we don't need to copy it
                        if (types[i] != "LIST")
                        {
                            // Write "MxCh" header
                            in_stream.Position += 4;
                            WriteString(out_stream, "MxCh");

                            // Read MxCh size
                            in_stream.Read(b, 0, b.Length);
                            Int32 leading_mxch_size = BitConverter.ToInt32(b, 0);
                            out_stream.Write(b, 0, b.Length);

                            // Copy MxCh data
                            // The first MxCh contains 50 bytes of data that we'll just copy
                            byte[] early_mxch_data = new byte[leading_mxch_size];
                            in_stream.Read(early_mxch_data, 0, early_mxch_data.Length);
                            out_stream.Write(early_mxch_data, 0, early_mxch_data.Length);

                            if (types[i] == " WAV")
                            {
                                wave_header_pos = out_stream.Position - 24;
                            }
                        }
                    }

                    const int si_buffer_size = 0x20000;

                    // We don't support replacing the other data at this time, but we'll still need to interleave it
                    if (ids.Count > 1)
                    {
                        //int chcounter = 0;
                        while (SeekUntil(in_stream, "MxCh", end_list_pos))
                        {
                            long chunk_start = in_stream.Position - 4;

                            // Get chunk size just in case we need to copy it
                            in_stream.Read(b, 0, b.Length);
                            Int32 chunk_sz = BitConverter.ToInt32(b, 0);

                            // Find which MxOb this belongs to
                            in_stream.Read(b, 0, 2);
                            Int16 chunk_flags = BitConverter.ToInt16(b, 0);

                            in_stream.Read(b, 0, b.Length);
                            Int32 chunk_id = BitConverter.ToInt32(b, 0);
                            int chunk_index = ids.IndexOf(chunk_id);

                            //Console.WriteLine("  Found chunk with ID " + chunk_id + " " + types[chunk_index] + " " + chunk_sz);

                            if (types[chunk_index] != " WAV" && chunk_sz > 14)
                            {
                                in_stream.Position = chunk_start;

                                byte[] foreign_data = new byte[chunk_sz + 8];
                                in_stream.Read(foreign_data, 0, foreign_data.Length);

                                data[chunk_index].Add(foreign_data);
                            }
                        }
                    }

                    UInt32 millis = 0;
                    UInt32 millisecond_length = 0;

                    string wav_file;

                    int replace_index = replace_dst.IndexOf(wav_fn);
                    if (replace_index == -1)
                    {
                        // We won't replace this file so just use the original
                        wav_file = temp_extract_path + "/" + wav_fn;
                    }
                    else
                    {
                        wav_file = replace_src[replace_index];
                    }

                    using (FileStream wav_str = new FileStream(wav_file, FileMode.Open, FileAccess.Read))
                    {
                        //Console.WriteLine("Inserting " + wav_fn);

                        // Ignore first part of the wav header
                        if (!SeekUntil(wav_str, "WAVE") || !SeekUntil(wav_str, "fmt "))
                        {
                            Console.WriteLine("Invalid file at " + wav_fn);
                            return;
                        }
                        wav_str.Position += 4;

                        long old_pos = out_stream.Position;
                        out_stream.Position = wave_header_pos;

                        // Copy WAV header data
                        byte[] wav_header = new byte[16];
                        wav_str.Read(wav_header, 0, wav_header.Length);
                        out_stream.Write(wav_header, 0, wav_header.Length);

                        // Total data size in MxCh is the WAV's data size + 4
                        if (!SeekUntil(wav_str, "data"))
                        {
                            Console.WriteLine("Invalid file at " + wav_fn);
                            return;
                        }
                        byte[] data_size_bytes = new byte[4];
                        wav_str.Read(data_size_bytes, 0, data_size_bytes.Length);
                        Int32 wav_data_size = (BitConverter.ToInt32(data_size_bytes, 0));
                        data_size_bytes = BitConverter.GetBytes((Int32)(wav_data_size + 4));
                        out_stream.Write(data_size_bytes, 0, data_size_bytes.Length);
                        long wav_end = wav_str.Position + wav_data_size;

                        Int32 data_rate_per_second = BitConverter.ToInt32(wav_header, 8);

                        millisecond_length = (UInt32) Math.Round((double)wav_data_size * 1000.0 / (double)data_rate_per_second);

                        //
                        // Write WAV's millisecond length and loop count
                        //
                        // LEGO Island's milliseconds are multiplied by the loop count, so we do that here (but we need to make sure the number doesn't overflow an Int32)
                        //

                        UInt32 loop_count = BitConverter.ToUInt32(mxob_data, mxob_wav_ms_pos + 4);
                        UInt64 effective_ms = millisecond_length * loop_count;

                        // If the value is too large for a 32-bit integer, resize it to fit
                        if (effective_ms > UInt32.MaxValue)
                        {
                            loop_count = UInt32.MaxValue / millisecond_length;
                            effective_ms = millisecond_length * loop_count;
                        }

                        // Write ms and loop values
                        out_stream.Position = wav_ms_pos + 4;
                        data_size_bytes = BitConverter.GetBytes((UInt32)effective_ms);
                        out_stream.Write(data_size_bytes, 0, data_size_bytes.Length);
                        data_size_bytes = BitConverter.GetBytes(loop_count);
                        out_stream.Write(data_size_bytes, 0, data_size_bytes.Length);

                        // Re-skip latter 4 bytes of MxCh header
                        out_stream.Position = old_pos;

                        // Write first FLC chunk
                        /*
                        if (flc_id != -1)
                        {
                            int index = ids.IndexOf(flc_id);
                            int remainder = (int)(si_buffer_size - (out_stream.Position % si_buffer_size));

                            if (data[index][0].Length > remainder)
                            {
                                WriteZeroes(out_stream, (int)remainder);
                            }

                            out_stream.Write(data[index][0], 0, data[index][0].Length);
                        }
                        */

                        // Loop over data in WAV
                        // Write out WAV file chunking along the way
                        int max_wav_data = BitConverter.ToInt32(wav_header, 8);
                        int max_chunk_size = max_wav_data + 22;
                        while (wav_str.Position < wav_end)
                        {
                            // SI files are also buffered at 20000h (or 131072 bytes), so we can only write that much at a time
                            Int32 this_chunk_max = (Int32)Math.Min(max_chunk_size, si_buffer_size - (out_stream.Position % si_buffer_size));

                            if (this_chunk_max <= 22)
                            {
                                WriteZeroes(out_stream, this_chunk_max);
                            }
                            else
                            {
                                Int16 flags = 0;
                                if (this_chunk_max < max_chunk_size)
                                {
                                    flags = 16;
                                }

                                this_chunk_max -= 8;

                                int actual_wav_chunk_size = this_chunk_max - 14;

                                int actual_data_chunk_size = max_wav_data;

                                if (wav_str.Position + actual_wav_chunk_size > wav_end)
                                {
                                    flags = 0;
                                    actual_wav_chunk_size = (int)(wav_end - wav_str.Position);
                                    this_chunk_max = actual_wav_chunk_size + 14;
                                    actual_data_chunk_size = actual_wav_chunk_size;
                                }

                                WriteMxChHeader(out_stream, this_chunk_max, flags, wav_id, millis, actual_data_chunk_size);

                                byte[] wav_bytes = new byte[actual_wav_chunk_size];
                                wav_str.Read(wav_bytes, 0, actual_wav_chunk_size);
                                out_stream.Write(wav_bytes, 0, actual_wav_chunk_size);

                                // If we had to split this chunk in half, write the second half now
                                if (flags == 16)
                                {
                                    int remaining_wav_data = max_wav_data - actual_wav_chunk_size;

                                    while (remaining_wav_data > 0)
                                    {
                                        int true_max_chunk_size = Math.Min(remaining_wav_data + 22, si_buffer_size);
                                        int true_wav_data_size = true_max_chunk_size - 22;

                                        WriteMxChHeader(out_stream, true_max_chunk_size - 8, flags, wav_id, millis, true_wav_data_size);

                                        byte[] joining_wav_bytes = new byte[true_wav_data_size];
                                        wav_str.Read(joining_wav_bytes, 0, true_wav_data_size);
                                        out_stream.Write(joining_wav_bytes, 0, true_wav_data_size);

                                        remaining_wav_data -= true_wav_data_size;
                                    }
                                }

                                millis += 1000;

                                // If we have FLC chunks
                                /*
                                if (flc_id != -1 && wav_str.Position < wav_end)
                                {
                                    // Write 10 FLC chunks
                                    for (int i=0;i<10;i++)
                                    {
                                        int flc_data_index = ids.IndexOf(flc_id);
                                        int flc_write_ind = (flc_write_count*2) % data[flc_data_index].Count;

                                        //if (flc_write_ind >= data[flc_data_index].Count)
                                        //{
                                            //break;
                                        //}

                                        long remainder = si_buffer_size - (out_stream.Position % si_buffer_size);

                                        if (data[flc_data_index][flc_write_ind].Length > remainder)
                                        {
                                            WriteZeroes(out_stream, (int)remainder);
                                        }

                                        out_stream.Write(data[flc_data_index][flc_write_ind], 0, data[flc_data_index][flc_write_ind].Length);

                                        flc_write_count++;
                                    }                                        
                                }
                                */
                            }
                        }
                    }

                    // Write ending chunk                    
                    for (int i = ids.Count - 1; i >= 0; i--)
                    {
                        WriteMxChHeader(out_stream, 14, 2, ids[i], millisecond_length, 0);
                    }

                    // Write correct LIST and MxSt size
                    Int32 list_size = (Int32)(out_stream.Position - list_size_pos - 4);
                    Int32 mxst_size = (Int32)(out_stream.Position - mxst_length_position - 4);

                    out_stream.Position = list_size_pos;
                    byte[] list_size_bytes = BitConverter.GetBytes(list_size);
                    out_stream.Write(list_size_bytes, 0, list_size_bytes.Length);

                    out_stream.Position = mxst_length_position;
                    list_size_bytes = BitConverter.GetBytes(mxst_size);
                    out_stream.Write(list_size_bytes, 0, list_size_bytes.Length);

                    in_stream.Position = end_list_pos;
                    out_stream.Position = out_stream.Length;
                }

                // Correct RIFF size
                out_stream.Position = 4;
                byte[] size_bytes = BitConverter.GetBytes((Int32)(out_stream.Length - 8));
                out_stream.Write(size_bytes, 0, size_bytes.Length);

                // Correct first LIST size
                out_stream.Position = 304;
                size_bytes = BitConverter.GetBytes((Int32)(out_stream.Length - 308));
                out_stream.Write(size_bytes, 0, size_bytes.Length);
            }
        }

        public int ReplaceCount()
        {
            return replace_src.Count;
        }

        public void LoadData(XmlReader stream)
        {
            if (stream.IsEmptyElement)
            {
                return;
            }

            string tag = stream.Name;

            while (stream.Read() && !(stream.Name == tag && stream.NodeType == XmlNodeType.EndElement))
            {
                if (stream.IsStartElement() && stream.Name == "replace")
                {
                    replace_dst.Add(stream.GetAttribute("original"));

                    stream.Read();
                    replace_src.Add(stream.Value);
                }
            }
        }

        public void SaveData(XmlWriter stream)
        {
            for (int i = 0; i < replace_src.Count; i++)
            {
                stream.WriteStartElement("replace");
                stream.WriteAttributeString("original", replace_dst[i]);
                stream.WriteString(replace_src[i]);
                stream.WriteEndElement(); // replace
            }
        }
    }
}
