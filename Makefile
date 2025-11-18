CPP = g++ -O2 -fopenmp
CC = gcc -O2 -fopenmp

CCFLAGS = -I. 

all: test.exe

clean:
	rm -f *.o test.exe

OBJS	= test.o

test.exe: $(OBJS)
	$(CPP) $(CCFLAGS) -o $@ $(OBJS) \
		-lpthread -ltcl86 -ltk86 \
		-lopengl32 -lglfw3 -lglew32 \
		-lwinmm -lgdi32 -lpthread \
		-lglu32 -lfreeglut \

.cpp.o:
	$(CPP) -c $(CCFLAGS) $< -o $@  

.c.o:
	$(CC) -c $(CCFLAGS) $< -o $@  
