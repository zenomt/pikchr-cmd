// Copyright © 2023 Michael Thornburgh
// SPDX-License-Identifier: MIT

// A pipeline-friendly command-line driver for the Pikchr C reference
// implementation pikchr.c by Richard Hipp <drh@sqlite.org>.
// Reads from stdin, writes to stdout.

// Pikchr's home page: https://pikchr.org/
// GitHub mirror: https://github.com/drhsqlite/pikchr

#include <ctype.h>
#include <iso646.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pikchr.h"

typedef struct {
	char   *buf;
	size_t  capacity;
	size_t  offset;
} buffer_t;

static int bufferAppend(buffer_t *buffer, char *str, size_t len)
{
	if(buffer->offset + len >= buffer->capacity)
	{
		while(buffer->offset + len >= buffer->capacity)
			buffer->capacity *= 2;
		buffer->buf = (char *)realloc(buffer->buf, buffer->capacity);
		if(not buffer->buf)
			return 1;
	}

	memcpy(buffer->buf + buffer->offset, str, len);
	buffer->offset += len;
	buffer->buf[buffer->offset] = 0;

	return 0;
}

static void bufferErase(buffer_t *buffer)
{
	buffer->offset = 0;
	buffer->buf[0] = 0;
}

static void bufferInit(buffer_t *buffer, size_t initialCapacity)
{
	buffer->capacity = initialCapacity;
	if(0 == buffer->capacity)
		buffer->capacity = 1;
	buffer->buf = (char *)malloc(buffer->capacity);
	bufferErase(buffer);
}

static void bufferFree(buffer_t *buffer)
{
	free(buffer->buf);
	buffer->buf = 0;
}

static const char * strword(const char *haystack, const char *needle)
{
	const char *rv;
	const char *cursor = haystack;
	size_t len = strlen(needle);

	while((rv = strstr(cursor, needle)))
	{
		if((rv == haystack) or isspace(rv[-1]))
		{
			if((0 == rv[len]) or isspace(rv[len]))
				return rv;
		}
		cursor += len ? len : 1;
	}

	return NULL;
}

static int usage(const char *name, int rv, const char *msg)
{
	if(msg)
		printf("%s\n", msg);

	printf("Usage: %s [options]\n", name);
	printf("  -c aClass   -- add class=\"aClass\" to <svg> tags\n");
	printf("  -b          -- bare mode, don't wrap <svg> in <div> to style max-width\n");
	printf("  -p          -- output plaintext error messages instead of HTML\n");
	printf("  -d          -- set dark mode\n");
	printf("  -q          -- don't copy non-diagram input to output\n");
	printf("  -Q          -- remove all diagrams\n");
	printf("  -n #        -- only translate diagram number # (starting from 1)\n");
	printf("  -N mod      -- only translate the first diagram that has modifier mod\n");
	printf("  -h          -- print this help\n");
	printf("\n");
	printf("Zero or more modifiers can follow the start delimiter. Unrecognized\n");
	printf("modifiers are ignored (but can be matched with -N). Known modifiers:\n");
	printf("\n");
	printf("  svg-only    -- bare mode, don't wrap this <svg> in <div> to style max-width\n");
	printf("\n");

	return rv;
}

int main(int argc, char **argv)
{
	int ch;
	const char *svgClass = NULL;
	const char *onlyModifier = NULL;
	bool bareMode = false;
	bool includeDocument = true;
	bool includeDiagrams = true;
	unsigned int plaintextErrors = 0;
	unsigned int darkmode = 0;
	unsigned int flags = 0;
	size_t onlyDiagramNumber = 0;
	int rv = 0;

	while((ch = getopt(argc, argv, "c:bpdqQn:N:h")) != -1)
	{
		switch(ch)
		{
		case 'c':
			svgClass = optarg;
			break;

		case 'b':
			bareMode = true;
			break;

		case 'p':
			plaintextErrors = PIKCHR_PLAINTEXT_ERRORS;
			break;

		case 'd':
			darkmode = PIKCHR_DARK_MODE;
			break;

		case 'q':
			includeDocument = false;
			break;

		case 'Q':
			includeDiagrams = false;
			break;

		case 'n':
			onlyDiagramNumber = atol(optarg);
			onlyModifier = NULL;
			break;

		case 'N':
			onlyModifier = optarg;
			onlyDiagramNumber = -1;
			break;

		case 'h':
		default:
			return usage(argv[0], 'h' != ch, NULL);
		}
	}

	if(optind < argc)
		return usage(argv[0], 1, "reads from stdin, writes to stdout");

	flags = flags | plaintextErrors | darkmode;

	regex_t startPattern;
	regex_t endPattern;

	if( regcomp(&startPattern, "^((\\.PS)|(((```+)|(~~~+))[[:space:]]*pikchr))([[:space:]].*)?$", REG_EXTENDED | REG_NOSUB)
	 or regcomp(&endPattern, "^((\\.PE)|(```+)|(~~~+))[[:space:]]*$", REG_EXTENDED | REG_NOSUB)
	)
		abort();

	size_t linecapp = 8192;
	char *line = (char *)malloc(linecapp);
	bool accumulating = false;
	size_t diagramNumber = 0;

	buffer_t accumulator;
	bufferInit(&accumulator, 8192);

	bool bareModeThisDiagram = bareMode;

	while(true)
	{
		ssize_t linelen = getline(&line, &linecapp, stdin);

		if(ferror(stdin))
		{
			perror("reading from stdin");
			rv = 1;
			break;
		}

		if(accumulating)
		{
			if((linelen < 0) or (0 == regexec(&endPattern, line, 0, NULL, 0)))
			{
				accumulating = false;

				if(includeDiagrams and ((diagramNumber == onlyDiagramNumber) or not onlyDiagramNumber))
				{
					int width = 0;
					int height = 0;

					char * svg = pikchr(accumulator.buf, svgClass, flags, &width, &height);
					if(svg)
					{
						if(width < 0)
						{
							rv = 1;
							printf("%s\n\n", svg);
						}
						else
						{
							if(not bareModeThisDiagram)
								printf("<div style=\"max-width:%dpx\">", width);
							printf("%s", svg);
							if(not bareModeThisDiagram)
								printf("</div>");
							printf("\n\n");
						}

						free(svg);
					}
				}
				bufferErase(&accumulator);
			}
			else
				bufferAppend(&accumulator, line, linelen);
		}
		else
		{
			if(linelen < 0)
				break;

			if(0 == regexec(&startPattern, line, 0, NULL, 0))
			{
				accumulating = true;
				diagramNumber++;
				bareModeThisDiagram = strword(line, "svg-only") ? true : bareMode;

				if(onlyModifier and strword(line, onlyModifier))
				{
					diagramNumber = onlyDiagramNumber;
					onlyModifier = NULL; // only the first match
				}
			}
			else if(includeDocument and (fwrite(line, linelen, 1, stdout) < 1))
			{
				perror("writing to stdout");
				rv = 1;
				break;
			}
		}
	}

	bufferFree(&accumulator);
	free(line);

	return rv;
}
