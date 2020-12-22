
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define strToInt(s,x) if(s!=NULL){x = 0;for(int i=0;(s[i]>='0'&&s[i]>='0');x=(x*10)+s[i++]-'0'){}}

int convert256(unsigned char *img, int w, int h, char *out, uint16_t *palette, int *pallen, int vid_txt);
int convert565(char *in, char *out, int vid_txt );
void usage(char *argv[]);

int mode = 0; //The input mode; 565 is png or jpg input, converted to 65536 colors; 256 is gif input, converted to 256 colors (color palette)
int verbose = 0; //Verbosity of the program (0, 1 or 2)
char *str_first   = NULL;    //The number of the first frame
int       first   =   1;
char *str_last    = NULL;    //The number of the last  frame
int       last    = 100;
char *str_step    = NULL;    //The size of the steps between frames (e.g 1,4,7,10,...)
int       step    =   1;
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
    
        int outNr = 0;
    	while (current<=last){
	        sprintf(infile, arg_in, current);
    	    sprintf(outfile,"%s/%04d.565",arg_out,outNr++);
	        if (verbose)
        		printf("convert565(\"%s\", \"%s\");\n",infile, outfile);
    		convert565(infile, outfile, current==first);
    		current+=step;
    	}
    }
    else{
        //printf("The gif mode is not implemented yet.\n");
       
    	char outfile[strlen(arg_out)+10+1]; //The filename is 9 digits ("/0000.565") or 10 digits ("/video.256") (And 0 terminator)

        //The color palette
        uint16_t palette[256]; //2 bytes are one color
        int pallen = 0;                      //start out with 0 items.
        memset(palette, 0, sizeof(palette)); //clear all colors to black

        FILE *f = stbi__fopen(arg_in, "rb");
        //unsigned char *result; //Probably actually an *stbi_uc
        int w,h,c;
        stbi__context s;        //The internal image stuff
        stbi__start_file(&s,f); //Open the file to that internal image stuff
        stbi__result_info ri;
        stbi_uc *u = 0;         //The image data
        stbi__gif g;            //The internal gif stuff
        memset(&g, 0, sizeof(g));//(it needs to be empty, I think...)
        STBI_NOTUSED(ri); //I have no idea.


        //For every image in the gif
        int current = 0;
        while( u = stbi__gif_load_next(&s, &g, &c, 3, 0)){ //Load the next image, make sure it is no empty pointer.
            if (u==(stbi_uc*)&s){
                u=0; //stbi__gif_load_next returns s instead of u if it was the last frame.
                if (verbose==2) fprintf(stderr,"Wait, stbi__gif_load_next rerurned &s instead of &u...\n");
                break; //Break the while
            }
            //if (u) { //If we have an image
                w=g.w;
                h=g.h;
                //because we want 3 layers, not 0 or 4
                //u = stbi__convert_format(u, 4, 3, g.w, g.h);
            //}
            
            
            unsigned char *img = u; //Probably actually an *stbi_uc
            /*
            if (img == NULL){
                fprintf(stderr,"Can't find \"%s\".\n",arg_in);
                return -1;
            }*/

            //build the filename
            sprintf(outfile,"%s/%04d.256",arg_out,current);

            //Now use convert256 to convert the rgb values to 565 values do the palette stuff.
            convert256(img, w, h, outfile, palette, &pallen, (current==0));


            if(verbose||w>320||h>528) printf("%s w:%d h:%d c:%d\n",arg_in,w,h,c);
            if(verbose) printf("Palette has %d colors.\n",pallen);
            if(verbose==2)
            printf("%02x%02x%02x %02x   %02x%02x%02x %02x   %02x%02x%02x %02x   %02x%02x%02x %02x\n",
              img[ 0],img[ 1],img[ 2],img[ 3],
              img[ 4],img[ 5],img[ 6],img[ 7],
              img[ 8],img[ 9],img[10],img[11],
              img[12],img[13],img[14],img[15]);

            current++; //The next frame
        } //while end
        //All frames processed.
        if(verbose){
            printf("There are %d colors on the palette:\n",pallen);
            int i=0; while (i<255){
                    printf("\
\033[48;2;%d;%d;%dm  \033[48;2;%d;%d;%dm  \033[48;2;%d;%d;%dm  \033[48;2;%d;%d;%dm  \
\033[48;2;%d;%d;%dm  \033[48;2;%d;%d;%dm  \033[48;2;%d;%d;%dm  \033[48;2;%d;%d;%dm  ",
                    palette[i+0]>>8 & 0b11111000, palette[i+0]>>3 & 0b11111100, palette[i+0]<<3 & 0b11111000,
                    palette[i+1]>>8 & 0b11111000, palette[i+1]>>3 & 0b11111100, palette[i+1]<<3 & 0b11111000,
                    palette[i+2]>>8 & 0b11111000, palette[i+2]>>3 & 0b11111100, palette[i+2]<<3 & 0b11111000,
                    palette[i+3]>>8 & 0b11111000, palette[i+3]>>3 & 0b11111100, palette[i+3]<<3 & 0b11111000,
                    palette[i+4]>>8 & 0b11111000, palette[i+4]>>3 & 0b11111100, palette[i+4]<<3 & 0b11111000,
                    palette[i+5]>>8 & 0b11111000, palette[i+5]>>3 & 0b11111100, palette[i+5]<<3 & 0b11111000,
                    palette[i+6]>>8 & 0b11111000, palette[i+6]>>3 & 0b11111100, palette[i+6]<<3 & 0b11111000,
                    palette[i+7]>>8 & 0b11111000, palette[i+7]>>3 & 0b11111100, palette[i+7]<<3 & 0b11111000);
         //palette[2*(i+0)+0]&0b11111000, ((palette[2*(i+0)+0]<<5)&0b11100000)|((palette[2*(i+0)+1]>>3)&0b00011100), (palette[2*(i+0)+1]<<3) & 0b11111000,
                    if(i%32==32-8) printf("\033[0m\n");
                    i+=8;
            }
        }
        //Free the stbi__gif
        //STBI_FREE(g.out);
        //STBI_FREE(g.history);
        //STBI_FREE(g.background);
        //STBI_FREE(g.color_table);
        fclose (f); //close the gif file

        //store the color palette in a file
	    if(verbose)
	        printf("creating color palette file ");
		FILE *file;
		char filename[strlen(arg_out)+12+1]; // "/palette.txt" is 12 characters long.
		strcpy(filename,arg_out);
		strcat(filename,"/palette.bin");
		if(verbose)
            printf("%s\n",filename);
		file = fopen(filename,"wb"); //open color palette file
        for(int i=0;i<256;i++){
            fputc(palette[i]>>8, file);
            fputc(palette[i], file);
        }
        //fwrite((void*)palette,256,2,file);
        fclose(file); //close color palette file

    }
}

