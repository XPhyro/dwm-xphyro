void
swaptags(const Arg *arg)
{
	unsigned int newtag = arg->ui & TAGMASK;
	unsigned int curtag = selmon->tagset[selmon->seltags];

	if (newtag == curtag || !curtag || (curtag & (curtag-1)))
		return;

	for (Client *c = selmon->clients; c; c = c->next) {
		if (c->scratchkey)
			continue;
		if ((c->tags & newtag) || (c->tags & curtag))
			c->tags ^= curtag ^ newtag;
		if (!c->tags)
			c->tags = newtag;
	}

	selmon->tagset[selmon->seltags] = newtag;

	focus(NULL);
	arrange(selmon);
}

void
movestack(const Arg *arg)
{
	Client *c = NULL, *p = NULL, *pc = NULL, *i;
	unsigned int n;

	for (n = 0, c = nexttiled(selmon->clients); c; c = nexttiled(c->next), n++);

	if (!n)
		return;

	c = NULL;

	if(arg->i > 0) {
		/* find the client after selmon->sel */
		for(c = selmon->sel->next; c && (!ISVISIBLE(c) || c->isfloating); c = c->next);
		if(!c)
			for(c = selmon->clients; c && (!ISVISIBLE(c) || c->isfloating); c = c->next);
	}
	else {
		/* find the client before selmon->sel */
		for(i = selmon->clients; i != selmon->sel; i = i->next)
			if(ISVISIBLE(i) && !i->isfloating)
				c = i;
		if(!c)
			for(; i; i = i->next)
				if(ISVISIBLE(i) && !i->isfloating)
					c = i;
	}
	/* find the client before selmon->sel and c */
	for(i = selmon->clients; i && (!p || !pc); i = i->next) {
		if(i->next == selmon->sel)
			p = i;
		if(i->next == c)
			pc = i;
	}

	/* swap c and selmon->sel selmon->clients in the selmon->clients list */
	if(c && c != selmon->sel) {
		Client *temp = selmon->sel->next==c?selmon->sel:selmon->sel->next;
		selmon->sel->next = c->next==selmon->sel?c:c->next;
		c->next = temp;

		if(p && p != c)
			p->next = c;
		if(pc && pc != selmon->sel)
			pc->next = selmon->sel;

		if(selmon->sel == selmon->clients)
			selmon->clients = c;
		else if(c == selmon->clients)
			selmon->clients = selmon->sel;

		arrange(selmon);
	}
}

void
setattachdir(const Arg *arg)
{
	attachdir = (arg->i != attachdir) * arg->i;
}

void
togglewarp(const Arg *arg)
{
	willwarp = !willwarp;
}

void
togglefakefullscr(const Arg *arg)
{
	isfakefullscreen = !isfakefullscreen;
}

void
togglegap(const Arg *arg)
{
	Monitor *m;

	if (gappx) {
		cgappx = gappx;
		gappx = 0;
	} else
		gappx = cgappx;

	for (m = mons; m; arrange(m), m = m->next);

	oldgappx = gappx;
}

void
incgap(const Arg *arg)
{
	Monitor *m;
	int *gap;

	if (gappx)
		gap = &gappx;
	else
		gap = &cgappx;

	if (!arg->i)
		*gap = initgappx;
	else 
		*gap = MAX(*gap + arg->i, 1);

	for (m = mons; m; arrange(m), m = m->next);

	oldgappx = gappx;
}

void
incborder(const Arg *arg)
{
	Monitor *m;

	if (!arg->i)
		borderpx = initborderpx;
	else if (-arg->i > borderpx)
		borderpx = 0;
	else
		borderpx = MAX(borderpx + arg->i, 1);

	for (m = mons; m; arrange(m), m = m->next);
}

void
toggleborder(const Arg *arg)
{
	Monitor *m;

	if (borderpx) {
		oldborderpx = borderpx;
		borderpx = 0;
	} else
		borderpx = oldborderpx;

	for (m = mons; m; arrange(m), m = m->next);
}
