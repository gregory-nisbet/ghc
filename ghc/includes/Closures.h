/* ----------------------------------------------------------------------------
 * $Id: Closures.h,v 1.16 2000/01/18 12:36:38 simonmar Exp $
 *
 * (c) The GHC Team, 1998-1999
 *
 * Closures
 *
 * -------------------------------------------------------------------------- */

#ifndef CLOSURES_H
#define CLOSURES_H

/*
 * The Layout of a closure header depends on which kind of system we're
 * compiling for: profiling, parallel, ticky, etc.
 */

/* -----------------------------------------------------------------------------
   The profiling header
   -------------------------------------------------------------------------- */

#ifdef PROFILING

typedef struct {
   CostCentreStack *ccs;
} StgProfHeader;

#else /* !PROFILING */

typedef struct {
	/* empty */
} StgProfHeader;

#endif /* PROFILING */

/* -----------------------------------------------------------------------------
   The parallel header
   -------------------------------------------------------------------------- */

#ifdef PAR

typedef struct {
  /* StgWord ga; */  /* nope! global addresses are managed via a hash table */
} StgParHeader;

#else /* !PAR */

typedef struct {
  /* empty */
} StgParHeader;

#endif /* PAR */

/* -----------------------------------------------------------------------------
   The GranSim header
   -------------------------------------------------------------------------- */

#if defined(GRAN)

typedef struct {
  StgWord procs; /* bitmask indicating on which PEs this closure resides */
} StgGranHeader;

#else /* !GRAN */

typedef struct {
  /* empty */
} StgGranHeader;

#endif /* GRAN */

/* -----------------------------------------------------------------------------
   The ticky-ticky header

   Comment from old Ticky.h:

   This is used to record if a closure has been updated but not yet
   entered. It is set when the closure is updated and cleared when
   subsequently entered.
   
   NB: It is {\em not} an ``entry count'', it is an
   ``entries-after-update count.''
   
   The commoning up of @CONST@, @CHARLIKE@ and @INTLIKE@ closures is
   turned off(?) if this is required. This has only been done for 2s
   collection.  It is done using a nasty hack which defines the
   @_Evacuate@ and @_Scavenge@ code for @CONST@, @CHARLIKE@ and @INTLIKE@
   info tables to be @_Evacuate_1@ and @_Scavenge_1_0@.
   -------------------------------------------------------------------------- */

#ifdef TICKY_TICKY

typedef struct {
  /* old: W_ updated; */
} StgTickyHeader;

#else /* !TICKY_TICKY */

typedef struct {
	/* empty */
} StgTickyHeader;

#endif /* TICKY_TICKY */

/* -----------------------------------------------------------------------------
   The full fixed-size closure header

   The size of the fixed header is the sum of the optional parts plus a single
   word for the entry code pointer.
   -------------------------------------------------------------------------- */

typedef struct {
	const struct _StgInfoTable* info;
#ifdef PROFILING
	StgProfHeader         prof;
#endif
#ifdef PAR
	StgParHeader          par;
#endif
#ifdef GRAN
	StgGranHeader         gran;
#endif
#ifdef TICKY_TICKY
	StgTickyHeader        ticky;
#endif
} StgHeader;

#define FIXED_HS (sizeof(StgHeader))

/* -----------------------------------------------------------------------------
   Closure Types

   For any given closure type (defined in InfoTables.h), there is a
   corresponding structure defined below.  The name of the structure
   is obtained by concatenating the closure type with '_closure'
   -------------------------------------------------------------------------- */

/* All closures follow the generic format */

typedef struct StgClosure_ {
    StgHeader   header;
    struct StgClosure_ *payload[0];
} StgClosure;

/* What a stroke of luck - all our mutable closures follow the same
 * basic layout, with the mutable link field as the second field after
 * the header.  This means the following structure is the supertype of
 * mutable closures.
 */

typedef struct StgMutClosure_ {
    StgHeader   header;
    StgPtr     *padding;
    struct StgMutClosure_ *mut_link;
    struct StgClosure_ *payload[0];
} StgMutClosure;

typedef struct {
    StgHeader   header;
    StgClosure *selectee;
} StgSelector;

typedef struct {
    StgHeader   header;
    StgWord     n_args;
    StgClosure *fun;
    StgPtr      payload[0];
} StgPAP;

typedef struct {
    StgHeader   header;
    StgWord     n_args;
    StgClosure *fun;
    StgPtr      payload[0];
} StgAP_UPD;

typedef struct {
    StgHeader  header;
    StgWord    n_ptrs;
    StgWord    n_words;
    StgWord    n_instrs;
    StgWord    stgexpr;
    StgPtr     payload[0];
} StgBCO;

typedef struct {
    StgHeader   header;
    StgClosure *indirectee;
} StgInd;

typedef struct {
    StgHeader   header;
    StgClosure *indirectee;
    StgMutClosure *mut_link;
} StgIndOldGen;

typedef struct {
    StgHeader   header;
    StgClosure *indirectee;
    StgClosure *static_link;
} StgIndStatic;

