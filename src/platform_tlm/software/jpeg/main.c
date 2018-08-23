#include <ctype.h>      /* to declare isupper(), tolower() */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libdycton.h"
#include "address_map.h"
#include "hal.h"

#include "cdjpeg.h"     /* Common decls for cjpeg/djpeg applications */

#include "jpeg_app.h"   // build time definition of the application parameters
                        // see that file for changing dataset used or coding/decoding function

#define LIBJPEG_TRACE_LVL           (3)


static const char * progname;   /* program name for error messages */
static char * outfilename;  /* for -outfile switch */

/* Create the add-on message string table. */

#define JMESSAGE(code,string)   string ,

static const char * const cdjpeg_message_table[] = {
#include "cderror.h"
  NULL
};


/*
 * This list defines the known output image formats
 * (not all of which need be supported by a given version).
 * You can change the default output format by defining DEFAULT_FMT;
 * indeed, you had better do so if you undefine PPM_SUPPORTED.
 */
// dycton port : only format supported for now is PPM
typedef enum {
    FMT_BMP,        /* BMP format (Windows flavor) */
    FMT_GIF,        /* GIF format */
    FMT_OS2,        /* BMP format (OS/2 flavor) */
    FMT_PPM,        /* PPM/PGM (PBMPLUS formats) */
    FMT_RLE,        /* RLE format */
    FMT_TARGA,      /* Targa format */
    FMT_TIFF        /* TIFF format */
} IMAGE_FORMATS;

#ifndef DEFAULT_FMT     /* so can override from CFLAGS in Makefile */
#define DEFAULT_FMT FMT_PPM
#endif

static IMAGE_FORMATS requested_fmt;



/*
 * Marker processor for COM markers.
 * This replaces the library's built-in processor, which just skips the marker.
 * We want to print out the marker as text, if possible.
 * Note this code relies on a non-suspending data source.
 */
unsigned int jpeg_getc (j_decompress_ptr cinfo)
/* Read next byte */
{
  struct jpeg_source_mgr * datasrc = cinfo->src;

  if (datasrc->bytes_in_buffer == 0) {
    if (! (*datasrc->fill_input_buffer) (cinfo))
      ERREXIT(cinfo, JERR_CANT_SUSPEND);
  }
  datasrc->bytes_in_buffer--;
  return GETJOCTET(*datasrc->next_input_byte++);
}

boolean COM_handler (j_decompress_ptr cinfo)
{
  boolean traceit = (cinfo->err->trace_level >= 1);
  INT32 length;
  unsigned int ch;
  unsigned int lastch = 0;

  length = jpeg_getc(cinfo) << 8;
  length += jpeg_getc(cinfo);
  length -= 2;          /* discount the length word itself */

  if (traceit)
    fprintf(stderr, "Comment, length %ld:\n", (long) length);

  while (--length >= 0) {
    ch = jpeg_getc(cinfo);
    if (traceit) {
      /* Emit the character in a readable form.
       * Nonprintables are converted to \nnn form,
       * while \ is converted to \\.
       * Newlines in CR, CR/LF, or LF form will be printed as one newline.
       */
      if (ch == '\r') {
    fprintf(stderr, "\n");
      } else if (ch == '\n') {
    if (lastch != '\r')
      fprintf(stderr, "\n");
      } else if (ch == '\\') {
    fprintf(stderr, "\\\\");
      } else if (isprint(ch)) {
    putc(ch, stderr);
      } else {
    fprintf(stderr, "\\%03o", ch);
      }
      lastch = ch;
    }
  }

  if (traceit)
    fprintf(stderr, "\n");

  return TRUE;
}




/*
 * The encoding function
 *
 */
