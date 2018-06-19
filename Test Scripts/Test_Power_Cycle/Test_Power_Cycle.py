#Test to run power cycles on the Power Board 1.0.
#The power board occasionally broke due to an issue with one of the transistors
#This test is to validate the fix

import serial
import time


#start def

#voltage 0.000 to 32.000
def set_voltage(voltage_to_set, channel, verify = True):
	
	voltage_to_set = str(voltage_to_set)
	string_to_send = "VSET" + str(channel) + ":" + str(voltage_to_set) + "\n"
	power_suppply_port.write(string_to_send)
	
	time.sleep(0.01)
	
	#write code to verify that it was set
	
	
	
	
def set_current_limit(current_limit, channel, verify = True):
	current_limit = str(current_limit)
	
	string_to_send = "ISET" + str(channel) + ":" + str(current_limit) + "\n"
	power_suppply_port.write(string_to_send)
	
	time.sleep(0.01)
	
	#write code to verify that it was set
	
	
def supply_output_on_off(output_state, verify = True):
	
	string_to_send = "OUT" + str(output_state) + "\n"
	power_suppply_port.write(string_to_send)
	
	time.sleep(0.01)
	
	#write code to verify that it was set
	
def is_power_board_connected():
	
	power_board_port.write("RID\n")
	time.sleep(0.1)
	receivedString = power_board_port.read(power_board_port.inWaiting())

	print(receivedString)
	if receivedString == "Power_Board\r\n":
		return True
	else:
		return False
	
#end def

#start script


power_suppply_port = serial.Serial("COM7", 9600)
power_board_port = serial.Serial("COM6", 9600)

#init
set_voltage(20, 1)
set_current_limit(0.200, 1)

sucess_count = 0
error_count = 0

#start test
while 1:
	
	supply_output_on_off(1)
	time.sleep(10)
	power_board_connected = is_power_board_connected()
	
	supply_output_on_off(0)
	
	if power_board_connected:
		sucess_count = sucess_count + 1
	else:
		error_count = error_count + 1
	
	print("SUCESS COUNT:" + str(sucess_count) + "  ERROR COUNT:" + str(error_count))

	time.sleep(10)

#end script