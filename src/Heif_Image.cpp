// by ML


#include "Heif_Image.H"
//#include <FL/Fl_Shared_Image.H>
#include <FL/fl_utf8.h>
#include <FL/Fl.H>
//#include <config.h>
#include <stdio.h>
#include <stdlib.h>
//#include <setjmp.h>
#include <iostream> // for cerr

#include <libheif/heif.h>

/*static*/ std::string Heif_Image::iso_8859_1_to_utf8(std::string &str) {
    std::string strOut;
    for (std::string::iterator it = str.begin(); it != str.end(); ++it)
    {
        uint8_t ch = *it;
        if (ch < 0x80) {
            strOut.push_back(ch);
        }
        else {
            strOut.push_back(0xc0 | ch >> 6);
            strOut.push_back(0x80 | (ch & 0x3f));
        }
    }
    return strOut;
}


/**

 */
Heif_Image::Heif_Image(const char* filename)
    : Fl_RGB_Image(0, 0, 0)
{
 
    heif_context* ctx = heif_context_alloc();
    heif_error err = heif_context_read_from_file(ctx, iso_8859_1_to_utf8(std::string(filename)).c_str(), nullptr);
    if (err.code != heif_error_Ok) {
        std::cerr << "failed to open " << filename << " : " << err.message << std::endl;
        return;
    }

    // get a handle to the primary image
    heif_image_handle* handle;
    err = heif_context_get_primary_image_handle(ctx, &handle);
    if (err.code != heif_error_Ok) {
        std::cerr << "failed to open " << filename << " : " << err.message << std::endl;
        return;
    }

    // decode the image and convert colorspace to RGB, saved as 24bit interleaved
    heif_image* img;

    heif_decoding_options* decode_opts = heif_decoding_options_alloc();
    if (decode_opts) {
        decode_opts->convert_hdr_to_8bit = 1;
    }

    err = heif_decode_image(handle, &img, heif_colorspace_RGB, heif_chroma_interleaved_RGB, decode_opts);
    if (err.code != heif_error_Ok) {
        std::cerr << "failed to open " << filename << " : " << err.message << std::endl;
        return;
    }

    heif_decoding_options_free(decode_opts);
    decode_opts = NULL;

    w(heif_image_handle_get_width(handle));
    h(heif_image_handle_get_height(handle));
    d(3);

    int stride;
    const uint8_t* data = heif_image_get_plane_readonly(img, heif_channel_interleaved, &stride);
    array = data;

    // clean up
    heif_context_free(ctx);

#ifdef HAVE_LIBJPEG
  FILE				*fp;	// File pointer
  jpeg_decompress_struct	dinfo;	// Decompressor info
  fl_jpeg_error_mgr		jerr;	// Error handler info
  JSAMPROW			row;	// Sample row pointer
  
  // the following variables are pointers allocating some private space that
  // is not reset by 'setjmp()'
  char* max_finish_decompress_err;      // count errors and give up afer a while
  char* max_destroy_decompress_err;     // to avoid recusion and deadlock
  
  // Clear data...
  alloc_array = 0;
  array = (uchar *)0;
  
  // Open the image file...
  if ((fp = fl_fopen(filename, "rb")) == NULL) return;
  
  // Setup the decompressor info and read the header...
  dinfo.err                = jpeg_std_error((jpeg_error_mgr *)&jerr);
  jerr.pub_.error_exit     = fl_jpeg_error_handler;
  jerr.pub_.output_message = fl_jpeg_output_handler;
  
  // Setup error loop variables
  max_finish_decompress_err = (char*)malloc(1);   // allocate space on the frame for error counters
  max_destroy_decompress_err = (char*)malloc(1);  // otherwise, the variables are reset on the longjmp
  *max_finish_decompress_err=10;
  *max_destroy_decompress_err=10;
  
  if (setjmp(jerr.errhand_))
  {
    // JPEG error handling...
    Fl::warning("JPEG file \"%s\" is too large or contains errors!\n", filename);
    // if any of the cleanup routines hits another error, we would end up 
    // in a loop. So instead, we decrement max_err for some upper cleanup limit.
    if ( ((*max_finish_decompress_err)-- > 0) && array)
      jpeg_finish_decompress(&dinfo);
    if ( (*max_destroy_decompress_err)-- > 0)
      jpeg_destroy_decompress(&dinfo);
    
    fclose(fp);
    
    w(0);
    h(0);
    d(0);
    
    if (array) {
      delete[] (uchar *)array;
      array = 0;
      alloc_array = 0;
    }
    
    free(max_destroy_decompress_err);
    free(max_finish_decompress_err);
    
    return;
  }
  
  jpeg_create_decompress(&dinfo);
  jpeg_stdio_src(&dinfo, fp);
  jpeg_read_header(&dinfo, 1);
  
  dinfo.quantize_colors      = false;
  dinfo.out_color_space      = JCS_RGB;
  dinfo.out_color_components = 3;
  dinfo.output_components    = 3;
 
  dinfo.scale_num = 1;
  dinfo.scale_denom = 1;
 
  // only decode to smaller size: lower quality but faster
  if (minDim > 0) {
    int maxDim = dinfo.image_width;
    if (dinfo.image_height > maxDim) maxDim = dinfo.image_height;
    while ((dinfo.scale_denom <= 8) && (maxDim > 2*minDim)) {
      dinfo.scale_denom *= 2;
      maxDim /= 2;
    }
  }
 
  jpeg_calc_output_dimensions(&dinfo);
 
  w(dinfo.output_width); 
  h(dinfo.output_height);
  d(dinfo.output_components);
  
  if (((size_t)w()) * h() * d() > max_size() ) longjmp(jerr.errhand_, 1);
  array = new uchar[w() * h() * d()];
  alloc_array = 1;
  
  jpeg_start_decompress(&dinfo);
  
  while (dinfo.output_scanline < dinfo.output_height) {
    row = (JSAMPROW)(array +
                     dinfo.output_scanline * dinfo.output_width *
                     dinfo.output_components);
    jpeg_read_scanlines(&dinfo, &row, (JDIMENSION)1);
  }
  
  jpeg_finish_decompress(&dinfo);
  jpeg_destroy_decompress(&dinfo);
  
  free(max_destroy_decompress_err);
  free(max_finish_decompress_err);
  
  fclose(fp);
#endif // HAVE_LIBJPEG

}



