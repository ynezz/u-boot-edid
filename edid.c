/*
 * (C) Copyright 2010
 * Petr Stetiar <ynezz@true.cz>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * Contains stolen code from ddcprobe project which is:
 * Copyright (C) Nalin Dahyabhai <bigfun@pobox.com>
 *
 */

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "edid.h"

void get_edid_ranges(struct edid1_info *edid, unsigned char *hmin,
		     unsigned char *hmax, unsigned char *vmin,
		     unsigned char *vmax)
{
	int i;
	struct edid_monitor_descriptor *monitor;
	*hmin = *hmax = *vmin = *vmax = 0;

	if (edid == NULL)
		return;

	for (i = 0; i < 4; i++) {
		monitor = &edid->monitor_details.monitor_descriptor[i];
		if (monitor->type == edid_monitor_descriptor_range) {
			*hmin = monitor->data.range_data.horizontal_min;
			*hmax = monitor->data.range_data.horizontal_max;
			*vmin = monitor->data.range_data.vertical_min;
			*vmax = monitor->data.range_data.vertical_max;
		}
	}
}

int check_edid(struct edid1_info *edid_info)
{
	if ((edid_info == NULL) || (edid_info->version == 0))
		return 0;

	if ((edid_info->version == 0xff && edid_info->revision == 0xff)
	    || (edid_info->version == 0 && edid_info->revision == 0))
		return 0;

	return 1;
}

static char *snip(char *string)
{
	int i;
	
	/* This is always a 13 character buffer */
	/* and it's not always terminated. */
	string[12] = '\0';
	
	while(((i = strlen(string)) > 0) &&
	       (isspace(string[i - 1]) ||
	        (string[i - 1] == '\n') ||
	        (string[i - 1] == '\r'))) {
		string[i - 1] = '\0';
	}
	return string;
}

