#
# In order to execute this "Makefile" just type "make"
#

OBJS	= initialize.o sync.o inode.o List.o operations_in_tree.o Tree.o copy_function.o inotify.o mirr.o
SOURCE  = initialize.c sync.c inode.c List.c operations_in_tree.c sync.c Tree.c copy_function.c inotify.c mirr.c
HEADER	= initialize.h sync.h  inode.h List.h operations_in_tree.h Tree.h copy_function.h inotify.h 
OUT	= mirr

CC      = gcc
FLAGS   = -g -c
# -g option enables debugging mode
# -c flag generates object code for separate files

all:	$(OBJS)
		$(CC) -g $(OBJS) -o $(OUT)

# create/compile the individual files >>separately<<
initialize.o: initialize.c
	$(CC) $(FLAGS) initialize.c

inode.o: inode.c
	$(CC) $(FLAGS) inode.c

List.o: List.c
	$(CC) $(FLAGS) List.c

operations_in_tree.o: operations_in_tree.c
	$(CC) $(FLAGS) operations_in_tree.c


sync.o: sync.c
	$(CC) $(FLAGS) sync.c

Tree.o: Tree.c
	$(CC) $(FLAGS) Tree.c

copy_function.o: copy_function.c
	$(CC) $(FLAGS) copy_function.c
inotify.o: inotify.c
	$(CC) $(FLAGS) inotify.c
mirr.o: mirr.c
	$(CC) $(FLAGS) mirr.c

# clean house
clean:
	rm -f $(OBJS) $(OUT)
# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)

 
