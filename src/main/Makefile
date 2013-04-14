PKGFLAG=

include ../Makefile.in

BINDIR=../../bin
TARGET=Server Client

LIBDIR=../../lib
DEPLIBS=$(addprefix $(LIBDIR)/, $(SRCLIBS))

target: $(TARGET)

%: %Main.o $(DEPLIBS)
	@echo "> building $@"
	@$(CXX) $(CFLAGS) -I$(EXTINCLUDE) $< -L$(LIBDIR) $(INCLIB) -o $(BINDIR)/$@
