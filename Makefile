6forth all: 6forth.c 6forth.h
	$(CC) $(CFLAGS) $(LDFLAGS) -DANSI_FUNC -o 6forth 6forth.c

test: 6forth test.fth
	./6forth < test.fth

clean distclean:
	rm -f a.out 6forth ./*.o core*

.NOTPARALLEL:

.PHONY: test clean distclean
