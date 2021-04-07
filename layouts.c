#define UNZERO(X) ((X) ? (X) : 1)

void
monocle(Monitor *m)
{
	unsigned int n = 0;
	Client *c;

	for (c = nexttiled(m->clients); c; c = nexttiled(c->next), n++)
		resize(c, m->wx, m->wy, m->ww, m->wh, 0, False);
	if (n) /* override layout symbol */
		snprintf(m->ltsymbol, sizeof m->ltsymbol, "[%d]", n);
}

void
tile(Monitor *m)
{
	unsigned int i, n, h, smh, mw, my, ty, bw, mch, ch;
	float mfacts = 0, sfacts = 0;
	Client *c;

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++) {
		if (n < m->nmaster)
			mfacts += c->cfact;
		else
			sfacts += c->cfact;
	}

	if (!n)
		return;

	bw = n == 1 ? 0 : borderpx;

	mch = m->wh/UNZERO(m->nmaster);
	ch = m->wh/UNZERO(n - m->nmaster);

	if (n > m->nmaster)
		mw = m->nmaster ? m->ww*m->mfact : 0;
	else
		mw = m->ww;
	for (i = my = ty = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++)
		if (i < m->nmaster) {
			h = (m->wh - my)*(c->cfact/mfacts);
			resize(c, m->wx, m->wy + my, mw - (2*bw), h - (2*bw), bw, False);
			my += mch;
			mfacts -= c->cfact;
		} else {
			sfacts -= c->cfact;
			smh = m->wh*m->smfact;
			if(!(nexttiled(c->next)))
				h = (m->wh - ty)/(n - i);
			else
				h = (m->wh - smh - ty)/(n - i);
			if(h < minwsz) {
				c->isfloating = True;
				XRaiseWindow(dpy, c->win);
				resize(c, m->mx + (m->mw/2 - WIDTH(c)/2), m->my + (m->wh/2 - ch/2), m->ww - mw - (2*bw), h - (2*bw), bw, False);
				ty -= ch;
			}
			else
				resize(c, m->wx + mw, m->wy + ty, m->ww - mw - (2*bw), h - (2*bw), bw, False);
			if(!(nexttiled(c->next)))
				ty += ch + smh;
			else
				ty += ch;
		}
}

void
tileright(Monitor *m)
{
	unsigned int i, n, h, smh, mw, my, ty, bw;
	float mfacts = 0, sfacts = 0;
	Client *c;

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++) {
		if (n < m->nmaster)
			mfacts += c->cfact;
		else
			sfacts += c->cfact;
	}
	if (!n)
		return;

	bw = n == 1 ? 0 : borderpx;

	if (n > m->nmaster)
		mw = m->nmaster ? m->ww*(m->mfact) : 0;
	else
		mw = m->ww;
	for (i = my = ty = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++)
		if (i < m->nmaster) {
			h = (m->wh - my)*(c->cfact/mfacts);
			resize(c, m->wx + m->ww - mw, m->wy + my, mw - (2*bw), h - (2*bw), bw, 0);
			my += HEIGHT(c);
			mfacts -= c->cfact;
		} else {
			sfacts -= c->cfact;
			smh = m->wh*m->smfact;
			if(!(nexttiled(c->next)))
				h = (m->wh - ty)/(n - i);
			else
				h = (m->wh - smh - ty)/(n - i);
			if(h < minwsz) {
				c->isfloating = True;
				XRaiseWindow(dpy, c->win);
				resize(c, m->mx + (m->mw/2 - WIDTH(c)/2), m->my + (m->wh/2 - HEIGHT(c)/2), m->ww - mw - (2*bw), h - (2*bw), bw, False);
				ty -= HEIGHT(c);
			}
			else
				resize(c, m->wx, m->wy + ty, m->ww - mw - (2*bw), h - (2*bw), bw, 0);
			if(!(nexttiled(c->next)))
				ty += HEIGHT(c) + smh;
			else
				ty += HEIGHT(c);
		}
}