int cjpeg(FILE * in, FILE * out)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cjpeg_source_ptr src_mgr;
    FILE * input_file;
    FILE * output_file;
    JDIMENSION num_scanlines;

    progname = "cjpeg";       /* in case C library doesn't provide it */
    /* Initialize the JPEG compression object with default error handling. */
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    /* Add some application-specific error messages (from cderror.h) */
    jerr.addon_message_table = cdjpeg_message_table;
    jerr.first_addon_message = JMSG_FIRSTADDONCODE;
    jerr.last_addon_message = JMSG_LASTADDONCODE;

    printf(">>> compression JPEG routine start\n");

    /* Initialize JPEG parameters.
      * Much of this may be overridden later.
      * In particular, we don't yet know the input file's color space,
      * but we need to provide some value for jpeg_set_defaults() to work.
      */

    cinfo.in_color_space = JCS_RGB; /* arbitrary guess */
    jpeg_set_defaults(&cinfo);

    /* Scan command line to find file names.
     * It is convenient to use just one switch-parsing routine, but the switch
     * values read here are ignored; we will rescan the switches after opening
     * the input file.
     */

    // jpeg parameter configuration (corresponding to parsing command line args in cjpeg)
    int quality = 75;         /* default -quality value */
    boolean force_baseline = FALSE;   /* by default, allow 16-bit quantizers */
    cinfo.err->trace_level = LIBJPEG_TRACE_LVL;

    cinfo.dct_method = JDCT_ISLOW;
    cinfo.optimize_coding = TRUE;

    // input and output in memory file setup
    input_file = in;
    output_file = out;

    /* Figure out the input file format, and set up to read it. */
    // src_mgr = select_file_type(&cinfo, input_file);
    src_mgr = jinit_read_ppm(&cinfo); // setup directly the right format as the mibench dataset is ppm
    src_mgr->input_file = input_file;

    /* Read the input file header to obtain file size & colorspace. */
    printf(">>> reading header\n");
    (*src_mgr->start_input) (&cinfo, src_mgr);

    /* Now that we know input colorspace, fix colorspace-dependent defaults */
    jpeg_default_colorspace(&cinfo);

    /* Set quantization tables for selected quality. */
    /* Some or all may be overridden if -qtables is present. */
    jpeg_set_quality(&cinfo, quality, force_baseline);
    jpeg_simple_progression(&cinfo);

    /* Specify data destination for compression */
    jpeg_stdio_dest(&cinfo, output_file);

    printf(">>> starting compression.\n");

    /* Start compressor */
    jpeg_start_compress(&cinfo, TRUE);

    printf("cinfo : \n");
    printf("cinfo.image_width : %d\n", cinfo.image_width);
    printf("cinfo.image_height : %d\n", cinfo.image_height);
    printf("cinfo.image_dest : 0x%X\n", (unsigned int)cinfo.dest);


    printf(">>> process data.\n");
    /* Process data */
    while (cinfo.next_scanline < cinfo.image_height) {
        num_scanlines = (*src_mgr->get_pixel_rows) (&cinfo, src_mgr);
        (void) jpeg_write_scanlines(&cinfo, src_mgr->buffer, num_scanlines);
    }


    printf(">>> finish phase\n");
    /* Finish compression and release memory */
    (*src_mgr->finish_input) (&cinfo, src_mgr);
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    printf(">>> all done : exiting compression JPEG routine\n");
    /* All done. */
    return (jerr.num_warnings ? EXIT_WARNING : EXIT_SUCCESS);
}


/*
 * The decoding function
 *
 */
