# This is a template makefile, generated to bind all the objects for HW#2. 
# Use this template to replace the placeholder text with the name of your specific program.
# Please note that you can either choose to use the example given below as it is, or make 
# a custom makefile by modifying the code.
# Here is an example of how the makefile should look -
##################### Start Example ############################
# CC = gcc
# ARGS = -Wall

# all: PingClient

# PingClient: PingClient.c
# 	$(CC) $(ARGS) -o PingClient PingClient.c
# clean:
#     rm -f *.o PingClient *~

PingClient: PingClient.c
	gcc -o PingClient PingClient.c