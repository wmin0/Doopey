EXTPKGS=
SRCPKGS=machine network file block config logger common
TESTPKGS=logger
LIBPKGS=$(EXTPKGS) $(SRCPKGS)

LIBS=$(addprefix -l, $(LIBPKGS))
SRCLIBS=$(addsuffix .a, $(addprefix lib, $(SRCPKGS)))

MAIN=main

all: libs main
test: libs utest runtest

libs:
	@for pkg in $(SRCPKGS); do \
		echo "checking $$pkg"; \
		make -C src/$$pkg --no-print-directory PKGNAME=$$pkg; \
	done

main:
	@echo "checking $(MAIN)"
	@make -C src/$(MAIN) --no-print-directory INCLIB="$(LIBS)"

utest:
	@for pkg in $(TESTPKGS); do \
		echo "checking test $$pkg"; \
		make -C test/$$pkg --no-print-directory INCLIB="$(LIBS)"; \
	done

runtest:
	@for pkg in $(TESTPKGS); do \
		echo "run $$pkg"; \
		make -C test/$$pkg --no-print-directory run; \
	done

clean:
	@for pkg in $(SRCPKGS); do \
		echo "clean $$pkg"; \
		make -C src/$$pkg --no-print-directory clean; \
	done
	@for pkg in $(TESTPKGS); do \
		echo "clean test $$pkg"; \
		make -C test/$$pkg --no-print-directory clean; \
	done
	@echo "clean $(MAIN)";
	@make -C src/$(MAIN) --no-print-directory clean;
	@echo "removing $(SRCLIBS)"
	@cd lib; rm -f $(SRCLIBS)
	@echo "removing bin"
	@rm -f bin/*