int djpeg(FILE * in, FILE * out)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    djpeg_dest_ptr dest_mgr = NULL;
    FILE * input_file;
    FILE * output_file;
    JDIMENSION num_scanlines;

    progname = "djpeg";      /* in case C library doesn't provide it */

    printf(">>> decoding JPEG routine start\n");
    /* Initialize the JPEG decompression object with default error handling. */
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    /* Add some application-specific error messages (from cderror.h) */
    jerr.addon_message_table = cdjpeg_message_table;
    jerr.first_addon_message = JMSG_FIRSTADDONCODE;
    jerr.last_addon_message = JMSG_LASTADDONCODE;
    /* Insert custom COM marker processor. */
    jpeg_set_marker_processor(&cinfo, JPEG_COM, COM_handler);

    /* Scan command line to find file names. */
    /* It is convenient to use just one switch-parsing routine, but the switch
     * values read here are ignored; we will rescan the switches after opening
     * the input file.
     * (Exception: tracing level set here controls verbosity for COM markers
     * found during jpeg_read_header...)
     */


    // jpeg parameter configuration (corresponding to parsing command line args in cjpeg)
    requested_fmt = FMT_PPM;
    outfilename = NULL;
    cinfo.err->trace_level = LIBJPEG_TRACE_LVL;

    cinfo.dct_method = JDCT_ISLOW;

    // input and output in memory file setup
    input_file = in;
    output_file = out;

    /* Specify data source for decompression */
    jpeg_stdio_src(&cinfo, input_file);

    /* Read file header, set default decompression parameters */
    (void) jpeg_read_header(&cinfo, TRUE);

    /* Initialize the output module now to let it override any crucial
     * option settings (for instance, GIF wants to force color quantization).
     */

    dest_mgr = jinit_write_ppm(&cinfo);
    dest_mgr->output_file = output_file;

    /* Start decompressor */
    printf(">>> starting decompression.\n");
    (void) jpeg_start_decompress(&cinfo);

    printf("cinfo : \n");
    printf("cinfo.output_width : %d\n", cinfo.output_width);
    printf("cinfo.output_height : %d\n", cinfo.output_height);


    /* Write output file header */
    (*dest_mgr->start_output) (&cinfo, dest_mgr);

    printf(">>> process data.\n");
    /* Process data */
    int i = 0;
    while (cinfo.output_scanline < cinfo.output_height) {
        i++;
        // printf("%d\n", i);
        num_scanlines = jpeg_read_scanlines(&cinfo, dest_mgr->buffer,
                                            dest_mgr->buffer_height);
        (*dest_mgr->put_pixel_rows) (&cinfo, dest_mgr, num_scanlines);

    }
    printf(">>> finish phase\n");
    /* Finish decompression and release memory.
     * I must do it in this order because output module has allocated memory
     * of lifespan JPOOL_IMAGE; it needs to finish before releasing memory.
     */
    (*dest_mgr->finish_output) (&cinfo, dest_mgr);
    (void) jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    printf(">>> all done : exiting decoding JPEG routine\n");

    /* All done. */
    return (jerr.num_warnings ? EXIT_WARNING : EXIT_SUCCESS);
}


/*
 * The main program.
 *
 */
int main(void)
{
    int ret = 0;
    FILE * out_image_ptr = NULL;
    FILE * in_image_ptr = NULL;

    printf("\n\n\n================== jpeg encoding application for DYCTON (from MIBENCH) ==================\n");
    printf("(TDk @ CITI Lab)\n\n");

    in_image_ptr = fmemopen((int*)JPEG_IN_ADDR, DATASET_SZ_IN, "rb");
    out_image_ptr = fmemopen((int*)JPEG_OUT_ADDR, DATASET_SZ_OUT+OUT_FILE_ADDITIONAL_BITS, "wb+");
    printf("image in memory at address 0x%X.\n", (unsigned int)in_image_ptr);

    #if(JPEG_FUNC == CODE)
        ret = cjpeg(in_image_ptr, out_image_ptr);
    #elif(JPEG_FUNC == DECODE)
        ret = djpeg(in_image_ptr, out_image_ptr);
    #else
        #error you must define the JPEG functionality used (encode or decode in software/jpeg/main.c)
    #endif
    printf("operation complete.\n");

    fclose(in_image_ptr);
    fclose(out_image_ptr);


    printf("\n========================================== THE END ==========================================\n");

    exit(ret);
    return 0;         /* suppress no-return-value warnings */
}

