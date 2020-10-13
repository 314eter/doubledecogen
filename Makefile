CFLAGS=-O3 -flto
DEBUGFLAGS=-O0 -pedantic -DDEBUG -g
PROFILEFLAGS=-O0 -g -pg -fprofile-arcs -ftest-coverage

OBJECTS=util.o extensions.o canon.o complete.o doubledecogen.o planar_code.o

doubledecogen: $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c *.h
	$(CC) -c $(CFLAGS) $< -o $@

debug: $(OBJECTS:%.o=%.debug.o)
	$(CC) $(DEBUGFLAGS) $^ -o $@

%.debug.o: %.c *.h
	$(CC) -c $(DEBUGFLAGS) $< -o $@

profile: $(OBJECTS:%.o=%.profile.o)
	$(CC) $(PROFILEFLAGS) $^ -o $@

%.profile.o: %.c
	$(CC) -c $(PROFILEFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -f *.o doubledecogen debug profile
