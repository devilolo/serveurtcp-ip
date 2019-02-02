ERROR = -W -Wall
GCC=gcc -c
H=headers/
S=sources/
B=bin/
CFLAG=-Iheaders

all:$(B)client $(B)serveur


client:
	./bin/client 11200 localhost

serveur:
	./bin/serveur 11200

clear:
	rm $(S)*.o

doxy:
	firefox doc/Doxygen/html/index.html

$(B)client:$(S)mainClient.o $(S)processusClient.o $(S)Write.o
	gcc -o $(B)client $(S)mainClient.o $(S)processusClient.o $(S)Write.o

$(S)mainClient.o:$(S)mainClient.c $(H)mainClient.h	
	$(GCC)  $(S)mainClient.c $(CFLAG) -o $(S)mainClient.o $(ERROR)

$(S)processusClient.o: $(S)processusClient.c $(H)processusClient.h $(H)struct.h $(H)Write.h
	$(GCC) $(S)processusClient.c $(CFLAG) -o $(S)processusClient.o $(ERROR)
 
	
$(B)serveur:$(S)mainServeur.o $(S)processusServeur.o $(S)Write.o
	gcc -o $(B)serveur $(S)mainServeur.o $(S)processusServeur.o $(S)Write.o


$(S)mainServeur.o:$(S)mainServeur.c $(H)mainServeur.h $(H)processusServeur.h	
	$(GCC) $(S)mainServeur.c $(CFLAG) -o $(S)mainServeur.o $(ERROR)

$(S)processusServeur.o: $(S)processusServeur.c $(H)processusServeur.h $(H)Write.h
	$(GCC) -c $(S)processusServeur.c $(CFLAG) -o $(S)processusServeur.o $(ERROR)

$(S)Write.o: $(S)Write.c $(H)Write.h
	$(GCC) $(S)Write.c $(CFLAG) -o $(S)Write.o $(ERROR)