//Converts an image from a rgba buffer to a file using a palette, updating the palette when needed (for the conversion of gif files)
// *img     pointer to the image data
// w        width of the image 
// h        height of the image
// *out     the filename of the output file
// *palette pointer to a colormap (256 entries of uint16_t)
// *pallen   length of the color palette (gets modified when entries are added to the palette)
// vid_txt  info if video.txt needs to be created
// returns  number of errors that occured, 0 if successfull, -1 if image too large or no image
int convert256(
    unsigned char *img, int w, int h,  char *out, 
    uint16_t *palette, int *pallen,    int vid_txt){
	if (img == NULL){
		fprintf(stderr,"Nullpointer as an image? Why?\n");
		return -1;
	}
	if(verbose||w>320||h>528)
    	printf("w:%d h:%d\n",w,h);
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
		strcat(vidtxt,"\n256\n"); //indicate, that we are using 256 colors (the colormap will be in a seperate file.
		strcat(vidtxt,description);
		fprintf(file, "%s", vidtxt );
		fclose (file);
	}

	FILE *fp;
	fp = fopen(out,"wb");

    int error = 0;

	unsigned long i = 0;
	while (i<(w*h)){ //for each pixel
        //convert the color to 565 first
		unsigned char r = img[i*4  ];
		unsigned char g = img[i*4+1];
		unsigned char b = img[i*4+2];
		uint16_t color  = ((r<<8) & 0b1111100000000000) | ((g<<3) & 0b0000011111100000) | ((b>>3) & 0b0000000000011111);
		if (verbose==2)
    		printf("r:%02x g:%02x b:%02x %04x\n",r,g,b,color);
        //now we know the color, we need to find it in the palette
        int index = 0;
        int found = 0;
        while (index < *pallen){
            if (color == palette[index]){
                if (verbose==2) printf("Found color \033[48;2;%d;%d;%dm  \033[0m %04x in the palette at %3d\n",r,g,b,color,index);
                found = 1;
                break;
            }
            index++;
        }
        //the serch is finished, either we found it, or we didn't found it (found=1 or found=0)
        if (!found){ //if we did not found it
            //we did not found it, so index points at the first empty element in the palette.
            if (index==256) { //we've compared color 0-255, no match and the palette is full. ERROR!!!
                fprintf(stderr, "Color %04x of pixel %3d/%3d can't be found in the palette.\nERROR!!! The video uses more than 256 colors.", color, (int)i%w, (int)i/w);
                error++;
            } else { // we didn't found it, but there is still space on the palette, so add it.
                palette[index]=color; //Add it to the palette
                (*pallen)++;          //the palette is now larger.
                found = 1;            //The color is now a color on the palette.
                if (verbose) printf("Added color \033[48;2;%d;%d;%dm  \033[0m %04x to the palette at %3d\n",r,g,b,color,index);
            }
        }
        if(found){//if the color is on the palette, save the palette index in the file.
    	    fputc(index , fp); //save the index in the palette, not the 
        } else {
    	    fputc(255, fp); //well, what should we do? we've already warned the user. 255 will be the last color on the list, if we have less than 256 colors, this will be black.
        }

		i++; //next pixel
	}
	fclose(fp);
	return error;
}

void usage(char *argv[]){
	fprintf(stderr, "Usage:\n\
%s [args] -i [input] -o [output] \n\
args:\n\
  -i INPUT   Filename of input  file (format string)\n\
  -o OUTPUT  Filename of output file\n\
  -f FIRST   Starts from frame FIRST   (default   1)\n\
  -s STEP    Converts every STEP frame (default   1)\n\
  -l LAST    Converts until this frame (default 100)\n\
  -n NAME    The name of the video\n\
  -d DESC    The description of the video\n\
  -v         verbose (you do not need -V)\n\
  -g         input file is a single .gif animation\n\
  -p         input file is a sequence of .png or .jpg files (default)\n\
             use %%04s in the filename as a replacement for\n\
             the 4 digit number. (or %%03s for 3 digits)\n\
\n\
Example:\n\
    Convert png files 0001.png, 0004.png, 0007.png and 0010.png from \n\
	video/png/ to video/out/\n\
    %s -p -i video/png/%%04d.png -o video/out/ -f 1 -s 3 -l 10\n\
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
