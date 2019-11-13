CC=gcc
RM=rm -f

SOURCES=$(wildcard *.c)
DEPENDENCIES=$(filter-out $(TESTS),$(SOURCES))
HEADERS=$(wildcard *.h)
BASENAMES=$(basename $(SOURCES)) 
OBJECTS=$(addsuffix .o , $(BASENAMES))
TESTS= 
MAIN?=tunnel46d.c
EXEC= $(basename $(MAIN)) 

#compile toutes les sources et créer l'executable
all: $(EXEC)
#Permet de compiler un seul fichier
%.o: %.c
	@echo "\033[1;33m=== COMPILING $*.c INTO $@ ===\033[0m"
	$(CC) $(CFLAGS) -c $*.c
	@echo
#Créer l'exécutable
$(EXEC): $(DEPENDENCIES) $(MAIN)
	@echo "\033[1;32m=== LINKING OBJECTS INTO $(EXEC) ===\033[0m"
	$(CC) $(CFLAGS) $^ -o $@

clean:
	@echo "\033[1;31m=== CLEANING OBJECTS ===\033[0m"
	$(RM) $(OBJECTS) $(basename $(TESTS))

display:
	@echo "SOURCES:" $(SOURCES)
	@echo "HEADERS:" $(HEADERS)
	@echo "BASENAMES:" $(BASENAMES)
	@echo "OBJECTS:" $(OBJECTS)
	@echo "EXEC:" $(EXEC)

depend: $(SOURCES) $(HEADERS)
	@echo "\033[1;34m=== UPDATING DEPENDENCIES ===\033[0m"
	gcc $(CFLAGS) -MM $(SOURCES)
