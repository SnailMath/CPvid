 #define FIRST  1 //The number of the first frame
 #define STEP   3 
 #define LAST 100 //The number of the last frame
 #define INFILE  "vid_source/0004.png" //Must contain the number of the first frame
 #define INNR    11 //first character that counts up in INFILE
 #define OUTFILE "../vid0/IMG_0000.565"
 #define OUTNR   12 //first character that counts up in OUTFILE (4 digits)

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define count(str,nr) if(str[nr]>'9'){str[nr]-=10; str[nr-1]++;} 
#define countUp(str,nr,step) str[nr+3]+=step; count(str,nr+3) count(str,nr+2) count(str,nr+1) count(str,nr)

void convert(char *in, char *out );

int main(int argc, char *argv[]){

	//convert("in.png", "out.565");

	int i = FIRST;
	char outfile[] = OUTFILE;
	int outnr      = OUTNR;
	char infile[]  = INFILE;
	int innr       = INNR;
	while (i<=LAST){
		printf("convert(\"%s\", \"%s\");\n",infile, outfile);
		convert(infile, outfile);

		//Count up the file names
		countUp(outfile,outnr,1)
		countUp( infile, innr,STEP )

		i+=STEP;
	}
	infile[innr  ] = 'c';
	infile[innr+1] = 'o';
	infile[innr+2] = 'd';
	infile[innr+3] = 'e';
	printf("convert(\"%s\", \"%s\"); //The QR-Code at the and.\n",infile, outfile);
	convert(infile, outfile );
}


void convert(char *in, char *out ){
	int w,h,c;
	unsigned char *img = stbi_load(in, &w, &h, &c, 3);
	if (img == NULL){
		printf("Can't find \"%s\".\n",in);
		return;
	}
	printf("%s w:%d h:%d c:%d\n",in,w,h,c);

	FILE *fp;
	fp = fopen(out,"wb");

	unsigned long i = 0;
	while (i<(w*h)){
		unsigned char r = img[i*3  ];
		unsigned char g = img[i*3+1];
		unsigned char b = img[i*3+2];
		uint16_t color  = ((r<<8) & 0b1111100000000000) | ((g<<3) & 0b0000011111100000) | ((b>>3) & 0b0000000000011111);
		//printf("r:%02x g:%02x b:%02x %04x\n",r,g,b,color);
		fputc((color>>8) & 0x00ff , fp);
		fputc((color   ) & 0x00ff , fp);
		i++;
	}
	fclose(fp);
}
