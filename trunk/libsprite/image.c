#include "allincludes.h"

void
W_SetImageDir(dir)
     char *dir;
{
    if(imagedir)
	free(imagedir);

    imagedir = malloc(strlen(dir)+256);
    strcpy(imagedir, dir);
}

void
W_FreeImage(image)
  W_Image *image;
{
  if(image->loaded) {
    XFreePixmap(W_Display, image->pixmap);
    if(image->clipmask && image->xpm) XFreePixmap(W_Display, image->clipmask);
    image->pixmap = image->clipmask = 0;
  }

  image->loaded = 0;
  image->xpm = 0;
  image->frames = 0;
}

W_Image *
W_BitmapToImage(width, height, bits)
  unsigned int width, height;
  char *bits;
{
  W_Image *image = (W_Image*)malloc(sizeof(W_Image));

  image->clipmask = XCreateBitmapFromData(W_Display, W_Root,
                                              bits, width, height);
  image->pixmap = image->clipmask;
  image->frames = 1;
  if(height % width == 0) {  /* probably has multiple frames [BDyess] */
    image->frames = height / width;
    height /= image->frames;
  }
  image->width = width;
  image->height = height;
  image->xpm = 0;
  image->loaded = 1;
  image->bad = 0;
  image->filename = "";
  return image;
}

void check_loaded(image)
     W_Image *image;
{
    if(!(image)->loaded) {
	/* not loaded, autoload [BDyess] */
	if(W_LoadImage(image)) {
	    sprintf(imagedir,"%s/%s",imagedir,(image)->filename);
	    fprintf(stderr,"Something is very wrong.  I can't find any image to load for %s, and all the alternates fail too.  I'm too bummed to continue.\n", imagedir);
	    exit(1);
	}
    }
}
 
/*
   Takes an array of image pointers, NULL terminated, and combines them
   to produce a new image.

   All images are written centered, and the combined image is just large 
   enough to hold the largest passed image.  If one image has more frames
   than the others, the largest frame size is used and the smaller frame
   images are replicated over the entire sequence. [BDyess] 
*/
W_Image *
W_CreateCombinedImage(imagelist,color)
  W_Image **imagelist;
  W_Color color;
{
  int width = 1, height = 1, frames = 1, i, centerx, centery;
  W_Image **list;
  W_Image *image;
  struct window fakewin;
  int tmpOR;

  for(list = imagelist; *list; list++) {
    check_loaded(*list);
    if((*list)->width > width) width = (*list)->width;
    if((*list)->height > height) height = (*list)->height;
    if((*list)->frames > frames) frames = (*list)->frames;
  }
  centerx = width / 2;
  centery = height / 2;
  image = (W_Image*)malloc(sizeof(W_Image));
  image->loaded = 1;
  image->xpm = 1;
  image->bad = 0;
  image->width = width;
  image->height = height;
  image->frames = frames;
  image->filename = "combined image";
  image->pixmap = XCreatePixmap(W_Display, W_Root, width, height * frames,
                                (unsigned)DefaultDepth(W_Display, W_Screen));
  image->clipmask = None;
  /* clear our new image */
  XFillRectangle(W_Display, image->pixmap, colortable[backColor].contexts[0], 
                 0, 0, width, height * frames);
  tmpOR = useOR;	/* don't want to OR when using W_DrawImage [BDyess] */
  useOR = 0;
  for(list = imagelist; *list; list++) {
    for(i=0;i < frames; i++) {
      /* copy the frames, one at a time */
      /* use a fake window for code reuse */
      fakewin.drawable = image->pixmap;
      W_DrawImage(W_Window2Void(&fakewin), centerx - (*list)->width / 2,
                            i*height + centery - (*list)->height / 2,
			    i,
			    *list,
			    color);
    }
  }
  useOR = tmpOR;
/*  printf("created composite image: width %d, height %d, frames %d.\n",
         width,height,frames);*/
  return image;
}

