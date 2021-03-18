DIR_SRC=../src_test
DIR_LIB=../lib
SRC=$(DIR_SRC)/testmain.cpp
OBJ=testmain.o
DLL=$(DIR_LIB)/libtinydicom.dll
TARGET=test
CFLAGS=-I$(DIR_LIB) -DUNICODE -D_UNICODE
LFLAGS=-mconsole $(DLL) -static -O3 -s

all: $(TARGET)

clean:
	@rm -f $(OBJ)
	@rm -f *.dll
	@rm -f *.DCM
	@rm -f $(TARGET)

$(OBJ): %.o: $(DIR_SRC)/%.cpp
	@echo "Compiling $< ..."
	@g++ $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	@echo "Linking $@ ..."
	@g++ $< $(LFLAGS) -o $@
	@cp -f $(DLL) .
