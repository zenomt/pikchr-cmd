/*
** version.h -- fallback version header for pikchr command-line tool
**
** This header only applies to the CLI wrapper. The version string
** may be overridden at build time with:
**
**   make CFLAGS="-DPIKCHR_CMD_VERSION=\"$(VERSION)\""
**
** where VERSION typically comes from `git describe --tags --dirty`.
** See `Makefile` for details.
**
** If no override is provided, the fallback value below is used.
*/

#ifndef PIKCHR_CMD_VERSION
#define PIKCHR_CMD_VERSION "unversioned"
#endif
