SOURCES = \
	  model.c

OBJECTS = $(SOURCES:.c=.o)

all: $(OBJECTS)

.c.o:
	gcc -c $< -o $@ -I../ -lglut

clean:
	rm *~ $(OBJECTS)