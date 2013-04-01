

// Standard Headers
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <errno.h>
#include <string.h>	

// libbootimage headers
#include <utils.h>
#include <bootimage.h>

// internal program headers
#include <actions.h>

typedef struct update_action update_action;

struct update_action{
	
	char *		bootimage_filename	;
	char *  	header_filename 	;
	char *  	kernel_filename 	;
	char ** 	ramdisk_filenames 	;
	char *  	ramdisk_cpioname 	;
	char *  	ramdisk_imagename 	;
	unsigned char * ramdisk_directory 	;
	char *		second_filename		;
	char * 		output_filename 	;
	unsigned	ramdisk_filenames_count	;
	char ** 	property_names 	;
	unsigned property_count ;
};

int update_bootimage(update_action* action){
    
    errno = 0 ;
    boot_image bimage ;
    
    char* current_working_directory = NULL; 
    getcwd(current_working_directory,PATH_MAX);
    int return_value = load_boot_image(action->bootimage_filename,&bimage);
    if(return_value != 0){
        if(bimage.start_addr != NULL  ) free(bimage.start_addr);
        return return_value;
    }
    
    // setup a new boot_image_struct to receive the modified
    // information
    boot_image bnewimage ;
    set_boot_image_defaults(&bnewimage);
    copy_boot_image_header_info(&bnewimage,&bimage);
    
    unsigned char* new_kernel_data = NULL;
    
    if(action->kernel_filename){
        
	
	//print_boot_image_header_info(&bimage) ;
	
	unsigned new_kernel_size = 0; 
	new_kernel_data = read_item_from_disk(action->kernel_filename,&new_kernel_size);
	    	
	bnewimage.kernel_addr = new_kernel_data;
	bnewimage.kernel_size = new_kernel_size;      
        set_boot_image_padding(&bnewimage);
        set_boot_image_offsets(&bnewimage);
        set_boot_image_content_hash(&bnewimage);
        
	//print_boot_image_header_info(&bnewimage) ;
    
	//FILE* boot_image_file_fp = fopen(action->output_filename,"wb");
	//fwrite(bimage.header_addr,1,bimage.header_size,boot_image_file_fp)  ;
	//fclose(boot_image_file_fp);
	
	//write_boot_image_header_to_disk("header.txt",&bimage) ;
	
       
	
       
    }
    
     if(write_boot_image(action->output_filename,&bnewimage)){
	    fprintf(stderr,"write_boot_image failed %d\n",errno);
	}
    

cleanup_bootimage:
    if(new_kernel_data) free(new_kernel_data);
    free(bimage.start_addr);
    return errno;
    
    
    
}
int process_update_action(int argc,char ** argv){

	
    // Initialize the action struct with NULL values
    update_action action;
    action.bootimage_filename 	= NULL 	;
    action.header_filename  	= NULL 	;
    action.kernel_filename  	= NULL 	;
    action.ramdisk_filenames  	= NULL 	;
    action.ramdisk_cpioname  	= NULL 	;
    action.ramdisk_imagename  	= NULL 	;
    action.ramdisk_directory  	= NULL 	;
    action.second_filename  	= NULL 	;
    action.output_filename  	= NULL 	;
    action.ramdisk_filenames_count	= 0	;

    FILE*file; int ramdisk_set = 0;
      
    while(argc > 0){
	    
	// check for a valid file name
	if(!action.bootimage_filename && (file=fopen(argv[0],"r+b"))){
		
		fclose(file);
		action.bootimage_filename = argv[0];
		fprintf(stderr,"action.bootimage_filename:%s\n",action.bootimage_filename);
		// set full extract if this is the last token 
		// or if the next token is NOT a switch. 
		
		if(argc == 1 || argv[1][0]!='-'){ 
		    fprintf(stderr,"extract all\n");
		    action.header_filename 	= "header";
		    action.kernel_filename 	= "kernel";
		    action.ramdisk_cpioname 	= "ramdisk.cpio";
		    action.ramdisk_imagename 	= "ramdisk.img";
		    action.ramdisk_directory 	= "ramdisk";
		    action.second_filename 	= "second";
		    // do we have an impiled output filename
		    if (argv[1]) action.output_filename = argv[1];

		}
		
		
	}else if(!strlcmp(argv[0],"--header") || !strlcmp(argv[0],"-h")){
		
		// we have an header, do we have a filename
		
		// use the default filename if this is the last token
		// or if the next token is a switch
		if(argc == 1 || argv[1][0]=='-'){
			action.header_filename = "header";
		}else{
			action.header_filename = argv[1];
			--argc; ++argv;
		}
		fprintf(stderr,"action.header_filename:%s\n",action.header_filename);
		
	}else if(!strlcmp(argv[0],"--kernel") || !strlcmp(argv[0],"-k")){
		
		// we have a kernel, do we have a filename
		
		// use the default filename if this is the last token
		// or if the next token is a switch
		if(argc == 1 || (argv[1][0]=='-')){
			action.kernel_filename = "kernel";
		}else{
			action.kernel_filename = argv[1];
			--argc; ++argv;
		}
		fprintf(stderr,"action.kernel_filename:%s\n",action.kernel_filename);
		
	}else if(!strlcmp(argv[0],"--output") || !strlcmp(argv[0],"-o")) {
		
		// the output directory. if this does not exist then
		// we will attempt to create it.
		
		// if it does exists we will offer to either wipe or 
		// append.
		
		// use the default filename if this is the last token
		// or if the next token is a switch
		if(argc == 1 || (argv[1][0]=='-')){
			action.output_filename = NULL;
		}else{
			action.output_filename = argv[1];
			--argc; ++argv;
		}

	    fprintf(stderr,"action.output_filename:%s\n",action.output_filename);
		
	}else if (!ramdisk_set){
    
	    if(!strlcmp(argv[0],"--cpio") || !strlcmp(argv[0],"-C")) {
		    
		// use the default filename if this is the last token
		// or if the next token is a switch
		if(argc == 1 || (argv[1][0]=='-')){
		    action.ramdisk_cpioname = "ramdisk.cpio";
		}else{
		    action.ramdisk_cpioname = argv[1];
		    --argc; ++argv;
		}
		ramdisk_set = 1 ;
		fprintf(stderr,"action.ramdisk_cpioname:%s\n",action.ramdisk_cpioname);
		
	    }else if(!strlcmp(argv[0],"--directory") || !strlcmp(argv[0],"-d")) {
		    
		// use the default filename if this is the last token
		// or if the next token is a switch
		if(argc == 1 || (argv[1][0]=='-')){
		    action.ramdisk_directory = "ramdisk";
		}else{
		    action.ramdisk_directory = argv[1];
		    --argc; ++argv;
		}
		ramdisk_set = 1 ;
		fprintf(stderr,"action.ramdisk_directory:%s\n",action.ramdisk_directory);
		    
	    }else if(!strlcmp(argv[0],"--image") || !strlcmp(argv[0],"-i")) {
		    
		// the ramdisk image as it is in the boot.img
		// this is normally a cpio.gz file but we need to 
		// check that later on.
		
		// use the default filename if this is the last token
		// or if the next token is a switch
		if(argc == 1 || (argv[1][0]=='-')){
		    action.ramdisk_imagename = "ramdisk.img";
		}else{
		    action.ramdisk_imagename = argv[1];
		    --argc; ++argv;
		}
		ramdisk_set = 1 ;
		fprintf(stderr,"action.ramdisk_imagename:%s\n",action.ramdisk_imagename);
		
	    } else if(!strlcmp(argv[0],"--files") || !strlcmp(argv[0],"-f")) {
		
		// ramdisk files. This is a variable length char array
		// containing a list of file to extract from the ramdisk
			
		// if this is the last token or if the next token is a switch
		// we need to create an array with 1 entry 
		
		// work out how much memory is required
		int targc = 0 ; 
		for(targc=0; targc < argc-1 ; targc++ ){
		    fprintf(stderr,"argv[%d] %s\n",targc,argv[targc]);
		    if(argv[targc+1] && argv[targc+1][0]=='-')
		      break;
		    else
		    action.ramdisk_filenames_count++;		
		    
		}
		fprintf(stderr,"action.ramdisk_filenames_count %d argc %d\n",action.ramdisk_filenames_count,argc);
		// allocate the memory and assign null to the end of the array
		action.ramdisk_filenames = calloc(action.ramdisk_filenames_count,sizeof(unsigned char*)) ;
		// populate the array with the values 
		for(targc =0 ; targc < action.ramdisk_filenames_count; targc++) {
		    argc--; argv++;
		    action.ramdisk_filenames[targc] = argv[0]; 
		    fprintf(stderr,"action.ramdisk_filenames[%d]:%s\n",targc,action.ramdisk_filenames[targc] );
		}
		ramdisk_set = 1 ;
	    }        
	}else if(!strlcmp(argv[0],"--second") || !strlcmp(argv[0],"-s")) {
		
		// the second bootloader. AFAIK there is only 1 device
		// in existance that
		
		// use the default filename if this is the last token
		// or if the next token is a switch
		if(argc == 1 || (argv[1][0]=='-')){
			action.second_filename = "second";
		}else{
			action.second_filename = argv[1];
			--argc; ++argv;
		}
		fprintf(stderr,"action.second_filename:%s\n",action.second_filename);
		
	} else if(!strlcmp(argv[0],"--properties") || !strlcmp(argv[0],"-p")) {
		
            // properties. This is a variable length char array
            // containing a list of properties to be modified in the 
            // default.prop
                    
            // if this is the last token or if the next token is a switch
            // we need to create an array with 1 entry 
            
            // work out how much memory is required
            int targc = 0 ; 
            for(targc=0; targc < argc-1 ; targc++ ){
                fprintf(stderr,"argv[%d] %s\n",targc,argv[targc]);
                if(argv[targc+1] && argv[targc+1][0]=='-')
                  break;
                else
                action.property_count++;		
                
            }
            fprintf(stderr,"action.ramdisk_filenames_count %d argc %d\n",action.property_count,argc);
            // allocate the memory and assign null to the end of the array
            action.property_names = calloc(action.property_count,sizeof(unsigned char*)) ;
            // populate the array with the values 
            for(targc =0 ; targc < action.property_count; targc++) {
                argc--; argv++;
                action.property_names[targc] = argv[0]; 
                fprintf(stderr,"action.ramdisk_filenames[%d]:%s\n",targc,action.property_names[targc] );
            }
        }    
        argc--; argv++ ;
    }
    // we must have at least a boot image to process
    if(!action.bootimage_filename){
	    fprintf(stderr,"no boot image:%s\n",action.bootimage_filename);
	    return EINVAL;
    }
    // output_file not set. use the bootimage filename
    if(!action.output_filename){
	fprintf(stderr,"no output set using bootimage_filename:%s\n",action.bootimage_filename);
	action.output_filename = action.bootimage_filename;
    }
	
    
    update_bootimage(&action);
       
    return 0;
}