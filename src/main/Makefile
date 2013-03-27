PKGFLAG=

include ../Makefile.in

BINDIR=../../bin
TARGET=$(BINDIR)/server

LIBDIR=../../lib
DEPLIBS=$(addprefix $(LIBDIR)/, $(SRCLIBS))

target: $(TARGET)

$(TARGET): $(OBJS) $(DEPLIBS)
	@echo "> building $(TARGET)"
	@$(CXX) $(CFLAGS) -I$(EXTINCLUDE) $(OBJS) -L$(LIBDIR) $(INCLIB) -o $@
