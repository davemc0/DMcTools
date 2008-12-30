/* Copyright (c) 1991 Regents of the University of California */

/* SCCSid "@(#)color.h 2.11 11/10/95 LBL" */

/*
 * RGBE.h - header for routines using pixel color values.
 *
 * 12/31/85
 *
 * Two color representations are used, one for calculation and
 * another for storage. Calculation is done with three floats
 * for speed. Stored color values use 4 bytes which contain
 * three single byte mantissas and a common exponent.
 */

#ifndef rgbe_io_h
#define rgbe_io_h

#include <cstdio>

#define RED 0
#define GRN 1
#define BLU 2
#define CIEX 0 /* or, if input is XYZ... */
#define CIEY 1
#define CIEZ 2
#define EXP 3 /* exponent same for either format */
#define COLXS 128 /* excess used for exponent */
#define WHT 3 /* used for RGBPRIMS type */

typedef unsigned char BYTE; /* 8-bit unsigned integer */

typedef BYTE RGBE[4]; /* red, green, blue (or X,Y,Z), exponent */

typedef float COLOR[3]; /* red, green, blue (or X,Y,Z) */

typedef float RGBPRIMS[4][2]; /* (x,y) chromaticities for RGBW */
typedef float (*RGBPRIMP)[2]; /* pointer to RGBPRIMS array */

typedef float COLORMAT[3][3]; /* color coordinate conversion matrix */

#define copyrgbe(c1,c2) (c1[0]=c2[0],c1[1]=c2[1],c1[2]=c2[2],c1[3]=c2[3])

#define colval(col,pri) ((col)[pri])

#define setcolor(col,r,g,b) ((col)[RED]=(r),(col)[GRN]=(g),(col)[BLU]=(b))

#define copycolor(c1,c2) ((c1)[0]=(c2)[0],(c1)[1]=(c2)[1],(c1)[2]=(c2)[2])

#define scalecolor(col,sf) ((col)[0]*=(sf),(col)[1]*=(sf),(col)[2]*=(sf))

#define addcolor(c1,c2) ((c1)[0]+=(c2)[0],(c1)[1]+=(c2)[1],(c1)[2]+=(c2)[2])

#define multcolor(c1,c2) ((c1)[0]*=(c2)[0],(c1)[1]*=(c2)[1],(c1)[2]*=(c2)[2])

#ifdef NTSC
#define CIE_x_r 0.670 /* standard NTSC primaries */
#define CIE_y_r 0.330
#define CIE_x_g 0.210
#define CIE_y_g 0.710
#define CIE_x_b 0.140
#define CIE_y_b 0.080
#define CIE_x_w 0.3333 /* use true white */
#define CIE_y_w 0.3333
#else
#define CIE_x_r 0.640 /* nominal CRT primaries */
#define CIE_y_r 0.330
#define CIE_x_g 0.290
#define CIE_y_g 0.600
#define CIE_x_b 0.150
#define CIE_y_b 0.060
#define CIE_x_w 0.3333 /* use true white */
#define CIE_y_w 0.3333
#endif

#define STDPRIMS {CIE_x_r,CIE_y_r,CIE_x_g,CIE_y_g, CIE_x_b,CIE_y_b,CIE_x_w,CIE_y_w}

#define CIE_D ( CIE_x_r*(CIE_y_g - CIE_y_b) + CIE_x_g*(CIE_y_b - CIE_y_r) + CIE_x_b*(CIE_y_r - CIE_y_g) )
#define CIE_C_rD ( (1./CIE_y_w) * ( CIE_x_w*(CIE_y_g - CIE_y_b) - CIE_y_w*(CIE_x_g - CIE_x_b) + CIE_x_g*CIE_y_b - CIE_x_b*CIE_y_g ) )
#define CIE_C_gD ( (1./CIE_y_w) * ( CIE_x_w*(CIE_y_b - CIE_y_r) - CIE_y_w*(CIE_x_b - CIE_x_r) - CIE_x_r*CIE_y_b + CIE_x_b*CIE_y_r ) )
#define CIE_C_bD ( (1./CIE_y_w) * ( CIE_x_w*(CIE_y_r - CIE_y_g) - CIE_y_w*(CIE_x_r - CIE_x_g) + CIE_x_r*CIE_y_g - CIE_x_g*CIE_y_r ) )

#define CIE_rf (CIE_y_r*CIE_C_rD/CIE_D)
#define CIE_gf (CIE_y_g*CIE_C_gD/CIE_D)
#define CIE_bf (CIE_y_b*CIE_C_bD/CIE_D)

/* As of 9-94, CIE_rf=.265074126, CIE_gf=.670114631 and CIE_bf=.064811243 */

/***** The following definitions are valid for RGB colors only... *****/

#define bright(col) (CIE_rf*(col)[RED]+CIE_gf*(col)[GRN]+CIE_bf*(col)[BLU])
#define normbright(c) ( ( (long)(CIE_rf*256.+.5)*(c)[RED] + (long)(CIE_gf*256.+.5)*(c)[GRN] + (long)(CIE_bf*256.+.5)*(c)[BLU] ) >> 8 )

 /* luminous efficacies over visible spectrum */
