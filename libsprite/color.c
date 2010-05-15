#include "allincludes.h"

#ifdef HACKED_XPMLIB
/* replacement color allocator.  By Robert Forsman. */
 int
SetColor(display, colormap, visual, colorname, color_index,
	 image_pixel, mask_pixel, mask_pixel_index,
	 pixels, npixels, attributes, cols, ncols)
    Display *display;
    Colormap colormap;
    Visual *visual;
    char *colorname;
    unsigned int color_index;
    Pixel *image_pixel, *mask_pixel;
    unsigned int *mask_pixel_index;
    Pixel **pixels;
    unsigned int *npixels;
    XpmAttributes *attributes;
    XColor *cols;
    int ncols;
{
    XColor xcolor;
    int	i,j;
    int	store=0;

    /* XParseColor necessarily can't handle "None" [BDyess] */
    if (strcasecmp(colorname, "None")) {
      if (!XParseColor(display, colormap, colorname, &xcolor)) {
	  fprintf(stderr,"Can't parse color: %s.\n",colorname);
	  return (1);
      }
    } else {
      /* transparent color [BDyess] */
      *image_pixel = 0;
      *mask_pixel = 0;
      *mask_pixel_index = color_index; /* store the color table index */
      return 0;
    }

    for (i=0; i<nallocated_colors; i++) {
	XColor	*xc = &allocated_colors[i];
	if (xc->red==xcolor.red
	    && xc->green==xcolor.green
	    && xc->blue == xcolor.blue) {
	    break;		/* match */
	}
    }

    if (i>=nallocated_colors) {	/* we need a new cell */
	XColor	*xc = &allocated_colors[i];

	if (nallocated_colors >= 1<<nplanes) {
	    fprintf(stderr, "Too many colors in XPMs.  Allocated colors: %d, available planes: %d.\n",nallocated_colors, nplanes);
	    return 1;	/* out of cells! */
	}

	nallocated_colors++;
	if(verbose_image_loading) 
	  printf("nallocated_colors = %d\n",nallocated_colors);
	store=1;
	memcpy(xc, &xcolor, sizeof(xcolor));
    }

    *image_pixel = base;
    for (j=0; i; j++,i>>=1)
	if (i&1)
	    *image_pixel |= planes[j];
    if (store) {
	xcolor.pixel = *image_pixel;
	XStoreColor(display, colormap, &xcolor);
    }
    *mask_pixel = 1;
    (*pixels)[*npixels] = xcolor.pixel;
    (*npixels)++;

    return 0;
}
#endif /*HACKED_XPMLIB*/

void W_SetRGB16(color, r, g, b)
     W_Color color;
     int r,g,b;
{
    XColor xcolor;
    
    /* XStoreColor crashes TrueColor and Mono displays! */
    if(DisplayPlanes(W_Display, W_Screen) != 8)
	return;

    xcolor.pixel = colortable[color].pixelValue;
    xcolor.red = (unsigned short)r;
    xcolor.green = (unsigned short)g;
    xcolor.blue = (unsigned short)b;
    xcolor.flags = 0xff;

    XStoreColor(W_Display, W_Colormap, &xcolor);
}

