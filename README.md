Introduction: 

The application dynamically monitors a hierarchy of files and directories and when changes occur the program updates a complete copy of the hierarchy in another directory. For this purpose, the Linux inotify system call interface [1, 2] is used.

The mirr can be called as follows: 

prompt >> ./mirr source backup

Such a program helps when we are working with a series of files and want to keep them in another folder for security reasons or simply because it is convenient (like Dropbox for example). If there is no automation, changes we have made to the source directory can be forgotten with the obvious consequences for the accuracy of the copy content in the backup. Although it can create a little extra load (in the system), the most appropriate time to base a consistent copy is to have an immediate response when a modification / change is detected.

In general, the program does the following two things: 

1. Initially, when the backup does not exist, it copies the contents of the source so that both have the same information (structure and data). 
2. Next, the program monitors the source and all the underlying directories and when a change occurs, the mirr immediately perceives it and act so that the structure and content of the target are updated accordingly.

Wording of the Problem: 

Files in the Linux operating system are logged at two different levels, names and i-nodes. Each i-node is linked to the data of a single file and holds all information except the filename (eg i-node number, date of last change, size). The relationship is ambiguous, as each file is indexed by a single i-node. But an i-node can be linked to multiple file names. In general, a logical structure of the file system can be sketched with a circularly directed graph (and / or tree) as: 

• Directories (having logical names) can contain files as well as other directories in a retrospective manner. 

• There are no hard-links to directories. This helps to avoid circles in the file system.

One way to look at a file's i-node is to use ls -i (at system program level) as well as stat () (at system call level). Especially in Linux there is also the system program with the same stat name that does the same shell-level work. Linking to an existing file is done with the command: ln existing name new name


For an inotify system call, see man inotify.

Implementation:

During initial synchronization (Step 1),structures that reflect the hierarchy of names and i-nodes are created, such as the operating system file management subsystem. In particular, i-nodes included in  structure  and the names that point to them are recorded. At the end of this step there are up-to-date and synchronized structures for both the source directory and the destination directory.

So here's a table with i-nodes, with contents: 

• Last change date

• File size 

• List of names pointing here 

• Number of names pointing here
• Source specific, a pointer to the corresponding destination structure (ie its copy) 

And a tree with names, with contents ( at each node): 

• Name 

• Index to i-node

The algorithm that synchronizes the source hierarchy with the destination hierarchy is: 
1. Created the above structures for both hierarchies, deriving information for each name. Alphabetically sort the node-brothers to the name tree. 
2. Depth first search the two trees in parallel, and:

    (a) If a directory name exists at the source but not at the destination, create it also at the destination directory (possibly by first disconnecting a file name, the same name that exists in the destination but pointing to node-file rather than node-directory) 
    
    (b) If a directory name does not exist at source but exists at destination, delete it from destination 
    
    (c) If a directory name exists at source but not at source destination, go to its i-node and see if it already has a copy to destination. 
        
    • If so, connect the name to the correct i-node of the destination. 
        
    • If not, create a new file at the destination and connect the source i-node to that of the destination. 
        
    (d) If a filename does not exist in the source but exists in the destination, disconnect it
    
    (e) If a filename exists in both, compare the i-node data. If it is the same (last change date and size), leave it as is and update the structures. If not, disconnect it from the destination and do that in step 2c.
    
    
In the second step, the directories included in the source directory are monitored. 

The procedure is as follows: 
1. Creating a queue of events with inotify_init()
2. Adding the objects to be monitored with inotify_add_watch ().
3. Repeat: 

    (a) Reading the next event from the queue 
    
    (b) Depending on the type of event, update the destination directory with changes
    
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

More in depth analysis:

Implemented by Dimitris Soumis:

Initially the function a () using stat () checks if the path directory is given at the source and the destination at the source. If it exists at the source but not at the destination, it is also created at the destination.Then, insert to backup tree the path of the new directory and returning the function to its location in the tree. If there is also in destination and it is a file, it disconnects the file using unlink () and creates the directory with mkdir. First we delete the file from the backup tree, insert the path of the new directory into the backup tree, and return its location to the tree from the function. If there is in destination and it is a directory, simply return its location to the tree from the function.

