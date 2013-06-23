/*
 * ramdisk.h
 * 
 * Copyright 2013 Trevor Drake <trevd1234@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
#ifndef _2638e606_9103_11e2_b47f_5404a601fa9d
#define _2638e606_9103_11e2_b47f_5404a601fa9d


#include <compression.h>

#define CPIO_HEADER_MAGIC "070701"
#define CPIO_HEADER_MAGIC_SIZE 6


#define CPIO_TRAILER_MAGIC "TRAILER!!!"
#define CPIO_TRAILER_MAGIC_SIZE 10

#define MAX_RAMDISK_SIZE (8192*1024)*4
#define RAMDISK_ENTRY_DATA_NON_CONTIGOUS -1


#define RAMDISK_COMPRESSION_NOT_SET COMPRESSION_NOT_SET   
#define RAMDISK_COMPRESSION_GZIP    COMPRESSION_GZIP_DEFLATE
#define RAMDISK_COMPRESSION_LZO     COMPRESSION_LZOP        
#define RAMDISK_COMPRESSION_XZ      COMPRESSION_XZ          
#define RAMDISK_COMPRESSION_LZMA    COMPRESSION_LZMA        
#define RAMDISK_COMPRESSION_BZIP2   COMPRESSION_BZIP2       
#define RAMDISK_COMPRESSION_LZ4     COMPRESSION_LZ4       
#define RAMDISK_COMPRESSION_NONE    COMPRESSION_NONE
#define RAMDISK_COMPRESSION_CPIO    
       


#define RAMDISK_TYPE_NOT_SET 0
#define RAMDISK_TYPE_NORMAL 1
#define RAMDISK_TYPE_RECOVERY 2
#define RAMDISK_TYPE_UBUNTU 3

#define RECOVERY_BRAND_NOT_SET 0
#define RECOVERY_BRAND_STOCK 1
#define RECOVERY_BRAND_CLOCKWORK 2
#define RECOVERY_BRAND_CWM 3
#define RECOVERY_BRAND_COT 4
#define RECOVERY_BRAND_TWRP 5
#define RECOVERY_BRAND_4EXT 6
#define RECOVERY_BRAND_UNKNOWN 7
#define RECOVERY_BRAND_NONE 8

#define RECOVERY_VERSION_UNKNOWN -1
#define RECOVERY_VERSION_NOT_SET 0




typedef struct ramdisk_entry ramdisk_entry;

struct ramdisk_entry{
    
    unsigned char* start_addr ;
     
    unsigned size;

    unsigned long mode;    
    
    unsigned char* name_addr ;
    unsigned name_size ;
    unsigned name_offset ;
    unsigned name_padding ;
    
    unsigned char* data_addr ;
    unsigned data_size ;
    unsigned data_offset ;
    unsigned data_padding ;
    
    unsigned char* next_addr ;
    
    
    
    } ;

typedef struct ramdisk_image ramdisk_image;

struct ramdisk_image {
    
    
    int compression_type;
    unsigned char* start_addr ;
    unsigned size;
    int type;
    int recovery_brand;   
    char *recovery_version;
    int recovery_version_size;
        
    unsigned entry_count;

    ramdisk_entry ** entries ;

    };

char * str_ramdisk_compression(unsigned compression); 

unsigned int_ramdisk_compression(char * compression);

unsigned init_ramdisk_image(ramdisk_image* image);

unsigned bird_open(const char *filename, ramdisk_image* image);

unsigned bird_read(unsigned char* ramdisk_addr,unsigned ramdisk_size,ramdisk_image* image );

unsigned bird_write_entries(const char *directory_name,ramdisk_image* image);
unsigned bird_write(const char *filename_name,ramdisk_image* image);

unsigned bird_open_directory(const char* directory_name, ramdisk_image* image);
        
unsigned bird_open_entries(const char* directory_name, ramdisk_image* image);

unsigned bird_read_entries(unsigned char* ramdisk_addr,unsigned ramdisk_size,ramdisk_image* image );


unsigned print_ramdisk_info(ramdisk_image* rimage);

char *str_ramdisk_type(int type);

char *str_recovery_brand(int ramdisk_brand);

unsigned update_ramdisk_entry_cpio_newc_header_info(ramdisk_entry* entry);

unsigned char* pack_noncontiguous_ramdisk_entries(ramdisk_image* rimage);

ramdisk_image* get_initialized_ramdisk_image();

//unsigned write_ramdisk_image_header_to_disk(const char *filename, ramdisk_image* image);

#endif
