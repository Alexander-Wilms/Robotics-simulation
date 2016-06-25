#include <windows.h>
#include <cmath>
#include <cassert>
#include <png.h>
#include "file.h"

/* Erzeugt eine Farbpalette mit 256 Einträgen (Index 0 = Weiß, Index 255 = Schwarz) */
static png_colorp create_palette(png_const_structrp png_ptr, int nColors)
{
    static const double white[]  = { 0.9, 0.8, 0.7 };
    static const double red[]    = { 1.0, 0.2, 0.2 };
    static const double yellow[] = { 1.0, 1.0, 0.0 };
    static const double blue[]   = { 0.2, 0.2, 1.0 };
    static const double violet[] = { 0.5, 0.0, 0.5 };
    static const double green[]  = { 0.2, 1.0, 0.2 };
    static const double orange[] = { 1.0, 0.5, 0.0 };
    static const double black[]  = { 0.1, 0.2, 0.3 };

    int nBase = (int)(pow(nColors, 1.0 / 3.0) + 1.0);
    int nCeil = (int)(pow(nBase, 3.0));
    int i, j, m;
    png_colorp palette;
    png_byte rgb[3];
    double *points, distance, max, tmp;

    assert(png_ptr && nColors > 0);

    palette = (png_colorp)png_malloc(png_ptr, nColors * sizeof(png_color));
    points = (double*)png_malloc(png_ptr, 3*nCeil * sizeof(double));

    for (i = 0; i < nCeil; ++i)
    {
        points[3*i]   = (unsigned)((double)i / (nBase *nBase)) / (nBase - 1.0);
        points[3*i+1] = ((unsigned)((double)i / nBase) % nBase) / (nBase - 1.0);
        points[3*i+2] = (unsigned)((double)(i % nBase)) / (nBase - 1.0);
    }

    for (i = 0; i < nCeil - 1; ++i)
    {
        max = 0.0;

        for (j = m = i + 1; j < nCeil; ++j)
        {
            distance = (points[3*j  ] - points[3*i  ]) * (points[3*j  ] - points[3*i  ])
                     + (points[3*j+1] - points[3*i+1]) * (points[3*j+1] - points[3*i+1])
                     + (points[3*j+2] - points[3*i+2]) * (points[3*j+2] - points[3*i+2]);

            if (distance > max)
            {
                m = j;
                max = distance;
            }
        }

        tmp = points[3*m  ]; points[3*m  ] = points[3*(i+1)  ]; points[3*(i+1)  ] = tmp;
        tmp = points[3*m+1]; points[3*m+1] = points[3*(i+1)+1]; points[3*(i+1)+1] = tmp;
        tmp = points[3*m+2]; points[3*m+2] = points[3*(i+1)+2]; points[3*(i+1)+2] = tmp;
    }

    for (i = 0; i < nColors; ++i)
    {
        for (j = 0; j < 3; j++)
        {
            tmp = white[j]  * (1.0 - points[3*i]) * (1.0 - points[3*i+1]) * (1.0 - points[3*i+2]) +
                  red[j]    *        points[3*i]  * (1.0 - points[3*i+1]) * (1.0 - points[3*i+2]) +
                  blue[j]   * (1.0 - points[3*i]) *        points[3*i+1]  * (1.0 - points[3*i+2]) +
                  violet[j] *        points[3*i]  *        points[3*i+1]  * (1.0 - points[3*i+2]) +
                  yellow[j] * (1.0 - points[3*i]) * (1.0 - points[3*i+1]) *        points[3*i+2]  +
                  orange[j] *        points[3*i]  * (1.0 - points[3*i+1]) *        points[3*i+2]  +
                  green[j]  * (1.0 - points[3*i]) *        points[3*i+1]  *        points[3*i+2]  +
                  black[j]  *        points[3*i]  *        points[3*i+1]  *        points[3*i+2];

            rgb[j] = (png_byte)(255.0 * tmp);
        }

        palette[i].red   = rgb[0];
        palette[i].green = rgb[1];
        palette[i].blue  = rgb[2];
    }

    png_free(png_ptr, points);

    palette[0].red = palette[0].green = palette[0].blue = 255;
    palette[nColors-1].red = palette[nColors-1].green = palette[nColors-1].blue = 0;

    return palette;
}

/*
 *  LoadFromPNG
 *  Lädt den Konfigurationsraum aus einer PNG Datei.
 *  Parameter:
 *  szFile  -   Dateiname
 *  data    -   Das 2D Array mit dem Konfigurationsraum
 *  width   -   Breite des Konfigurationsraums
 *  height  -   Höhe des Konfigrationsraums
 */
bool LoadFromPNG(const char *szFile, char ***data, int *width, int *height)
{
    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;
    png_colorp palette;

    fopen_s(&fp, szFile, "rb");
    if (fp == NULL)
        return false;

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL)
    {
        fclose(fp);
        return false;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return false;
    }

    png_init_io(png_ptr, fp);
    png_read_info(png_ptr, info_ptr);
    png_get_IHDR(png_ptr, info_ptr, (png_uint_32p)width, (png_uint_32p)height, NULL, NULL, NULL, NULL, NULL);
    palette = create_palette(png_ptr, 256);
    png_set_quantize(png_ptr, palette, 256, 256, NULL, 1);
    *data = new char*[*height];
    for (int y = 0; y < *height; ++y)
        (*data)[y] = new char[*width];
    png_read_image(png_ptr, (png_bytepp)(*data));
    png_read_end(png_ptr, info_ptr);
    png_free(png_ptr, palette);
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    fclose(fp);

    return true;
}

/*
 *  SaveAsPNG
 *  Speichert den Konfigurationsraum als PNG.
 *  Parameter:
 *  szFile  -   Dateiname
 *  data    -   Das 2D Array mit dem Konfigurationsraum
 *  width   -   Breite des Konfigurationsraums
 *  height  -   Höhe des Konfigrationsraums
 */
bool SaveAsPNG(const char *szFile, char **data, int width, int height)
{
    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;
    png_colorp palette;

    fopen_s(&fp, szFile, "wb");
    if (fp == NULL)
        return false;

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL)
    {
        fclose(fp);
        return false;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        fclose(fp);
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        fclose(fp);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return false;
    }

    png_init_io(png_ptr, fp);
    png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    palette = create_palette(png_ptr, 256);
    png_set_PLTE(png_ptr, info_ptr, palette, 256);
    png_write_info(png_ptr, info_ptr);
    png_write_image(png_ptr, (png_bytepp)data);
    png_write_end(png_ptr, info_ptr);
    png_free(png_ptr, palette);
    png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    fclose(fp);

    return true;
}
