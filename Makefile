SUBDIRS = \
	  model

OBJECTS = \
	  model/model.o

all:
	for i in $(SUBDIRS); \
		do cd $$i; \
		make; \
		cd ..; \
	done
	gcc -o findmatch main.c -lglut $(OBJECTS)

clean:
	for i in $(SUBDIRS); \
		do cd $$i; \
		make clean; \
		cd ..; \
	done
	rm *~ findmatch