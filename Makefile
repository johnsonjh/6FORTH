6forth all: 6forth.c 6forth.h
	$(CC) $(CFLAGS) $(LDFLAGS) -DANSI_FUNC -o 6forth 6forth.c

test: 6forth test.fth
	@rm -f test.out > /dev/null 2>&1 || :
	./6forth < test.fth > test.out
	@printf '%s\n' "" 2> /dev/null || :
	cat test.out
	@printf '%s\n' "" 2> /dev/null || :
	diff test.log test.out

clean distclean:
	rm -f a.out 6forth ./*.o core*

.NOTPARALLEL:

.PHONY: test clean distclean