#define MAXEFFICACY 683. /* defined maximum at 550 nm */
#define WHTEFFICACY 179. /* uniform white light */
#define D65EFFICACY 203. /* standard illuminant D65 */
#define INCEFFICACY 160. /* illuminant A (incand.) */
#define SUNEFFICACY 208. /* illuminant B (solar dir.) */
#define SKYEFFICACY D65EFFICACY /* skylight */
#define DAYEFFICACY D65EFFICACY /* combined sky and solar */

#define luminance_rgbe(col) (WHTEFFICACY * bright(col))

/***** ...end of stuff specific to RGB colors *****/

#define intens(col) ( (col)[0] > (col)[1] ? (col)[0] > (col)[2] ? (col)[0] : (col)[2] : (col)[1] > (col)[2] ? (col)[1] : (col)[2] )

#define rgbeval(c,p) ( (c)[EXP] ?ldexp((c)[p]+.5,(int)(c)[EXP]-(COLXS+8)) : 0. )

#define WHTCOLOR {1.0,1.0,1.0}
#define BLKCOLOR {0.0,0.0,0.0}
#define WHTCOLR {128,128,128,COLXS+1}
#define BLKCOLR {0,0,0,0}

 /* picture format identifier */
#define COLRFMT "32-bit_rle_rgbe"
#define CIEFMT "32-bit_rle_xyze"
#define PICFMT "32-bit_rle_???e" /* matches either */
#define LPICFMT 15 /* max format id len */

 /* macros for exposures */
#define EXPOSSTR "EXPOSURE="
#define LEXPOSSTR 9
#define isexpos(hl) (!strncmp(hl,EXPOSSTR,LEXPOSSTR))
#define exposval(hl) atof((hl)+LEXPOSSTR)
#define fputexpos(ex,fp) fprintf(fp,"%s%e\n",EXPOSSTR,ex)

 /* macros for pixel aspect ratios */
#define ASPECTSTR "PIXASPECT="
#define LASPECTSTR 10
#define isaspect(hl) (!strncmp(hl,ASPECTSTR,LASPECTSTR))
#define aspectval(hl) atof((hl)+LASPECTSTR)
#define fputaspect(pa,fp) fprintf(fp,"%s%f\n",ASPECTSTR,pa)

 /* macros for primary specifications */
#define PRIMARYSTR "PRIMARIES="
#define LPRIMARYSTR 10
#define isprims(hl) (!strncmp(hl,PRIMARYSTR,LPRIMARYSTR))
#define primsval(p,hl) sscanf(hl+LPRIMARYSTR,"%f %f %f %f %f %f %f %f", &(p)[RED][CIEX],&(p)[RED][CIEY], &(p)[GRN][CIEX],&(p)[GRN][CIEY], &(p)[BLU][CIEX],&(p)[BLU][CIEY], &(p)[WHT][CIEX],&(p)[WHT][CIEY])
#define fputprims(p,fp) fprintf(fp,"%s %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f\n", PRIMARYSTR, (p)[RED][CIEX],(p)[RED][CIEY], (p)[GRN][CIEX],(p)[GRN][CIEY], (p)[BLU][CIEX],(p)[BLU][CIEY], (p)[WHT][CIEX],(p)[WHT][CIEY])

 /* macros for color correction */
#define COLCORSTR "COLORCORR="
#define LCOLCORSTR 10
#define iscolcor(hl) (!strncmp(hl,COLCORSTR,LCOLCORSTR))
#define colcorval(cc,hl) sscanf(hl+LCOLCORSTR,"%f %f %f", &(cc)[RED],&(cc)[GRN],&(cc)[BLU])
#define fputcolcor(cc,fp) fprintf(fp,"%s %f %f %f\n",COLCORSTR, (cc)[RED],(cc)[GRN],(cc)[BLU])

/*
 * Conversions to and from XYZ space generally don't apply WHTEFFICACY.
 * If you need Y to be luminance (cd/m^2), this must be applied when
 * converting from radiance (watts/sr/m^2).
 */

extern RGBPRIMS stdprims; /* standard primary chromaticities */
extern COLORMAT rgb2xyzmat; /* RGB to XYZ conversion matrix */
extern COLORMAT xyz2rgbmat; /* XYZ to RGB conversion matrix */

#define cie_rgb(rgb,xyz) colortrans(rgb,xyz2rgbmat,xyz)
#define rgb_cie(xyz,rgb) colortrans(xyz,rgb2xyzmat,rgb)

#ifdef BSD
#define cpcolormat(md,ms) bcopy((char *)ms,(char *)md,sizeof(COLORMAT))
#else
#define cpcolormat(md,ms) (void)memcpy((char *)md,(char *)ms,sizeof(COLORMAT))
#endif

char *tempbuffer(unsigned int len); /* get a temporary buffer */

int fwritergbe(RGBE *scanline, int len, FILE *fp); /* write out a rgbe scanline */

int freadrgbe(RGBE* scanline, int len, FILE *fp); /* read in an encoded rgbe scanline */

int oldreadrgbe(RGBE *scanline, int len, FILE *fp); /* read in an old rgbe scanline */

int fwritescan(COLOR *scanline, RGBE *helpit, int len, FILE *fp); /* write out a scanline */

int freadscan(COLOR *scanline, RGBE *helpit, int len, FILE *fp); /* read in a scanline */

int setrgbe(RGBE clr,double r,double g,double b); /* assign a short color value */

int rgbe_color(COLOR col, RGBE clr); /* convert short to float color */

#endif
