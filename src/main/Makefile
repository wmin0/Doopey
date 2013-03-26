PKGFLAG=

include ../Makefile.in

BINDIR=../../bin
TARGET=$(BINDIR)/server

target: $(TARGET)

$(TARGET): $(OBJS)
	@echo "> building $(TARGET)"
	$(CXX) $(CFLAGS) -I$(EXTINCLUDE) $(OBJS) -L$(LIBDIR) $(INCLIB) -o $@
