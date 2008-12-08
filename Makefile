SUBDIRS = \
	  camera \
	  model

OBJECTS = \
	  camera/camera.o \
	  model/model.o

G_FLAGS = `pkg-config --cflags --libs libglade-2.0 gtkglext-1.0` -export-dynamic

all: findmatch modelviewer

findmatch: objects
	gcc -o findmatch main.c -lglut $(OBJECTS)

objects:
	for i in $(SUBDIRS); \
		do cd $$i; \
		make; \
		cd ..; \
	done

modelviewer: objects
	gcc -o modelviewer modelviewer.c -lglut $(G_FLAGS) $(OBJECTS) 

clean:
	for i in $(SUBDIRS); \
		do cd $$i; \
		make clean; \
		cd ..; \
	done
	rm *~ findmatch modelviewer