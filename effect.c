

#define MAXRNDSET 200

int rndset = 200;
int tablen = 8;
char *buf = NULL, *enc = NULL;
int *per = NULL;
int rows, cols, nchr;

int randper()
{
	int i, j, o;

	o = rand() % nchr;
	
	for (i = 0; i < nchr;) {
	
		switch (rand() % 9) {
			case 0: {
				o = (o + 1) % nchr;
				break;
			}
			case 1: {
				o = (o + nchr - 1) % nchr;
				break;
			}
			case 2: {
				o = (o + cols) % nchr;
				break;
			}
			case 3: {
				o = (o + nchr - cols) % nchr;
				break;
			}
			case 4: {
				o = (o + cols + 1) % nchr;
				break;
			}
			case 5: {
				o = (o + cols - 1) % nchr;
				break;
			}
			case 6: {
				o = (o + nchr - cols + 1) % nchr;
				break;
			}
			case 7: {
				o = (o + nchr - cols - 1) % nchr;
				break;
			}
			case 8: {
				o = rand() % nchr;
				break;
			}
		}
		for (j = 0; j < i; j++) {
			if (per[j] == o)
				break;
		}
		if (j >= i) {
			per[i++] = o;
		}
	}
	
	return 0;
}

int do_art()
{
	int ch, i, x, y, o, n, b, done;
	int set[MAXRNDSET];	
	FILE *f = fopen("asset/art.txt", "r");
	int status = 0;
	char lin[256];

    initscr();

	noecho();
	nodelay(stdscr, TRUE);

	// check that it is sane
	rows = LINES - 1;
	cols = COLS;

	nchr = rows * cols;

	// read the file
	if ((buf = malloc(nchr)) == NULL ||
		(enc = malloc(nchr)) == NULL ||
		(per = calloc(nchr, sizeof(int))) == NULL) {
		exit(-1);
	}
	memset(buf, ' ', nchr);

	for (y = 0; y < rows; y++) {
		if (fgets(lin, sizeof(lin), f) == NULL)
			break;
		x = 0;
		for (i = 0; i < sizeof(lin) && x < cols; i++) {
			ch = lin[i];
			if (ch == 0 || ch == '\n' || ch == '\r')
				break;
			if (ch == '\t') {
				x += tablen - (x % tablen);
				continue;
			}
			if (ch < 32 || ch > 127)
				ch = '*';
			buf[y * cols + x] = ch;
			x++;
		}
	}

		fclose(f);
	
	// randomize initial state
	for (i = 0; i < 6; i++) {

		for (y = 0; y < rows; y++) {
			for (x = 0; x < cols; x++) {
				ch = (rand() % 94) + 33;
				enc[y * cols + x] = ch;
				mvaddch(y, x, ch);
			}
		}
		refresh();
	
		if (i < 5) {
			refresh();
			usleep(200000);
		}
	}
		
	randper();

	// initial set is random

	n = nchr;
	for (i = 0; i < rndset; i++) {
		set[i] = per[--n];
	}
	
	done = 0;
	b = 0;
	while (done < rndset) {
	
		if (getch() != ERR){
				status = -1;
				break;
		}
		// animate the set
	
		for (i = 0; i < rndset; i++) {
			o = set[i];
			if (o < 0)
				continue;

			x = o % cols;
			y = o / cols;
			
			ch = enc[o];
			if (buf[o] == ' ') {	// reveal space right away
				ch = ' ';
			} else {
				if (ch != buf[o])
					ch = ((ch - 32 + 94) % 95) + 32;
			}
			mvaddch(y, x, ch);
			enc[o] = ch;

			// get a new thing in the set
			if (buf[o] == enc[o]) {
				if (n <= 0) {
					set[i] = -1;
					done++;
				} else {
					set[i] = per[--n];
				}
			}
		}

		usleep(30000);

		refresh();
		
		b++;
		if (b == 20)
			b = 0;
	}
	refresh();	
	
	usleep(40000);

	if (buf != NULL)
		free(buf);
	if (enc != NULL)
		free(enc);
	if (per != NULL)
		free(per);
	nodelay(stdscr, FALSE);
	endwin();
}