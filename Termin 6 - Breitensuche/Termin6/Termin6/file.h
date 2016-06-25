#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED

bool LoadFromPNG(const char *szFile, char ***data, int *width, int *height);
bool SaveAsPNG(const char *szFile, char **data, int width, int height);

#endif