int
W_LoadImage(image)
  W_Image *image;
{
  XpmAttributes attributes;
  int ErrorStatus;
  char *warn = NULL, *error = NULL;
  char buf[BUFSIZ];
  static char *imagedirend=NULL;

  if(!imagedir)
      W_SetImageDir(".");

  if(!image->compiled_in || verbose_image_loading) {
    sprintf(buf,"%s %s...",image->compiled_in ? "Storing" : "Loading",
            image->filename);
/*    warning(buf);*/
    if(verbose_image_loading) {
      puts(buf);
      fflush(stdout);
    }
  }
  if(imagedirend == NULL) imagedirend = imagedir + strlen(imagedir);

  if(image->loaded) return 0;
  if(xpm) {
    attributes.valuemask = XpmCloseness|XpmReturnExtensions|XpmColormap;
#ifdef HACKED_XPMLIB
    if(useOR) {
      if (nallocated_colors>=0) {
	  attributes.valuemask |= XpmColorFunction;
	  attributes.color_allocator = SetColor;
      }
    }
#endif /*HACKED_XPMLIB*/
    attributes.extensions = NULL;
    attributes.colormap = W_Colormap;
    /* take colors that are close [BDyess] */
    attributes.closeness = 40000;

    sprintf(imagedirend,"%s.xpm",image->filename);
    if(image->compiled_in) {
      if(image->xpmdata) {
	ErrorStatus = XpmCreatePixmapFromData(W_Display, W_Root, image->xpmdata,
				&image->pixmap, &image->clipmask, &attributes);
      } else { /* fool the rest of the code into thinking that it is xpm mode
		  but loading the xpm failed */
	error = "";
	ErrorStatus = XpmOpenFailed;
      }
    } else {
      ErrorStatus = XpmReadFileToPixmap(W_Display, W_Root, imagedir, 
				&image->pixmap, &image->clipmask, &attributes);
    }
    switch (ErrorStatus) {
      case XpmColorError:
	  warn = "Could not parse or alloc requested color";
	  break;
      case XpmOpenFailed:
	  error = "Cannot open file";
	  break;
      case XpmFileInvalid:
	  error = "Invalid XPM file";
	  break;
      case XpmNoMemory:
	  error = "Not enough memory";
	  break;
      case XpmColorFailed:
	  error = "Failed to parse or alloc some color";
	  break;
    }
  } else { /* fool the rest of the code into thinking that it is xpm mode
              but loading the xpm failed */
    error = "";
    ErrorStatus = XpmOpenFailed;
  }

  if (error) {
    if(ErrorStatus != XpmOpenFailed) {
      printf("Error reading in %s: %s.\n", imagedir, error);
    }
    /* ok, can't find or load the xpm, try the xbm [BDyess] */
    if(image->compiled_in) {
      if(image->xbmdata) {
	image->clipmask = XCreateBitmapFromData(W_Display, W_Root, 
	                  (char *)image->xbmdata, image->width, image->height);
        ErrorStatus = BitmapSuccess;
      } else {
        ErrorStatus = !BitmapSuccess;
      }
    } else {
      sprintf(imagedirend,"%s.xbm",image->filename);
      ErrorStatus = XReadBitmapFile(W_Display, W_Root, imagedir, 
		 &image->width, &image->height, &image->clipmask, NULL, NULL);
    }
    if(ErrorStatus != BitmapSuccess) {
      printf("Bitmap read failed for %s.\n",imagedir);
      /*abort();*/
      return 1;		/* let the calling function handle it [BDyess]*/
    }
    if(verbose_image_loading) printf("xbm\n");
    /* bitmap loaded successfully */
    image->loaded = 1;
    image->pixmap = image->clipmask;
    if(image->frames == 0) {
    /* if an image doesn't have a number of frames, then guess how many there
       are by assuming that a frame is square.  This is easily overriden by
       setting frames = 1. [BDyess] */
      if((image->height % image->width) == 0) {   /* even multiple */
        image->frames = image->height / image->width;
      } else {
        image->frames = 1;
      }
    }
    image->height /= image->frames;
    image->xpm = 0;
    return 0;
  }
  if(verbose_image_loading) printf("xpm\n");
  if (warn)
      printf("Warning reading in %s: %s.\n", imagedir, warn);
  /* ok, xpm loaded successfully.  Lets store the data and get outta here
     [BDyess] */
  image->loaded = 1;
  image->xpm = 1;
  image->width = attributes.width;
  /* check for an extension that changes the number of frames [BDyess] */
  if(attributes.extensions) {
    int i;

    for (i=0; i<attributes.nextensions; i++) {
	if (0==strcmp("num_views", attributes.extensions[i].name))
	    break;
	if (0==strcmp("nviews", attributes.extensions[i].name))
	    break;
    }
    if (i<attributes.nextensions) {
      image->frames = atoi(attributes.extensions[i].lines[0]);
    }
  }
  if(image->frames == 0) {
  /* if an image doesn't have a number of frames, then guess how many there
     are by assuming that a frame is square.  This is easily overriden by
     setting frames = 1. [BDyess] */
    if((attributes.height % attributes.width) == 0) {   /* even multiple */
      image->frames = attributes.height / attributes.width;
    } else {
      image->frames = 1;
    }
  }
  image->height = attributes.height / image->frames;
  return 0;
} 

