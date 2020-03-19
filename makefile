#
# this makefile will compile and and all source
# found in the ~/src directory and place the object files
# into the ~/obj directory.
#
# After all is compiled, the linker is invoked to create the final
# executable (which will be stored in the ~/bin directory


# 
# ============================================= 
#  Instructions to build final encodeInput 
#  application – call the compiler, but 
#  instead of compiling, use it as a linker 
# ============================================= 
./bin/encodeInput : ./obj/encodeInput.o ./obj/encode.o ./obj/printMessage.o 
	 	cc ./obj/encodeInput.o ./obj/encode.o ./obj/printMessage.o -o ./bin/encodeInput 
 
# 
# ========================================= 
#  Instructions for compiling the  
#  dependencies that the final encodeInput 
#  binary depends on 
# ========================================= 
./obj/encodeInput.o : ./src/encodeInput.c ./inc/encodeInput.h
	 	cc -c ./src/encodeInput.c -c -o ./obj/encodeInput.o

./obj/encode.o : ./src/encode.c ./inc/encode.h
	 	cc -c ./src/encode.c -c -o ./obj/encode.o

./obj/printMessage.o : ./src/printMessage.c ./inc/printMessage.h
	 	cc -c ./src/printMessage.c -c -o ./obj/printMessage.o

# 
# ========================================= 
#  Example of a useful “extra” command 
#  that you can place in your makefile 
#  to help you recompile your entire 
#  project from scratch – the “clean” 
#  command.  You run it by typing 
#  make clean at the command line. 
# ========================================= 
 clean: 
	 	rm -f ./bin/encodeInput
	 	rm -f ./obj/*.o 
	 	rm -f ./inc/*.h~
	 	rm -f ./src/*.c~
