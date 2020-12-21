void
grid(Monitor *m) {
	unsigned int i, n, cx, cy, cw, ch, aw, ah, cols, rows, bw;
	Client *c;

	for(n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next))
		n++;

	/* grid dimensions */
	for(rows = 0; rows <= n/2; rows++)
		if(rows*rows >= n)
			break;
	cols = (rows && (rows - 1) * rows >= n) ? rows - 1 : rows;

    if (n == 1)
        bw = 0;
    else
        bw = borderpx;

	/* window geoms (cell height/width) */
    ch = m->wh / (rows ? rows : 1);
	cw = m->ww / (cols ? cols : 1);
	for(i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next)) {
		cx = m->wx + (i / rows) * cw;
		cy = m->wy + (i % rows) * ch;
		/* adjust height/width of last row/column's windows */
		ah = ((i + 1) % rows == 0) ? m->wh - ch * rows : 0;
		aw = (i >= rows * (cols - 1)) ? m->ww - cw * cols : 0;
		resize(c, cx, cy, cw - 2 * bw + aw, ch - 2 * bw + ah, bw, False);
		i++;
	}
}

void
gridfill(Monitor *m) {
	unsigned int i, n, cx, cy, cw, ch, aw, ah, cols, rows, bw, nm, mch, dn;
	Client *c;

	for(n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);

    if (n == 0)
        return;

    /* grid dimensions */
    for(rows = 0; rows <= n/2 && rows*rows < n; rows++);
    cols = ((rows - 1) * rows >= n) ? rows - 1 : rows;
    nm = n % rows;
    dn = n - nm;

    if (n == 1)
        bw = 0;
    else
        bw = borderpx;

	/* window geoms (cell height/width) */
    ch = m->wh / rows;
	cw = m->ww / cols;
    mch = m->wh / (nm ? nm : 1);

    for(i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next)) {
        unsigned int h = i < dn ? ch : mch;

        cx = m->wx + (i / rows) * cw;
        cy = m->wy + (i % rows) * h;
        /* adjust height/width of last row/column's windows */
        ah = ((i + 1) % rows == 0) ? m->wh - h * rows : 0;
        aw = (i >= rows * (cols - 1)) ? m->ww - cw * cols : 0;
        resize(c, cx, cy, cw - 2 * bw + aw, h - 2 * bw + ah, bw, False);
        i++;
    }
}

void
monoclegridfill(Monitor *m) {
	unsigned int i, n, cx, cy, cw, ch, aw, ah, cols, rows, bw, nm, mch, dn;
	Client *c;

	for(n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);

    n = MIN(n, m->nmaster);

    if (n == 0)
        return;

    /* grid dimensions */
    if (n == 2) {
        rows = 1;
        cols = 2;
    } else {
        for(rows = 0; rows <= n/2 && rows*rows < n; rows++);
        cols = ((rows - 1) * rows >= n) ? rows - 1 : rows;
    }

    nm = n % rows;
    dn = n - nm;

    if (n == 1)
        bw = 0;
    else
        bw = borderpx;

	/* window geoms (cell height/width) */
    ch = m->wh / rows;
	cw = m->ww / cols;
    mch = m->wh / (nm ? nm : 1);

    for(i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++) {
        if (i < n) {
            unsigned int h = i < dn ? ch : mch;

            cx = m->wx + (i / rows) * cw;
            cy = m->wy + (i % rows) * h;
            /* adjust height/width of last row/column's windows */
            ah = ((i + 1) % rows == 0) ? m->wh - h * rows : 0;
            aw = (i >= rows * (cols - 1)) ? m->ww - cw * cols : 0;
            resize(c, cx, cy, cw - 2 * bw + aw, h - 2 * bw + ah, bw, False);
        } else {
            resize(c, -m->ww, -m->wh, m->ww, m->wh, 0, False);
        }
    }
}
    
