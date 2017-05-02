obj = server.o extention.o subscribe.o publish.o global.o db.o get.o

.PHONY : all
all : server $(obj)

server : $(obj)
	cc -o server $(obj) -pthread -lsqlite3
server.o : server.c
	cc -c server.c
extention.o : extention.c
	cc -c extention.c
subscribe.o : subscribe.c
	cc -c subscribe.c
publish.o : publish.c
	cc -c publish.c
global.o : global.c
	cc -c global.c
db.o : db.c
	cc -c db.c
get.o : get.c
	cc -c get.c
.PHONY:clean
clean:
	rm -rf $(obj)
