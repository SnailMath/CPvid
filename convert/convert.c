
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define strToInt(s,x) if(s!=NULL){x = 0;for(int i=0;(s[i]>='0'&&s[i]>='0');x=(x*10)+s[i++]-'0'){}}

int convert565(char *in, char *out, int vid_txt );
void usage(char *argv[]);

int mode = 0; //The input mode; 565 is png or jpg input, converted to 65536 colors; 256 is gif input, converted to 256 colors (color palette)
int verbose = 0; //Verbosity of the program (0, 1 or 2)
char *str_first   = NULL;    //The number of the first frame
int       first   =   1;
char *str_last    = NULL;    //The number of the last  frame
int       last    = 100;
char *str_step    = NULL;    //The size of the steps between frames (e.g 1,4,7,10,...)
int       step    =   3;
char *description = NULL;    //The description of the video
char *name        = "Video"; //The name of the video
char *arg_in      = NULL;    //The path of the input file(s), format string (contains %04d)
char *arg_out     = NULL;    //The path of the output folder

int main(int argc, char *argv[]){
    for (int i=1; i<argc; i++){
        if      (strcmp("-p",argv[i])==0) mode = 565;
        else if (strcmp("-g",argv[i])==0) mode = 256;
        else if (strcmp("-i",argv[i])==0){
            if(++i>=argc){usage(argv);return 0;}
            arg_in    = argv[i];}
        else if (strcmp("-o",argv[i])==0){
            if(++i>=argc){usage(argv);return 0;}
            arg_out   = argv[i];}
        else if (strcmp("-f",argv[i])==0){
            if(++i>=argc){usage(argv);return 0;}
            str_first = argv[i];}
        else if (strcmp("-l",argv[i])==0){
            if(++i>=argc){usage(argv);return 0;}
            str_last  = argv[i];}
        else if (strcmp("-s",argv[i])==0){
            if(++i>=argc){usage(argv);return 0;}
            str_step  = argv[i];}
        else if (strcmp("-d",argv[i])==0){
            if(++i>=argc){usage(argv);return 0;}
            description = argv[i];}
        else if (strcmp("-n",argv[i])==0){
            if(++i>=argc){usage(argv);return 0;}
            name = argv[i];}
        else if (strcmp("-v",argv[i])==0) verbose = 1;
        else if (strcmp("-V",argv[i])==0) verbose = 2;
        else{
            fprintf(stderr,"Wrong argument '%s'!\n", argv[i]);
            return -1;
        }
    }

    if (arg_in ==NULL){usage(argv);return 0;} //No input file specified
    if (arg_out==NULL){
        arg_out="vid/";
        printf("no output file specified, assuming '%s'\n",arg_out);
    }
    if (mode==0){
        mode = 565;
        if(verbose)
            printf("no mode specified, assuming '-p'\n");
    }
    
    //Clean arg_in from frormat strings except the %04d
    int formatstring = 0;
    for (int i=0; arg_in[i]!=0;i++){
        if(arg_in[i]=='%'){
            formatstring++;
            if(formatstring==1){
                if(arg_in[i+1]!='d'&&arg_in[i+1]!='x'&&arg_in[i+1]!='X'){ //%d is allowed. Otherwise test more.
                    //arg_in[i+1]!='d'
                    if(arg_in[i+1]=='0'){ //%01d to %08d is allowed.
                        if(arg_in[i+2]<'1'||arg_in[i+2]>'8'){ //This is not  between %01d and %08d
                            printf("ERROR. The format string '%%0%c' is not allowed in the input filename. Please use %%d, %%1d to %%8d, %%01d to %%08d. Thank you.\n", arg_in[i+2] );
                            return -1;
                        }else{ //%04 is safe. Now. Is the last character a 'd'?
                            if(arg_in[i+3]!='d'&&arg_in[i+3]!='x'&&arg_in[i+3]!='X'){ //It is not %01d to %08d
                                printf("ERROR. The format string '%%0%c%c' is not allowed in the input filename. Please use %%d, %%1d to %%8d, %%01d to %%08d. Thank you.\n", arg_in[i+2], arg_in[i+3] );
                            return -1;
                            }//It is %01d to %08d
                        }
                    }
                    else if(arg_in[i+1]<'1'||arg_in[i+1]>'8'){ // %1d to %8d is allowed, everything else not.
                        //the formatstring is not allowed.
                        printf("ERROR. The format string '%%%c' is not allowed in the input filename.\n", arg_in[i+1] );
                        return -1;
                    
                    }else{ //It is between %1 and %8. Check if it is %1d and %8d. That is allowed.
                        if(arg_in[i+2]!='d'&&arg_in[i+2]!='x'&&arg_in[i+2]!='X'){
                            printf("ERROR. The format string '%%%c%c' is not allowed in the input filename.\n", arg_in[i+1], arg_in[i+2] );
                            return -1;
                        }//%1d to %8d is allowed.
                    }
                } //It is %d
            }
            if(formatstring>=2){
                printf("ERROR. There is more than one format string in the input filename.\n");
                return -1;
            }
        }
    }
    
    if (description==NULL)
        description = arg_in;
    //cleanf(arg_out);


/////////The actual converting

    if(mode==565){
    
    	//convert("in.png", "out.565");
        
        strToInt(str_first, first);
        strToInt(str_last,  last );
        strToInt(str_step,  step );
        
        if(verbose)
            printf("first %d; last %d; step %d\n",first,last,step);
        
    
    	int current = first;
    	char outfile[strlen(arg_out)+10+1]; //The filename is 9 digits ("/0000.565") or 10 digits ("/video.256") (And 0 terminator)
    	char  infile[strlen(arg_in )+ 6+1]; //The number in the filename cannot be longer than 8 digits. (10000000 is 6 digits shorter than %d
    
    	while (current<=last){
	        sprintf(infile, arg_in, current);
    	    sprintf(outfile,"%s/%04d.565",arg_out,current);
	        if (verbose)
        		printf("convert565(\"%s\", \"%s\");\n",infile, outfile);
    		convert565(infile, outfile, current==first);
    		current+=step;
    	}
    }
    else{
        printf("The gif mode is not implemented yet.\n");
    }
}


