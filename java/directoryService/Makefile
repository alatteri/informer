PROJECT = directoryService
SRC = src
BIN = bin
LIB = lib
JARS = jars
DIST = dist
RESOURCES = resources
SOURCES = $(SRC)/$(PROJECT).java

JARFILE = $(JARS)/$(PROJECT).jar
JARFILE_SIGNED = $(JARS)/$(PROJECT)_signed.jar

all : buildjar

buildjar : compile
	install -d $(DIST)
	jar cvfm $(JARFILE) $(RESOURCES)/Manifest -C $(BIN) .

compile : init
	javac -d $(BIN) -sourcepath $(SRC) -classpath $(LIB) -source "1.3" -target "1.2" $(SOURCES)

init :
	install -d $(SRC)
	install -d $(BIN)
	install -d $(JARS)

clean :
	-rm -Rf $(BIN)
	-rm -Rf $(JARS)
	-rm -Rf $(DIST)