/**

 */
Heif_Image::Heif_Image(const char* name, const unsigned char* data)
    : Fl_RGB_Image(0, 0, 0)
{
/*
#ifdef HAVE_LIBJPEG
  jpeg_decompress_struct	dinfo;	// Decompressor info
  fl_jpeg_error_mgr		jerr;	// Error handler info
  JSAMPROW			row;	// Sample row pointer
  
  // the following variables are pointers allocating some private space that
  // is not reset by 'setjmp()'
  char* max_finish_decompress_err;      // count errors and give up afer a while
  char* max_destroy_decompress_err;     // to avoid recusion and deadlock
  
   // Clear data...
   alloc_array = 0;
   array = (uchar *)0;
  
   // Setup the decompressor info and read the header...
   dinfo.err                = jpeg_std_error((jpeg_error_mgr *)&jerr);
   jerr.pub_.error_exit     = fl_jpeg_error_handler;
   jerr.pub_.output_message = fl_jpeg_output_handler;

   // Setup error loop variables
   max_finish_decompress_err = (char*)malloc(1);   // allocate space on the frame for error counters
   max_destroy_decompress_err = (char*)malloc(1);  // otherwise, the variables are reset on the longjmp
   *max_finish_decompress_err=10;
   *max_destroy_decompress_err=10;

   if (setjmp(jerr.errhand_))
   {
     // JPEG error handling...
    Fl::warning("JPEG data is too large or contains errors!\n");
    // if any of the cleanup routines hits another error, we would end up 
     // in a loop. So instead, we decrement max_err for some upper cleanup limit.
     if ( ((*max_finish_decompress_err)-- > 0) && array)
       jpeg_finish_decompress(&dinfo);
     if ( (*max_destroy_decompress_err)-- > 0)
       jpeg_destroy_decompress(&dinfo);

     w(0);
     h(0);
     d(0);

     if (array) {
       delete[] (uchar *)array;
       array = 0;
       alloc_array = 0;
     }

     free(max_destroy_decompress_err);
     free(max_finish_decompress_err);
    
     return;
   }

   jpeg_create_decompress(&dinfo);
  jpeg_mem_src(&dinfo, data);
  jpeg_read_header(&dinfo, 1);
  
   dinfo.quantize_colors      = (boolean)FALSE;
   dinfo.out_color_space      = JCS_RGB;
   dinfo.out_color_components = 3;
   dinfo.output_components    = 3;
 
  dinfo.scale_num = 1;
  dinfo.scale_denom = 1;
 
  // only decode to smaller size: lower quality but faster
  if (minDim > 0) {
    int maxDim = dinfo.image_width;
    if (dinfo.image_height > maxDim) maxDim = dinfo.image_height;
    while ((dinfo.scale_denom <= 8) && (maxDim > 2*minDim)) {
      dinfo.scale_denom *= 2;
      maxDim /= 2;
    }
  }
  
  jpeg_calc_output_dimensions(&dinfo);
  
  w(dinfo.output_width); 
   h(dinfo.output_height);
   d(dinfo.output_components);

   if (((size_t)w()) * h() * d() > max_size() ) longjmp(jerr.errhand_, 1);
   array = new uchar[w() * h() * d()];
   alloc_array = 1;

   jpeg_start_decompress(&dinfo);

   while (dinfo.output_scanline < dinfo.output_height) {
     row = (JSAMPROW)(array +
                      dinfo.output_scanline * dinfo.output_width *
                      dinfo.output_components);
     jpeg_read_scanlines(&dinfo, &row, (JDIMENSION)1);
   }

   jpeg_finish_decompress(&dinfo);
   jpeg_destroy_decompress(&dinfo);

   free(max_destroy_decompress_err);
   free(max_finish_decompress_err);
 

 #endif // HAVE_LIBJPEG
*/
}

