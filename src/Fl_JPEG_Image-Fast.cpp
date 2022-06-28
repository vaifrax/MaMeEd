// FL_JPEG_Image copy, modified by ML to use jpeg-turbo

//
// Fl_JPEG_ImageFast routines.
//
// Copyright 1997-2011 by Easy Software Products.
// Image support by Matthias Melcher, Copyright 2000-2009.
//
// Copyright 2013-2021 by Bill Spitzak and others.
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     https://www.fltk.org/COPYING.php
//
// Please see the following page on how to report bugs and issues:
//
//     https://www.fltk.org/bugs.php
//
// Contents:
//
//   Fl_JPEG_ImageFast::Fl_JPEG_ImageFast() - Load a JPEG image file.
//

//
// Include necessary header files...
//

#include "Fl_JPEG_Image-Fast.H"
//#include <FL/Fl_Shared_Image.H>
#include <FL/fl_utf8.h>
#include <FL/Fl.H>
//#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

#ifndef HAVE_LIBJPEG
#define HAVE_LIBJPEG // always use
#endif

// Some releases of the Cygwin JPEG libraries don't have a correctly
// updated header file for the INT32 data type; the following define
// from Shane Hill seems to be a usable workaround...

#if defined(__CYGWIN__)
#  define XMD_H
#endif // __CYGWIN__


extern "C"
{
#ifdef HAVE_LIBJPEG
#  include <jpeglib.h>
#endif // HAVE_LIBJPEG
}


//
// Custom JPEG error handling structure...
//

#ifdef HAVE_LIBJPEG
struct fl_jpeg_error_mgr {
    jpeg_error_mgr        pub_;           // Destination manager...
    jmp_buf               errhand_;       // Error handler
};
#endif // HAVE_LIBJPEG


//
// Error handler for JPEG files...
//

#ifdef HAVE_LIBJPEG
extern "C" {
    static void
        fl_jpeg_error_handler(j_common_ptr dinfo) {   // I - Decompressor info
        longjmp(((fl_jpeg_error_mgr*)(dinfo->err))->errhand_, 1);
    }

    static void
        fl_jpeg_output_handler(j_common_ptr) {        // I - Decompressor info (not used)
    }
}
#endif // HAVE_LIBJPEG


/**
 \brief The constructor loads the JPEG image from the given jpeg filename.

 The inherited destructor frees all memory and server resources that are used
 by the image.

 Use Fl_Image::fail() to check if Fl_JPEG_ImageFast failed to load. fail() returns
 ERR_FILE_ACCESS if the file could not be opened or read, ERR_FORMAT if the
 JPEG format could not be decoded, and ERR_NO_IMAGE if the image could not
 be loaded for another reason. If the image has loaded correctly,
 w(), h(), and d() should return values greater than zero.

 \param[in] filename a full path and name pointing to a valid jpeg file.

 \see Fl_JPEG_ImageFast::Fl_JPEG_ImageFast(const char *imagename, const unsigned char *data)
 */
Fl_JPEG_ImageFast::Fl_JPEG_ImageFast(const char* filename, int minDim /*= -1*/)
    : Fl_RGB_Image(0, 0, 0)
{
    //load_jpg_(filename, 0L, 0L);

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









#ifdef HAVE_LIBJPEG
typedef struct {
    struct jpeg_source_mgr pub;
    const unsigned char* data, * s;
    // JOCTET * buffer;              /* start of buffer */
    // boolean start_of_file;        /* have we gotten any data yet? */
} my_source_mgr;

typedef my_source_mgr* my_src_ptr;


extern "C" {

    static void init_source(j_decompress_ptr cinfo) {
        my_src_ptr src = (my_src_ptr)cinfo->src;
        src->s = src->data;
    }

    static boolean fill_input_buffer(j_decompress_ptr cinfo) {
        my_src_ptr src = (my_src_ptr)cinfo->src;
        size_t nbytes = 4096;
        src->pub.next_input_byte = src->s;
        src->pub.bytes_in_buffer = nbytes;
        src->s += nbytes;
        return TRUE;
    }

    static void term_source(j_decompress_ptr cinfo)
    {
    }

    static void skip_input_data(j_decompress_ptr cinfo, long num_bytes) {
        my_src_ptr src = (my_src_ptr)cinfo->src;
        if (num_bytes > 0) {
            while (num_bytes > (long)src->pub.bytes_in_buffer) {
                num_bytes -= (long)src->pub.bytes_in_buffer;
                fill_input_buffer(cinfo);
            }
            src->pub.next_input_byte += (size_t)num_bytes;
            src->pub.bytes_in_buffer -= (size_t)num_bytes;
        }
    }

} // extern "C"

static void jpeg_mem_src(j_decompress_ptr cinfo, const unsigned char* data)
{
    my_src_ptr src = (my_source_mgr*)malloc(sizeof(my_source_mgr));
    cinfo->src = &(src->pub);
    src->pub.init_source = init_source;
    src->pub.fill_input_buffer = fill_input_buffer;
    src->pub.skip_input_data = skip_input_data;
    src->pub.resync_to_restart = jpeg_resync_to_restart;
    src->pub.term_source = term_source;
    src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
    src->pub.next_input_byte = NULL; /* until buffer loaded */
    src->data = data;
    src->s = data;
}
#endif // HAVE_LIBJPEG











/**
 \brief The constructor loads the JPEG image from memory.

 Construct an image from a block of memory inside the application. Fluid offers
 "binary Data" chunks as a great way to add image data into the C++ source code.
 name_png can be NULL. If a name is given, the image is added to the list of
 shared images (see: Fl_Shared_Image) and will be available by that name.

 The inherited destructor frees all memory and server resources that are used
 by the image.

 Use Fl_Image::fail() to check if Fl_JPEG_ImageFast failed to load. fail() returns
 ERR_FILE_ACCESS if the file could not be opened or read, ERR_FORMAT if the
 JPEG format could not be decoded, and ERR_NO_IMAGE if the image could not
 be loaded for another reason. If the image has loaded correctly,
 w(), h(), and d() should return values greater than zero.

 \param name A unique name or NULL
 \param data A pointer to the memory location of the JPEG image

 \see Fl_JPEG_ImageFast::Fl_JPEG_ImageFast(const char *filename)
 \see Fl_Shared_Image
 */
Fl_JPEG_ImageFast::Fl_JPEG_ImageFast(const char* name, const unsigned char* data, int minDim /*= -1*/)
    : Fl_RGB_Image(0, 0, 0)
{
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
 
/*
  if (w() && h() && name) {
    Fl_Shared_Image *si = new Fl_Shared_Image(name, this);
     si->add();
   }
*/
 #endif // HAVE_LIBJPEG
}