void usage(char *argv[]){
	fprintf(stderr, "Usage:\n\
%s [args] -i [input] -o [output] \n\
args:\n\
  -i INPUT   Filename of input  file (format string)\n\
  -o OUTPUT  Filename of output file\n\
  -f FIRST   Starts from frame FIRST   (default   1)\n\
  -s STEP    Converts every STEP frame (default   3)\n\
  -l LAST    Converts until this frame (default 100)\n\
       (default frames: 1, 4, 7, 10, 13, ... , 94, 97, 100 )\n\
  -n NAME    The name of the video\n\
  -d DESC    The description of the video\n\
  -v         verbose (you do not need -V)\n\
  -g         input file is a single .gif animation\n\
  -p         input file is a sequence of .png or .jpg files (default)\n\
             use %%04s in the filename as a replacement for\n\
             the 4 digit number. (or %%03s for 3 digits)\n\
\n\
Example:\n\
    Convert png files from vid_source/0001.png to ../vid0/\n\
    %s -p -i vid_source/%%04d.png -o ../vid0/ -f 1 -s 3 -l 100\n\
\n\
(The arguments are not filtered before they are passed to printf,\n\
so do not run this on a server.)\n\
\n",argv[0],argv[0]);
}

//Convert the png or jpg file *in to the binary file *out (in 565 format)
//Create the video.txt file, if vid_txt != 0
int convert565(char *in, char *out ,int vid_txt){
	int w,h,c;
	unsigned char *img = stbi_load(in, &w, &h, &c, 3);
	if (img == NULL){
		fprintf(stderr,"Can't find \"%s\".\n",in);
		return -1;
	}
	if(verbose||w>320||h>528)
    	printf("%s w:%d h:%d c:%d\n",in,w,h,c);
    if(w>320||h>528){
        printf("The image is too large! (larger than 320x528)\n");
        return -1;
    }
	if (vid_txt) { //If we have to create video.txt
	    if(verbose)
	        printf("creating file ");
		FILE *file;
		char filename[strlen(arg_out)+10+1]; // "/video.txt" is 10 characters long.
		strcpy(filename,arg_out);
		strcat(filename,"/video.txt");
		if(verbose)
            printf("%s\n",filename);
		file = fopen(filename,"wb");
		char vidtxt[256];
		char str_w[4];
		char str_h[4];
		sprintf(str_w,"%d",w);
		sprintf(str_h,"%d",h);
		strcpy(vidtxt,name);
		strcat(vidtxt,"\n");
		strcat(vidtxt,str_w);
		strcat(vidtxt,"x");
		strcat(vidtxt,str_h);
		strcat(vidtxt,"\n565\n");
		strcat(vidtxt,description);
		fprintf(file, "%s", vidtxt );
		fclose (file);
	}

	FILE *fp;
	fp = fopen(out,"wb");

	unsigned long i = 0;
	while (i<(w*h)){
		unsigned char r = img[i*3  ];
		unsigned char g = img[i*3+1];
		unsigned char b = img[i*3+2];
		uint16_t color  = ((r<<8) & 0b1111100000000000) | ((g<<3) & 0b0000011111100000) | ((b>>3) & 0b0000000000011111);
		if (verbose==2)
    		printf("r:%02x g:%02x b:%02x %04x\n",r,g,b,color);
		fputc((color>>8) & 0x00ff , fp);
		fputc((color   ) & 0x00ff , fp);
		i++;
	}
	fclose(fp);
	return 0;
}
