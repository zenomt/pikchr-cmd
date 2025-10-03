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
#include "version.h"

typedef struct {
	char   *buf;
	size_t  capacity;
	size_t  offset;
} buffer_t;

const char *svgAttrs = "style='font-size:initial;'";
const char *summaryText = "Pikchr Source";
const char *summaryAttrs = "";

static bool bufferAppend(buffer_t *buffer, char *str, size_t len)
{
	if(buffer->offset + len >= buffer->capacity)
	{
		while(buffer->offset + len >= buffer->capacity)
			buffer->capacity *= 2;
		buffer->buf = (char *)realloc(buffer->buf, buffer->capacity);
		if(not buffer->buf)
			return false;
	}

	memcpy(buffer->buf + buffer->offset, str, len);
	buffer->offset += len;
	buffer->buf[buffer->offset] = 0;

	return true;
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
		cursor = rv + 1;
	}

	return NULL;
}

static void printIndented(const char *str)
{
	int ch;
	bool need_indent = true;
	while((ch = *str++))
	{
		if(need_indent)
		{
			printf("    ");
			need_indent = false;
		}
		fputc(ch, stdout);
		if('\n' == ch)
			need_indent = true;
	}
}

static int usage(const char *name, int rv, const char *msg)
{
	if(msg)
		printf("%s\n", msg);

	printf("Usage: %s [options]\n", name);
	printf("  -c aClass   -- add class=\"aClass\" to <svg> tags\n");
	printf("  -a attrs    -- add attrs to <svg> tags, default: %s\n", svgAttrs);
	printf("  -s summary  -- summary text for <details>, default: %s\n", summaryText);
	printf("  -S attrs    -- add attrs to <summary> tags, default: %s\n", summaryAttrs);
	printf("  -b          -- bare mode, don't wrap <svg> in <div> to style max-width\n");
	printf("  -p          -- output plaintext error messages instead of HTML\n");
	printf("  -d          -- set dark mode (but consider using -C instead)\n");
	printf("  -C          -- set x-current-color for all diagrams\n");
	printf("  -R          -- requote all diagram source\n");
	printf("  -D          -- put all requoted diagram source in <details>\n");
	printf("  -q          -- don't copy non-diagram input to output\n");
	printf("  -Q          -- remove all diagrams\n");
	printf("  -N mod      -- only translate diagrams that have modifier mod\n");
	printf("  -h          -- print this help\n");
	printf("\n");
	printf("Zero or more modifiers can follow the start delimiter. Unrecognized\n");
	printf("modifiers are ignored (but can be matched with -N). Known modifiers:\n");
	printf("\n");
	printf("  bare-svg    -- bare mode, don't wrap this <svg> in <div> to style max-width\n");
	printf("  requote     -- output the Pikchr source in an indented code block\n");
	printf("  delimiters  -- include the start and end delimiter lines in a requote\n");
	printf("  details     -- if requoting, put source in a <details> element\n");
	printf("  open        -- default <details> to visible/open\n");
	printf("  svg-only    -- synonym for bare-svg (for compatibility with old documents)\n");
	printf("  x-current-color -- Experimental, use \"currentColor\" instead of \"rgb(0,0,0)\"\n");
	printf("                     for black (0), to paint with the inherited foreground color.\n");
	printf("\n");
	printf("Version: %s\n", PIKCHR_CMD_VERSION);
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
	bool requoteAllDiagrams = false;
	bool detailsAllDiagrams = false;
	int rv = 0;

	while((ch = getopt(argc, argv, "c:a:s:S:bpdCRDqQN:h")) != -1)
	{
		switch(ch)
		{
		case 'c':
			svgClass = optarg;
			break;

		case 'a':
			svgAttrs = optarg;
			break;

		case 's':
			summaryText = optarg;
			break;

		case 'S':
			summaryAttrs = optarg;
			break;

		case 'b':
			bareMode = true;
			break;

		case 'p':
			plaintextErrors = PIKCHR_PLAINTEXT_ERRORS;
			break;

		case 'd':
			darkmode |= PIKCHR_DARK_MODE;
			break;

		case 'C':
			darkmode |= PIKCHR_CURRENTCOLOR_FOR_BLACK;
			break;

		case 'R':
			requoteAllDiagrams = true;
			break;

		case 'D':
			detailsAllDiagrams = true;
			break;

		case 'q':
			includeDocument = false;
			break;

		case 'Q':
			includeDiagrams = false;
			break;

		case 'N':
			onlyModifier = optarg;
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

	buffer_t accumulator;
	bufferInit(&accumulator, 8192);

	bool includeThisDiagram = false;
	bool bareModeThisDiagram = false;
	bool requoteThisDiagram = false;
	bool includeDelimitersThisDiagram = false;
	bool detailsThisDiagram = false;
	bool detailsOpenThisDiagram = false;
	int flagsThisDiagram = flags;
	size_t pikchrOffset = 0;

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

				if(includeThisDiagram)
				{
					int width = 0;
					int height = 0;

					char * svg = pikchr(accumulator.buf + pikchrOffset, svgClass, flagsThisDiagram, &width, &height);
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
								printf("<div style=\"max-width:%dpx\">\n", width);

							char *afterFirstElement = strstr(svg, "<svg");
							if(afterFirstElement)
								afterFirstElement = strchr(afterFirstElement, '>');
							if(afterFirstElement)
							{
								*afterFirstElement++ = 0;
								printf("%s %s>%s", svg, svgAttrs, afterFirstElement);
							}
							else
								printf("%s", svg); // will only happen if pikchr() doesn't answer a complete SVG element

							if(not bareModeThisDiagram)
								printf("</div>\n");
							printf("\n");

							if(includeDocument and requoteThisDiagram)
							{
								if(detailsThisDiagram)
									printf("<details markdown=\"1\"%s>\n\n<summary %s>%s</summary>\n\n", detailsOpenThisDiagram ? " open" : "", summaryAttrs, summaryText);

								printIndented(accumulator.buf);
								if(includeDelimitersThisDiagram)
									printf("    %s", line);

								if(detailsThisDiagram)
									printf("\n</details>\n\n");
							}
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
				bareModeThisDiagram = bareMode or strword(line, "bare-svg") or strword(line, "svg-only");
				requoteThisDiagram = requoteAllDiagrams or strword(line, "requote");
				includeDelimitersThisDiagram = strword(line, "delimiters") and requoteThisDiagram;
				detailsThisDiagram = requoteThisDiagram and (detailsAllDiagrams or strword(line, "details"));
				detailsOpenThisDiagram = detailsThisDiagram and strword(line, "open");
				flagsThisDiagram = flags | (strword(line, "x-current-color") ? PIKCHR_CURRENTCOLOR_FOR_BLACK : 0);
				includeThisDiagram = includeDiagrams and (not onlyModifier or strword(line, onlyModifier));

				if(includeDelimitersThisDiagram)
					bufferAppend(&accumulator, line, linelen);
				pikchrOffset = accumulator.offset;
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
