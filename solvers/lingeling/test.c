
#include "lglib.h"

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int catchedsig, verbose, ignmissingheader, ignaddcls;

static int next (FILE * in, int *linenoptr) {
  int res = getc (in);
  if (res == '\n') *linenoptr += 1;
  return res;
}

static char isoptchartab[256];
static int isoptchartabinitialized;

static int isoptchar (unsigned char uch) { 
  int i;
  if (!isoptchartabinitialized) {
    for (i = 'a'; i <= 'z'; i++) isoptchartab[i] = 1;
    for (i = 'A'; i <= 'Z'; i++) isoptchartab[i] = 1;
    for (i = '0'; i <= '9'; i++) isoptchartab[i] = 1;
    isoptchartab['-'] = isoptchartab['_'] = 1;
    isoptchartabinitialized = 1;
  }
  return isoptchartab[uch];
}
typedef struct Opt { char * name; int size, count; } Opt;

static void pushoptch (Opt * opt, int ch) {
  if (opt->count + 1 >= opt->size)
   opt->name = realloc (opt->name, opt->size = opt->size ? 2*opt->size : 2);

  opt->name[opt->count++] = ch;
  opt->name[opt->count] = 0;
}
static const char * parse (LGL * lgl, FILE * in, int * lp) {
  int ch, prev, m, n, v, c, l, lit, sign, val, embedded = 0, header;
  Opt opt;
  memset (&opt, 0, sizeof opt);
SKIP:
  ch = next (in, lp);
  if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') goto SKIP;
  if (ch == 'c') {
    ch  = next (in, lp);
    while (ch != '\n') {
      if (ch == EOF) return "end of file in comment";
      prev = ch;
      ch = next (in, lp);
      if (prev != '-') continue;
      if (ch != '-') continue;
      assert (!opt.count);
      ch = next (in, lp);
      while (isoptchar (ch)) {
	assert (ch != '\n');
	pushoptch (&opt, ch);
	ch = next (in, lp);
      }
      opt.count = 0;
      if (ch != '=') continue;
      ch = next (in, lp);
      if (ch == '-') sign = -1, ch = next (in, lp); else sign = 1;
      if (!isdigit (ch)) continue;
      val = ch - '0';
      while (isdigit (ch = next (in, lp)))
	val = 10 * val + (ch - '0');

      if (!lglhasopt (lgl, opt.name)) {
	fprintf (stderr, 
	         "*** lingeling warning: "
		 "parsed invalid embedded option '--%s'\n", 
		 opt.name);
	continue;
      }

      val *= sign;

      if (!embedded++ && verbose >= 0)
	printf ("c\nc embedded options:\nc\n");
      if (!strcmp (opt.name, "verbose")) verbose = val;
      if (verbose >= 0) printf ("c --%s=%d\n", opt.name, val);
      lglsetopt (lgl, opt.name, val);
    }
    goto SKIP;
  }
  free (opt.name);
  if (verbose >= 0) {
     if (embedded) printf ("c\n"); else printf ("c no embedded options\n");
     fflush (stdout);
  }
  header = m = n = v = l = c = 0;
  if (ignmissingheader) {
    if (ch == 'p')  {
      if (verbose >= 0) printf ("will not read header");
      while ((ch = next (in, lp)) != '\n' && ch != EOF)
	;
    } else if (verbose >= 0) printf ("c skipping missing header\n");
    goto BODY2;
  }
  if (ch != 'p') return "missing 'p ...' header";
  if (next (in, lp) != ' ') return "invalid header: expected ' ' after 'p'";
  while ((ch = next (in, lp)) == ' ')
    ;
  if (ch != 'c') return "invalid header: expected 'c' after ' '";
  if (next (in, lp) != 'n') return "invalid header: expected 'n' after 'c'";
  if (next (in, lp) != 'f') return "invalid header: expected 'f' after 'n'";
  if (next (in, lp) != ' ') return "invalid header: expected ' ' after 'f'";
  while ((ch = next (in, lp)) == ' ')
    ;
  if (!isdigit (ch)) return "invalid header: expected digit after 'p cnf '";
  m = ch - '0';
  while (isdigit (ch = next (in, lp)))
    m = 10 * m + (ch - '0');
  if (ch != ' ') return "invalid header: expected ' ' after 'p cnf <m>'"; 
  while ((ch = next (in, lp)) == ' ')
    ;
  if (!isdigit (ch)) 
    return "invalid header: expected digit after 'p cnf <m> '";
  n = ch - '0';
  while (isdigit (ch = next (in, lp)))
    n = 10 * n + (ch - '0');
  while (ch == ' ')
    ch = next (in, lp);
  if (ch != '\n') return "invalid header: expected new line after header";
  if (verbose >= 0)
    printf ("c found 'p cnf %d %d' header\n", m, n), fflush (stdout);
  header = 1;
BODY:
  ch = next (in, lp);
BODY2:
  if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') goto BODY;
  if (ch == 'c') {
    while ((ch = next (in, lp)) != '\n')
      if (ch == EOF) return "end of file in comment";
    goto BODY;
  }
  if (ch == EOF) {
    if (header && c + 1 == n) return "clause missing";
    if (header && c < n) return "clauses missing";
DONE:
    if (verbose >= 0) {
      printf ("c read %d variables, %d clauses, %d literals in %.2f seconds\n", 
	      v, c, l, lglsec (lgl));
      fflush (stdout);
    }
    return 0;
  }
  if (ch == '-') {
    ch = next (in, lp);
    if (ch == '0') return "expected positive digit after '-'";
    sign = -1;
  } else sign = 1;
  if (!isdigit (ch)) return "expected digit";
  if (header && c == n) return "too many clauses";
  lit = ch - '0';
  while (isdigit (ch = next (in, lp)))
    lit = 10 * lit + (ch - '0');
  if (header && lit > m) return "maxium variable index exceeded";
  if (lit > v) v = lit;
  if (lit) l++;
  else c++;
  lit *= sign;
  lgladd (lgl, lit);
  if (!lit && ignaddcls && c == n) goto DONE;
  goto BODY;
}

int main (int argc, char ** argv) {
  int res;
  FILE * in, * out, * pfile;
  const char * iname, * oname, * pname, * match, * p, * err, * thanks;
  in = out = 0;
  LGL * lgl = lglinit ();
  int lineno = 1;
  iname = argv[1];
  in = fopen (iname, "r");
  parse (lgl, in, &lineno);
  res = lglsat (lgl);
  if (verbose >= 0) fputs ("c\n", stdout), lglstats (lgl);
  exit(2);
}
