EXTPKGS=
SRCPKGS=machine network file block config logger common
LIBPKGS=$(EXTPKGS) $(SRCPKGS)

LIBS=$(addprefix -l, $(LIBPKGS))
SRCLIBS=$(addsuffix .a, $(addprefix lib, $(SRCPKGS)))

MAIN=main

all: libs main

libs:
	@for pkg in $(SRCPKGS); do \
		echo "checking $$pkg"; \
		make -C src/$$pkg --no-print-directory PKGNAME=$$pkg; \
	done

main:
	@echo "checking $(MAIN)"
	@make -C src/$(MAIN) --no-print-directory INCLIB="$(LIBS)"

clean:
	@for pkg in $(SRCPKGS); do \
		echo "clean $$pkg"; \
		make -C src/$$pkg --no-print-directory clean; \
	done
	@echo "clean $(MAIN)";
	@make -C src/$(MAIN) --no-print-directory clean;
	@echo "removing $(SRCLIBS)"
	@cd lib; rm -f $(SRCLIBS)
	@echo "removing bin"
	@rm -f bin/*


