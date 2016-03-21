#include "wrappers/lingeling.h"
extern "C"
{
	#include "lglib.h"
	#include <signal.h>
}
#include <iostream>

static LGL * lgl4sigh;
static void (*sig_alrm_handler)(int);
static int caughtalarm = 0;
static void catchalrm (int sig) {
  assert (sig == SIGALRM);
  if (!caughtalarm) {
    caughtalarm = 1;
  }
}

void LingelingWrapper::addProblem(const Cnf& cnf)
{
	for (auto cl: cnf)
	{
		for (auto lt: cl)
			lgladd (lgl_, lt);
		lgladd (lgl_, 0);
	}
}

static int checkalarm (void * ptr) {
  assert (ptr == (void*) &caughtalarm);
  return caughtalarm;
}
void LingelingWrapper::InitSolver(const Cnf& cnf)
{
	lgl_ = lglinit ();
	lglseterm (lgl_, checkalarm, &caughtalarm);
	sig_alrm_handler = signal (SIGALRM, catchalrm);
	addProblem(cnf);
lglsetopt(lgl_, "actavgmax", 34);
lglsetopt(lgl_, "actdblarithlim", 0);
lglsetopt(lgl_, "actgeomlim", 2);
lglsetopt(lgl_, "actgsdul", 98);
lglsetopt(lgl_, "acts", 2);
lglsetopt(lgl_, "actstdmax", 86);
lglsetopt(lgl_, "actstdmin", 94);
lglsetopt(lgl_, "actvlim", 1499966846);
lglsetopt(lgl_, "agile", 0);
lglsetopt(lgl_, "agilelim", 24);
lglsetopt(lgl_, "agilesinint", 2616328);
lglsetopt(lgl_, "bate", 0);
lglsetopt(lgl_, "batewait", 2);
lglsetopt(lgl_, "bca", 1);
lglsetopt(lgl_, "bcamaxeff", 1257787640);
lglsetopt(lgl_, "bcaminuse", 550040532);
lglsetopt(lgl_, "bcawait", 0);
lglsetopt(lgl_, "bias", 1);
lglsetopt(lgl_, "binlocsdel", 731);
lglsetopt(lgl_, "binsimpdel", 261);
lglsetopt(lgl_, "bkwdscale", 9);
lglsetopt(lgl_, "blkboost", 694);
lglsetopt(lgl_, "blkboostvlim", 906923068);
lglsetopt(lgl_, "blkclslim", 14569698);
lglsetopt(lgl_, "blklarge", 0);
lglsetopt(lgl_, "blkmaxeff", 1478916479);
lglsetopt(lgl_, "blkmineff", 466024376);
lglsetopt(lgl_, "blkocclim", 28);
lglsetopt(lgl_, "blkocclim1", 1152500523);
lglsetopt(lgl_, "blkocclim2", 541098517);
lglsetopt(lgl_, "blkreleff", 652);
lglsetopt(lgl_, "blkrtc", 1);
lglsetopt(lgl_, "blksched2b2", 1);
lglsetopt(lgl_, "blkschedmin", 0);
lglsetopt(lgl_, "blkschedprod", 1);
lglsetopt(lgl_, "blkschedpure", 1);
lglsetopt(lgl_, "blkschedsum", 0);
lglsetopt(lgl_, "blksmall", 0);
lglsetopt(lgl_, "blksuccesslim", 1248443303);
lglsetopt(lgl_, "blksuccessrat", 600934);
lglsetopt(lgl_, "block", 1);
lglsetopt(lgl_, "blockwait", 0);
lglsetopt(lgl_, "boost", 0);
lglsetopt(lgl_, "bumpbcplits", 0);
lglsetopt(lgl_, "bumpclslits", 1);
lglsetopt(lgl_, "bumpseenaftermin", 1);
lglsetopt(lgl_, "bumpseenbeforemin", 1);
lglsetopt(lgl_, "bumpseenlits", 0);
lglsetopt(lgl_, "bumpseenminsize", 1058907591);
lglsetopt(lgl_, "bva", 0);
lglsetopt(lgl_, "card", 0);
lglsetopt(lgl_, "cardcut", 1);
lglsetopt(lgl_, "cardexpam1", 528690559);
lglsetopt(lgl_, "cardglue", 4);
lglsetopt(lgl_, "cardignused", 0);
lglsetopt(lgl_, "cardmaxeff", 1354875805);
lglsetopt(lgl_, "cardmaxlen", 2076502736);
lglsetopt(lgl_, "cardmineff", 1963902803);
lglsetopt(lgl_, "cardminlen", 303801922);
lglsetopt(lgl_, "cardocclim1", 1261497557);
lglsetopt(lgl_, "cardocclim2", 89804880);
lglsetopt(lgl_, "cardreleff", 5483);
lglsetopt(lgl_, "cardreschedint", 13);
lglsetopt(lgl_, "carduse", 0);
lglsetopt(lgl_, "cardwait", 1);
lglsetopt(lgl_, "cce", 2);
lglsetopt(lgl_, "cce2wait", 1086769972);
lglsetopt(lgl_, "cce3wait", 228736232);
lglsetopt(lgl_, "cceboost", 153);
lglsetopt(lgl_, "cceboostvlim", 1967562528);
lglsetopt(lgl_, "ccemaxeff", 1160491369);
lglsetopt(lgl_, "ccemineff", 1350825032);
lglsetopt(lgl_, "cceonlyifstuck", 0);
lglsetopt(lgl_, "ccereleff", 932);
lglsetopt(lgl_, "ccertc", 0);
lglsetopt(lgl_, "ccesuccesslim", 509640620);
lglsetopt(lgl_, "ccesuccessrat", 232714994);
lglsetopt(lgl_, "ccewait", 1);
lglsetopt(lgl_, "cgrclsr", 0);
lglsetopt(lgl_, "cgrclsrwait", 0);
lglsetopt(lgl_, "cgreleff", 5730);
lglsetopt(lgl_, "cgrextand", 0);
lglsetopt(lgl_, "cgrexteq", 0);
lglsetopt(lgl_, "cgrextite", 0);
lglsetopt(lgl_, "cgrextunits", 0);
lglsetopt(lgl_, "cgrextxor", 0);
lglsetopt(lgl_, "cgrmaxeff", 602696177);
lglsetopt(lgl_, "cgrmaxority", 20);
lglsetopt(lgl_, "cgrmineff", 917153975);
lglsetopt(lgl_, "cintinc", 22);
lglsetopt(lgl_, "cintincdiv", 1);
lglsetopt(lgl_, "cintmaxhard", 1236947701);
lglsetopt(lgl_, "cintmaxsoft", 935389670);
lglsetopt(lgl_, "cliff", 1);
lglsetopt(lgl_, "cliffmaxeff", 1509707790);
lglsetopt(lgl_, "cliffmineff", 1579200863);
lglsetopt(lgl_, "cliffreleff", 7125);
lglsetopt(lgl_, "cliffwait", 0);
lglsetopt(lgl_, "clim", 1332279070);
lglsetopt(lgl_, "compact", 0);
lglsetopt(lgl_, "deco", 0);
lglsetopt(lgl_, "decolim", 960881060);
lglsetopt(lgl_, "decompose", 1);
lglsetopt(lgl_, "defragfree", 10);
lglsetopt(lgl_, "defragint", 63664702);
lglsetopt(lgl_, "delmax", 6);
lglsetopt(lgl_, "dlim", 70742100);
lglsetopt(lgl_, "elim", 1);
lglsetopt(lgl_, "elmaxeff", 1395831629);
lglsetopt(lgl_, "elmblk", 0);
lglsetopt(lgl_, "elmblkwait", 1);
lglsetopt(lgl_, "elmboost", 15);
lglsetopt(lgl_, "elmclslim", 3);
lglsetopt(lgl_, "elmfull", 1);
lglsetopt(lgl_, "elmineff", 1575874658);
lglsetopt(lgl_, "elmlitslim", 1874360037);
lglsetopt(lgl_, "elmocclim", 256803914);
lglsetopt(lgl_, "elmocclim1", 2825);
lglsetopt(lgl_, "elmocclim2", 4530450);
lglsetopt(lgl_, "elmreleff", 4462);
lglsetopt(lgl_, "elmroundlim", 135165);
lglsetopt(lgl_, "elmrtc", 0);
lglsetopt(lgl_, "elmsched2b2", 1);
lglsetopt(lgl_, "elmschediff", 0);
lglsetopt(lgl_, "elmschedmin", 0);
lglsetopt(lgl_, "elmschedprod", 1);
lglsetopt(lgl_, "elmschedpure", 0);
lglsetopt(lgl_, "elmschedsum", 0);
lglsetopt(lgl_, "elmsuccesslim", 562014715);
lglsetopt(lgl_, "elmsuccessrat", 7);
lglsetopt(lgl_, "factmax", 1620713);
lglsetopt(lgl_, "factor", 2);
lglsetopt(lgl_, "flipdur", 26);
lglsetopt(lgl_, "flipint", 78740779);
lglsetopt(lgl_, "flipldmod", 25);
lglsetopt(lgl_, "fliplevels", 25);
lglsetopt(lgl_, "flipping", 0);
lglsetopt(lgl_, "fliptop", 1);
lglsetopt(lgl_, "flipvlim", 776287102);
lglsetopt(lgl_, "force", 255114256);
lglsetopt(lgl_, "gauss", 0);
lglsetopt(lgl_, "gaussexptrn", 0);
lglsetopt(lgl_, "gaussextrall", 0);
lglsetopt(lgl_, "gaussmaxeff", 49581972);
lglsetopt(lgl_, "gaussmaxor", 28);
lglsetopt(lgl_, "gaussmineff", 184208605);
lglsetopt(lgl_, "gaussreleff", 4055);
lglsetopt(lgl_, "gausswait", 1);
lglsetopt(lgl_, "gluekeep", 1012754000);
lglsetopt(lgl_, "gluescale", 2);
lglsetopt(lgl_, "import", 0);
lglsetopt(lgl_, "incredcint", 1);
lglsetopt(lgl_, "incredconfslim", 59);
lglsetopt(lgl_, "incsavevisits", 1);
lglsetopt(lgl_, "inprocessing", 0);
lglsetopt(lgl_, "irrlim", 1);
lglsetopt(lgl_, "itlocsdel", 89006);
lglsetopt(lgl_, "itsimpdel", 65357);
lglsetopt(lgl_, "jwhred", 1);
lglsetopt(lgl_, "keepmaxglue", 0);
lglsetopt(lgl_, "lftmaxeff", 1393213965);
lglsetopt(lgl_, "lftmineff", 639462210);
lglsetopt(lgl_, "lftreleff", 5984);
lglsetopt(lgl_, "lftroundlim", 114059963);
lglsetopt(lgl_, "lhbr", 0);
lglsetopt(lgl_, "lift", 0);
lglsetopt(lgl_, "liftlrg", 2091767233);
lglsetopt(lgl_, "liftwait", 2);
lglsetopt(lgl_, "lkhd", -1);
lglsetopt(lgl_, "lkhdmisifelmrtc", 1);
lglsetopt(lgl_, "locs", 1718665596);
lglsetopt(lgl_, "locsboost", 56);
lglsetopt(lgl_, "locscint", 185859);
lglsetopt(lgl_, "locsclim", 7461477);
lglsetopt(lgl_, "locsdec", 1);
lglsetopt(lgl_, "locset", 2);
lglsetopt(lgl_, "locsexport", 1);
lglsetopt(lgl_, "locsmaxeff", 23304298);
lglsetopt(lgl_, "locsmineff", 1781876326);
lglsetopt(lgl_, "locsred", 1);
lglsetopt(lgl_, "locsreleff", 9);
lglsetopt(lgl_, "locsrtc", 0);
lglsetopt(lgl_, "locsvared", 805);
lglsetopt(lgl_, "locswait", 0);
lglsetopt(lgl_, "maxglue", 1217158434);
lglsetopt(lgl_, "maxscorexp", 165271);
lglsetopt(lgl_, "mega", 0);
lglsetopt(lgl_, "megaint", 23);
lglsetopt(lgl_, "megawait", 1);
lglsetopt(lgl_, "memlim", 1778296950);
lglsetopt(lgl_, "minimize", 2);
lglsetopt(lgl_, "minlocalgluelim", 2082541236);
lglsetopt(lgl_, "minrecgluelim", 1702223288);
lglsetopt(lgl_, "mocint", 1);
lglsetopt(lgl_, "move", 2);
lglsetopt(lgl_, "otfs", 0);
lglsetopt(lgl_, "otfsbump", 2);
lglsetopt(lgl_, "otfsconf", 0);
lglsetopt(lgl_, "penmax", 13);
lglsetopt(lgl_, "phase", 0);
lglsetopt(lgl_, "phaseflip", 0);
lglsetopt(lgl_, "phasegluebit", 188262780);
lglsetopt(lgl_, "phaseneginit", 1842373918);
lglsetopt(lgl_, "plain", 0);
lglsetopt(lgl_, "plim", 1043431687);
lglsetopt(lgl_, "prbasic", 1);
lglsetopt(lgl_, "prbasicmaxeff", 1606402215);
lglsetopt(lgl_, "prbasicmineff", 1611871908);
lglsetopt(lgl_, "prbasicreleff", 7520);
lglsetopt(lgl_, "prbasicroundlim", 85309471);
lglsetopt(lgl_, "prbasicrtc", 1);
lglsetopt(lgl_, "prbrtc", 1);
lglsetopt(lgl_, "prbsimple", 2);
lglsetopt(lgl_, "prbsimpleboost", 32);
lglsetopt(lgl_, "prbsimpleliftdepth", 4);
lglsetopt(lgl_, "prbsimplemaxeff", 1063017869);
lglsetopt(lgl_, "prbsimplemineff", 1598251036);
lglsetopt(lgl_, "prbsimplereleff", 3606);
lglsetopt(lgl_, "prbsimplertc", 1);
lglsetopt(lgl_, "probe", 1);
lglsetopt(lgl_, "psm", 2);
lglsetopt(lgl_, "pure", 1);
lglsetopt(lgl_, "randec", 0);
lglsetopt(lgl_, "randecflipint", 396012319);
lglsetopt(lgl_, "randecint", 544);
lglsetopt(lgl_, "rdp", 0);
lglsetopt(lgl_, "rdpclslim", 635184447);
lglsetopt(lgl_, "rdplim", 72);
lglsetopt(lgl_, "rdpmaxeff", 1885050429);
lglsetopt(lgl_, "rdpmineff", 1829859191);
lglsetopt(lgl_, "rdpmodelm", 0);
lglsetopt(lgl_, "rdpreleff", 274);
lglsetopt(lgl_, "rdpwait", 2);
lglsetopt(lgl_, "redfixed", 0);
lglsetopt(lgl_, "redinoutinc", 505);
lglsetopt(lgl_, "redlbound", 1);
lglsetopt(lgl_, "redlexpfac", 876);
lglsetopt(lgl_, "redlinc", 7);
lglsetopt(lgl_, "redlinit", 5);
lglsetopt(lgl_, "redlmaxabs", 410);
lglsetopt(lgl_, "redlmaxinc", 2);
lglsetopt(lgl_, "redlmaxrel", 1398);
lglsetopt(lgl_, "redlminabs", 54);
lglsetopt(lgl_, "redlmininc", 94);
lglsetopt(lgl_, "redlminrel", 832);
lglsetopt(lgl_, "redloutinc", 409828);
lglsetopt(lgl_, "redoutclim", 810017266);
lglsetopt(lgl_, "redoutvlim", 60217634);
lglsetopt(lgl_, "reduce", 3);
lglsetopt(lgl_, "rephase", 2);
lglsetopt(lgl_, "rephaseinc", 2837);
lglsetopt(lgl_, "restart", 2);
lglsetopt(lgl_, "restartinit", 268092655);
lglsetopt(lgl_, "restartint", 5840);
lglsetopt(lgl_, "restartintscale", 0);
lglsetopt(lgl_, "reusetrail", 0);
lglsetopt(lgl_, "rmincpen", 4);
lglsetopt(lgl_, "rstinoutinc", 61);
lglsetopt(lgl_, "saturating", 142);
lglsetopt(lgl_, "scincinc", 892);
lglsetopt(lgl_, "score", 4);
lglsetopt(lgl_, "seed", 954469376);
lglsetopt(lgl_, "simpdelay", 1921414564);
lglsetopt(lgl_, "simpen", 3);
lglsetopt(lgl_, "simpidiv", 89);
lglsetopt(lgl_, "simpinterdelay", 801339892);
lglsetopt(lgl_, "simpiscale", 3944);
lglsetopt(lgl_, "simplify", 0);
lglsetopt(lgl_, "simprtc", 96);
lglsetopt(lgl_, "simpvarchg", 7);
lglsetopt(lgl_, "simpvarlim", 574886638);
lglsetopt(lgl_, "sizemaxpen", 16);
lglsetopt(lgl_, "sizepen", 4513);
lglsetopt(lgl_, "smallirr", 3);
lglsetopt(lgl_, "smallve", 1);
lglsetopt(lgl_, "smallvevars", 4);

lglsetopt(lgl_, "smallvewait", 1);
lglsetopt(lgl_, "sortlits", 0);
lglsetopt(lgl_, "subl", 730);
lglsetopt(lgl_, "synclsall", 0);
lglsetopt(lgl_, "synclsglue", 455197353);
lglsetopt(lgl_, "synclsint", 4);
lglsetopt(lgl_, "synclslen", 1681909122);
lglsetopt(lgl_, "syncunint", 819235);
lglsetopt(lgl_, "tabr", 4);
lglsetopt(lgl_, "tabrcfactor", 94938);
lglsetopt(lgl_, "tabrkeep", 2);
lglsetopt(lgl_, "tabrvfactor", 599247834);
lglsetopt(lgl_, "termint", 333396);
lglsetopt(lgl_, "ternres", 0);
lglsetopt(lgl_, "ternresboost", 6);
lglsetopt(lgl_, "ternresrtc", 1);
lglsetopt(lgl_, "ternreswait", 0);

lglsetopt(lgl_, "transred", 0);
lglsetopt(lgl_, "transredwait", 1);
lglsetopt(lgl_, "trdmaxeff", 277838070);
lglsetopt(lgl_, "trdmineff", 1700224539);
lglsetopt(lgl_, "trdreleff", 7687);

lglsetopt(lgl_, "treelook", 2);
lglsetopt(lgl_, "treelookboost", 20547);
lglsetopt(lgl_, "treelookfull", 0);
lglsetopt(lgl_, "treelooklrg", 1);
lglsetopt(lgl_, "treelookmaxeff", 1648006187);
lglsetopt(lgl_, "treelookmineff", 1380576682);
lglsetopt(lgl_, "treelookreleff", 559);
lglsetopt(lgl_, "treelookrtc", 1);
//lglsetopt(lgl_, "trep", 1);

lglsetopt(lgl_, "trepint", 52583303);
lglsetopt(lgl_, "trnreleff", 844);
lglsetopt(lgl_, "trnrmaxeff", 1265637314);
lglsetopt(lgl_, "trnrmineff", 1162624033);
lglsetopt(lgl_, "unhdatrn", 2);
lglsetopt(lgl_, "unhdextstamp", 0);
lglsetopt(lgl_, "unhdhbr", 0);
lglsetopt(lgl_, "unhdlnpr", 575227946);
lglsetopt(lgl_, "unhdmaxeff", 886764955);
lglsetopt(lgl_, "unhdmineff", 646318256);
lglsetopt(lgl_, "unhdreleff", 523);
lglsetopt(lgl_, "unhdroundlim", 18);
lglsetopt(lgl_, "unhide", 1);
lglsetopt(lgl_, "unhidewait", 2);
lglsetopt(lgl_, "wait", 1);
lglsetopt(lgl_, "waitmax", 1284343515);
}

