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

TEST_FLAGS = `pkg-config --cflags --libs gio-2.0 gtk+-2.0 glib-2.0`

GLADE_FLAGS = `pkg-config --cflags --libs libglade-2.0` -export-dynamic

GTK_GL_FLAGS = `pkg-config --cflags --libs gtkglext-1.0`

CV_FLAGS = `pkg-config --cflags opencv`

CV_LIBS = `pkg-config --libs opencv`

#C_FLAGS = -O3
C_FLAGS = -g -Wall

DEPS = \
	freeglut3-dev \
	libcv-dev \
	libglade2-dev \
	libgtk2.0-dev \
	libgtkglext1-dev 

all: findmatch modelviewer test feedback

findmatch: $(MATCH_OBJECTS) $(OBJECTS) main.c
	gcc -o findmatch main.c -lglut $(MATCH_OBJECTS) $(OBJECTS) $(CV_FLAGS) $(CV_LIBS) $(C_FLAGS) $(GTK_GL_FLAGS)

modelviewer: $(OBJECTS) modelviewer.c
	gcc -o modelviewer modelviewer.c -lglut $(GLADE_FLAGS) $(OBJECTS) $(C_FLAGS) $(GTK_GL_FLAGS) $(CV_FLAGS)

test: test.c
	gcc -o test test.c $(TEST_FLAGS) $(C_FLAGS)

test_all: test findmatch
	for i in poses/*; \
		do for j in images/*`basename $$i`.png;\
			do ./test $$j $$i; \
		done; \
	done;

feedback: feedback.c
	gcc -o feedback feedback.c $(C_FLAGS) $(OBJECTS) $(GTK_GL_FLAGS) $(CV_FLAGS) $(CV_LIBS)

.c.o:
	gcc -c $< -o $@ $(CV_FLAGS) $(C_FLAGS)

clean:
	rm *~ findmatch modelviewer test feedback $(MATCH_OBJECTS) $(OBJECTS) match.pose match.png match.data

ubuntudeps:
	sudo apt-get install $(DEPS)