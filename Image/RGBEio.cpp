/* Copyright (c) 1991 Regents of the University of California */

/*
* color.c - routines for color calculations.
*
* 10/10/85
*/

#include <stdio.h>

#include <math.h>
#include <malloc.h>

#include <Image/RGBEio.h>

#define MINELEN 8 /* minimum scanline length for encoding */
#define MAXELEN 0x7fff /* maximum scanline length for encoding */
#define MINRUN 4 /* minimum run length */

int fwritergbe(RGBE *scanline, int len, FILE *fp)/* write out a rgbe scanline */
{
    int i, j, beg, cnt=0;
    int c2;
    
    if (len < MINELEN || len > MAXELEN) /* OOBs, write out flat */
        return(int(fwrite((char *)scanline,
		int(sizeof(RGBE)),
		len,
		fp))
		- len);
    /* put magic header */
    putc(2, fp);
    putc(2, fp);
    putc(len>>8, fp);
    putc(len&255, fp); 
    /* put components seperately */
    for (i = 0; i < 4; i++) {
        for (j = 0; j < len; j += cnt) { /* find next run */
            for (beg = j; beg < len; beg += cnt) {
                for (cnt = 1; cnt < 127 && beg+cnt < len &&
                    scanline[beg+cnt][i] == scanline[beg][i]; cnt++)
                    ;
                if (cnt >= MINRUN)
                    break; /* long enough */
            }
            if (beg-j > 1 && beg-j < MINRUN) {
                c2 = j+1;
                while (scanline[c2++][i] == scanline[j][i])
                    if (c2 == beg) { /* short run */
                        putc(128+beg-j, fp);
                        putc(scanline[j][i], fp);
                        j = beg;
                        break;
                    }
            }
            while (j < beg) { /* write out non-run */
                if ((c2 = beg-j) > 128) c2 = 128;
                putc(c2, fp);
                while (c2--)
                    putc(scanline[j++][i], fp);
            }
            if (cnt >= MINRUN) { /* write out run */
                putc(128+cnt, fp);
                putc(scanline[beg][i], fp);
            } else
                cnt = 0;
        }
    }
    return(ferror(fp) ? -1 : 0);
}


int freadrgbe(RGBE* scanline, int len, FILE *fp)/* read in an encoded rgbe scanline */
{
    int i, j;
    int code, val;
    /* determine scanline type */
    if (len < MINELEN || len > MAXELEN)
        return(oldreadrgbe(scanline, len, fp));
    if ((i = getc(fp)) == EOF)
        return(-1);
    if (i != 2) {
        ungetc(i, fp);
        return(oldreadrgbe(scanline, len, fp));
    }
    scanline[0][GRN] = getc(fp);
    scanline[0][BLU] = getc(fp);
    if ((i = getc(fp)) == EOF)
        return(-1);
    if (scanline[0][GRN] != 2 || scanline[0][BLU] & 128) {
        scanline[0][RED] = 2;
        scanline[0][EXP] = i;
        return(oldreadrgbe(scanline+1, len-1, fp));
    } 
    if ((scanline[0][BLU]<<8 | i) != len)
        return(-1); /* length mismatch! */
    /* read each component */
    for (i = 0; i < 4; i++)
        for (j = 0; j < len; ) {
            if ((code = getc(fp)) == EOF)
                return(-1);
            if (code > 128) { /* run */
                code &= 127;
                val = getc(fp);
                while (code--)
                    scanline[j++][i] = val;
            } else /* non-run */
                while (code--)
                    scanline[j++][i] = getc(fp);
        }
        return(feof(fp) ? -1 : 0);
}


int oldreadrgbe(RGBE *scanline, int len, FILE *fp) /* read in an old rgbe scanline */
{
    int rshift;
    int i;
    
    rshift = 0;
    
    while (len > 0) {
        scanline[0][RED] = getc(fp);
        scanline[0][GRN] = getc(fp);
        scanline[0][BLU] = getc(fp);
        scanline[0][EXP] = getc(fp);
        if (feof(fp) || ferror(fp))
            return(-1);
        if (scanline[0][RED] == 1 &&
            scanline[0][GRN] == 1 &&
            scanline[0][BLU] == 1) {
            for (i = scanline[0][EXP] << rshift; i > 0; i--) {
                copyrgbe(scanline[0], scanline[-1]);
                scanline++;
                len--;
            }
            rshift += 8;
        } else {
            scanline++;
            len--;
            rshift = 0;
        }
    }
    return(0);
}


int fwritescan(COLOR *scanline, RGBE *clrscan, int len, FILE *fp) /* write out a scanline */
{
    int n;
    RGBE *sp = clrscan;
    
    /* convert scanline */
    n = len;
    while (n-- > 0) {
        setrgbe(sp[0], scanline[0][RED],
            scanline[0][GRN],
            scanline[0][BLU]);
        scanline++;
        sp++;
    }
    return(fwritergbe(clrscan, len, fp));
}


int freadscan(COLOR *scanline, RGBE *clrscan, int len, FILE *fp) /* read in a scanline */
{
    if (freadrgbe(clrscan, len, fp) < 0)
        return(-1);
    /* convert scanline */
    rgbe_color(scanline[0], clrscan[0]);
    while (--len > 0) {
        scanline++; clrscan++;
        if (clrscan[0][RED] == clrscan[-1][RED] &&
            clrscan[0][GRN] == clrscan[-1][GRN] &&
            clrscan[0][BLU] == clrscan[-1][BLU] &&
            clrscan[0][EXP] == clrscan[-1][EXP]) 
            copycolor(scanline[0], scanline[-1]);
        else
            rgbe_color(scanline[0], clrscan[0]);
    }
    return(0);
}


inline int setrgbe(RGBE rgbe,double r,double g,double b) /* assign a short color value */
{
    double d;
    int e;
    
    d = r > g ? r : g;
    if (b > d) d = b;
    
    if (d <= 1e-32) {
        rgbe[RED] = rgbe[GRN] = rgbe[BLU] = 0;
        rgbe[EXP] = 0;
        return(0);
    }
    
    d = frexp(d, &e) * 255.9999 / d;
    
    rgbe[RED] = (unsigned char)(r * d);
    rgbe[GRN] = (unsigned char)(g * d);
    rgbe[BLU] = (unsigned char)(b * d);
    rgbe[EXP] = e + COLXS;
    return(0);
}


inline int rgbe_color(COLOR col, RGBE rgbe) /* convert short to float color */
{
    float f;
    
    if (rgbe[EXP] == 0)
        col[RED] = col[GRN] = col[BLU] = 0.f;
    else {
        f = (float)ldexp(1.0, (int)rgbe[EXP]-(COLXS+8));
        col[RED] = (rgbe[RED] + 0.5f)*f;
        col[GRN] = (rgbe[GRN] + 0.5f)*f;
        col[BLU] = (rgbe[BLU] + 0.5f)*f;
    }
    return(0);
}
