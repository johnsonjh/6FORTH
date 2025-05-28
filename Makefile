RM?=rm -f

6forth all: 6forth.c 6forth.h
	$(CC) $(CFLAGS) $(LDFLAGS) -DANSI_FUNC -o 6forth 6forth.c

clean:
	$(RM) a.out 6forth *.o core*
