SOURCES = \
	camera.c \
	model.c \
	nrc.c

OBJECTS = $(SOURCES:.c=.o)

G_FLAGS = `pkg-config --cflags --libs libglade-2.0 gtkglext-1.0` -export-dynamic

C_FLAGS = `pkg-config --cflags opencv`

all: findmatch modelviewer

findmatch: $(OBJECTS)
	gcc -o findmatch main.c -lglut $(OBJECTS)

modelviewer: $(OBJECTS)
	gcc -o modelviewer modelviewer.c -lglut $(G_FLAGS) $(OBJECTS) 

.c.o:
	gcc -c $< -o $@ $(C_FLAGS)

clean:
	rm *~ findmatch modelviewer $(OBJECTS)