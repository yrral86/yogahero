MATCH_SOURCES = \
	errorfunc.c \
	nrc.c

MATCH_OBJECTS = $(MATCH_SOURCES:.c=.o)

SOURCES = \
	camera.c \
	model.c

OBJECTS = $(SOURCES:.c=.o)

TEST_SOURCES = \
	test.c

TEST_FLAGS = `pkg-config --cflags --libs gio-2.0 gtk+ glib-2.0`

GLADE_FLAGS = `pkg-config --cflags --libs libglade-2.0` -export-dynamic

GTK_GL_FLAGS = `pkg-config --cflags --libs gtkglext-1.0`

CV_FLAGS = `pkg-config --cflags opencv`

CV_LIBS = `pkg-config --libs opencv`

C_FLAGS = -g -Wall

DEPS = \
	freeglut3-dev \
	libcv-dev \
	libglade2-dev \
	libgtk2.0-dev \
	libgtkglext1-dev 

all: findmatch modelviewer test

findmatch: $(MATCH_OBJECTS) $(OBJECTS) main.c
	gcc -o findmatch main.c -lglut $(MATCH_OBJECTS) $(OBJECTS) $(CV_FLAGS) $(CV_LIBS) $(C_FLAGS) $(GTK_GL_FLAGS)

modelviewer: $(OBJECTS) modelviewer.c
	gcc -o modelviewer modelviewer.c -lglut $(GLADE_FLAGS) $(OBJECTS) $(C_FLAGS) $(GTK_GL_FLAGS)

test: test.c
	gcc -o test test.c $(TEST_FLAGS) $(C_FLAGS)

.c.o:
	gcc -c $< -o $@ $(CV_FLAGS) $(C_FLAGS)

clean:
	rm *~ findmatch modelviewer $(MATCH_OBJECTS) $(OBJECTS) pose.out pose.png

ubuntudeps:
	sudo apt-get install $(DEPS)