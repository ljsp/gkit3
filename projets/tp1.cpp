
#include "image.h"
#include "image_io.h"



int main( )
{
#if 0
    Image image(1024, 512);
    //~ Image image= read_image("image.bmp");
    
    for(unsigned i= 0; i < image.size(); i++)
        //~ image(i)= Red();
        image(i)= Color(0.5);
        //~ image(i)= Color(0.8, 0.4, 0);
        //~ image(i)= Color(0.8, 0.4, 0, 0.5);
        //~ image(i)= Color(White() - image(i), 1);
        //~ image(i)= Color(0.5, 0.5, 0.5, 0.5);
    
    write_image_png(image, "image.png");
    write_image_bmp(image, "image.bmp");
    write_image_hdr(image, "image.hdr");
#else
    
    Image image= read_image("render64.hdr");
    
    write_image_preview(image, "preview.png");
#endif
    return 0;
}
