MATCH_SOURCES = \
	errorfunc.c \
	nrc.c

MATCH_OBJECTS = $(MATCH_SOURCES:.c=.o)

SOURCES = \
	camera.c \
	model.c

OBJECTS = $(SOURCES:.c=.o)

VIEWER_FLAGS = `pkg-config --cflags --libs libglade-2.0 gtkglext-1.0` -export-dynamic

CV_FLAGS = `pkg-config --cflags opencv`

CV_LIBS = `pkg-config --libs opencv`

C_FLAGS = -g

all: findmatch modelviewer

findmatch: $(MATCH_OBJECTS) $(OBJECTS) main.c
	gcc -o findmatch main.c -lglut $(MATCH_OBJECTS) $(OBJECTS) $(CV_FLAGS) $(CV_LIBS) $(C_FLAGS)

modelviewer: $(OBJECTS) modelviewer.c
	gcc -o modelviewer modelviewer.c -lglut $(VIEWER_FLAGS) $(OBJECTS) $(C_FLAGS)

.c.o:
	gcc -c $< -o $@ $(CV_FLAGS) $(C_FLAGS)

clean:
	rm *~ findmatch modelviewer $(MATCH_OBJECTS) $(OBJECTS)