typedef struct StgCAF_ {
    StgHeader     header;
    StgClosure    *body;
    StgMutClosure *mut_link;
    StgClosure    *value;
    struct StgCAF_ *link;
} StgCAF;

typedef struct {
    StgHeader  header;
    StgWord    words;
    StgWord    payload[0];
} StgArrWords;

typedef struct {
    StgHeader   header;
    StgWord     ptrs;
    StgMutClosure *mut_link;	/* mutable list */
    StgClosure *payload[0];
} StgMutArrPtrs;

typedef struct {
    StgHeader   header;
    StgClosure *var;
    StgMutClosure *mut_link;
} StgMutVar;

typedef struct _StgUpdateFrame {
    StgHeader  header;
    struct _StgUpdateFrame *link;
    StgClosure *updatee;
} StgUpdateFrame;

typedef struct {
    StgHeader  header;
    struct _StgUpdateFrame *link;
} StgSeqFrame;  

typedef struct {
    StgHeader  header;
    struct _StgUpdateFrame *link;
    StgInt      exceptions_blocked;
    StgClosure *handler;
} StgCatchFrame;

typedef struct {
    StgHeader  header;
} StgStopFrame;  

typedef struct {
    StgHeader   header;
    StgClosure *evacuee;
} StgEvacuated;

typedef struct {
  StgHeader header;
  StgWord data;
} StgIntCharlikeClosure;

/* statically allocated */
typedef struct {
  StgHeader  header;
} StgRetry;

typedef struct _StgForeignObj {
  StgHeader      header;
  StgAddr        data;		/* pointer to data in non-haskell-land */
} StgForeignObj;
  
typedef struct _StgStableName {
  StgHeader      header;
  StgWord        sn;
} StgStableName;

typedef struct _StgWeak {	/* Weak v */
  StgHeader header;
  StgClosure *key;
  StgClosure *value;		/* v */
  StgClosure *finalizer;
  struct _StgWeak *link;
} StgWeak;

typedef struct _StgDeadWeak {	/* Weak v */
  StgHeader header;
  struct _StgWeak *link;
} StgDeadWeak;

/* Dynamic stack frames - these have a liveness mask in the object
 * itself, rather than in the info table.  Useful for generic heap
 * check code.
 */
 
typedef struct {
  const struct _StgInfoTable* info;
  StgWord        liveness;
  StgWord        ret_addr;
  StgWord        payload[0];
} StgRetDyn;

/* Concurrent communication objects */

typedef struct {
  StgHeader       header;
  struct StgTSO_ *head;
  StgMutClosure  *mut_link;
  struct StgTSO_ *tail;
  StgClosure*     value;
} StgMVar;

#if defined(PAR) || defined(GRAN)
/*
  StgBlockingQueueElement represents the types of closures that can be 
  found on a blocking queue: StgTSO, StgRBHSave, StgBlockedFetch.
  (StgRBHSave can only appear at the end of a blocking queue).  
  Logically, this is a union type, but defining another struct with a common
  layout is easier to handle in the code (same as for StgMutClosures).
*/
typedef struct StgBlockingQueueElement_ {
  StgHeader                         header;
  struct StgBlockingQueueElement_  *link;
  StgMutClosure                    *mut_link;
  struct StgClosure_               *payload[0];
} StgBlockingQueueElement;

typedef struct StgBlockingQueue_ {
  StgHeader                 header;
  struct StgBlockingQueueElement_ *blocking_queue;
  StgMutClosure            *mut_link;
} StgBlockingQueue;

/* this closure is hanging at the end of a blocking queue in (par setup only) */
typedef struct StgRBHSave_ {
  StgHeader    header;
  StgPtr       payload[0];
} StgRBHSave;

typedef struct StgRBH_ {
  StgHeader                                header;
  struct StgBlockingQueueElement_         *blocking_queue;
  StgMutClosure                           *mut_link;
} StgRBH;

#else

typedef struct StgBlockingQueue_ {
  StgHeader          header;
  struct StgTSO_    *blocking_queue;
  StgMutClosure     *mut_link;
} StgBlockingQueue;

#endif

#if defined(PAR)
/* global indirections aka FETCH_ME closures */
typedef struct StgFetchMe_ {
  StgHeader              header;
  globalAddr            *ga;		/* type globalAddr is abstract here */
  StgMutClosure         *mut_link;
} StgFetchMe;

/* same contents as an ordinary StgBlockingQueue */
typedef struct StgFetchMeBlockingQueue_ {
  StgHeader                          header;
  struct StgBlockingQueueElement_   *blocking_queue;
  StgMutClosure                     *mut_link;
} StgFetchMeBlockingQueue;

/* entry in a blocking queue, indicating a request from a TSO on another PE */
typedef struct StgBlockedFetch_ {
  StgHeader                         header;
  struct StgBlockingQueueElement_  *link;
  StgMutClosure                    *mut_link;
  StgClosure                       *node;
  globalAddr                        ga;
} StgBlockedFetch;
#endif

#endif /* CLOSURES_H */
