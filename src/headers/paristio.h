/* Copyright (C) 2000  The PARI group.

This file is part of the PARI/GP package.

PARI/GP is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation. It is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY WHATSOEVER.

Check the License for details. You should have received a copy of it, along
with the package; see the file 'COPYING'. If not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

/* This file contains memory and I/O management definitions       */

typedef struct {
  long s, us;
} pari_timer;

typedef unsigned char *byteptr;
typedef ulong pari_sp;

/* iterator over primes */
typedef struct {
  int strategy; /* 1 to 4 */
  GEN bb; /* iterate through primes <= bb */
  ulong c, q; /* congruent to c (mod q) */

  /* strategy 1: private prime table */
  byteptr d; /* diffptr + n */
  ulong p; /* current p = n-th prime */
  ulong b; /* min(bb, ULONG_MAX) */

  /* strategy 2: sieve, use p */
  unsigned char *sieve;
  ulong cache[9]; /* look-ahead primes already computed */
  ulong chunk; /* # of odd integers in sieve */
  ulong a, end, sieveb; /* [a,end] interval currently being sieved,
                         * end <= sieveb = min(bb, maxprime^2, ULONG_MAX) */
  ulong pos, maxpos; /* current cell and max cell */

  /* strategy 3: unextprime, use p */

  /* strategy 4: nextprime */
  GEN pp;
} forprime_t;

typedef struct {
  int first;
  GEN b, n, p;
  forprime_t T;
} forcomposite_t;

typedef struct forvec_t {
  long first;
  GEN *a, *m, *M; /* current n-uplet, minima, Maxima */
  long n; /* length */
  GEN (*next)(struct forvec_t *);
} forvec_t;

/* Iterate over partitions */
typedef struct
{
  long k;
  long amax, amin, nmin, nmax, strip;
  GEN v;
} forpart_t;

/* binary I/O */
typedef struct GENbin {
  size_t len; /* gsizeword(x) */
  GEN x; /* binary copy of x */
  GEN base; /* base address of p->x */
  int canon; /* 1: t_INT in canonical (native kernel) form,
                0: t_INT according to current kernel */
} GENbin;

struct pari_mainstack
{
  pari_sp top, bot, vbot;
  size_t size, rsize, vsize, memused;
};

extern THREAD struct pari_mainstack *pari_mainstack;

struct pari_thread
{
  struct pari_mainstack st;
  GEN data;
};

typedef struct pariFILE {
  FILE *file;
  int type;
  const char *name;
  struct pariFILE* prev;
  struct pariFILE* next;
} pariFILE;
/* pariFILE.type */
enum { mf_IN  = 1, mf_PIPE = 2, mf_FALSE = 4, mf_OUT = 8, mf_PERM = 16 };

typedef struct entree {
  const char *name;
  ulong valence;
  void *value;
  long menu;
  const char *code;
  const char *help;
  void *pvalue;
  long arity;
  struct entree *next;
} entree;

struct pari_parsestate
{
  long node;
  int once;
  long discarded;
  const char *lex_start, *unused_chars;
  GEN lasterror;
};

struct pari_compilestate
{
  long opcode, operand, data, localvars, frames, dbginfo;
  long offset;
  const char *dbgstart;
};

struct pari_evalstate
{
  pari_sp avma;
  long sp;
  long rp;
  long var;
  long lvars;
  long prec;
  long trace;
  long pending_threads;
  struct pari_compilestate comp;
};

struct gp_context
{
  long listloc;
  long prettyp;
  struct pari_evalstate eval;
  struct pari_parsestate parse;
  pariFILE *file;
  jmp_buf *iferr_env;
  GEN err_data;
};

struct mt_state
{
  GEN worker;
  GEN pending;
  long workid;
};

struct pari_mt
{
  struct mt_state mt;
  GEN (*get)(struct mt_state *mt, long *workid, long *pending);
  void (*submit)(struct mt_state *mt, long workid, GEN work);
  void (*end)(void);
};

typedef struct PariOUT {
  void (*putch)(char);
  void (*puts)(const char*);
  void (*flush)(void);
} PariOUT;

/* hashtables */
typedef struct hashentry {
  void *key, *val;
  ulong hash; /* hash(key) */
  struct hashentry *next;
} hashentry;

