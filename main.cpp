// Copyright © 2023 Michael Thornburgh
// SPDX-License-Identifier: MIT

// A pipeline-friendly command-line driver for the Pikchr C reference
// implementation pikchr.c by Richard Hipp <drh@sqlite.org>.
// Reads from stdin, writes to stdout.

// Pikchr's home page: https://pikchr.org/
// GitHub mirror: https://github.com/drhsqlite/pikchr

#include <regex>
#include <string>

extern "C" {

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "drhsqlite-pikchr/pikchr.h"

}

static int usage(const char *name, int rv, const char *msg = nullptr, const char *arg = nullptr)
{
	if(msg)
		printf("%s", msg);
	if(arg)
		printf("%s", arg);
	if(msg or arg)
		printf("\n");

	printf("usage: %s [options]\n", name);
	printf("  -c aClass   -- add class=\"aClass\" to <svg> tags\n");
	printf("  -b          -- set bare mode, don't wrap in a <div> to style max-width\n");
	printf("  -p          -- output plaintext error messages instead of HTML\n");
	printf("  -d          -- set dark mode\n");
	printf("  -h          -- print this help\n");

	return rv;
}

int main(int argc, char **argv)
{
	int ch;
	const char *svgClass = nullptr;
	bool bareMode = false;
	unsigned int plaintextErrors = 0;
	unsigned int darkmode = 0;
	unsigned int flags = 0;
	int rv = 0;

	while((ch = getopt(argc, argv, "c:bpdh")) != -1)
	{
		switch(ch)
		{
		case 'c':
			svgClass = optarg;
			break;

		case 'b':
			bareMode = true;
			break;

		case 'P':
			plaintextErrors = PIKCHR_PLAINTEXT_ERRORS;
			break;

		case 'd':
			darkmode = PIKCHR_DARK_MODE;
			break;

		case 'h':
		default:
			return usage(argv[0], 'h' != ch);
		}
	}

	if(optind < argc)
		return usage(argv[0], 1, "reads from stdin, writes to stdout");

	flags = flags | plaintextErrors | darkmode;

	auto startPattern = std::regex("^((\\.PS)|(((```)|(~~~))\\s*pikchr(\\s.*)?))\\s*$");
	auto endPattern = std::regex("^((\\.PE)|(```)|(~~~))\\s*$");

	size_t linecapp = 8192;
	char * line = (char *)malloc(linecapp);
	ssize_t linelen;
	bool accumulating = false;
	std::string accumulator;

	while((linelen = getline(&line, &linecapp, stdin)))
	{
		if(ferror(stdin))
		{
			perror("reading from stdin");
			rv = 1;
			break;
		}

		if(accumulating)
		{
			if(feof(stdin) or std::regex_match(line, line + linelen, endPattern))
			{
				accumulating = false;

				int width = 0;
				int height = 0;

				char * svg = pikchr(accumulator.data(), svgClass, flags, &width, &height);

				if(width < 0)
				{
					rv = 1;
					printf("%s\n\n", svg);
				}
				else
				{
					if(not bareMode)
						printf("<div style=\"max-width:%dpx\">", width);
					printf("%s", svg);
					if(not bareMode)
						printf("</div>");
					printf("\n\n");
				}

				free(svg);
				accumulator.clear();
			}
			else
				accumulator.append(line, linelen);
		}
		else
		{
			if(feof(stdin))
				break;

			if(std::regex_match(line, line + linelen, startPattern))
				accumulating = true;
			else if(fwrite(line, linelen, 1, stdout) < 1)
			{
				perror("writing to stdout");
				rv = 1;
				break;
			}
		}

	}

	free(line);

	return rv;
}
