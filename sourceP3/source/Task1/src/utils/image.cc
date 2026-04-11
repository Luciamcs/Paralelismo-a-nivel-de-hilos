#include "image.h"
#include "png.h"
#include "jpeglib.h"
#include <filesystem>

Image<unsigned char> read_png(const std::string &filename) {
  FILE *fp = fopen(filename.c_str(), "rb");
  
  
  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if(!png) abort();

  png_infop info = png_create_info_struct(png);
  if(!info) abort();

  if(setjmp(png_jmpbuf(png))) abort();

  png_init_io(png, fp);

  png_read_info(png, info);

  int width      = png_get_image_width(png, info);
  int height     = png_get_image_height(png, info);
  png_byte color_type = png_get_color_type(png, info);
  png_byte bit_depth  = png_get_bit_depth(png, info);
  int channels = 0;

  // Read any color_type into 8bit depth, RGBA format.
  // See http://www.libpng.org/pub/png/libpng-manual.txt

  if(bit_depth == 16)
    png_set_strip_16(png);

  if(color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb(png);

  // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
  if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
    png_set_expand_gray_1_2_4_to_8(png);

  if(png_get_valid(png, info, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(png);

  png_read_update_info(png, info);
  
  if(color_type == PNG_COLOR_TYPE_RGB) channels = 3;
  else if(color_type == PNG_COLOR_TYPE_RGBA) channels = 4;
  else if(color_type == PNG_COLOR_TYPE_GRAY) channels = 1;
  else {
    std::cerr<<"Unsupported color type "<<color_type<<std::endl;
    exit(1);
  }

  Image<unsigned char> image(width, height, channels);
    
  unsigned char **rows = new unsigned char*[image.height];
  for(int i=0;i<image.height;i++)
    rows[i] = new unsigned char[image.width*image.channels];
  png_read_image(png, rows);
  for(int j=0;j<image.height;j++)
    for(int i=0; i<image.width;i++)
        for(int c=0;c<image.channels;c++)
            image.set(j,i,c, rows[j][i*image.channels + c]);
            
  for(int i=0;i<image.height;i++)
    delete [] rows[i];
  delete [] rows;

  fclose(fp);
  return image;
}

void write_png(const std::string &filename, const Image<unsigned char> &image) {
  int y;

  FILE *fp = fopen(filename.c_str(), "wb");
  if(!fp) abort();

  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) abort();

  png_infop info = png_create_info_struct(png);
  if (!info) abort();

  if (setjmp(png_jmpbuf(png))) abort();

  png_init_io(png, fp);
   
  png_byte color_type;
  switch(image.channels){
    case 1:
        color_type = PNG_COLOR_TYPE_GRAY;
        break;
    case 3:
        color_type = PNG_COLOR_TYPE_RGB;
    case 4:
        color_type = PNG_COLOR_TYPE_RGBA;
  }

  // Output is 8bit depth, RGBA format.
  png_set_IHDR(
    png,
    info,
    image.width, image.height,
    8,
    color_type,
    PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_DEFAULT
  );
  png_write_info(png, info);

  // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
  // Use png_set_filler().
  // png_set_filler(png, 0, PNG_FILLER_AFTER);
  
  unsigned char **rows = new unsigned char*[image.height];
  for(int i=0;i<image.height;i++)
    rows[i] = new unsigned char[image.width*image.channels];

  for(int j=0;j<image.height;j++)
    for(int i=0; i<image.width;i++)
        for(int c=0;c<image.channels;c++)
            rows[j][i*image.channels + c] = image.get(j,i,c); 

  png_write_image(png, rows);
  png_write_end(png, NULL);
  
  for(int i=0;i<image.height;i++)
    delete [] rows[i];
  delete [] rows;

  fclose(fp);
}


Image<unsigned char> read_jpeg(const std::string &filename) {
	struct jpeg_decompress_struct info;
	struct jpeg_error_mgr err;

	unsigned long int imgWidth, imgHeight;
	int numComponents;

	unsigned long int dwBufferBytes;

	unsigned char* lpRowBuffer[1];

	FILE* fHandle;

	fHandle = fopen(filename.c_str(), "rb");
	if(fHandle == NULL) {
		#ifdef DEBUG
			fprintf(stderr, "%s:%u: Failed to read file %s\n", __FILE__, __LINE__, lpFilename);
		#endif
		return Image<unsigned char>();
	}

	info.err = jpeg_std_error(&err);
	jpeg_create_decompress(&info);

	jpeg_stdio_src(&info, fHandle);
	jpeg_read_header(&info, TRUE);

	jpeg_start_decompress(&info);
	imgWidth = info.output_width;
	imgHeight = info.output_height;
	numComponents = info.num_components;

	dwBufferBytes = imgWidth * imgHeight * numComponents; /* We only read RGB, not A */
	Image<unsigned char> image(imgWidth, imgHeight, numComponents);

	/* Read scanline by scanline */
	while(info.output_scanline < info.output_height) {
		lpRowBuffer[0] = (unsigned char *)(&image.matrix[numComponents*info.output_width*info.output_scanline]);
		jpeg_read_scanlines(&info, lpRowBuffer, 1);
	}

	jpeg_finish_decompress(&info);
	jpeg_destroy_decompress(&info);
	fclose(fHandle);

	return image;
}

int write_jpeg(const std::string &filename, const Image<unsigned char> &image, int quality=90) {
    assert(image.channels == 3 || image.channels == 1);
    assert(quality>=10 && quality<=100);
	struct jpeg_compress_struct info;
	struct jpeg_error_mgr err;

	unsigned char* lpRowBuffer[1];

	FILE* fHandle;

	fHandle = fopen(filename.c_str(), "wb");
	if(fHandle == NULL) {
		#ifdef DEBUG
			fprintf(stderr, "%s:%u Failed to open output file %s\n", __FILE__, __LINE__, lpFilename);
		#endif
		return 1;
	}

	info.err = jpeg_std_error(&err);
	jpeg_create_compress(&info);

	jpeg_stdio_dest(&info, fHandle);

	info.image_width = image.width;
	info.image_height = image.height;
	info.input_components = image.channels;
	if (image.channels == 3)
	    info.in_color_space = JCS_RGB;
	else info.in_color_space = JCS_GRAYSCALE;

	jpeg_set_defaults(&info);
	jpeg_set_quality(&info, quality, TRUE);

	jpeg_start_compress(&info, TRUE);

	/* Write every scanline ... */
	while(info.next_scanline < info.image_height) {
		lpRowBuffer[0] = &(image.matrix[info.next_scanline * (image.width * image.channels)]);
		jpeg_write_scanlines(&info, lpRowBuffer, 1);
	}

	jpeg_finish_compress(&info);
	fclose(fHandle);

	jpeg_destroy_compress(&info);
	return 0;
}

Image<unsigned char> load_from_file(const std::string &filename){
    std::string extension = std::filesystem::path(filename).extension();
    if (extension == ".png" || extension == ".PNG")
        return read_png(filename);
    else if (extension == ".jpeg" || extension == ".JPEG" || extension == ".jpg" || extension == ".JPG")
        return read_jpeg(filename);
    return Image<unsigned char>();
}

void save_to_file(const std::string &filename, const Image<unsigned char> &image, int quality){
    std::string extension = std::filesystem::path(filename).extension();
    if (extension == ".png" || extension == ".PNG")
        write_png(filename, image);
    else if (extension == ".jpeg" || extension == ".JPEG" || extension == ".jpg" || extension == ".JPG")
        write_jpeg(filename, image, quality);
}
