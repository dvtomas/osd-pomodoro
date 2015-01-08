/* for getline() */
#define _GNU_SOURCE

#include <xosd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <signal.h>

struct cfg {
	int seconds;
	char *font;
	char *color;
	char *outline_color;
	int outline_width;
	int hoffset;
	int voffset;
	xosd_pos vpos;
	xosd_pos hpos;
	int shadow;
};

static int visibility = 1;

void 
signal_handler(int signum)
{
	switch(signum) {
		case SIGUSR1:
			visibility = 0;
			break;
		case SIGUSR2:
			visibility = 1;
			break;
		case SIGHUP:
			visibility = 1 - visibility;
			break;
	}
}

void 
setup_signal_handlers(void)
{
	struct sigaction sigaction_description;
	sigset_t block_mask;

	sigaction_description.sa_handler = &signal_handler;
	sigemptyset(&block_mask);
	sigaction_description.sa_mask = block_mask;
	sigaction_description.sa_flags = 0;
	if (
			sigaction(SIGUSR1, &sigaction_description, NULL) ||
			sigaction(SIGUSR2, &sigaction_description, NULL) ||
			sigaction(SIGHUP, &sigaction_description, NULL)
	   ) {
		perror("sigaction");
		exit(EXIT_FAILURE);
	}
}

static struct option long_options[] = {
	{"seconds",   1, NULL, 'S'},
	{"font",     1, NULL, 'f'},
	{"color",    1, NULL, 'c'},
	{"shadow",   1, NULL, 's'},
	{"outline-width", 1, NULL, 'O'},
	{"outline-color", 1, NULL, 'C'},

	{"top",      0, NULL, 't'},
	{"vcenter",  0, NULL, 'v'},
	{"bottom",   0, NULL, 'b'},
	{"right",    0, NULL, 'r'},
	{"middle",   0, NULL, 'm'},
	{"left",     0, NULL, 'l'},
	{"offset",   1, NULL, 'o'},
	{"hoffset",  1, NULL, 'H'},

	{"help",     0, NULL, 'h'},
	{NULL,       0, NULL, 0}
};
#define N_LONG_OPTIONS (sizeof(long_options)/sizeof(struct option))

static struct option_description
{
	const char *option; 
	const char *description;
} option_descriptions[] = {
	{"seconds", "Number of seconds for the timer (default: 25*60 = 1500)"},
	{"font", "fully qualified font."},
	{"color", "color. (default: green)"},
	{"outline-width", "outline width. (default: 0)"},
	{"outline-color", "outline color. (default: black)"},

	{"top", "locate at top of screen (default: bottom)"},
	{"vcenter", "locate in vertical center of screen (default: bottom)"},
	{"bottom", "locate at bottom of screen (default)"},
	{"right", "locate at right of screen (default: left)"},
	{"middle", "locate at middle of screen (default: left)"},
	{"left", "locate at left of screen (default)"},
	{"offset", "vertical offset in pixels. (default: 0)"},
	{"hoffset", "horizontal offset in pixels. (default: 0)"},

	{"help", "this help message"},
	{NULL, NULL}
};

void print_usage(const char *progname)
{
	struct option *o;
	struct option_description *od;
	const char *description;

	printf("USAGE: %s [-flag args]\n", progname);
	for (o = long_options; o->name; o++) {
		printf("\t-%c, --%s\t", o->val, o->name);
		description = "DESCRIPTION MISSING!";
		for (od = option_descriptions; od->option; od++)
			if (!strcmp(od->option, o->name)) {
				description = od->description;
				break;
			}
		printf("%s\n", description);
	}
}

void 
parse_options(int argc, char *argv[], struct cfg *cfg)
{
	char shortops[2 * N_LONG_OPTIONS];
	struct option *o;
	int i;
	char c;

	cfg->seconds = 25*60;
	cfg->font = "";
	cfg->color = "green";
	cfg->outline_color = "black";
	cfg->outline_width = 1;
	cfg->hoffset = 0;
	cfg->voffset = 0;
	cfg->shadow = 0;
	cfg->vpos = XOSD_bottom;
	cfg->hpos = XOSD_left;

	/* build short options for getopt_long */
	i = 0;
	for (o = long_options; o->name; o++) {
		shortops[i++] = o->val;
		if (o->has_arg)
			shortops[i++] = ':';
	}
	shortops[i] = '\0';

	while ((c = getopt_long(argc ,argv, shortops, long_options, NULL)) != -1)
	{
		switch(c)
		{
			case 'f': cfg->font = optarg; break;
			case 'c': cfg->color = optarg; break;
			case 'S': cfg->seconds = atoi(optarg); break;
			case 's': cfg->shadow = atoi(optarg); break;
			case 'o': cfg->voffset = atoi(optarg); break;
			case 'H': cfg->hoffset = atoi(optarg); break;
			case 'r': cfg->hpos = XOSD_right; break;
			case 'm': cfg->hpos = XOSD_center; break;
			case 'l': cfg->hpos = XOSD_left; break;
			case 't': cfg->vpos = XOSD_top; break;
			case 'v': cfg->vpos = XOSD_middle; break;
			case 'b': cfg->vpos = XOSD_bottom; break;
			case 'O': cfg->outline_width = atoi(optarg); break;
			case 'C': cfg->outline_color = optarg; break;
			case 'h': print_usage(argv[0]); exit(EXIT_SUCCESS);
		}
	}
};

/* Main *********************************************************************/

int 
main(int argc, char *argv[])
{
	struct cfg cfg;
	xosd *osd;

	parse_options(argc, argv, &cfg);

	if (!(osd = xosd_create (2))) {
		fprintf (stderr, "Error initializing osd\n");
		return EXIT_FAILURE;
	}

	if (cfg.font != NULL && strlen(cfg.font) > 0) 
		xosd_set_font (osd, cfg.font);
	xosd_set_colour (osd, cfg.color);
	xosd_set_shadow_offset (osd, cfg.shadow);
	xosd_set_pos (osd, cfg.vpos);
	xosd_set_align (osd, cfg.hpos);
	xosd_set_vertical_offset(osd, cfg.voffset);
	xosd_set_horizontal_offset(osd, cfg.hoffset);
	xosd_set_outline_offset(osd, cfg.outline_width);
	xosd_set_outline_colour(osd, cfg.outline_color);

	setup_signal_handlers();

	time_t end_time = time(NULL) + cfg.seconds;
	while (1) {
		int seconds_until_end = (int)(end_time - time(NULL));
		if (seconds_until_end <= 0)
			break;

		if (visibility) {
			char output[256];
			sprintf(output, "%02i:%02i", seconds_until_end / 60, seconds_until_end % 60);
			xosd_display(osd, 1, XOSD_string, output);
		}

		usleep(1000000);
	}

	xosd_destroy (osd);
	return EXIT_SUCCESS;
}
