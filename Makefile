# Name of program
OBJNAME=  bricks

# Source folders
SRCFLDRS= src
INCFLDRS= header
# Compiler options
# gcc   -> Use the GNU CCompiler
# -Wall -> Show all warnings
# -g    -> Allow debugging
# -O2   -> Enable tail-recursion
# -D_BSD_SOURCE -D_DEFAULT_SOURCE -> for cfmakeraw(), usleep()

CC=       gcc
CFLAGS=   -Wall -g -O0 -std=c99 -D_BSD_SOURCE -D_DEFAULT_SOURCE

# Actual compiler options from folders
SRC=      $(foreach folder,$(SRCFLDRS),$(wildcard $(folder)/*.c))
HFILES=   $(foreach folder,$(INCFLDRS),$(wildcard $(folder)/*.h))
INC=      $(addprefix -I,$(INCFLDRS))

# Compile
.PHONY : all
all : $(OBJNAME)

$(OBJNAME) : $(HFILES) $(SRC)
	$(CC) $(CFLAGS) $(INC) -o $(OBJNAME) $(SRC) -lm



install :
	cp $(OBJNAME) "$(DESTDIR)/$(OBJNAME)"


clean:
	rm $(OBJNAME)