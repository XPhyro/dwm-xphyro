/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int initborderpx  = 1;        /* initial border pixel of windows */
static const int initgappx              = 10;       /* initial gap pixel of windows */
static const double initalpha           = 0.85;
static const unsigned int snap          = 16;       /* snap pixel */
static const unsigned int minwsz        = 20;       /* minimal height of a client for smfact */
static const int showbar                = 1;        /* 0 means no bar */
static const int topbar                 = 1;        /* 0 means bottom bar */
static const char *fonts[]              = { "monospace:size=10" }; /* fonts should have an even size, otherwise some layouts will leave gaps */
static const char dmenufont[]           = "monospace:size=10";
static const char col_gray1[]           = "#222222";
static const char col_gray2[]           = "#444444";
static const char col_gray3[]           = "#bbbbbb";
static const char col_gray4[]           = "#eeeeee";
static const char col_cyan[]            = "#005577";
static const char *colors[][3]          = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};

#include "layouts.c"

//static const char norm_fg[] = "#a89984";
//static const char norm_bg[] = "#282828";
//static const char norm_border[] = "#928374";
//
//static const char sel_fg[] = "#a89984";
//static const char sel_bg[] = "#d79921";
//static const char sel_border[] = "#a89984";
//
//static const char urg_fg[] = "#a89984";
//static const char urg_bg[] = "#cc241d";
//static const char urg_border[] = "#cc241d";
//
//static const char *colors[][3]      = {
//    /*               fg           bg         border                         */
//    [SchemeNorm] = { norm_fg,     norm_bg,   norm_border }, // unfocused wins
//    [SchemeSel]  = { sel_fg,      sel_bg,    sel_border },  // the focused win
//    [SchemeUrg] =  { urg_fg,      urg_bg,    urg_border },
//};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class              instance      title         tags mask iscentered isfloating monitor scratch key alpha*/
	{ "st",               NULL,         NULL,         0,        0,         0,         -1,     0,          1.00 },
    { "Zathura",          NULL,         NULL,         0,        0,         0,         -1,     0,          1.00 },
    { "vlc",              NULL,         NULL,         0,        0,         0,         -1,     0,          1.00 },
    { "obs",              NULL,         NULL,         0,        0,         0,         -1,     0,          1.00 },
    { "sxiv",             NULL,         NULL,         0,        0,         0,         -1,     0,          1.00 },
    { "Sxiv",             NULL,         NULL,         0,        0,         0,         -1,     0,          1.00 },
    { "lightcord",        NULL,         NULL,         0,        0,         0,         -1,     0,          0.95 },
    { "freetube",         NULL,         NULL,         0,        0,         0,         -1,     0,          1.00 },
    { "musescore",        NULL,         NULL,         0,        0,         0,         -1,     0,          1.00 },
    { "Unity",            NULL,         NULL,         0,        0,         0,         -1,     0,          0.95 },
    { "jetbrains-rider",  NULL,         NULL,         0,        0,         0,         -1,     0,          0.95 },
    { "Krita",            NULL,         NULL,         0,        0,         1,         -1,     0,          1.00 },
    { "qbittorrent",      NULL,         NULL,         0,        0,         1,         -1,     0,          0.90 },
    { "wow.exe",          NULL,         NULL,         0,        0,         1,         -1,     0,          1.00 },
	{ NULL,               "scratchpad", NULL,         0,        1,         1,         -1,     's',        1.00 },
	{ NULL,               "popcalc",    NULL,         0,        1,         1,         -1,     's',        1.00 },
	{ NULL,               "popcal",     NULL,         0,        1,         1,         -1,     's',        1.00 },
};

static const WarpRule warprules[] = {
    /* class              instance      title         willwarp */
    { NULL,               NULL,         "vlc",        0,       },
};

