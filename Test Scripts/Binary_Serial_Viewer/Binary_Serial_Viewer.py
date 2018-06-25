import serial
import struct

#AUVIC
#Application to print binary received from AUVIC boards through serial
#Received data should be in little endian

#global defines
comPortName = "COM6"

#start script
print("")
print("**Binary Serial Viewer Application**")
print("")

try:
	print("Opening serial port: " + comPortName)
	com = serial.Serial(comPortName, 9600)
except serial.SerialException as e:
	print("Could not open serial port. Exiting...")
	quit()

print("Enter command to send followed by bytes to receive. Example: TMP 2")
print("Enter 'QUIT QUIT' to quit application.")
print("")
while 1:
	userInput = raw_input("Command to send Num bytes expected: ")
	
	try:
		command, numBytes = userInput.split();
	except ValueError as e:
		print(e)
		print("Must enter two arguments. Example: TMP 2")
		print("")
		continue
	
	if command == "QUIT" and numBytes == "QUIT":
		break
	
	command = command + "\n"
	
	try:
		numBytes = int(numBytes)
	except ValueError as e:
		print(e)
		print("Second argument must be a number. Example: TMP 2")
		print("")
		continue 
	
	com.reset_input_buffer()
	com.write(command)

	while com.inWaiting() < numBytes:
		pass

	receivedInput = com.read(numBytes)
	receivedInput = "".join(format(ord(x), 'x') for x in receivedInput)
	receivedInput = int(receivedInput, 16)

	print(receivedInput)
	print("")

print("Closing serial port...")
com.close()
print("Exiting...")