void LingelingWrapper::Solve()
{
	alarm (scans_limit_);
	int res = lglsat (lgl_);
	//FIXME: Dirty workaround for bad signal handling!
	if (!caughtalarm) pause();
	caughtalarm = 0;
	(void) signal (SIGALRM, sig_alrm_handler);
	if (res == 10)
		state = SAT;
	else if (res == 20)
		state = UNSAT;
	else
		state = STOPPED;
}

void LingelingWrapper::AddUCs(const UnitClauseVector& uc_vector)
{
		for (auto lt: uc_vector)
		{
			lgladd (lgl_, lt);
			lgladd (lgl_, 0);
		}
}

SolverReport LingelingWrapper::GetReport()
{
	SolverReport out;
	out.state = state;
	//out.watch_scans = lglvisits(lgl_);
	out.watch_scans = 1+1000*lglsearchtime(lgl_);
	return out;
}

UnitClauseVector LingelingWrapper::GetSolution()
{
	UnitClauseVector out;
	//assert(S.nVars()==S.model.size());
	int maxvar = lglmaxvar (lgl_);
	for (int i = 1; i <= maxvar; i++)
	{
		Lit lit = (lglderef (lgl_, i) > 0) ? i : -i;
		out.push_back(lit);
	}

	lglstats(lgl_);
	return out;
}
void LingelingWrapper::SetWatchScansLimit(long long unsigned int scans_limit)
{
	//lglsetopt(lgl_,"plim" ,scans_limit);
	//lglsetopt(lgl_,"verbose" ,5);
	scans_limit_ = scans_limit;
}
LingelingWrapper::~LingelingWrapper(void)
{
	lglrelease (lgl_);
}

