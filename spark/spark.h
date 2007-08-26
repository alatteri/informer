/* --------------------------------------------------------------------------- *\

        EFFECTS: INFERNO/FLAME/FLINT/EFFECT -  EDITING: FIRE/SMOKE 
	
	Last Modification: 02/2005

        Description : SPARKs plug-in header file for Advanced Systems 
                      applications.


	Sample programs:

	The following source files demonstrate the use of
	the Spark API:

        sparkAverage1.c     sparkImage.c
	sparkAverage2.c     sparkInvert.c
	sparkBlend.c        sparkKey.c
	sparkBroadcast.c    sparkMotionKey.c
        sparkCCKopy.c       sparkReverse.c
        sparkComposite.c    sparkRGB.c
	sparkCompress.c     sparkWipe.c
	sparkFractal.c      sparkYUV.c
 

                           
        --------------------------------------------------------------- 
        PLEASE CONSULT THE SPARKs DEVELOPERS GUIDE FOR MORE INFORMATION
        ---------------------------------------------------------------  

        (c) Copyright 1991-2005 Autodesk Canada Inc. / Autodesk, Inc.
        All rights reserved.

        These coded instructions, statements, and computer programs  contain
        unpublished proprietary information written by Autodesk Canada Inc./
        Autodesk, Inc. and are protected by Federal copyright law.  They may
	not be  disclosed to  third parties  or copied  or duplicated in any
        form,  in whole or in  part,  without  the prior  written consent of
        Autodesk Canada Inc. / Autodesk, Inc.

\* --------------------------------------------------------------------------- */

#ifndef _SPARK_H
#define _SPARK_H