Then in function b () we first check with stat () if a path does not exist at source but exists at the destination and this path is a directory and we call recursion_for_b () which deletes a directory by first deleting all its contents retrospectively. The backup tree is also updated by deleting the corresponding directory accordingly.

In addition, to the c () function we first check with stat () if a path exists in the source but does not exist in the destination and this path is a file. If the source inode destination is NULL then the name of the source file we get from the inode names list with the path to the destination becomes a link () and we add the path to the backup tree. Otherwise we create the file at the destination and copy its contents and add the path to the backup tree. Finally we return the file location to the backup tree.

Then in function d () we first check with stat () if a path does not exist at source but exists at destination and this path is a file. We unlink the file with unlink and delete the path from the backup tree.

Finally in the e () function we check with stat () if the path exists in both the source and the destination and that it is a file. If the source inode destination is the same as the destination inode, no action is needed. If not, we disconnect the file, delete the path from the backup tree, call the c () function, which returns the destination inode structure, and assign this value at the source inode destination.

Inotify.c. At each event reception we check that it is not length 0 and that it is not exactly the same as the previous event and that we execute whatever is requested in the table of events by always updating the trees (any extra information is provided in comments in source files). 

Note: event IN_DELETE_SELF has len 0. 

We pass 2 tables as arguments containing all directory paths of the source and destination in the form of strings. For each source directory we assign a watch descriptor with inotify_add_watch (). The whole event process ends as soon as the user presses Ctrl + C.

Implemented by Adam Mahjoub:

For inodes we use a simple list structure.Create_inode creates a new node, list_insert adds it, check_list to check if the inode is already in the list so it won't be added again. Rm_inode_list finds the inode and if there is only one name pointing at it, it deletes it, otherwise it simply deletes the name we gave it. In the first case if we are in backup tree inode we disconnect the corresponding Slist destination. For the inode names we have a simple list of simple deletion and push functions.

Nodes are used to implement the tree: 

(a) point to the next node and therefore in the same folder. 

b) point to directories and files (so it only makes sense to use them if the node represents a directory to point to its files and subfolders).

In inserting a node we always look for the subpath, that is, if we are at home / 4i in the tree and want to find home / 4i / a / k / pc, we search through strstrt first for home / 4i / a and after the subfolder k. When we find the folder then depending on whether it is a directory or file is added to the appropriate list of nodes in the folder. Similarly, the find and delete function works. The last one also deletes all subfolders and files if the desired node is a folder, while also calls rm_node_list. modify deletes the previous inode and modifies the previous node's data.traverse functions just cross the tree nodes, one to find all the folder names and the other to print the whole tree.

The trees are initialized to represent the folders given to the terminal using opendir, readdir and stat to open all the folders and find all the files and add them to the trees. 

Then the trees are synchronized as follows: 

If both nodes are NULL return.

If we have a NULL source node then the next backup nodes are deleted (redundant function call), since they are redundant. This is done via d(), b() and deletions are done to the file system but also to the bakup tree. 

If we have backup NULL then nodes are missing from the backup so we add with the use of appropriate functions (a(), c()) to the file system but also to the bakup tree.

If we have nowhere null and have the same name and are files then e() is called to check the inodes. If folders are nothing is deleted. 

In the above cases, the function is called recursively to the next nodes of both but also to the subfolders and files if the two nodes are folders.

In the case of non-same names, the backup node is deleted and the source node is added to it, but if we do not have the same name it means that the last node was not added to exactly the same tree location as the source so we need to check at the same backup node as next to the source and then if it's folders to look at the next ones (if we didn't, we'd notice some folders not being deleted and adding files to the wrong subfolders in certain cases) as well as their subfolders and files. 

In any case the inodes are connected, also note that in the beginning there may be the same names in the two folders but they are not connected so the backup tree is actually made from the beginning out of its folders!

Compile command : make

Clean command : make clean 

Count command : make count 

Run command  : ./mirr (source_directory_path) (backup_directory_path)