/* layout(s) */
static const float mfact     = 0.50; /* factor of master area size [0.05..0.95] */
static const float smfact    = 0.00; /* factor of tiled clients [0.00..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int decorhints  = 1;    /* 1 means respect decoration hints */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ "HHI",      gridfill },
	{ "|M|",      centeredmaster },
	{ "|||",      col },
	{ "###",      horizgrid },
	{ "=[]",      tileright },
	{ "TTT",      bottomstack },
	{ "{M}",      monoclenogap },
	{ "HHH",      gridfit },
	{ " |I",      vsplit },
	{ "===",      hsplit },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask|Mod1Mask,    KEY,      swaptags,       {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

#include "functions.c"

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, /*"-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, */ NULL };
static const char *termcmd[]  = { "st", NULL };

/*First arg only serves to match against key in rules*/
static const char *scratchpadcmd[] = { "s", "st", "-n", "scratchpad", "-t", "scratchpad", "-e", "scratchpad", NULL }; 
static const char *popcalccmd[] = { "s", "st", "-n", "popcalc", "-t", "popcalc", "-e", "popcalc", NULL }; 
static const char *popcalcmd[] = { "s", "st", "-n", "popcal", "-t", "popcal", "-e", "popcal", NULL }; 

static Key keys[] = {
	/* modifier                                  key                  function                argument */
	{ MODKEY,                                    XK_p,                spawn,                  {.v = dmenucmd } },
	{ MODKEY|ShiftMask,                          XK_Return,           spawn,                  {.v = termcmd } },
    { MODKEY,                                    XK_apostrophe,       togglescratch,          {.v = scratchpadcmd } },
    { MODKEY,                                    XK_numbersign,       togglescratch,          {.v = popcalccmd } },
    { MODKEY,                                    XK_Menu,             togglescratch,          {.v = popcalcmd } },
	{ MODKEY,                                    XK_b,                togglebar,              {0} },
	{ MODKEY,                                    XK_j,                focusstack,             {.i = +1 } },
	{ MODKEY,                                    XK_k,                focusstack,             {.i = -1 } },
	{ MODKEY|ShiftMask,                          XK_j,                movestack,              {.i = +1 } },
	{ MODKEY|ShiftMask,                          XK_k,                movestack,              {.i = -1 } },
	{ MODKEY,                                    XK_i,                incnmaster,             {.i = +1 } },
	{ MODKEY,                                    XK_d,                incnmaster,             {.i = -1 } },
	{ MODKEY,                                    XK_h,                setmfact,               {.f = -0.05} },
	{ MODKEY,                                    XK_l,                setmfact,               {.f = +0.05} },
	{ MODKEY|ShiftMask,                          XK_h,                setsmfact,              {.f = +0.05} },
	{ MODKEY|ShiftMask,                          XK_l,                setsmfact,              {.f = -0.05} },
    { MODKEY|ShiftMask,                          XK_e,                setcfact,               {.f = +0.25} },
	{ MODKEY|ShiftMask,                          XK_q,                setcfact,               {.f = -0.25} },
	{ MODKEY|ShiftMask,                          XK_r,                setcfact,               {.f =  0.00} },
	{ MODKEY,                                    XK_Return,           zoom,                   {0} },
	{ MODKEY,                                    XK_Tab,              view,                   {0} },
	{ MODKEY|ShiftMask,                          XK_c,                killclient,             {0} },
	{ MODKEY|ShiftMask,                          XK_x,                killunsel,              {0} },
	{ MODKEY,                                    XK_t,                setlayout,              {.v = &layouts[0]} },
	{ MODKEY,                                    XK_f,                setlayout,              {.v = &layouts[1]} },
	{ MODKEY,                                    XK_m,                setlayout,              {.v = &layouts[2]} },
	{ MODKEY,                                    XK_e,                setlayout,              {.v = &layouts[3]} },
	{ MODKEY,                                    XK_n,                setlayout,              {.v = &layouts[4]} },
	{ MODKEY,                                    XK_c,                setlayout,              {.v = &layouts[5]} },
	{ MODKEY,                                    XK_o,                setlayout,              {.v = &layouts[6]} },
	{ MODKEY,                                    XK_r,                setlayout,              {.v = &layouts[7]} },
	{ MODKEY,                                    XK_u,                setlayout,              {.v = &layouts[8]} },
	{ MODKEY,                                    XK_s,                setlayout,              {.v = &layouts[9]} },
	{ MODKEY,                                    XK_g,                setlayout,              {.v = &layouts[10]} },
	{ MODKEY,                                    XK_v,                setlayout,              {.v = &layouts[11]} },
	{ MODKEY,                                    XK_z,                setlayout,              {.v = &layouts[12]} },
    { MODKEY,                                    XK_space,            setlayout,              {0} },
    { MODKEY|ShiftMask,                          XK_v,                setattachdir,           {.i = -1 } },
	{ MODKEY|ShiftMask,                          XK_a,                setattachdir,           {.i = +1 } },
	{ MODKEY,                                    XK_KP_Subtract,      incgap,                 {.i = -1 } },
	{ MODKEY,                                    XK_KP_Add,           incgap,                 {.i = +1 } },
	{ MODKEY|ShiftMask,                          XK_KP_Subtract,      incgap,                 {.i = -5 } },
	{ MODKEY|ShiftMask,                          XK_KP_Add,           incgap,                 {.i = +5 } },
	{ MODKEY,                                    XK_Num_Lock,         incgap,                 {.i = 0 } },
	{ MODKEY|ShiftMask,                          XK_g,                togglegap,              {.i = 0 } },
	{ MODKEY|Mod1Mask,                           XK_KP_Subtract,      incborder,              {.i = -1 } },
	{ MODKEY|Mod1Mask,                           XK_KP_Add,           incborder,              {.i = +1 } },
	{ MODKEY|ShiftMask|Mod1Mask,                 XK_KP_Subtract,      incborder,              {.i = -5 } },
	{ MODKEY|ShiftMask|Mod1Mask,                 XK_KP_Add,           incborder,              {.i = +5 } },
	{ MODKEY|Mod1Mask,                           XK_Num_Lock,         incborder,              {.i = 0 } },
	{ MODKEY|ShiftMask,                          XK_b,                toggleborder,           {.i = 0 } },
	{ MODKEY|ShiftMask,                          XK_space,            togglefloating,         {0} },
	{ MODKEY|ShiftMask,                          XK_f,                togglefullscr,          {0} },
	{ MODKEY|ShiftMask,                          XK_s,                togglesticky,           {0} },
    { MODKEY|ShiftMask,                          XK_w,                togglewarp,             {0} },
    { MODKEY|ShiftMask|Mod1Mask,                 XK_f,                togglefakefullscr,      {0} },
	{ MODKEY,                                    XK_0,                view,                   {.ui = ~0 } },
	{ MODKEY|ShiftMask,                          XK_0,                tag,                    {.ui = ~0 } },
    { MODKEY,                                    XK_comma,            focusmon,               {.i = -1 } },
    { MODKEY,                                    XK_period,           focusmon,               {.i = +1 } },
	{ MODKEY|ShiftMask,                          XK_comma,            tagmon,                 {.i = -1 } },
	{ MODKEY|ShiftMask,                          XK_period,           tagmon,                 {.i = +1 } },
	TAGKEYS(                                     XK_1,                                        0)
	TAGKEYS(                                     XK_2,                                        1)
	TAGKEYS(                                     XK_3,                                        2)
	TAGKEYS(                                     XK_4,                                        3)
	TAGKEYS(                                     XK_5,                                        4)
	TAGKEYS(                                     XK_6,                                        5)
	TAGKEYS(                                     XK_7,                                        6)
	TAGKEYS(                                     XK_8,                                        7)
	TAGKEYS(                                     XK_9,                                        8)
	{ MODKEY|ShiftMask|ControlMask|Mod1Mask,     XK_q,                quit,                   {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
