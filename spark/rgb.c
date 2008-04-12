#include <stdio.h>
#include "spark.h"

#define IXSIZE      (23)
#define IYSIZE      (15)

putbyte(outf,val)
FILE *outf;
unsigned char val;
{
unsigned char buf[1];
 
buf[0] = val;
fwrite(buf,1,1,outf);
}
 
putshort(outf,val)
FILE *outf;
unsigned short val;
{
unsigned char buf[2];
 
buf[0] = (val>>8);
buf[1] = (val>>0);
fwrite(buf,2,1,outf);
}
 
static int putlong(outf,val)
FILE *outf;
unsigned long val;
{
unsigned char buf[4];
 
buf[0] = (val>>24);
buf[1] = (val>>16);
buf[2] = (val>>8);
buf[3] = (val>>0);
return fwrite(buf,4,1,outf);
}

void
RgbWriteHeader(FILE *fp, int width, int height, int depth)
{
    int i, x, y, pixmax, bpc;
    static char iname[] = "Instinctual Informer 1.0";

    bpc = depth > 8 ? 2 : 1;
    pixmax = (1 << depth) - 1;

    InformerDEBUG("RGB: depth (%d) bpc (%d) pixmax (%d)\n", depth, bpc, pixmax);

    putshort(fp, 474);         /* MAGIC               */
    putbyte(fp, 0);            /* STORAGE is VERBATIM */
    putbyte(fp, bpc);          /* BPC is 1 or 2       */
    putshort(fp, 3);           /* DIMENSION is 3      */
    putshort(fp, width);       /* XSIZE               */
    putshort(fp, height);      /* YSIZE               */
    putshort(fp, 3);           /* ZSIZE               */
    putlong(fp, 0);            /* PIXMIN is 0         */
    putlong(fp, pixmax);       /* PIXMAX              */
    for(i=0; i<4; i++)         /* DUMMY 4 bytes       */
        putbyte(fp, 0);
    fwrite(iname, 80, 1, fp);  /* IMAGENAME           */
    putlong(fp, 0);            /* COLORMAP is 0       */
    for(i=0; i<404; i++)       /* DUMMY 404 bytes     */
        putbyte(fp,0);
}

void
RgbStage(SparkMemBufStruct input, SparkMemBufStruct stage, SparkInfoStruct spark_info)
{
    int offset, index, inc, r, g, b;
    unsigned char *ptr, *buf;

    ptr = (unsigned char *) input.Buffer;
    buf = (unsigned char *) stage.Buffer;

    InformerDEBUG("RGB: RgbStage3x8 called\n");

    // rgb files are: header, red, green, blue
    // memory is: [rgb], [rgb], [rgb]

    InformerDEBUG("input BufDepth: %d\n", input.BufDepth);
    InformerDEBUG("stage BufDepth: %d\n", stage.BufDepth);
    InformerDEBUG("input inc: %d\n", stage.Inc);

    offset = 0;
    inc = stage.Inc/3;
    if (2 == inc) offset = 1;

    for (index = 0; index < spark_info.FramePixels; index++) {
        r = inc * (3 * index + 0) + offset;
        g = inc * (3 * index + 1) + offset;
        b = inc * (3 * index + 2) + offset;

        if (index < 10) {
            InformerDEBUG("pixel %d: (%d, %d, %d) [%d, %d, %d]\n",
                          index, ptr[r], ptr[g], ptr[b], r, g, b);
        }
        buf[index + 0*spark_info.FramePixels] = ptr[r];
        buf[index + 1*spark_info.FramePixels] = ptr[g];
        buf[index + 2*spark_info.FramePixels] = ptr[b];
    }
}

void
RgbResize(SparkMemBufStruct input, SparkMemBufStruct resize, SparkInfoStruct spark_info)
{
    InformerDEBUG("RGB: now resizing...\n");
    sparkCopyBuffer(input.Buffer, resize.Buffer);
    // sparkResizeBuffer(resize.Buffer, spark_info.FrameDepth, resize.Buffer, SPARKBUF_RGB_24_3x8);
    sparkCopyChannel(input.Buffer,  SPARK_RGB, spark_info.FrameDepth,
                    resize.Buffer, SPARK_RGB, SPARKBUF_RGB_24_3x8);
    // sparkCopyChannel(input.Buffer,  SPARK_RED, spark_info.FrameDepth,
    //                  resize.Buffer, SPARK_RED, SPARKBUF_RGB_24_3x8);
    // sparkCopyChannel(input.Buffer,  SPARK_GREEN, spark_info.FrameDepth,
    //                  resize.Buffer, SPARK_GREEN, SPARKBUF_RGB_24_3x8);
    // sparkCopyChannel(input.Buffer,  SPARK_BLUE, spark_info.FrameDepth,
    //                  resize.Buffer, SPARK_BLUE, SPARKBUF_RGB_24_3x8);
    InformerDEBUG("RGB: resize done!\n");
}

int
RgbWriteSparkFrame(char *path,
                   SparkMemBufStruct input,
                   SparkMemBufStruct stage,
                   SparkMemBufStruct resize,
                   SparkInfoStruct spark_info)
{
    FILE *fp;

    if ((fp=fopen(path, "w")) == NULL) {
        InformerERROR("can't write datafile [%s]", path);
        return FALSE;
    }

    if (SPARKBUF_RGB_24_3x8 == spark_info.FrameDepth) {
        // RgbStage(input, stage, spark_info);
        RgbResize(input, resize, spark_info);
        RgbStage(resize, stage, spark_info);
    } else {
        RgbResize(input, resize, spark_info);
        RgbStage(resize, stage, spark_info);
    }

    InformerDEBUG("RGB: FramePixels (%d)\n", spark_info.FramePixels);

    RgbWriteHeader(fp, input.BufWidth, input.BufHeight, 8);
    fwrite((unsigned char *) stage.Buffer, sizeof(unsigned char), spark_info.FramePixels * 3, fp);
    fclose(fp);

    InformerDEBUG("OK. Frame has been written!\n");
    return TRUE;
}

