objs = authorizer.o message_handle.o 

authorizer: $(objs)
	gcc -o authorizer $(objs) -L ~/.local/lib -Wl,-rpath ~/.local/lib  -l pbc 
authorizer.o: authorizer.c  ~/.local/include/pbc
	gcc -c authorizer.c  -I ~/.local/include/pbc
message_handle.o: message_handle.c 
	gcc -c message_handle.c
.PHONY: clean
clean:
	-rm -f $(objs)