typedef struct hashtable {
  ulong len; /* table length */
  hashentry **table; /* the table */
  ulong nb, maxnb; /* number of entries stored and max nb before enlarging */
  ulong pindex; /* prime index */
  ulong (*hash) (void *k); /* hash function */
  int (*eq) (void *k1, void *k2); /* equality test */
  int use_stack; /* use stack_malloc instead of malloc ? */
} hashtable;

typedef struct {
  void **data;
  long n;
  long alloc;
  size_t size;
} pari_stack;

/* GP_DATA */
typedef struct {
  GEN z; /* result */
  time_t t; /* time to obtain result */
} gp_hist_cell;
typedef struct {
  gp_hist_cell *v; /* array of previous results, FIFO */
  size_t size; /* # res */
  ulong total; /* # of results computed since big bang */
} gp_hist; /* history */

typedef struct {
  pariFILE *file;
  char *cmd;
} gp_pp; /* prettyprinter */

typedef struct {
  char *PATH;
  char **dirs;
} gp_path; /* path */
typedef struct {
  char format; /* e,f,g */
  long sigd;   /* -1 (all) or number of significant digits printed */
  int sp;      /* 0 = suppress whitespace from output */
  int prettyp; /* output style: raw, prettyprint, etc */
  int TeXstyle;
} pariout_t; /* output format */

enum { gpd_QUIET=1, gpd_TEST=2, gpd_EMACS=256, gpd_TEXMACS=512};
enum { DO_MATCHED_INSERT = 2, DO_ARGS_COMPLETE = 4 };
typedef struct {
  gp_hist *hist;
  gp_pp *pp;
  gp_path *path, *sopath;
  pariout_t *fmt;
  ulong lim_lines, flags, linewrap, readline_state;
  int echo, breakloop, recover, use_readline;
  char *help, *histfile, *prompt, *prompt_cont, *prompt_comment;
  GEN colormap, graphcolors;

  int secure, simplify, strictmatch, strictargs, chrono;
  pari_timer *T;
  ulong primelimit; /* deprecated */
  ulong threadsizemax, threadsize;
} gp_data;
extern gp_data *GP_DATA;

/* Common global variables: */

extern PariOUT *pariOut, *pariErr;
extern FILE    *pari_outfile, *pari_logfile, *pari_infile, *pari_errfile;
extern ulong    logstyle;

enum logstyles {
    logstyle_none,        /* 0 */
    logstyle_plain,        /* 1 */
    logstyle_color,        /* 2 */
    logstyle_TeX         /* 3 */
};

enum { c_ERR, c_HIST, c_PROMPT, c_INPUT, c_OUTPUT, c_HELP, c_TIME, c_LAST,
       c_NONE = 0xffffUL };

enum { TEXSTYLE_PAREN=2, TEXSTYLE_BREAK=4 };

extern THREAD pari_sp avma;
#define DISABLE_MEMUSED (size_t)-1
extern byteptr diffptr;
extern char *current_psfile, *pari_datadir;

#define gcopyifstack(x,y)  STMT_START {pari_sp _t=(pari_sp)(x); \
  (y)=(_t>=pari_mainstack->bot &&_t<pari_mainstack->top)? \
      gcopy((GEN)_t): (GEN)_t;} STMT_END
#define copyifstack(x,y)  STMT_START {pari_sp _t=(pari_sp)(x); \
  (y)=(_t>=pari_mainstack->bot &&_t<pari_mainstack->top)? \
      gcopy((GEN)_t): (GEN)_t;} STMT_END
#define icopyifstack(x,y) STMT_START {pari_sp _t=(pari_sp)(x); \
  (y)=(_t>=pari_mainstack->bot &&_t<pari_mainstack->top)? \
      icopy((GEN)_t): (GEN)_t;} STMT_END

/* Define this to thoroughly check "random" garbage collecting */
#ifdef DEBUG_LOWSTACK
#  define low_stack(x,l) 1
#  define gc_needed(av,n) 1
#else
#  define low_stack(x,l) ((void)(x),avma < (pari_sp)(l))
#  define gc_needed(av,n) (avma < (pari_sp)stack_lim(av,n))
#endif

#define stack_lim(av,n) (pari_mainstack->bot+(((av)-pari_mainstack->bot)>>(n)))

#ifndef SIG_IGN
#  define SIG_IGN (void(*)())1
#endif
#ifndef SIGINT
#  define SIGINT 2
#endif