void print_edid(struct edid1_info *edid_info)
{
	int i;
	char manufacturer[4];
	unsigned char *timings;
	struct edid_monitor_descriptor *monitor;
	unsigned char *timing;
	unsigned int have_timing = 0;

	printf("EDID version: %d.%d\n",
	       edid_info->version, edid_info->revision);

	manufacturer[0] = edid_info->manufacturer_name.char1 + 'A' - 1;
	manufacturer[1] = edid_info->manufacturer_name.char2 + 'A' - 1;
	manufacturer[2] = edid_info->manufacturer_name.char3 + 'A' - 1;
	manufacturer[3] = '\0';
	printf("Product ID code: %04x\n", edid_info->product_code);
	printf("Manufacturer: %s\n", manufacturer);
	
	if (edid_info->serial_number != 0xffffffff) {
		if (strcmp(manufacturer, "MAG") == 0) {
			edid_info->serial_number -= 0x7000000;
		}
		if (strcmp(manufacturer, "OQI") == 0) {
			edid_info->serial_number -= 456150000;
		}
		if (strcmp(manufacturer, "VSC") == 0) {
			edid_info->serial_number -= 640000000;
		}
	}
	printf("Serial number: %08x\n", edid_info->serial_number);
	printf("Manufactured in week: %d year: %d\n",
	       edid_info->week, edid_info->year + 1990);

	printf("Video input definition: %s%s%s%s\n",
	       edid_info->video_input_definition.separate_sync ?
	       "separate sync, " : "",
	       edid_info->video_input_definition.composite_sync ?
	       "composite sync, " : "",
	       edid_info->video_input_definition.sync_on_green ?
	       "sync on green, " : "",
	       edid_info->video_input_definition.digital ?
	       "digital signal" : "analog signal");

	printf("Monitor is %s, gamma: %.02f\n",
		edid_info->feature_support.rgb ? "RGB" : "non-RGB",
		edid_info->gamma / 100.0 + 1);

	printf("Maximum image visible display size: %d cm x %d cm\n",
	       edid_info->max_size_horizontal,
	       edid_info->max_size_vertical);

	printf("Power management features: %s%s, %s%s, %s%s\n",
	       edid_info->feature_support.active_off ? "" : "no ", "active off",
	       edid_info->feature_support.suspend ? "" : "no ", "suspend",
	       edid_info->feature_support.standby ? "" : "no ", "standby");

	printf("Estabilished timings:\n");
	if (edid_info->established_timings.timing_720x400_70)
		printf("\t720x400\t\t70 Hz (VGA 640x400, IBM)\n");
	if (edid_info->established_timings.timing_720x400_88)
		printf("\t720x400\t\t88 Hz (XGA2)\n");
	if (edid_info->established_timings.timing_640x480_60)
		printf("\t640x480\t\t60 Hz (VGA)\n");
	if (edid_info->established_timings.timing_640x480_67)
		printf("\t640x480\t\t67 Hz (Mac II, Apple)\n");
	if (edid_info->established_timings.timing_640x480_72)
		printf("\t640x480\t\t72 Hz (VESA)\n");
	if (edid_info->established_timings.timing_640x480_75)
		printf("\t640x480\t\t75 Hz (VESA)\n");
	if (edid_info->established_timings.timing_800x600_56)
		printf("\t800x600\t\t56 Hz (VESA)\n");
	if (edid_info->established_timings.timing_800x600_60)
		printf("\t800x600\t\t60 Hz (VESA)\n");
	if (edid_info->established_timings.timing_800x600_72)
		printf("\t800x600\t\t72 Hz (VESA)\n");
	if (edid_info->established_timings.timing_800x600_75)
		printf("\t800x600\t\t75 Hz (VESA)\n");
	if (edid_info->established_timings.timing_832x624_75)
		printf("\t832x624\t\t75 Hz (Mac II)\n");
	if (edid_info->established_timings.timing_1024x768_87i)
		printf("\t1024x768\t87 Hz Interlaced (8514A)\n");
	if (edid_info->established_timings.timing_1024x768_60)
		printf("\t1024x768\t60 Hz (VESA)\n");
	if (edid_info->established_timings.timing_1024x768_70)
		printf("\t1024x768\t70 Hz (VESA)\n");
	if (edid_info->established_timings.timing_1024x768_75)
		printf("\t1024x768\t75 Hz (VESA)\n");
	if (edid_info->established_timings.timing_1280x1024_75)
		printf("\t1280x1024\t75 (VESA)\n");

	/* Standard timings. */
	printf("Standard timings:\n");
	for (i = 0; i < 8; i++) {
		double aspect = 1;
		unsigned int x, y;
		unsigned char xres, vfreq;
		xres = edid_info->standard_timing[i].xresolution;
		vfreq = edid_info->standard_timing[i].vfreq;
		if ((xres != vfreq) ||
		   ((xres != 0) && (xres != 1)) ||
		   ((vfreq != 0) && (vfreq != 1))) {
			switch (edid_info->standard_timing[i].aspect) {
				case 0: aspect = 1; break; /*undefined*/
				case 1: aspect = 0.750; break;
				case 2: aspect = 0.800; break;
				case 3: aspect = 0.625; break;
			}
			x = (xres + 31) * 8;
			y = x * aspect;
			printf("\t%dx%d%c\t%d Hz\n", x, y,
			       x > 1000 ? ' ' : '\t', (vfreq & 0x3f) + 60);
			have_timing = 1;
		}
	}

	/* Detailed timing information. */
	/* The original dtiming code didn't actually work at all, so I've
	 * removed it and replaced it with my own dtiming code, which is derived
	 * from the VESA spec and parse-edid.c. How well it works on monitors
	 * with multiple dtimings is unknown, since I don't have one. -daniels */
	timings = (unsigned char *)&edid_info->monitor_details.detailed_timing;
	monitor = NULL;
	for (i = 0; i < 4; i++) {
		timing = &(timings[i*18]);
		if (timing[0] == 0 && timing[1] == 0) {
			monitor = &edid_info->monitor_details.monitor_descriptor[i];
			if (monitor->type == edid_monitor_descriptor_serial)
				printf("Monitor serial number: %s\n", snip(monitor->data.string));
			else if (monitor->type == edid_monitor_descriptor_ascii)
				printf("Monitor ID: %s\n", snip(monitor->data.string));
			else if (monitor->type == edid_monitor_descriptor_name)
				printf("Monitor name: %s\n", snip(monitor->data.string));
			else if (monitor->type == edid_monitor_descriptor_range)
				printf("Monitor range limits, horizontal sync: %d-%d Hz," \
				       " vertical refresh: %d-%d KHz\n",
				       monitor->data.range_data.horizontal_min,
				       monitor->data.range_data.horizontal_max,
				       monitor->data.range_data.vertical_min,
				       monitor->data.range_data.vertical_max);
		}
		else {
			int h_active_high, h_active_low, h_active;
			int h_blanking_high, h_blanking_low, h_blanking;
			int v_active_high, v_active_low, v_active;
			int v_blanking_high, v_blanking_low, v_blanking;
			int pixclock_high, pixclock_low, pixclock;
			int h_total, v_total, vfreq;
			pixclock_low = timing[0];
			pixclock_high = timing[1];
			pixclock = ((pixclock_high << 8) | pixclock_low) * 10000;
			h_blanking_high = ((1|2|4|8) & (timing[4])) >> 4;
			h_blanking_low = timing[3];
			h_blanking = ((h_blanking_high) << 8) | h_blanking_low;
			h_active_high = ((128|64|32|16) & (timing[4])) >> 4;
			h_active_low = timing[2];
			h_active = ((h_active_high) << 8) | h_active_low;
			h_total = h_active + h_blanking;
			v_blanking_high = ((1|2|4|8) & (timing[7])) >> 4;
			v_blanking_low = timing[6];
			v_blanking = ((v_blanking_high) << 8) | v_blanking_low;
			v_active_high = ((128|64|32|16) & (timing[7])) >> 4;
			v_active_low = timing[5];
			v_active = ((v_active_high) << 8) | v_active_low;
			v_total = v_active + v_blanking;
			vfreq = (double)pixclock/((double)v_total*(double)h_total);
			printf("\t%dx%d\%c\t%d Hz (detailed)\n", h_active, v_active,
			       h_active > 1000 ? ' ' : '\t', vfreq);
			have_timing = 1;
		}
	}

	if (!have_timing)
		printf("\tNone\n");
}