/* same as W_DrawImage except that clipmask is not used [BDyess] */
void
W_DrawImageNoClip(window, x, y, frame, image, color)
  W_Window window;
  int x,y,frame;
  W_Image *image;
  W_Color color;
{
  check_loaded(image);
  frame = frame % image->frames;
  if(xpm) {
    if(image->xpm) {
      XSetClipMask(W_Display, colortable[W_Black].contexts[BITMASKGC], 
		   None);
      XCopyArea(W_Display, image->pixmap, W_Void2Window(window)->drawable,
	 colortable[W_Black].contexts[BITMASKGC], 0,
	 (int)(image->height * frame), image->width, image->height, x, y);
    } else {
      XSetClipMask(W_Display, colortable[color].contexts[BITMASKGC], 
		   None);
      XCopyPlane(W_Display, image->pixmap, W_Void2Window(window)->drawable,
	   colortable[color].contexts[BITMASKGC], 0, 
	   (int)(image->height * frame), 
	   image->width, image->height, x, y, 1);
    }
  } else {    
    if(image->xpm)
      XCopyArea(W_Display, image->pixmap, W_Void2Window(window)->drawable,
	   colortable[color].contexts[BITGC], 0,
	   (int)(image->height * frame), image->width, image->height, x, y);
    else
      XCopyPlane(W_Display, image->pixmap, W_Void2Window(window)->drawable,
	   colortable[color].contexts[BITGC], 0, 
	   (int)(image->height * frame), 
	   image->width, image->height, x, y, 1);
  }
}

/* similar to W_DrawImage except the clipmask used for image is the sum of the
   clipmasks for image and clipimage.  Currently this is only used for cloaking.
   [BDyess] */
void
W_OverlayImage(window, x, y, frame, image, overframe, overimage,color)
  W_Window window;
  int x,y,frame;
  W_Image *image;
  int overframe;
  W_Image *overimage;
  W_Color color;
{
  int width, height;
  Pixmap workarea;
  struct window foolwin;
  int tmpOR;

  check_loaded(image);
  check_loaded(overimage);
  width = image->width > overimage->width ? image->width : overimage->width;
  height =image->height > overimage->height ? image->height : overimage->height;
  frame = frame % image->frames;
  overframe = overframe % overimage->frames;

  /* create a temporary offscreen working area [BDyess] */
  workarea = XCreatePixmap(W_Display, W_Root, width, height, 
                           DefaultDepth(W_Display, W_Screen));
  /* fool other routines that want a window into using the work area [BDyess] */
  foolwin.drawable = workarea;
  
  /* copy the first image there [BDyess] */
  W_DrawImageNoClip(W_Window2Void(&foolwin), 
                   (width - image->width) / 2, 
		   (height - image->height) / 2, 
		   frame, image, color);

  /* copy the second image over it [BDyess] */
  /* don't want to use OR... [BDyess] */
  tmpOR = useOR;
  useOR = 0;
  W_DrawImage(W_Window2Void(&foolwin), 
              (width - overimage->width) / 2,
	      (height - overimage->height) / 2, 
	      overframe, overimage, color);
  useOR = tmpOR;

  /* display the work area [BDyess] */
  if(xpm && !useOR) {
      XSetClipOrigin(W_Display, colortable[W_Black].contexts[BITMASKGC], x, 
		     (int)(y - (image->height * frame)));
      XSetClipMask(W_Display, colortable[W_Black].contexts[BITMASKGC], 
		   image->clipmask);
      XCopyArea(W_Display, workarea, W_Void2Window(window)->drawable,
	 colortable[W_Black].contexts[BITMASKGC], 0, 0, width, height, 
	 x - (width - image->width) / 2, 
	 y - (height - image->height) / 2);
  } else {    
      XCopyArea(W_Display, workarea, W_Void2Window(window)->drawable,
	   colortable[color].contexts[BITGC], 0, 0, width, height, x, y);
  }

  /* clean up [BDyess] */
  XFreePixmap(W_Display, workarea);
}
  