void
gridfill(Monitor *m)
{
	unsigned int i, n, cx, cy, cw, ch, aw, ah, cols, rows, bw, nm, dn, h;
	Client *c;

	for(n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);

	if (!n)
		return;

	bw = n == 1 ? 0 : borderpx;

	/* grid dimensions */
	for(rows = 0; rows <= n/2 && rows*rows < n; rows++);
	cols = (rows - 1)*rows >= n ? rows - 1 : rows;
	nm = n % rows;
	dn = n - nm;

	/* window geoms (cell height/width) */
	ch = m->wh/rows;
	cw = m->ww/cols;

	for(i = 0, c = nexttiled(m->clients); c; i++, c = nexttiled(c->next)) {
		h = (i < dn || !nm ? ch : m->wh/nm);

		cx = m->wx + (i/rows)*cw;
		cy = m->wy + (i % rows)*h;
		/* adjust height/width of last row/column's windows */
		ah = i == n - 1 && nm
			 ? m->wh - (m->wh/nm)*nm
			 : i + 1 % rows == 0
			 ? m->wh - ch*rows
			 : 0;
		aw = (i >= rows*(cols - 1)) ? m->ww - cw*cols : 0;
		resize(c, cx, cy, cw - 2*bw + aw, h - 2*bw + ah, bw, False);
	}
}

void
gridfit(Monitor *m)
{
	unsigned int n, cols, rows, cn, rn, i, cx, cy, cw, ch, bw;
	Client *c;

	for(n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++) ;
	if(!n)
		return;

	bw = n == 1 ? 0 : borderpx;

	/* grid dimensions */
	for(cols = 0; cols <= n/2; cols++)
		if(cols*cols >= n)
			break;
	if(n == 5) /* set layout against the general calculation: not 1:2:2, but 2:3 */
		cols = 2;
	rows = n/cols;

	/* window geometries */
	cw = cols ? m->ww/cols : m->ww;
	cn = 0; /* current column number */
	rn = 0; /* current row number */
	for(i = 0, c = nexttiled(m->clients); c; i++, c = nexttiled(c->next)) {
		if(i/rows + 1 > cols - n%cols)
			rows = n/cols + 1;
		ch = rows ? m->wh/rows : m->wh;
		cx = m->wx + cn*cw;
		cy = m->wy + rn*ch;
		resize(c, cx, cy, cw - 2*bw, ch - 2*bw, bw, False);
		rn++;
		if(rn >= rows) {
			rn = 0;
			cn++;
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
	if (!n)
		return;

	bw = n == 1 ? 0 : borderpx;

	/* initialize areas */
	mw = m->ww;
	mx = 0;
	my = 0;
	tw = mw;

	if (n > m->nmaster) {
		/* go mfact box in the center if more than nmaster clients */
		mw = m->nmaster ? m->ww*m->mfact : 0;
		tw = m->ww - mw;

		if (n - m->nmaster > 1) {
			/* only one client */
			mx = (m->ww - mw)/2;
			tw = (m->ww - mw)/2;
		}
	}

	oty = 0;
	ety = 0;
	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++)
	if (i < m->nmaster) {
		/* nmaster clients are stacked vertically, in the center
		 * of the screen */
		h = (m->wh - my)/(MIN(n, m->nmaster) - i);
		resize(c, m->wx + mx, m->wy + my, mw - (2*bw),
			   h - (2*bw), bw, 0);
		my += HEIGHT(c);
	} else {
		/* stack clients are stacked vertically */
		if ((i - m->nmaster) % 2 ) {
			h = (m->wh - ety)/( (1 + n - i)/2);
			resize(c, m->wx, m->wy + ety, tw - (2*bw),
				   h - (2*bw), bw, 0);
			ety += HEIGHT(c);
		} else {
			h = (m->wh - oty)/((1 + n - i)/2);
			resize(c, m->wx + mx + mw, m->wy + oty,
				   tw - (2*bw), h - (2*bw), bw, 0);
			oty += HEIGHT(c);
		}
	}
}

void
col(Monitor *m)
{
	unsigned int i, n, w, bw;
	Client *c;

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);
	if (!n)
		return;
	bw = n == 1 ? 0 : borderpx;
	w = m->ww/n;
	for (i = 0, c = nexttiled(m->clients); c; i++, c = nexttiled(c->next))
		resize(c, m->wx + w*i, m->wy, w - 2*bw + (i == n - 1 ? m->ww - w*n : 0), m->wh - 2*bw, bw, False);
}