#ifdef __cplusplus
extern "C" {
#endif

/* Please do not change the following line */
#define SPARK_VERSION_NUMBER 4.06


/**************************************************************************/
/* This include file contains the data structures and function prototypes */
/* that are required when writing a Spark.                                */
/**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <fcntl.h>

#include <sys/types.h>

/**********************************************************************/
/* For portable and fixed-size buffer structures, the types defined   */
/* in /usr/include/inttypes.h have been used.                         */
/* But both sys/types and inttypes defines the following...           */
/**********************************************************************/
#include <inttypes.h>

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

#define SPARK_FAILURE 0
#define SPARK_SUCCESS 1

/**********************************************************************/
/* For multi-platform compatibility, some platforms are big endian    */
/* and some are little endian.                                        */
/**********************************************************************/

#if defined __i386__ || defined __x86_64__
#define SPARK_LITTLE_ENDIAN 1
#endif

/***************************************************************************/
/*----------------------------- SPARK INTERFACE ---------------------------*/
/***************************************************************************/

/***************************************************************************/
/* The SparkInfoStruct is the structure that contains all the information  */
/* about the selected clips, the frame sizes, the image buffers, etc. that */
/* the Spark will require to perform its processing.                       */
/***************************************************************************/
typedef struct spark_info_struct {
   char            Name[FILENAME_MAX];	/* Filename for Spark to save its
					 * current State */
   int             FrameBufferX;/* Origin of Frame Buffer on Screen */
   int             FrameBufferY;/* Origin of Frame Buffer on Screen */
   int             TotalBuffers;/* Maximum number of Framebuffers
				 * available */
   int             TotalClips;	/* Number of Clips requested */
   int             FramePixels;	/* Total Pixels of image data in a single
				 * frame */
   int             FrameBytes;	/* Total Bytes of image data in a single
				 * frame */
   char            Mode;	/* Currently Active Mode */
   unsigned int    FrameNo;	/* Current Frame (from 0 to TotalFrameNo-1 */
   unsigned int    TotalFrameNo;/* Total Number of Frames */
   int             FrameWidth;	/* Current Frame Width in Pixels */
   int             FrameHeight;	/* Current Frame Height in Pixels */
   int             FrameDepth;	/* Current Pixel Depth (see return values below) */
   ulong         **Buffers;	     /* Array of pointers to frame buffers */
   char            NumProcessors;    /* Number of Processors on-line */
   char            Context;	     /* Setups, Anim or Control */
   ulong	   RenderQuality;    /* Level of rendering quality */
   ulong           reserved[11];
} SparkInfoStruct;

/***************************************************************************/
/*------------------- SPARK MEMORY BUFFER INTERFACE -----------------------*/
/***************************************************************************/

/* buffer state bit flags */

#define MEMBUF_UNREGISTERED (1<<0)
#define MEMBUF_REGISTERED   (1<<1)
#define MEMBUF_LOCKED       (1<<2)
#define MEMBUF_CLEAN        (1<<3)
#define MEMBUF_DIRTY        (1<<4)
#define MEMBUF_INVALID      (1<<5)

typedef int BState; 

/***************************************************************************/
/*   Used to specify spark memory buffer component ordering                */
/***************************************************************************/
typedef enum spark_buffer_fmt
{                               /* Increment:            */
    SPARK_FMT_UNKNOWN = 0,
    SPARK_FMT_MONO,             /* 1 channel, 8/12 bits  */
    SPARK_FMT_FLOAT,            /* sizeof(float)         */
    SPARK_FMT_RGB,              /* 3 channels, 8/12 bits */
    SPARK_FMT_HLS,              /* 3 channels, 8/12 bits */
    SPARK_FMT_YUV,              /* 3 channels, 8/12 bits */
    SPARK_FMT_RGBA,             /* 4 channels, 8/12 bits */
    SPARK_FMT_HLSA,             /* 4 channels, 8/12 bits */
    SPARK_FMT_YUVA,             /* 4 channels, 8/12 bits */
    SPARK_FMT_UYVY,             /* 4 channels, 8/12 bits */
    
    SPARK_FMT_AUDIO_INT16 = 100 /* 16-bit signed integer samples */

} SparkBufferFmt;


/****************************************************************/
/* SparkMemBufStruct parameters                                 */
/*                                                              */
/* Video Context:                                               */
/*                                                              */
/* TotalBuffers   Total number of registered memory buffers     */
/* Buffer         Pointer to memory buffer                      */
/* BufState       State of memory (see return values above)     */ 
/* BufSize        Size of the buffer in bytes                   */
/* BufFmt         Buffer component ordering                     */
/*                                                              */
/* BufWidth       Current frame width in pixels                 */
/* BufHeight      Current frame height in pixels                */
/* BufDepth       Current pixel depth (see return values below) */
/* Stride         Length of a line in bytes                     */
/* Inc            Increment in bytes to move to next pixel      */
/*                                                              */
/* Audio Context:                                               */
/*                                                              */
/* TotalBuffers   Total number of registered memory buffers     */
/* Buffer         Pointer to memory buffer                      */
/* BufState       State of memory (see return values above)     */
/* BufSize        Size of the buffer in bytes                   */ 
/* BufFmt         Sample format                                 */
/*                                                              */
/* BufWidth       Number of samples in a track                  */
/* BufHeight      Number of tracks                              */
/* BufDepth       Bits per sample                               */
/* Stride         Length to next track in bytes                 */
/* Inc            Increment in bytes to move to next sample     */
/****************************************************************/
typedef struct spark_membuf_struct {
   int             TotalBuffers;
   ulong          *Buffer;     
   BState          BufState;   
   int             BufSize;    
   SparkBufferFmt  BufFmt;

   int             BufWidth;
   int             BufHeight;
   int             BufDepth;
   int             Stride; 
   int             Inc;

   ulong           reserved[10];
} SparkMemBufStruct;


/***********************************************************************/
/*--------------------  IMAGE BUFFER DEFINITIONS   --------------------*/
/*                                                                     */
/* RGB8_OGL corresponds to an 8 bit per component buffer format with   */
/* the pixel color components ordered in memory. This is the format    */
/* for OpenGL-compatible image buffers as used in the Advanced Systems */
/* Effects and Editing products configured in 8 bit mode.              */
/*                                                                     */
/* For 12 bit per color component formats (only supported in Flame and */
/* Inferno), the 12 data bits are stored in the 12 most significant    */
/* bits of a 16 bit short. The 4 least significant bits are ignored.   */
/* Note that on Linux, little endian system, the least significant     */
/* bits to be ignored are not the same as on Irix. The padding in the  */
/* structure must be changed to follow the system requirements.        */
/*                                                                     */
/* RGB12_OGL corresponds to a 12 bit per component buffer format with  */
/* the 16 bit pixel color components ordered in memory. There is no    */
/* extra Alpha component. This is the format used by Inferno in the    */
/* Advanced Systems Effects Editing products configured in 8 bit mode. */
/*                                                                     */
/***********************************************************************/

/*-- OpenGL buffers --*/
typedef struct {
   uint8_t   r;
   uint8_t   g;
   uint8_t   b;
} RGB8_OGL;

typedef struct {
#ifdef SPARK_LITTLE_ENDIAN
   uint16_t  pad1:4;
   uint16_t  r:12;
   uint16_t  pad2:4;
   uint16_t  g:12;
   uint16_t  pad3:4;
   uint16_t  b:12;
#else
   uint16_t  r:12;
   uint16_t  pad1:4;
   uint16_t  g:12;
   uint16_t  pad2:4;
   uint16_t  b:12;
   uint16_t  pad3:4;
#endif
} RGB12_OGL;

typedef  RGB8_OGL   RGB24_Struct;
typedef  RGB12_OGL  RGBA64_Struct;

/************************************************************************/
/* The following constants define the number of bits making up a        */
/* particular image. The constants other than PIX_24 or PIX_64 (the     */
/* INFERNO-supported image channel widths) may be used for images that  */
/* are being converted within a given Spark (with the sparkResizeBuffer */
/* function).                                                           */
/************************************************************************/
#define SPIX_8            8	/* Mono, 8 bits per component */
#define SPIX_16          16	/* Mono, 16 bits per component --
				 * essentially the same as PIX_12, since
				 * the lower 4 bits of each component are
				 * ignored, at least in this release */
#define SPIX_24          24	/* RGB, 8 bits per component */
#define SPIX_32          32	/* RGBA, 8 bits per component */
#define SPIX_48          48	/* RGB,  12 bits per component;    OpenGL */
#define SPIX_64          64	/* RGBA, 12/16 bits per component; IrisGL */

/***********************************************************************/
/* COMPATIBILITY NOTES: although in previous versions the FrameDepth   */
/* field in SparkInfoStruct could be interpreted as the number of      */
/* bytes per pixel and thus a buffer could be allocated using          */
/* FrameWidth * FrameHeight * FrameDepth bytes of memory, this is no   */
/* longer the case: FrameDepth should be interpreted as an arbitrary   */
/* enum. In all cases, the FrameBytes member of the SparkInfoStruct    */
/* structure should be used as the size of the memory to be allocated. */
/***********************************************************************/
typedef enum {

    /* Deprecated */
    SPARKBUF_MONO_8  = SPIX_8,
    SPARKBUF_MONO_16 = SPIX_16, 
    SPARKBUF_BGR_24  = SPIX_24,
    SPARKBUF_RGB_24,
    SPARKBUF_ABGR_32 = SPIX_32, 
    SPARKBUF_RGB_48  = SPIX_48,
    SPARKBUF_ABGR_64 = SPIX_64,
    
    /* New formats */
    SPARKBUF_RGB_24_3x8  = SPIX_24 + 1, /* RGB, 8bit components */
    SPARKBUF_RGB_48_3x10,           /* RGB, 10 significant bits / 16bits comp.*/
    SPARKBUF_RGB_48_3x12 = SPIX_48, /* RGB, 12 significant bits / 16bits comp.*/
    SPARKBUF_RGB_48_3x16_FP         /* RGB, 16bit floating point components.  */
} SparkPixelFormat;


/***************************************************************************/
/* These constants define the level(s) at which the Spark is to operate    */
/* and constitute the group of valid return values for the SparkInitialise */
/* function (see below). Should the Spark be required to operate at        */
/* different levels (e.g. Desktop and Module), the return value can be a   */
/* bitwise OR of these values (e.g. (SPARK_DESKTOP | SPARK_MODULE) ), but  */
/* the source code must support each of levels as well.                    */
/***************************************************************************/
#define SPARK_DESKTOP	      (1<<0)
#define SPARK_MODULE	      (1<<1)
#define SPARK_AUDIO_TIMELINE  (1<<2)

/***************************************************************************/
/* This enumerated type defines the return values of the sparkCallingEnv   */
/* function. It indicates under which environment the Spark plug-in is     */
/* being called.                                                           */
/***************************************************************************/

enum {
    SPARK_COMPONENT = 1,
    SPARK_NODE_BATCH
};

/***************************************************************************/
/* FIRE: This enumerated type defines the possible rendering schemes that  */
/* can be used in FIRE.                                                    */
/***************************************************************************/

enum {
    SPARK_RENDER_QUALITY_HIGH = 1, /* high quality setting;          */
                                   /* recomputed at full resolution  */
    
    SPARK_RENDER_QUALITY_LOW       /* low quality setting;                     */
                                   /* can be recomputed in close to real time  */
};

/***************************************************************************/
/* This enumerated type defines the range of return values that the        */
/* 'CallbackArg' parameter associated to callback functions can take. This */
/* value can be used to determine the processing that should take place    */
/* given the action that the user has taken on the given user interface    */
/* control.                                                                */
/***************************************************************************/
enum {
   SPARK_UPDATE_CONTINUOUS = 0,
   SPARK_UPDATE_DRAG       = 0,
   SPARK_UPDATE_END,
   SPARK_UPDATE_NONE,
   SPARK_UPDATE_DRAG_END
};

#define	    SPARK_FLAG_NONE	(0)
/* Do not use 1<<0 -- for compatibility with earlier versions */
#define	    SPARK_FLAG_X	(1<<1)
#define	    SPARK_FLAG_Y	(1<<2)
#define     SPARK_FLAG_NO_ANIM  (1<<3)
#define     SPARK_FLAG_NO_INPUT (1<<4)

/***************************************************************************/
/* This enumerated type defines the different states in which a Spark can  */
/* be active at the Module level. The information is contained in the      */
/* 'Context' field of the SparkInfoStruct structure. The Spark-writer need */
/* not be concerned with this field, but it might be useful to note that   */
/* the user-defined interface controls are only active when the Spark is   */
/* in the CONTROL state.                                                   */
/***************************************************************************/
enum {
   SPARK_MODE_SETUP    = 0,
   SPARK_MODE_CONTROL  = 1,	 /* Kept for compatibility with old sparks */
   SPARK_MODE_CONTROL1 = 1,
   SPARK_MODE_ANIM     = 2,
   SPARK_MODE_CONTROL2 = 3,
   SPARK_MODE_CONTROL3 = 4,
   SPARK_MODE_CONTROL4 = 5,
   SPARK_MODE_CONTROL5 = 6
       
};

/***************************************************************************/
/* This enumerated type defines the range of values that are contained in  */
/* the 'Mode' field of the SparkInfoStruct structure. The value in this    */
/* field defines the state at which the spark is currently active.         */
/***************************************************************************/
enum {
   SPARK_CURRENT_DESKTOP = 0,
   SPARK_CURRENT_MODULE
};


/***************************************************************************/
/* The SparkGraphInfoStruct contains information about the graphics setup. */
/* This structure is passed to a Spark by sparkGraphSetup.                 */
/***************************************************************************/
typedef struct spark_graph_info_struct {
   float           PixelAspectRatio; /* Current pixel aspect ratio */
   long            GraphicsMaxX;     /* Horizontal size of the screen in pixels */
   long            GraphicsMaxY;     /* Vertical size of the screen in pixels */
   ulong           reserved[12];
} SparkGraphInfoStruct;


/***************************************************************************/
/* This enumerated type defines the clips from which a call to the         */
/* sparkGetFrame() will select an individual frame for extended processing.*/
/* Also used with sparkGetAudio() and SparkClipFilter()                    */
/***************************************************************************/

typedef enum {
   SPARK_FRONT_CLIP = 0,
   SPARK_BACK_CLIP  = 1,
   SPARK_MATTE_CLIP = 2,
   SPARK_AUX_CLIP   = 3
} SparkClipSelect;

/***************************************************************************/
/* This enumerated type defines the control title buttons                  */
/***************************************************************************/

typedef enum {
   SPARK_CONTROL_1 = 0,
   SPARK_CONTROL_2 = 1,
   SPARK_CONTROL_3 = 2,
   SPARK_CONTROL_4 = 3,
   SPARK_CONTROL_5 = 4,
   SPARK_PROCESS   = 5,
   SPARK_ANALYSE   = 6
} SparkControlSelect;

/***************************************************************************/
/* This enumerated type defines the different event callbacks available to */
/* sparks.                                                                 */
/***************************************************************************/
typedef enum {
    SPARK_EVENT_EXIT       = 0,  
    SPARK_EVENT_LOADSETUP  = 1,
    SPARK_EVENT_SAVESETUP  = 2,
    SPARK_EVENT_PREVIEW    = 3,
    SPARK_EVENT_PROCESS    = 4,
    SPARK_EVENT_PLAY       = 5,
    SPARK_EVENT_EXITPLAY   = 6, 
    SPARK_EVENT_SETUP      = 7,  
    SPARK_EVENT_EXITSETUP  = 8,
    SPARK_EVENT_CONTROL1   = 9,
    SPARK_EVENT_CONTROL2   = 10,    
    SPARK_EVENT_CONTROL3   = 11,    
    SPARK_EVENT_CONTROL4   = 12,    
    SPARK_EVENT_FRONT      = 13,
    SPARK_EVENT_BACK       = 14,
    SPARK_EVENT_MATTE      = 15,      
    SPARK_EVENT_RESULT     = 16,
    SPARK_EVENT_ANIMATION  = 17,
    SPARK_EVENT_CONTROL5   = 18,
    SPARK_EVENT_CE_RESETALL= 19,
    SPARK_EVENT_ANALYSE    = 20,
    SPARK_EVENT_AUX1       = 21,
    SPARK_EVENT_AUX2       = 22,
    SPARK_EVENT_AUX3       = 23,        
    SPARK_EVENT_AUX4       = 24,    
    SPARK_EVENT_AUX5       = 25,    
    SPARK_EVENT_AUX6       = 26,    
    SPARK_EVENT_AUX7       = 27,    
    SPARK_EVENT_AUX8       = 28,    
    SPARK_EVENT_AUX9       = 29,    /* ... All other aux between ...*/
    SPARK_EVENT_AUX_LAST   = 50,
    SPARK_EVENT_UNKNOWN    = 255                
} SparkModuleEvent;

   
/***************************************************************************/
/* This enumerated type defines the different cursors available to sparks. */
/* All currently available Inferno cursors are listed here.  Some          */
/* cursors may change in future releases.                                  */
/***************************************************************************/
enum {
   SPARK_CURSOR_ARROW = -1,	/* Standard small red arrow */
   SPARK_CURSOR_ACTION = 0,	/* Product logo */
   SPARK_CURSOR_NORMAL,		/* Yellow cross */
   SPARK_CURSOR_FRONT,		/* Red clip selection arrow */
   SPARK_CURSOR_BACK,		/* Green clip selection arrow */
   SPARK_CURSOR_MATTE,		/* Blue clip selection arrow */
   SPARK_CURSOR_BUMP,		/* Cyan clip selection arrow */
   SPARK_CURSOR_DESTINATION,	/* White output clip selection arrow */
   SPARK_CURSOR_CLONESOURCE,	/* Red cross */
   SPARK_CURSOR_CLONEDESTINATION,	/* Pink cross */
   SPARK_CURSOR_FRAMEGRAB,	/* Red cross */
   SPARK_CURSOR_PICK,		/* White color picker */
   SPARK_CURSOR_PAINT,		/* Green cross */
   SPARK_CURSOR_VIEW,		/* Green cross */
   SPARK_CURSOR_CONFIRM,	/* Red cross */
   SPARK_CURSOR_PAN,		/* White hand */
   SPARK_CURSOR_ZOOM,		/* Magnifying glass */
   SPARK_CURSOR_ZOOMIN,		/* Magnifying glass with + sign */
   SPARK_CURSOR_ZOOMOUT,	/* Magnifying glass with - sign */
   SPARK_CURSOR_ADD_KEY,	/* Green cross with + sign */
   SPARK_CURSOR_REMOVE_KEY,	/* Red cross with - sign */
   SPARK_CURSOR_MOVE_HANDLE,	/* Blue cross with square box in middle */
   SPARK_CURSOR_BUSY_0,		/* Animation sequence of rotating white cross */
   SPARK_CURSOR_BUSY_1,
   SPARK_CURSOR_BUSY_2,
   SPARK_CURSOR_BUSY_3,
   SPARK_CURSOR_GEOMETRY,	/* Small green cross */
   SPARK_CURSOR_FILLPAINT,	/* Green paint bucket overspilling */
   SPARK_CURSOR_ROLLPAINT,	/* Green hand */
   SPARK_CURSOR_LINEPAINT,	/* Small green cross */
   SPARK_CURSOR_AUTOPAINT,	/* Green cross */
   SPARK_CURSOR_DESTINATION_MORE/* White selection arrow with large + sign */
};

/***************************************************************************/
/*-------------------------- SPARK CHANNEL EDITOR -------------------------*/
/* The following enums are used when customizing the channel editor layout.*/
/* Folders can be created and controls can be added to the channel editor  */
/* exclusively (i.e not related to Spark UI controls).                     */  
/***************************************************************************/
enum {
   SPARK_CE_CONTROL = 0,    /* exclusively channel editor control */
   SPARK_UI_CONTROL         /* Spark UI control */
};

enum{
    SPARK_CE_LEVEL1 = 1,    /* folders and curves */
    SPARK_CE_LEVEL2,	    /* folders and curves */
    SPARK_CE_LEVEL3, 	    /* folders and curves */
    SPARK_CE_LEVEL4	    /* curves only */
};


/***************************************************************************/
/*------------------------------ SPARK UI ---------------------------------*/
/* The following structures define the user interface controls that are    */
/* available within the Spark environment. The supporting documentation    */
/* describes the required naming conventions for instantiating any of the  */
/* desired controls.                                                       */
/***************************************************************************/
/**************************/
/* Integer Slider Control */
/**************************/
typedef struct {
   int             Value;	/* Current value of parameter */
   int             Min;		/* Minimum allowable value */
   int             Max;		/* Maximum allowable value */
   int             Increment;	/* Slider Increment */
   unsigned int    Flags;	/* Control Flags */
   char           *Title;	/* Printf formatted string for display of
				 * parameter */

/* Callback function invoked when the 'Value' field changes */
   ulong          *( *Callback ) ( int, SparkInfoStruct );
}               SparkIntStruct;

/*********************************/
/* Floating Point Slider Control */
/*********************************/
typedef struct {
   float           Value;	/* Current value of parameter */
   float           Min;		/* Minimum allowable value */
   float           Max;		/* Maximum allowable value */
   float           Increment;	/* Slider Increment */
   unsigned int    Flags;	/* Control Flags */
   char           *Title;	/* Printf formatted string for display of
				 * parameter */

/* Callback function invoked when the 'Value' field changes */
   ulong          *( *Callback ) ( int, SparkInfoStruct );
} SparkFloatStruct;

/*************************/
/* Color Display Control */
/*************************/
typedef struct {
   ulong           Color;	/* Current value of color */

/* Callback function invoked when the 'Color' field changes */
   ulong          *( *Callback ) ( int, SparkInfoStruct );
}               SparkColorStruct;

/**********************/
/* Popup Menu Control */
/**********************/
typedef struct {
   int             Value;	/* Current Popup Index */
   int             TitleCount;	/* Count of Entries in Popup */

/* Callback function invoked when the 'Value' field changes */
   ulong          *( *Callback ) ( int, SparkInfoStruct );
   char           *Titles[20];	/* Popup Titles */
}               SparkPupStruct;

/*************************/
/* Toggle Button Control */
/*************************/
typedef struct {
   char            Value;	/* Current State of Boolean */
   char           *Title;	/* Printf formatted string for display of
				 * parameter */

/* Callback function invoked when the 'Value' field changes */
   ulong          *( *Callback ) ( int, SparkInfoStruct );
}               SparkBooleanStruct;

/***********************/
/* Push Button Control */
/***********************/
typedef struct {
   char           *Title;

/* Callback function invoked when button pushed */
   ulong          *( *Callback ) ( int, SparkInfoStruct );
}               SparkPushStruct;

/**********************/
/* Text Entry Control */
/**********************/
#define SPARK_MAX_STRING_LENGTH  200

typedef struct {
   char            Value[SPARK_MAX_STRING_LENGTH];	/* Current Text String */
   char           *Title;	/* Printf formatted string for display of
				 * parameter */
   unsigned int    Flags;	/* Control Flags */

/* Callback function invoked when the 'Value' field changes */
   ulong          *( *Callback ) ( int, SparkInfoStruct );
}               SparkStringStruct;

/**********************/
/*    Canvas Control  */
/**********************/

typedef struct {
    int XO;      /* Canvas Origin */
    int YO;
    int Width;   /* Canvas Width  */
    int Height;  /* Canvas Height */
}               SparkCanvasInfo;

typedef struct {
   void           (*DisplayCallback) (SparkCanvasInfo);
   int            (*InteractCallback)(SparkCanvasInfo Canvas, 
                                      int PointerX, int PointerY, 
                                      float PointerPressure); /* return 1 for canvas display */
}               SparkCanvasStruct;


/************************************************************************/
/* The following enumerated type defines the channels associated to     */
/* image buffers of different formats. These are the values that are    */
/* expected as the 'Channel' parameters for the sparkBlur and           */
/* sparkCopyChannel routines.                                           */
/************************************************************************/
enum {
   SPARK_RED = 0,		/* Chan 1      : expects a 3-channel
				 * buffer */
   SPARK_GREEN,			/* Chan 2      : expects a 3-channel
				 * buffer */
   SPARK_BLUE,			/* Chan 3      : expects a 3-channel
				 * buffer */
   SPARK_ALPHA,			/* Chan 4      : expects a 4-channel
				 * buffer */
   SPARK_RGB,			/* Chan 1,2,3  : expects a 3-channel
				 * buffer */

   SPARK_RGB_4C,		/* Chan 1,2,3  : expects a 4-channel
				 * buffer */
   SPARK_RGBA,			/* Chan 1,2,3,4: expects a 4-channel
				 * buffer */
   SPARK_RED_4C,		/* Chan 1      : expects a 4-channel
				 * buffer */
   SPARK_GREEN_4C,		/* Chan 2      : expects a 4-channel
				 * buffer */
   SPARK_BLUE_4C,		/* Chan 3      : expects a 4-channel
				 * buffer */

   SPARK_MONO			/* Chan 1      : expects a 1-channel
				 * buffer */
};

/************************************************************************/
/* The following enumerated type defines the scan mode of the current   */
/* spark setup.                                                         */
/************************************************************************/
typedef enum {
   SPARK_SCAN_FORMAT_UNDEFINED = -1,
   SPARK_SCAN_FORMAT_FIELD_1 = 0,
   SPARK_SCAN_FORMAT_FIELD_2 = 1,
   SPARK_SCAN_FORMAT_PROGRESSIVE = 2
} SparkScanFormat;

/****************************/
/* Process Management Types */
/****************************/

typedef volatile void SparkTaskHandle_t;
typedef void    SparkTaskFunc_t(void*);

/************************************************************************/
/* The following functions (capital Spark prefix) are the Spark-defined */
/* routines that will be called by the application environment. Please  */
/* refer to the Spark support documentation for a full description of   */
/* the uses and requirements for each function.                         */
/************************************************************************/

/************************************************************************/
/*-------------- PROTOTYPES FOR MANDATORY SPARK FUNCTIONS --------------*/
/************************************************************************/

unsigned int    SparkInitialise(        SparkInfoStruct SparkInfo );
int             SparkClips(             void );
ulong          *SparkProcess(           SparkInfoStruct SparkInfo );
void            SparkUnInitialise(      SparkInfoStruct SparkInfo );

/************************************************************************/
/*--------------- PROTOTYPES FOR OPTIONAL SPARK FUNCTIONS --------------*/
/****************** Called by the application if defined ****************/

int	        SparkProcessStart(      SparkInfoStruct SparkInfo );
void	        SparkProcessEnd(        SparkInfoStruct SparkInfo );

ulong	       *SparkAnalyse(           SparkInfoStruct SparkInfo );
int 	        SparkAnalyseStart(      SparkInfoStruct SparkInfo );
void	        SparkAnalyseEnd(        SparkInfoStruct SparkInfo );

void            SparkMemoryTempBuffers( void );
ulong          *SparkInteract(          SparkInfoStruct SparkInfo,
			                int SX, int SY,
			                float Pressure,
			                float VX, float VY, 
			                float zoom_factor );
void            SparkOverlay(           SparkInfoStruct SparkInfo,
			                float zoom_factor );
void		SparkChannelEditor(     void );
void            SparkEvent(             SparkModuleEvent Event );
void            SparkSetupIOEvent(      SparkModuleEvent Event, 
                                        char *Path, 
                                        char *File );
void            SparkIdle(              void );
void            SparkFrameChange(       SparkInfoStruct SparkInfo);

                /* If not defined, support for SPARKBUF_RGB_24_3x8 and */
                /* SPARKBUF_RGB_48_3x12 is assumed.                    */
int             SparkIsInputFormatSupported( SparkPixelFormat fmt );

/*-- Macro for negative zoom_factor correction --*/
#define SPARK_GET_APPLIED_ZOOM(z)   \
{				    \
    if ( z < 1 ) { z = 1/(2-z); }   \
}

/************************************************************************/
/*--------------- PROTOTYPES FOR SPARK UTILITY FUNCTIONS ---------------*/
/************************************************************************/

/*********************************************/
/* Functions for Intermediate Memory Buffers */
/*********************************************/
int             sparkMemRegisterBuffer(     void );
int             sparkMemRegisterBufferSize( unsigned long size );
int             sparkMemRegisterBufferFmt(  int width, 
                                            int height, 
                                            SparkBufferFmt fmt );

int             sparkMemGetBuffer(          int id, 
                                            SparkMemBufStruct *memBufInfo );
int             sparkMemGetFreeMemory(      void );


/********************/
/* System Functions */
/********************/
void            sparkError(             char *ErrorString );
void            sparkMpFork(            void ( *Function ) (), 
                                        int NumArgs,... );
void            sparkMpForkPixels(      void ( *Function ) (), 
                                        ulong Pixels, 
                                        int NumArgs,... );
void            sparkMpInfo(            ulong *offset, 
                                        ulong *pixels );
SparkTaskHandle_t* sparkMpAllocateTaskHandle(void );
int             sparkMpCreateTask(      SparkTaskHandle_t* handle, 
				        const char* name,
					int cpu,
				        SparkTaskFunc_t* entry, 
					void* arg );
void            sparkMpWaitTask(        SparkTaskHandle_t* handle );
void            sparkMpFreeTaskHandle(  SparkTaskHandle_t* handle );
int             sparkMpGetCpu(          void );
int             sparkMpIsMainTask(      void );
void            sparkSetPermissions(    char *filename );
void            sparkSaveSetup(         char *filename );
void            sparkLoadSetup(         char *filename );
void            sparkReprocess(         void );
void            sparkProcessTruncate(   void );
void            sparkProcessAbort(      void );

void            sparkGraphSetup(        SparkGraphInfoStruct *SparkGraphInfo );
float           sparkAPIVersionInfo(    void );
const char*     sparkProgramGetName(    void );
int		sparkCallingEnv(        void );
void		sparkWorkingDir(        char *Dir );

void            sparkGetInfo(           SparkInfoStruct *SparkInfo );
SparkScanFormat sparkGetScanFormat(     void );


/* Note that environment variables and current working directory are   */
/* not propagated with the sparkSystemSh and sparkSystemNoSh calls.    */
/* If some environment variables are needed, they should be set as     */
/* part of the command.                                                */

/* wrapper to pcreatel(). If "wait" is TRUE, will wait for the child    */
/* process to complete before returning. The argument "cmd" will be     */
/* passed to a subshell (/bin/sh) which will perform path search,       */
/* wildcard expansion (if required) as well as break up the command     */
/* line into separate arguments to be passed to the process to be       */
/* started.                                                             */
int             sparkSystemSh( int wait,
			       const char *cmd );

/* wrapper to pcreatev(). If "wait" is TRUE, will wait for the child    */
/* process to complete before returning. path points to the executable. */
/* argv is a pointer to an array of strings representing the arguments  */
/* to the subprocess.  Usually argv[0] is the path to the executable,   */
/* or any other name you care to give it. No path searching is done, no */
/* wildcard expansion is performed. Don't forget that the last entry in */
/* argv[] should be a zero to indicate its end.                         */
int              sparkSystemNoSh( int wait,
				  const char *path,
				  const char **argv );

/* wrapper to waitpid(). pid should be the value that was returned from */
/* sparkSystemSh or sparkSystemNoSh if called with wait FALSE.          */
/* If statptr is non-zero, 16 bits of information called status are     */
/* stored in the low-order 16 bits of the location pointed to by        */
/* statptr.  See wstat for more information about statPtr.              */
/* The options argument is constructed from the bitwise inclusive OR of */
/* zero or more of the following flags : WNOHANG, WUNTRACED.            */
/* See waitpid for more information.                                    */
int              sparkWaitpid( pid_t pid,
			       int *statptr,
			       int option );

/****************************/
/* User Interface Functions */
/****************************/
void            sparkCursorBusy(        void );
void            sparkControlUpdate(     int ControlNo );
void            sparkSetupControlUpdate(int ControlNo );
void            sparkViewingDraw(       void );
float           sparkGetViewerRatio(    void );
void            sparkMessage(           char *MessageString );
int             sparkMessageConfirm(    char *MessageString );
void            sparkMessageDelay( int delayMilliSec, char *MessageString );
void            sparkViewingCursor(     int CursorIndex );
void            sparkClipControlTitle(  SparkClipSelect Clip, 
                                        char *Title );
void            sparkControlTitle(      SparkControlSelect Control, 
                                        char *Title );
void            sparkResultClipName(    char * NewName );
int             sparkIsAutoKeyOn(       void) ;
void            sparkDisableParameter(  int ControlType, 
                                        int ControlNo );
void            sparkEnableParameter(   int ControlType, 
                                        int ControlNo );

float           sparkPointerInfo(       int *PX,
                                        int *PY );
int             sparkPointerRead(       void );
void            sparkPointerWaitOff(    void );
void            sparkPointerWaitOn(     void );
int             sparkQueryKeyMap(       long Device ); /* uses the old device codes extracted from gl.h and device.h */

/**************************/
/* File Browser Functions */
/**************************/
void		sparkFileBrowserDisplayLoad(char *Path, 
                                            char *Extension, 
                                            void (*Callback) (char *FileName) );
void		sparkFileBrowserDisplaySave(char *Path, 
                                            char *Extension, 
                                            void (*Callback) (char *FileName) );
void		sparkFileBrowserDisplayLoadSequence(char *Path, 
					    char *Extension, 
                                            void (*Callback) (char *FileName) );

int             sparkFileCheckOverwrite(    const char *Name );
int             sparkFileHasExtension(      const char *File, 
                                            const char *Extension );

/*****************************/
/* Channel Editor Functions  */
/*****************************/
void		sparkCeAddFolder(       int Level, 
                                        char *Name );
void		sparkCeAddControl(      int Level, 
                                        int Type, 
                                        int ControlNo );
void            sparkChRemoveKey(       int ControlType,
                                        int ControlNo, 
                                        float Frame );
void            sparkChClear(           int ControlType,
                                        int ControlNo );
void            sparkChCopy(            int SrcControlType,
                                        int SrcControlNo, 
                                        int DstControlType,
                                        int DstControlNo );
void		sparkSetCurveKey(       int Type, 
                                        int ControlNo, 
                                        int FrameNo, 
                                        float Value );
void		sparkSetCurveKeyf(      int Type, 
                                        int ControlNo, 
                                        float FrameNo, 
                                        float Value );
float		sparkGetCurveValue(     int Type, 
                                        int ControlNo,  
                                        int FrameNo );
float		sparkGetCurveValuef(    int Type, 
                                        int ControlNo, 
                                        float FrameNo );
int		sparkSetControlName(    int Type, 
                                        int ControlNo, 
                                        char *NewName );
int             sparkChReadRawKeys(     const char *FileName, 
                                        int ControlType,
                                        int ControlNo );
int             sparkChWriteRawKeys(    const char *FileName, 
                                        int ControlType,
                                        int ControlNo,
                                        float FrameStart,
                                        float FrameEnd,
                                        float FrameStep );

/* Returns SPARK_SUCESS if values have been correctly precomputed, SPARK_FAILURE otherwise */
int             sparkChPrecomputeValues(    int ControlType, 
                                            int ControlNo, 
                                            float FrameStart, 
                                            float FrameEnd );
float           sparkChGetPrecomputedValue( int ControlType, 
                                            int ControlNo, 
                                            float FrameNo );

void            sparkUpdateChannelValues(void);

/**************************/
/* Image Access Functions */
/**************************/
void            sparkGetFrame(      SparkClipSelect Clip,
			            int ClipFrameIndex,
			            ulong *Destination );

/*************************************************/
/* Image Color Space Conversion Functions        */
/*                                               */
/* Note: These functions only support            */
/* SPARKBUF_RGB_24_3x8 and SPARKBUF_RGB_48_3x12. */
/*************************************************/
void            sparkMonochrome(    ulong *Source,
				    ulong *Destination );
void            sparkNegative(      ulong *Source,
			            ulong *Destination );

void            sparkFromYUV(       ulong *Source,
			            ulong *Destination );
void            sparkToYUV(         ulong *Source,
			            ulong *Destination );

void            sparkFromHLS(       ulong *Source,
			            ulong *Destination,
			            int HueShift );
void            sparkToHLS(         ulong *Source,
			            ulong *Destination );

/* conforming to documentation */
#define SparkToHLS   sparkToHLS
#define SparkFromHLS sparkFromHLS

/****************************/
/*  Pixel-Based Conversions */
/****************************/
void            sparkRGBtoYUV( int R, int G, int B,
			       int *Y, int *U, int *V );
void            sparkYUVtoRGB( int Y, int U, int V,
			       int *R, int *G, int *B );
void            sparkRGBtoHLS( int R, int G, int B,
			       int *H, int *L, int *S );
void            sparkHLStoRGB( int H, int L, int S,
			       int *R, int *G, int *B,
			       int HueShift );

/*************************************************/
/* Image Processing Functions                    */
/*                                               */
/* Note: These functions only support            */
/* SPARKBUF_RGB_24_3x8 and SPARKBUF_RGB_48_3x12. */
/*************************************************/
void            sparkBlur(      ulong *Source,
			        ulong *Destination,
			        int x, int y,
			        int Channels );
void            sparkComposite( ulong *Front,
				ulong *Back,
				ulong *Matte,
				ulong *Destination );

/***************************************/
/* Image Buffer Manipulation Functions */
/***************************************/
void            sparkCopyBuffer(    ulong *Source,
				    ulong *Destination );
void            sparkResizeBuffer(  ulong *Source,
				    int SourceSize,
				    ulong *Destination,
				    int DestinationSize );
void            sparkCopyChannel(   ulong *Source,
				    int SourceChannel,
				    int SourceSize,
				    ulong *Destination,
				    int DestinationChannel,
				    int DestinationSize );


/***************************************************************************/
/*------------------------- SPARK AUDIO INTERFACE -------------------------*/
/***************************************************************************/

/****************************************************************************/
/* SparkClipInfoStruct carries the information about the content of a clip. */
/****************************************************************************/
typedef struct spark_clip_info_struct {
   int    NbVideoFrames;  /* number of video frames */
   int    NbAudioTracks; /* number of audio tracks */

   ulong  reserved[6];
} SparkClipInfoStruct;

/*************************************************************************************/
/* SparkTrackInfoStruct carries the information about the content of an audio track. */
/*                                                                                   */
/* Field StartFrame is negative if the track starts before the first video frame.    */
/*************************************************************************************/
typedef struct spark_track_info_struct {
   float  StartFrame;    /* offset in frames from the first video frame */
   int    LengthSamples; /* length in samples */

   ulong  reserved[6];
} SparkTrackInfoStruct;


/************************************************************************/
/*----------- PROTOTYPES FOR OPTIONAL SPARK AUDIO FUNCTIONS ------------*/
/***************** Called by the application if defined *****************/

/* Returns SPARK_SUCCESS if clip is accepted, SPARK_FAILURE otherwise. */
int 	       SparkClipFilter( SparkClipSelect Clip, 
				SparkClipInfoStruct ClipInfo );


/************************************************************************/
/*------------ PROTOTYPES FOR SPARK AUDIO UTILITY FUNCTIONS ------------*/
/************************************************************************/

/* Returns SPARK_SUCCESS on success, SPARK_FAILURE otherwise. */
int 	       sparkGetClipInfo( SparkClipSelect Clip,
				 SparkClipInfoStruct* ClipInfo );					

/* Returns SPARK_SUCCESS on success, SPARK_FAILURE otherwise. */
int            sparkGetAudioTrackInfo( SparkClipSelect Clip, 
				       int TrackNo, 
				       SparkTrackInfoStruct *TrackInfo );

/* Set to FALSE to disable audio data transfer into destination clip. */
void           sparkAudioOutputEnable( int status );
  
/*************************************/
/* Global Audio Parameters Functions */
/*************************************/

/* Returns internal sample format. */
SparkBufferFmt sparkSampleFormat( void );

/* Returns sample width in bytes. */
int            sparkSampleWidth( void );

/* Returns internal block size in bytes. */
int            sparkAudioBlockSize( void );

/* Returns free audio space available in bytes. */
uint64_t       sparkAudioFreeSpace( void );

/* Returns the maximum number of output audio tracks. */
int            sparkAudioMaxOutputTracks( void );

/* Returns current frame rate. */
double         sparkFrameRate( void );

/* Returns current sampling rate. */
double         sparkSamplingRate( void );


/**************************/
/* Audio Access Functions */
/**************************/

/* Returns number of samples read in track. */
int            sparkReadAudio( 	   SparkClipSelect Clip, 
				   int    TrackNo,
				   int    StartIndex, 
				   int    LengthSamples,
				   void  *Buffer );  

/* Returns number of samples allocated in new audio element. */
int            sparkNewAudio(      int    TrackNo, 
				   float  StartFrame, 
				   int    LengthSamples, 
				   int    SetToZero );

/* Returns number of samples written in previously allocated audio element. */
int            sparkWriteAudio(    int    TrackNo, 
				   int    StartIndex, 
				   int    LengthSamples,
				   void  *Buffer ); 

/* Returns SPARK_SUCCESS on success, SPARK_FAILURE otherwise. */
/* WARNING : This function only changes the track's out point without freeing memory. */
int            sparkTruncateAudio( int TrackNo, int LengthSamples ); 


/****************************/
/* Audio Playback Functions */
/****************************/

/* Returns SPARK_SUCCESS on success, SPARK_FAILURE otherwise. */
/* Enables the audio playback mode - should be called in SparkInitialise() */
int            sparkEnablePlayAudio( void );

/* Returns SPARK_SUCCESS on success, SPARK_FAILURE otherwise. */
/* Prepares the audio player.                                 */
int            sparkPrePlayAudio( long    nbSamplesTotalPerTrack,
                                  int     nbTracks );

/* Returns SPARK_SUCCESS on success, SPARK_FAILURE otherwise. */
/* Plays audio */
int            sparkPlayLoopAudio( int    startSample,
                                   int    nbSamplesPerTrack,
                                   void **bufferArray );

/* Returns SPARK_SUCCESS on success, SPARK_FAILURE otherwise. */
/* Stops the audio player */
int            sparkPostPlayAudio( int    abort);

/* Returns 0 if user touched a widget that can be modified during playback.         */
/* Returns 1 if user clicked anywhere else. This should be considered as an abort . */
int            sparkPlayMenuCheck( void );


/* Return the name of the last setup the spark loaded or saved */
const char* sparkGetLastSetupName( void );

/************************************************************************/
/*                                OBSOLETE                              */
/* The following definitions/functions are considered obsolete for the  */
/* current SPARK_VERSION_NUMBER.                                        */
/************************************************************************/
#define SPARK_IMAGE_IMPORT  (1<<2)
#define SPARK_IMAGE_EXPORT  (1<<3)
#define SPARK_MEDIA	    (1<<4)
#define SPARK_FILTER	    (1<<5)

#define SPARK_RGB_32            SPARK_RGB_4C
#define SPARK_RED_32            SPARK_RED_4C
#define SPARK_GREEN_32          SPARK_GREEN_4C
#define SPARK_BLUE_32           SPARK_BLUE_4C

enum {
    SPARK_FLINT = 1,
    SPARK_FLINT_OGL, 
    SPARK_FLAME, 
    SPARK_FLAME_OGL, 
    SPARK_INFERNO, 
    SPARK_INFERNO_OGL, 
    SPARK_FIRE, 
    SPARK_FIRE_OGL
};

#ifdef SPARK_IRISGL
    typedef  BGR8_GL   RGB24_Struct;
    typedef  BGR12_GL  RGBA64_Struct;
    typedef  ABGR8_GL  RGBA32_Struct;

    /*-- IrisGL buffers --*/
    typedef struct {
        uint8_t   b;
        uint8_t   g;
        uint8_t   r;
    } BGR8_GL;
    
    typedef struct {
        uint32_t    a:12;
        uint32_t    pad0:4;
        uint32_t    b:12;
        uint32_t    pad1:4;
        uint32_t    g:12;
        uint32_t    pad2:4;
        uint32_t    r:12;
        uint32_t    pad3:4;
    } BGR12_GL;
 
    /* Plus many other possible types used internally */
    /* by your Sparks: eg. 32 bit RGBA-GL pixels...  */
    typedef struct {
        uint8_t   a;
        uint8_t   b;
        uint8_t   g;
        uint8_t   r;
    } ABGR8_GL;
#endif

#define RGB   RGB24_Struct
#define RGBA  RGBA32_Struct

enum {
   SPARK_CURRENT_IMAGE_IMPORT = 2,
   SPARK_CURRENT_IMAGE_EXPORT,
   SPARK_CURRENT_MEDIA,
   SPARK_CURRENT_FILTER
};

void		sparkFileBrowserDisplay( char *Path, 
                                         char *Extension,  
                                         void (*Callback) (char *FileName) );
void            sparkProcessFinish( void );
int		sparkCallingSys(void);
/************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /*_SPARK_H */