void
W_DrawImage(window, x, y, frame, image, color)
  W_Window window;
  int x,y,frame;
  W_Image *image;
  W_Color color;
{
  int height, width;

  check_loaded(image);
  if(frame < 0) {	/* draw the whole thing regardless of frames [BDyess] */
    height = image->height * image->frames;
    frame = 0;
  } else {		/* draw the frame given [BDyess] */
    height = image->height;
    frame = frame % image->frames;
  }
  width = image->width;
  if(xpm && !useOR) {
    if(image->xpm) {
      if (useClipMask) {
        XSetClipOrigin(W_Display, colortable[color].contexts[BITMASKGC], x,
		       (int)(y - (height * frame)));
        XSetClipMask(W_Display, colortable[color].contexts[BITMASKGC],
		     image->clipmask);
      }
      XCopyArea(W_Display, image->pixmap, W_Void2Window(window)->drawable,
	 colortable[color].contexts[BITMASKGC], 0,
	 (int)(height * frame), width, height, x, y);
    } else {
      if (useClipMask) {
        XSetClipOrigin(W_Display, colortable[color].contexts[BITMASKGC], x, 
		       (int)(y - (height * frame)));
        XSetClipMask(W_Display, colortable[color].contexts[BITMASKGC], 
		     image->clipmask);
      }
      XCopyPlane(W_Display, image->pixmap, W_Void2Window(window)->drawable,
	   colortable[color].contexts[BITMASKGC], 0, 
	   (int)(height * frame), 
	   width, height, x, y, 1);
    }
  } else {    
    if(image->xpm) {
      XCopyPlane(W_Display, image->clipmask, W_Void2Window(window)->drawable,
	   maskGC, 0,
	   (int)(height * frame), 
	   width, height, x, y, 1);
      XCopyArea(W_Display, image->pixmap, W_Void2Window(window)->drawable,
	   colortable[color].contexts[BITGC], 0,
	   (int)(height * frame), width, height, x, y);
    } else
      XCopyPlane(W_Display, image->pixmap, W_Void2Window(window)->drawable,
	   colortable[color].contexts[BITGC], 0, 
	   (int)(height * frame), 
	   width, height, x, y, 1);
  }
}

/* same as W_DrawImage except image is drawn using GXor instead of GXcopy 
   [BDyess] */
void
W_DrawImageOr(window, x, y, frame, image, color)
  W_Window window;
  int x,y,frame;
  W_Image *image;
  W_Color color;
{
  check_loaded(image);
  frame = frame % image->frames;
  if(image->xpm) {
      XCopyArea(W_Display, image->pixmap, W_Void2Window(window)->drawable,
	 colortable[W_Black].contexts[BITGC], 0,
	 (int)(image->height * frame), image->width, image->height, x, y);
  } else {    
    XCopyPlane(W_Display, image->pixmap, W_Void2Window(window)->drawable,
	 colortable[color].contexts[BITGC], 0, 
	 (int)(image->height * frame), 
	 image->width, image->height, x, y, 1);
  }
}


void
W_DrawImageBar(win, x, y, len, image)
  W_Window win;
  int x,y,len;
  W_Image *image;
{
   if(len == 0) return;
   XSetClipMask(W_Display,colortable[W_Black].contexts[BITMASKGC],None);
   XCopyArea(W_Display, image->pixmap, W_Void2Window(win)->drawable,
       colortable[W_Black].contexts[BITMASKGC], 0, 0, 
       (unsigned)len+1, image->height, x, y);
}

