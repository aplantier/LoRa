import serial as s 
ser=s.Serial()
ser.baudrate=9600
ser.port ="/dev/ttyUSB0"
ser.timeout=10
print("Hello World")
ser.open()
for x in range(1,10):
	x=ser.readline()
	print(x)
ser.close()