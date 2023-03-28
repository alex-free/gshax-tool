/*
BSD 3-Clause License
Copyright (c) 2023, Alex Free & MottZilla
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

FILE *exe, *out;

unsigned int mips_assembly_bin_cutoff_offset;
unsigned int current_fpos = 0;
unsigned int bytes_counter = 0;

unsigned int new_instruction;
unsigned int new_instruction_write_val;
unsigned int new_instruction_compare_code;
unsigned int new_instruction_val;

unsigned int original_instruction;
unsigned int original_instruction_write_val;
unsigned int original_instruction_compare_code;
unsigned int original_instruction_address;
unsigned int original_instruction_address_write_val;
unsigned int original_instruction_val;

unsigned short gslite_save_checksum = 0;
unsigned short gslite_save_checksum_checked_bytes_len;

unsigned char *buf;
unsigned char *bytes;
unsigned char *bytes_for_sanity_check;
unsigned char gs_write_byte_1;
unsigned char gs_write_byte_2;
unsigned char gshax1_code_size = 0x80; // Code list counters always start with this offset
unsigned char gshax2_code_size = 0x80;
unsigned char instruction_type;
//unsigned char wipeout[] = { 0x57, 0x49, 0x50, 0x45, 0x4F, 0x55, 0x54}; test 'WIPEOUT' known game title

unsigned char start_card[] = {
  0x53, 0x43, 0x11, 0x00, 0x43, 0x6f, 0x64, 0x65, 0x20, 0x4c, 0x69, 0x73,
  0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x40, 0x00, 0x02, 0x00, 0x42, 0x10, 0x00, 0x10, 0x40,
  0x10, 0x02, 0x10, 0x42, 0x00, 0x00, 0x00, 0x7c, 0xe0, 0x03, 0xe0, 0x7f,
  0x1f, 0x00, 0xff, 0x03, 0x73, 0x4e, 0xff, 0x7f, 0xef, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xfe, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
  0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xef, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0xfe, 0xef, 0x11, 0xee, 0xe1, 0xee, 0xee, 0x11, 0xfe,
  0xef, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0xfe, 0xef, 0x1e, 0xee, 0x1e,
  0xee, 0xee, 0xe1, 0xfe, 0xef, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0xfe,
  0xef, 0x11, 0xee, 0xe1, 0xee, 0xe1, 0x11, 0xfe, 0xef, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xfe, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
  0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xef, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0xfe, 0xef, 0x11, 0xdd, 0xdd, 0x1d, 0xdd, 0x11, 0xfe,
  0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00
};

unsigned char end_card[] = {
  0x06, 0x00, 0x00, 0xd8, 0x05, 0x00, 0x00, 0x00, 0x00, 0x8b, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8b
};

unsigned char gslite_save_start[] = { 0xAD, 0xDE, 0x00, 0x00}; //0x104, 0x105, 0x106, 0x107 in gslite save.

unsigned char gshax1[] = { 0x47, 0x53, 0x48, 0x41, 0x58, 0x31, 0x00, 0x00 }; // 'GSHAX1', 0x00 termination, and number_of_codes for code listing. 0xBE is the maximum at 64 code lines per code 'entry'
unsigned char gshax2[] = { 0x47, 0x53, 0x48, 0x41, 0x58, 0x32, 0x00, 0x00 }; // 'GSHAX2', 0x00 termination, and number_of_codes. number_of_codes is written to later and is hardcoded at 0x28E.

bool large_gs_lite_save = 0;
bool compare_code_1_enable = 1;
bool compare_code_2_enable = 1;

void gen_txt()
{
    if(compare_code_1_enable)
    {
        // first D0 compare code address
        original_instruction_address_write_val = original_instruction_address;
        bytes=(unsigned char *)&original_instruction_address_write_val;
        bytes[3] = 0xD0;
        original_instruction_address_write_val = bytes[0] + (bytes[1] << 8) + (bytes[2] << 16) + (bytes[3] << 24); 
        fprintf(out, "%08X ", original_instruction_address_write_val);

        // original bytes for first compare code address 
        original_instruction_write_val = original_instruction_val;
        bytes=(unsigned char *)&original_instruction_write_val;
        fprintf(out, "%02X", bytes[1]);
        fprintf(out, "%02X\n", bytes[0]);

        // actual first address for first compare code 
        fprintf(out, "%08X ", original_instruction_address);

        // actual new bytes to write at first address
        new_instruction_write_val = new_instruction_val;
        bytes=(unsigned char *)&new_instruction_write_val;
        fprintf(out, "%02X", bytes[1]);
        fprintf(out, "%02X\n", bytes[0]);
    }

    if(compare_code_2_enable)
    {
        // second D0 compare code address
        original_instruction_address_write_val = (original_instruction_address + 2);
        bytes=(unsigned char *)&original_instruction_address_write_val;
        bytes[3] = 0xD0;
        original_instruction_address_write_val = bytes[0] + (bytes[1] << 8) + (bytes[2] << 16) + (bytes[3] << 24);
        fprintf(out, "%08X ", original_instruction_address_write_val);

        // original bytes for second compare code address 
        original_instruction_write_val = original_instruction_val;
        bytes=(unsigned char *)&original_instruction_write_val;
        fprintf(out, "%02X", bytes[3]);
        fprintf(out, "%02X\n", bytes[2]);

        // actual second address for second compare code
        fprintf(out, "%08X ", (original_instruction_address + 2));

        // actual new bytes to write at second address
        new_instruction_write_val = new_instruction_val;
        bytes=(unsigned char *)&new_instruction_write_val;
        fprintf(out, "%02X", bytes[3]);
        fprintf(out, "%02X\n", bytes[2]);
    }

    while(1)
    {
        if(current_fpos == mips_assembly_bin_cutoff_offset)
            break;

        gs_write_byte_1 = buf[current_fpos];
        gs_write_byte_2 = buf[current_fpos + 1];

        fprintf(out, "%08X %02X%02X\n", new_instruction, gs_write_byte_2, gs_write_byte_1);
        current_fpos = (current_fpos + 2);
        new_instruction = (new_instruction + 2);
    }
}

void gen_gslite_save()
{
    for(int i = 0; i < 0x2000; i++)
        fputc(0, out);
    fseek(out, 0, SEEK_SET); // pad to 0x1fff

    for(int i = 0; i < 0x100; i++)       
        fputc(start_card[i], out); // first 0x100 bytes is constant and shared by all saves

    for(int i = 0; i < 4; i++)
        fputc(0x00, out); // save file checksum is written at 0x100 and 0x101. Size of checked area is saved at 0x102 and 0x103.

    for(int i = 0; i < 4; i++)
        fputc(gslite_save_start[i], out); // hardcoded data. Not sure what 0x104 and 0x105 is.

    fputc(0x41, out); // name of GSHAX game title name, we use 'A' since it appears first in the list of games.
    // for(int i = 0; i < 7; i++)
    //   fputc(wipeout[i], out); // test known game title
    
    fputc(0x00, out); // terminate GSHAX game title
    fputc(0x02, out); // number of code entires in the 'A' game, hardcode to 2.
    
    for(int i = 0; i < 8; i++)
        fputc(gshax1[i], out); // name of code 1, 'GSHAX1' with termination (0x00) and zeroed out number_of_codes (will be calculated later)

    if(compare_code_1_enable)
    {
        bytes_counter = 0xC; // Change from 0 to 0xC for 12 bytes to add this first D0 code to the sum of data in this save file (2x 6 byte code lines)
        // first D0 compare code address
        original_instruction_address_write_val = original_instruction_address;
        bytes=(unsigned char *)&original_instruction_address_write_val;
        bytes[3] = 0xD0;
        original_instruction_address_write_val = bytes[3] + (bytes[2] << 8) + (bytes[1] << 16) + (bytes[0] << 24); 
        fwrite(&original_instruction_address_write_val, 1, 4, out);

        // original bytes for first compare code address 
        original_instruction_write_val = original_instruction_val;
        bytes=(unsigned char *)&original_instruction_write_val;
        original_instruction_write_val = bytes[1] + (bytes[0] << 8) + (bytes[2] << 16) + (bytes[3] << 24); 
        fwrite(&original_instruction_write_val, 1, 2, out);

        // actual first address for first compare code
        original_instruction_address_write_val = original_instruction_address;
        bytes=(unsigned char *)&original_instruction_address_write_val;
        original_instruction_address_write_val = bytes[3] + (bytes[2] << 8) + (bytes[1] << 16) + (bytes[0] << 24); 
        fwrite(&original_instruction_address_write_val, 1, 4, out);

        // actual new bytes to write at first address
        new_instruction_write_val = new_instruction_val;
        bytes=(unsigned char *)&new_instruction_write_val;
        new_instruction_write_val = bytes[1] + (bytes[0] << 8) + (bytes[2] << 16) + (bytes[3] << 24); 
        fwrite(&new_instruction_write_val, 1, 2, out);
    }

    if(compare_code_2_enable)
    {
        bytes_counter = (bytes_counter + 0xC); // 12 bytes per complete D0 code change (2x 6 byte lines), may be starting from 0, may be starting from 0xC depending on the first D0 optional code
        // second D0 compare code address
        original_instruction_address_write_val = (original_instruction_address + 2);
        bytes=(unsigned char *)&original_instruction_address_write_val;
        bytes[3] = 0xD0;
        original_instruction_address_write_val = bytes[3] + (bytes[2] << 8) + (bytes[1] << 16) + (bytes[0] << 24); 
        fwrite(&original_instruction_address_write_val, 1, 4, out);

        // original bytes for first compare code address 
        original_instruction_write_val = original_instruction_val;
        bytes=(unsigned char *)&original_instruction_write_val;
        original_instruction_write_val = bytes[3] + (bytes[2] << 8) + (bytes[1] << 16) + (bytes[0] << 24); 
        fwrite(&original_instruction_write_val, 1, 2, out);

        // actual second address for second compare code
        original_instruction_address_write_val = (original_instruction_address + 2);
        bytes=(unsigned char *)&original_instruction_address_write_val;
        original_instruction_address_write_val = bytes[3] + (bytes[2] << 8) + (bytes[1] << 16) + (bytes[0] << 24); 
        fwrite(&original_instruction_address_write_val, 1, 4, out);

        // actual new bytes to write at second address
        new_instruction_write_val = new_instruction_val;
        bytes=(unsigned char *)&new_instruction_write_val;
        new_instruction_write_val = bytes[3] + (bytes[2] << 8) + (bytes[1] << 16) + (bytes[0] << 24); 
        fwrite(&new_instruction_write_val, 1, 2, out);
    }

    while(1)
    {
        if(current_fpos == mips_assembly_bin_cutoff_offset)
            break;

        if(bytes_counter == 0x174) // 64 line limit of codes per 'code entry'.
        {
            gshax1_code_size = (gshax1_code_size + (bytes_counter / 6)); // get number of code lines.  0x174 divided by 6 = 0x3E, which equals the 64 code lines maximum per code entry.
            printf("GSHAX1 Code Size: %02X\n", gshax1_code_size);
            fseek(out, 0x112, SEEK_SET); // hardcoded fpos for GSHAX1 number_of_codes after termination for game title 'A'
            fputc(gshax1_code_size, out);

            fseek(out, 0x287, SEEK_SET); // hardcoded fpos for GSHAX1 number_of_codes after termination for game title 'A'

            for(int i = 0; i < 8; i++)
                fputc(gshax2[i], out); // name of code 2, 'GSHAX2' with termination (0x00) and number_of_codes set to 00 (will be updated at the end)
            bytes_counter = 0; // reset counter to calculate GSHAX2 number_of_codes
            large_gs_lite_save = 1;
        }

        gs_write_byte_1 = buf[current_fpos];
        gs_write_byte_2 = buf[current_fpos + 1];
       // printf("%08X %02X%02X\n", new_instruction, gs_write_byte_2, gs_write_byte_1);

        new_instruction_write_val = new_instruction;
        bytes=(unsigned char *)&new_instruction_write_val;
        new_instruction_write_val = bytes[3] + (bytes[2] << 8) + (bytes[1] << 16) + (bytes[0] << 24); 
        fwrite(&new_instruction_write_val, 1, 4, out);
        fwrite(&gs_write_byte_2, 1, 1, out);
        fwrite(&gs_write_byte_1, 1, 1, out);
        current_fpos = (current_fpos + 2);
        new_instruction = (new_instruction + 2); // add 2 for next write address
        bytes_counter = (bytes_counter + 6); // 6 bytes per code
        // printf("%04X\n", bytes_counter); // 4 digit counter after checksum (0x102 and 0x103)
    }

    if(large_gs_lite_save)
    {
        gshax2_code_size = (gshax2_code_size + (bytes_counter / 6)); // get number of code lines
        printf("GSHAX2 Code Size: %02X\n", gshax2_code_size);
        fseek(out, 0x28E, SEEK_SET); // hardcoded fpos next byte after termination for game title 'A'
        // fseek(out, 0x294, SEEK_SET); test for 'WIPEOUT' game title
        fputc(gshax2_code_size, out);
        // 0xd - 0x106 to 0x112 (start of checked area BEFORE D0 codes) - missing from current count
        // 0x18 - 0x113 to 0x12a (DO codes = 4 lines, 6 bytes each) - ALREADY ADDED!!!
        // 0x08 - 0x287 to 0x28E ('GSHAX2', termination, number_of_codes) - missing from current count
        // 0x06 - 0x34f to 0x354 (termination for code list, 6 0x00 bytes) - missing from current count
        // 0x174 (reset to 0 previously to calculate the GSHAX2 number_of_codes) - missing from current count
        // 0xd+0x08+0x06+0x174 = 0x18F, magic number of missing bytes not in the current counter!
        gslite_save_checksum_checked_bytes_len = (bytes_counter + 0x18F);
        fseek(out, 0x102, SEEK_SET);
        printf("Checked Bytes Size: %04X\n", gslite_save_checksum_checked_bytes_len);
        fwrite(&gslite_save_checksum_checked_bytes_len, 1, 2, out);

        fseek(out, 0x10A, SEEK_SET);
        fputc(0x02, out); // set number of codes to 2 for the `A` game title

        fseek(out, 0x403, SEEK_SET); // 0x402 is the last possible byte in the 'GSHAX2' code listing
    } else {
        gshax1_code_size = (gshax1_code_size + (bytes_counter / 6)); // get number of code lines
        printf("GSHAX1 Code Size: %02X\n", gshax1_code_size);
        fseek(out, 0x112, SEEK_SET); // hardcoded fpos for GSHAX1 number_of_codes after termination for game title 'A'
        fputc(gshax1_code_size, out);
        // 0xd - 0x106 to 0x112 (start of checked area BEFORE D0 codes) - missing from current count
        // 0x18 - 0x113 to 0x12a (DO codes = 4 lines, 6 bytes each) - ALREADY ADDED!!!
        // 0x06 - 0x34f to 0x354 (termination for code list, 6 0x00 bytes) - missing from current count
        // 0x174 (reset to 0 previously to calculate the GSHAX2 number_of_codes) - missing from current count
        // 0xd+0x06+0x174 = 0x187, magic number of missing bytes not in the current counter!
        gslite_save_checksum_checked_bytes_len = (bytes_counter + 0x187);
        fseek(out, 0x102, SEEK_SET);
        printf("Checked Bytes Size: %04X\n", gslite_save_checksum_checked_bytes_len);
        fwrite(&gslite_save_checksum_checked_bytes_len, 1, 2, out);

        fseek(out, 0x10A, SEEK_SET);
        fputc(0x01, out); // set number of codes to 1 for the `A` game title

        fseek(out, (0x287 + 0x06), SEEK_SET); // 0x287 is the last possible byte in the 'GSHAX2' code listing
    }

    for(int i = 0; i < 0x6A; i++)
        fputc(end_card[i], out);

    fseek(out, 0x106, SEEK_SET); // seek to start of checksum area
    
    int i = 0;
    while(i < gslite_save_checksum_checked_bytes_len) // check from 0x106 to the last gameshark code in the save + 6 0x00 bytes for termination of the code listing
    {
        gslite_save_checksum = gslite_save_checksum + fgetc(out);
        i++;
    }

    printf("Checksum: %04X\n",gslite_save_checksum);
    fseek(out, 0x100, SEEK_SET);
    fwrite(&gslite_save_checksum, 1, 2, out);
}

int main (int argc, const char * argv[]) 
{

    printf("GSHAX Tool v2.0\nBy Alex Free & MottZilla\n(C)2023 3-BSD\n");

   if(argc != 9)
    {
        printf("Error: gshax requires 8 arguments\n\ngshax <file output mode> <address in RAM where the original instruction resides> <instruction type> <actual target address of original instruction> <custom replacement target address/EXE load address for instruction> <mips assembly binary> <mips assembly binary padding cutoff offset> <output file>\n\nExplanation of arguments:\n\n<file output mode>    This must be -g (to generate a GameShark Lite save game file) or -t (to generate a txt file containing the codes).\n\n<address in RAM where the original instruction resides>   This is the literal RAM address that the instruction is loaded to.\n\n<instruction type>   This must be -j (for a jump instruction type that you want to change) or -jal (for a jump and link instruction type that you want to change).\n\n<actual target address of the original instruction>  This is the actual address that the original instruction either jumps or jump and links to.\n\n<custom replacement target address/EXE load address for instruction>    This is the address in RAM that is freely available and not in use at the time of the jump or jump and link instruction.\n\n<mips assembly binary>   This is the MIPS binary filepath/filename that will be jumped to by the modified instruction.\n\n<mips assembly binary padding cutoff offset>    This must be an even amount of bytes, it sets the offset to disregard any extra padding in the MIPS binary used.\n\n<output file>    This is the filepath/filename of the GameShark Lite save game file (must be CODELIST00 in that file output mode) or the name of the txt file generated by GSHAX tool.\n\n");
        return(1);
    }


    original_instruction_address = strtoul(argv[2], NULL, 16);
    printf("Address in RAM where the original instruction resides: %08X\n", original_instruction_address);
    
    if((strcmp("-j", argv[3])) == 0) 
    {
        printf("Instruction type is Jump\n");
        instruction_type = 0x08;
    } else if((strcmp("-jal", argv[3])) == 0) {
        printf("Instruction type is Jump And Link\n");
        instruction_type = 0x0C;
    } else {
        printf("Error: The 3rd argument must be -j or -jal to specify the instruction type\n");
        return(1);
    }


    original_instruction = strtoul(argv[4], NULL, 16);
    printf("Actual target address of original instruction: %08X\n", original_instruction);
    bytes=(unsigned char *)&original_instruction;
    original_instruction_val = bytes[0] + (bytes[1] << 8) + (bytes[2] << 16);
    original_instruction_val = (original_instruction_val / 4);
    bytes=(unsigned char *)&original_instruction_val;
    bytes[3] = instruction_type;
    printf("Original instruction: %08X\n", original_instruction_val);

    new_instruction = strtoul(argv[5], NULL, 16);
    printf("Replacement instruction target address/EXE load address: %08X\n", new_instruction);
    bytes=(unsigned char *)&new_instruction;
    new_instruction_val = bytes[0] + (bytes[1] << 8) + (bytes[2] << 16);
    new_instruction_val = (new_instruction_val / 4);
    bytes=(unsigned char *)&new_instruction_val;
    bytes[3] = instruction_type;
    printf("New instruction: %08X\n", new_instruction_val);
    
    bytes_for_sanity_check=(unsigned char *)&original_instruction_val;
    /*
    printf("Old JAL instruction Byte 0: %02X\n", bytes_for_sanity_check[0]);
    printf("Old JAL instruction Byte 1: %02X\n", bytes_for_sanity_check[1]);
    printf("Old JAL instruction Byte 2: %02X\n", bytes_for_sanity_check[2]);
    printf("Old JAL instruction Byte 3: %02X\n", bytes_for_sanity_check[3]);
    printf("New JAL instruction Byte 0: %02X\n", bytes[0]);
    printf("New JAL instruction Byte 1: %02X\n", bytes[1]);
    printf("New JAL instruction Byte 2: %02X\n", bytes[2]);
    printf("New JAL instruction Byte 3: %02X\n", bytes[3]);
    */

    if(bytes[0] == bytes_for_sanity_check[0] && bytes[1] == bytes_for_sanity_check[1])
    {
        printf("First D0 code not needed\n");
        compare_code_1_enable = 0;
    }

    if(bytes[2] == bytes_for_sanity_check[2] && bytes[3] == bytes_for_sanity_check[3])
    {
        printf("Second D0 code not needed\n");
        compare_code_2_enable = 0;
    }

    if((exe = fopen(argv[6], "rb+")) == NULL)
    {
        printf("Error: Cannot open the executable file: %s\n", argv[6]);
        return(1);
    }

    mips_assembly_bin_cutoff_offset = strtoul(argv[7], NULL, 16);
    // currently only handles a maximum of up to 2 code entries. Each code entry can be up to 64 lines in length. The first 4 lines are taken up by the 2 D0 compare codes. So 60 * 2 = 120. 64 * 2 = 128. 120+128= 248 or 0xF8.
    
    if(mips_assembly_bin_cutoff_offset > 0xF8)
    {
        printf("Error: the MIPS Assembly bin cutoff offset: %02X is too large, the maximum filesize is 248 (decimal) or 0xF8 (hex) bytes!\n", mips_assembly_bin_cutoff_offset);
        return(1);
    } else {
        printf("MIPS Assembly Size Cutoff: %04X\n", mips_assembly_bin_cutoff_offset);
    }

    if(mips_assembly_bin_cutoff_offset % 2 != 0)
    {
        printf("Error: the MIPS Assembly bin cutoff offset: %02X is not divisible by two, and can not be used!\n", mips_assembly_bin_cutoff_offset);
        return(1);
    }
    
    fseek(exe, 0, SEEK_SET);
    buf = (unsigned char *)malloc(mips_assembly_bin_cutoff_offset * sizeof(unsigned char));
    fread(buf, mips_assembly_bin_cutoff_offset, 1, exe);


    if((strcmp("-g", argv[1])) == 0) 
    {
        if((out = fopen(argv[8], "wb+")) == NULL)
        {
            printf("Error: Cannot create the output file: %s\n", argv[8]);
            return(1);
        }
        gen_gslite_save();
    } else if((strcmp("-t", argv[1])) == 0) {
        if((out = fopen(argv[8], "w+")) == NULL)
        {
            printf("Error: Cannot create the output file: %s\n", argv[8]);
            return(1);
        }            
        gen_txt();
    } else {
        printf("Error: 1st argument must be -g or -t\n");
        return(1);
    }

    fclose(out);
    fclose(exe);
}