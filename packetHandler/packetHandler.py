import serial as s 
import datetime
from tkinter import * 


ser=s.Serial()
ser.baudrate=9600
ser.port ="/dev/ttyUSB0"
ser.timeout=10
print("#packet Handler running ")

#construction de l'IHM 

# Mesure = Tk()
# windowLabel = Label(Mesure,text="Mesures a Distance")
# graphique= PanedWindow(Mesure,orient="vertical",handlesize=4,showhandle=True,sashrelief='sunken',sashwidth=10)
# info= PanedWindow(Mesure,orient="vertical",handlesize=4,showhandle=True,sashrelief='sunken',sashwidth=4)

# lgraphique= Label(graphique, text='Graphique:', foreground="#CCF5DD", background="#13FF42")
# linfo= Label(info, text='Info:', foreground="#CCF5DD", background="#55FFFF")

# graphique.add(lgraphique,height=800, width =700, pady=0, sticky="ew")
# info.add(linfo,height=800, width =200, pady=0, sticky="ew")
# graphique.grid(sticky="w",row=0,column=0)
# info.grid(sticky="w",row=0,column=1)

# Mesure.mainloop()


# Aquisitions mesures
ser.open()
for x in range(1,10):
	x=ser.readline()
	print(x)
ser.close()