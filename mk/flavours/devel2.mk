SRC_HC_OPTS        = -O -H64m
GhcStage1HcOpts    = -O2
GhcStage2HcOpts    = -O0 -DDEBUG
GhcLibHcOpts       = -O -dcore-lint
BUILD_PROF_LIBS    = NO
SplitSections      = NO
HADDOCK_DOCS       = NO
BUILD_SPHINX_HTML  = NO
BUILD_SPHINX_PDF   = NO
BUILD_MAN          = NO

LAX_DEPENDENCIES   = YES