void
horizgrid(Monitor *m)
{
	Client *c;
	unsigned int n, i, bw;
	int w = 0;
	int ntop, nbottom = 0;

	/* Count windows */
	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);

	if (!n)
		return;
	if (n == 1) { /* Just fill the whole screen */
		bw = 0;
		c = nexttiled(m->clients);
		resize(c, m->wx, m->wy, m->ww - (2*bw), m->wh - (2*bw), bw, False);
	} else if (n == 2) { /* Split vertically */
		bw = borderpx;
		w = m->ww/2;
		c = nexttiled(m->clients);
		resize(c, m->wx, m->wy, w - (2*bw), m->wh - (2*bw), bw, False);
		c = nexttiled(c->next);
		resize(c, m->wx + w, m->wy, w - (2*bw), m->wh - (2*bw), bw, False);
	} else {
		bw = borderpx;
		ntop = n/2;
		nbottom = n - ntop;
		for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++) {
			if (i < ntop)
				resize(c, m->wx + i*m->ww/ntop, m->wy, m->ww/ntop - (2*bw), m->wh/2 - (2*bw), bw, False);
			else
				resize(c, m->wx + (i - ntop)*m->ww/nbottom, m->wy + m->wh/2, m->ww/nbottom - (2*bw), m->wh/2 - (2*bw), bw, False);
		}
	}
}

void
bottomstack(Monitor *m)
{
	int mcw, cw, h;
	unsigned int i, n, bw;
	Client *c;

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);

	if (!n)
		return;
	if (n == 1) {
		bw = 0;
		c = nexttiled(m->clients);
		resize(c, m->wx, m->wy, m->ww - 2*bw, m->wh - 2*bw, bw, False);
		return;
	} else
		bw = borderpx;

	h = m->wh/2;
	mcw = m->ww/UNZERO(m->nmaster);
	cw = m->ww/UNZERO(n - m->nmaster);

	for (i = 0, c = nexttiled(m->clients); c; i++, c = nexttiled(c->next)) {
		if (i < m->nmaster)
			resize(c, m->wx + mcw*i, m->wy, mcw - 2*bw + (i == m->nmaster - 1 ? m->ww - mcw*m->nmaster : 0), h - 2*bw, bw, False);
		else
			resize(c, m->wx + cw*(i - m->nmaster), m->wy + h, cw - 2*bw + (i == n - 1 ? m->ww - cw*(n - m->nmaster) : 0), h - 2*bw, bw, False);
	}
}

void
monoclenogap(Monitor *m)
{
	unsigned int n = 0;
	Client *c;

	for (c = nexttiled(m->clients); c; c = nexttiled(c->next), n++)
		resize(c, m->wx - gappx, m->wy - gappx, m->ww + 2*gappx, m->wh + 2*gappx, 0, False);
	if (n) /* override layout symbol */
		snprintf(m->ltsymbol, sizeof m->ltsymbol, "{%d}", n);
}

void
vsplit(Monitor *m)
{
	int i, x, w, tw, bw;
	unsigned int n;
	Client *c;

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);

	if (n == 0)
		return;

	bw = n == 1 ? 0 : borderpx;

	x = m->wx;
	tw = w = m->ww*m->mfact;
	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++) {
		resize(c, x, m->wy, (i == n - 1 ? w + m->ww - tw : w) - 2*bw, m->wh - 2*bw, bw, False);
		x += w;
		w /= 2;
		tw += w;
	}
}

void
hsplit(Monitor *m)
{
	int i, y, h, th, bw;
	unsigned int n;
	Client *c;

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);

	if (n == 0)
		return;

	bw = n == 1 ? 0 : borderpx;

	y = m->wy;
	th = h = m->wh*m->mfact;
	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++) {
		resize(c, m->wx, y, m->ww - 2*bw, (i == n - 1 ? h + m->wh - th : h) - 2*bw, bw, False);
		y += h;
		h /= 2;
		th += h;
	}
}
