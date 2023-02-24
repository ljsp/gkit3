
#ifndef _IMAGE_IO_H
#define _IMAGE_IO_H

#include "image.h"


Image read_image( const char *filename, const bool flipY= true );

bool write_image_png( const Image& image, const char *filename, const bool flipY= true );
bool write_image_bmp( const Image& image, const char *filename, const bool flipY= true );
bool write_image_hdr( const Image& image, const char *filename, const bool flipY= true );

// raccourci pour write_image_png(tone(image, range(image)), "image.png")
bool write_image_preview( const Image& image, const char *filename, const bool flipY= true, const float gamma= float(2.2));

// utilitaires
// conversion rgb lineaire vers srgb
Image gamma( const Image& image, const float g= float(2.2) );
// conversion srgb vers rgb
Image inverse_gamma( const Image& image, const float g= float(2.2) );

// exposition 
float range( const Image& image );
// correction exposition / gamma, 
Image tone( const Image& image, const float saturation, const float gamma= float(2.2) );


#endif