void
centeredmaster(Monitor *m)
{
	unsigned int i, n, h, mw, mx, my, oty, ety, tw, bw;
	Client *c;

	/* count number of clients in the selected monitor */
	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);
	if (n == 0)
		return;

    if (n == 1)
        bw = 0;
    else
        bw = borderpx;

	/* initialize areas */
    mw = m->ww;
	mx = 0;
	my = 0;
	tw = mw;

	if (n > m->nmaster) {
		/* go mfact box in the center if more than nmaster clients */
		mw = m->nmaster ? m->ww * m->mfact : 0;
		tw = m->ww - mw;

		if (n - m->nmaster > 1) {
			/* only one client */
			mx = (m->ww - mw) / 2;
			tw = (m->ww - mw) / 2;
		}
	}

	oty = 0;
	ety = 0;
	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++)
	if (i < m->nmaster) {
		/* nmaster clients are stacked vertically, in the center
		 * of the screen */
		h = (m->wh - my) / (MIN(n, m->nmaster) - i);
		resize(c, m->wx + mx, m->wy + my, mw - (2*bw),
		       h - (2*bw), bw, 0);
		my += HEIGHT(c);
	} else {
		/* stack clients are stacked vertically */
		if ((i - m->nmaster) % 2 ) {
			h = (m->wh - ety) / ( (1 + n - i) / 2);
			resize(c, m->wx, m->wy + ety, tw - (2*bw),
			       h - (2*bw), bw, 0);
			ety += HEIGHT(c);
		} else {
			h = (m->wh - oty) / ((1 + n - i) / 2);
			resize(c, m->wx + mx + mw, m->wy + oty,
			       tw - (2*bw), h - (2*bw), bw, 0);
			oty += HEIGHT(c);
		}
	}
}

void
col(Monitor *m) {
	unsigned int i, n, w, x, y, mw, bw;
	Client *c;

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);
	if (n == 0)
		return;
    if (n == 1)
        bw = 0;
    else
        bw = borderpx;
    mw = m->ww;
	for (i = x = y = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++) {
        w = (mw - x) / (n - i);
        resize(c, x + m->wx, m->wy, w - (2*bw), m->wh - (2*bw), bw, False);
        x += WIDTH(c);
    }
}

void
horizgrid(Monitor *m) {
	Client *c;
	unsigned int n, i, bw;
	int w = 0;
	int ntop, nbottom = 0;

	/* Count windows */
	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);

	if (n == 0)
		return;
	else if (n == 1) { /* Just fill the whole screen */
        bw = 0;
		c = nexttiled(m->clients);
		resize(c, m->wx, m->wy, m->ww - (2*bw), m->wh - (2*bw), bw, False);
	} else if (n == 2) { /* Split vertically */
        bw = borderpx;
		w = m->ww / 2;
		c = nexttiled(m->clients);
		resize(c, m->wx, m->wy, w - (2*bw), m->wh - (2*bw), bw, False);
		c = nexttiled(c->next);
		resize(c, m->wx + w, m->wy, w - (2*bw), m->wh - (2*bw), bw, False);
	} else {
        bw = borderpx;
		ntop = n / 2;
		nbottom = n - ntop;
		for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++) {
			if (i < ntop)
				resize(c, m->wx + i * m->ww / ntop, m->wy, m->ww / ntop - (2*bw), m->wh / 2 - (2*bw), bw, False);
			else
   			    resize(c, m->wx + (i - ntop) * m->ww / nbottom, m->wy + m->wh / 2, m->ww / nbottom - (2*bw), m->wh / 2 - (2*bw), bw, False);
		}
	}
}

static void
bottomstack(Monitor *m) {
	int w, h, mh, mx, tx, ty, tw;
	unsigned int i, n;
	Client *c;

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);
	if (n == 0)
		return;
	if (n > m->nmaster) {
		mh = m->nmaster ? m->mfact * m->wh : 0;
		tw = m->ww / (n - m->nmaster);
		ty = m->wy + mh;
	} else {
		mh = m->wh;
		tw = m->ww;
		ty = m->wy;
	}
	for (i = mx = 0, tx = m->wx, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++) {
		if (i < m->nmaster) {
			w = (m->ww - mx) / (MIN(n, m->nmaster) - i);
			resize(c, m->wx + mx, m->wy, w - (2 * c->bw), mh - (2 * c->bw), 0, False);
			mx += WIDTH(c);
		} else {
			h = m->wh - mh;
			resize(c, tx, ty, tw - (2 * c->bw), h - (2 * c->bw), 0, False);
			if (tw != m->ww)
				tx += WIDTH(c);
		}
	}
}
