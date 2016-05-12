.PHONY: all test clean
CFLAGS=-O2
LDFLAGS=-lnettle
LN=ln -f
MKDIR=mkdir
DIFF=diff
RMDIR=rmdir

all: sha3-224sum sha3-256sum sha3-384sum sha3-512sum

sha3-224sum: sha3-256sum
	$(LN) $< $@
sha3-384sum: sha3-256sum
	$(LN) $< $@
sha3-512sum: sha3-256sum
	$(LN) $< $@

test-actual:
	$(MKDIR) $@
test-actual/%: % test-actual
	./$< abc empty million-a > $@
	$(DIFF) test-expected/$< test-actual/$<
test: test-actual/sha3-224sum test-actual/sha3-256sum test-actual/sha3-384sum test-actual/sha3-512sum

clean:
	$(RM) sha3-224sum sha3-256sum sha3-384sum sha3-512sum test-actual/sha3-224sum test-actual/sha3-256sum test-actual/sha3-384sum test-actual/sha3-512sum
	$(RMDIR) test-actual
