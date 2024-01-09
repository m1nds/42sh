SUBDIRS = src

CLEAN_FILES=42sh-1.0.tar.gz \
					 aclocal.m4 \
					 autom4te.cache \
					 configure \
					 install-sh \
					 Makefile.in \
					 missing \
					 ar-lib \
					 compile \
					 config.log \
					 config.status \
					 configure~ \
					 configure.ac \
					 depcomp \
					 src/42sh

clean:
	$(RM) -rf $(CLEAN_FILES) $(shell find -name "Makefile.in" -o -name "Makefile" -o -name "*.o" -o -name "lib*.a")
