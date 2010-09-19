/*
 * Copyright (C) 2010 Petr Stetiar <ynezz@true.cz>
 *
 * Contains stolen code from ddcprobe project which is:
 * Copyright (C) Nalin Dahyabhai <bigfun@pobox.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "edid.h"

#define EDID_LEN 0x80
struct edid1_info *load_edid(const char *filename)
{
	FILE *fd = NULL;
	size_t read = 0;
	struct edid1_info *edid = NULL;

	if (!filename) {
		return NULL;
	}

	fd = fopen(filename, "rb");
	if (fd == NULL) {
		return NULL;
	}

	edid = malloc(EDID_LEN);
	if (!edid)
		return NULL;

	read = fread(edid, sizeof(unsigned char), EDID_LEN, fd);
	if (read != EDID_LEN) {
		free(edid);
		return NULL;
	}

	return edid;
}

int main(int argc, char **argv)
{
	struct edid1_info *edid_info = NULL;

	if (argc != 2) {
		printf("usage: edid <eeprom_file>\n");
		exit(EXIT_FAILURE);
	}

	edid_info = load_edid(argv[1]);
	if (!edid_info || !check_edid(edid_info)) {
		printf("no edid\n");
		exit(EXIT_FAILURE);
	}

	print_edid(edid_info);
	exit(EXIT_SUCCESS);
}
