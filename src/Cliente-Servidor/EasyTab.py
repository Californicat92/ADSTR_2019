#!/usr/bin/python3
# -*- coding: utf-8 -*-
import tkinter
import sqlite3
from tkinter import ttk

def sql(sentencia):
    con = sqlite3.connect("BD_GA-B.db")
    cursorObj = con.cursor()
    cursorObj.execute(sentencia)
    data = cursorObj.fetchall()
    return data

class Data(ttk.Frame):    
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        valMAX, valMED, valMIN = (70, 44, 7)
        fecha1, fecha2 = ("22/10/2016", "24/01/2020")
        sensores = [1, 2, 3]

        #Creación selector de Sensores
        var = tkinter.IntVar()
        s1 = tkinter.Radiobutton(self, text = "Sensor 1", variable = var,
                                 value = 1, command = self.buscar)
        s2 = tkinter.Radiobutton(self, text = "Sensor 2", variable = var,
                                 value = 2, command = self.buscar)
        s3 = tkinter.Radiobutton(self, text = "Sensor 3", variable = var,
                                 value = 2, command = self.buscar)

        global info
        info = tkinter.Label(self, text = "Valores estadísticos entre {} y {}"
                             .format(fecha1, fecha2),
                             anchor = "center", justify = "left"
                             )        
        maximo = tkinter.Label(self,
                           text = "Máximo:\t{}".format(valMAX),
                           anchor = "center", justify = "left"
                           )
        media = tkinter.Label(self,
                           text = "Media:\t{}".format(valMED),
                           anchor = "center", justify = "left"
                           )
        minimo = tkinter.Label(self,
                           text = "Mínimo:\t{}".format(valMIN),
                           anchor = "center", justify = "left"
                           )
        desde = tkinter.Label(self,
                              text = "Fecha desde:",
                              anchor = "center", justify = "left"
                              )
        global fechaIn
        fechaIn = tkinter.Entry(self)
        hasta = tkinter.Label(self,
                           text = "hasta:",
                           anchor = "center", justify = "left"
                           )
        global fechaFi
        fechaFi = tkinter.Entry(self)
        buscar = tkinter.Button(self, text = "Buscar",
                               anchor = "center", justify = "center",
                               overrelief="flat", command = self.buscar
                               )
        actualizar = tkinter.Button(self, text = "Actualizar",
                               anchor = "center", justify = "center",
                               overrelief="flat"
                               )
        exVen = tkinter.Button(self, text = "Salir",
                               anchor = "center", justify = "center",
                               overrelief="flat", command = self.exitVen
                               ) #If u want to pass arguments use command=lambda:

        #Posicionamiento de los widgets
        info.place(x = 20, y = 15)
        s1.place(x = 497, y = 15)
        s2.place(x = 587, y = 15)
        s3.place(x = 677, y = 15)
        maximo.place(x = 20, y = 40)
        media.place(x = 20, y = 65)
        minimo.place(x = 20, y = 90)
        desde.place(x = 20, y = 120)
        fechaIn.place(x = 20, y = 140)
        hasta.place(x = 20, y = 165)
        fechaFi.place(x = 20, y = 185)
        buscar.place(x = 20, y = 230) 
        actualizar.place(x = 20, y = 320)    
        exVen.place(x = 20, y = (size[1] - 70))

    def exitVen(*args):
        ven.destroy()

    def buscar(*args):
        fecha1, fecha2 = (fechaIn.get(), fechaFi.get())
        valMax = sql("SELECT MAX(Value_Max) FROM Resum_table WHERE (DATA_Max < fecha1 AND DATA_Max > fecha2")
        valMed = sql("SELECT AVG(Avg) FROM Resum_table WHERE (DATA_start < fecha1 AND DATA_start> fecha2")
        valMin = sql("SELECT MIN(Value_Min) FROM Resum_table WHERE (DATA_Min < fecha1 AND DATA_Min > fecha2")
        
        info.configure(text = "Valores estadísticos entre {} y {}"
                             .format(fecha1, fecha2))
        maximo.configure(text = "Máximo:\t{}".format(valMAX))
        media.configure(text = "Media:\t{}".format(valMED))
        minimo.configure(text = "Mínimo:\t{}".format(valMIN))
        

class Sensores(ttk.Frame):    
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        exVen = tkinter.Button(self, text = "Salir",
                               anchor = "center", justify = "center",
                               overrelief="flat"
                               ) #If u want to pass arguments use command=lambda:

        #Posicionamiento de los widgets  
        exVen.place(x = 20, y = (size[1] - 70))

class Informe(ttk.Frame):    
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        exVen = tkinter.Button(self, text = "Salir",
                               anchor = "center", justify = "center",
                               overrelief="flat"
                               ) #If u want to pass arguments use command=lambda:

        #Posicionamiento de los widgets  
        exVen.place(x = 20, y = (size[1] - 70))

class Alarmas(ttk.Frame):    
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        exVen = tkinter.Button(self, text = "Salir",
                               anchor = "center", justify = "center",
                               overrelief="flat"
                               ) #If u want to pass arguments use command=lambda:

        #Posicionamiento de los widgets  
        exVen.place(x = 20, y = (size[1] - 70))

################# Main window #################
ven = tkinter.Tk()
#Centrar ventana
w = ven.winfo_screenwidth() #Obtiene el ancho de la pantalla
h = ven.winfo_screenheight() #Obtiene el alto de la pantalla
size = [800, 600]
x = w/2 - size[0]/2 
y = h/2 - size[1]/2
ven.geometry("%dx%d+%d+%d" % (size[0], size[1], x, y))
ven.title("ADSTR | Cliente: Control sensores")

#Creacion de las pestañas
notebook = ttk.Notebook(ven)
notebook.pack(fill = "both", expand = 1)
data = Data(notebook)
sensores = Sensores(notebook)
informe = Informe(notebook)
alarmas = Alarmas(notebook)

#Se añaden y se les pone nombre
notebook.add(data, text = "Data")
notebook.add(sensores, text = "Sensores")
notebook.add(informe, text = "Informe")
notebook.add(alarmas, text = "Alarmas")

ven.mainloop()
