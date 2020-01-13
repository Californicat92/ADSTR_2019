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

        #Creación selector de Sensores
        global sensorVar
        sensorVar = tkinter.IntVar()
        #nomSens = sql("SELECT Descrip_sensor FROM Sensor_table")
        nomSens = ["Tensión", "Intensidad", "Temperatura"]
        s1 = tkinter.Radiobutton(self, text = ("{}".format(nomSens[0])), variable = sensorVar,
                                 value = 1, command = self.buscar)
        s2 = tkinter.Radiobutton(self, text = ("{}".format(nomSens[1])), variable = sensorVar,
                                 value = 2, command = self.buscar)
        s3 = tkinter.Radiobutton(self, text = ("{}".format(nomSens[2])), variable = sensorVar,
                                 value = 3, command = self.buscar)

        #Creación de los widgets
        global info
        info = tkinter.Label(self, anchor = "center", justify = "left"
                             )
        global maximo
        maximo = tkinter.Label(self, anchor = "center", justify = "left"
                           )
        global media
        media = tkinter.Label(self, anchor = "center", justify = "left"
                           )
        global minimo
        minimo = tkinter.Label(self, anchor = "center", justify = "left"
                           )
        global desde
        desde = tkinter.Label(self,
                              text = "Fecha desde:",
                              anchor = "center", justify = "left"
                              )
        global fechaIn
        fechaIn = tkinter.Entry(self)
        global hasta
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
        s1.place(x = 467, y = 15)
        s2.place(x = 567, y = 15)
        s3.place(x = 667, y = 15)
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

        #Setear la selección de sensor al primero
        sensorVar.set(1) #No vaaaaaaaaaaaaaaa T.T

    def exitVen(*args):
        ven.destroy()

    def buscar(*args):
        fecha1, fecha2 = (fechaIn.get(), fechaFi.get())
        if (sensorVar.get() == 1):
            valMAX, valMED, valMIN = ("15.1 V", "13.4 V", "12.7 v")
        elif (sensorVar.get()  == 2):
            valMAX, valMED, valMIN = ("2.51 A", "2.17 A", "1.4 A")
        elif (sensorVar.get()  == 3):
            valMAX, valMED, valMIN = ("16.4ºC", "12.6ºC", "2.3ºC")

        if ((fecha1 == "") and (fecha2 == "")):
            fecha1, fecha2 = ("31/12/2018", "24/01/2020")
            pass
        
        """
        valMax = sql("ELECT MAX(Value_Max) FROM Resum_table WHERE ID = {} AND Date_Start >= {} AND Date_Start <= {}"
        .format(sensorVar.get(), fecha1, fecha2))
        valMed = sql("SELECT MIN(Value_Min) FROM Resum_table WHERE ID = {} AND Date_Start >= {} AND Date_Start <= {}"
        .format(sensorVar.get(), fecha1, fecha2))
        valMin = sql("ELECT AVG(Avg) FROM Resum_table WHERE ID = {} AND Date_Start >= {} AND Date_Start <= {}"
        .format(sensorVar.get(), fecha1, fecha2))
        """
        info.configure(text = "Valores estadísticos entre {} y {}"
                             .format(fecha1, fecha2))

        maximo.configure(text = "Máximo:\t{}".format(valMAX))
        media.configure(text = "Media:\t{}".format(valMED))
        minimo.configure(text = "Mínimo:\t{}".format(valMIN))

class Sensores(ttk.Frame):    
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        idxLB = tkinter.Label(self,
                           text = "IDX",
                           anchor = "center", justify = "left"
                           )
        xSegLB = tkinter.Label(self,
                           text = "Xseg",
                           anchor = "center", justify = "left"
                           )
        yMinLB = tkinter.Label(self,
                           text = "Ymin",
                           anchor = "center", justify = "left"
                           )
        ipLB = tkinter.Label(self,
                           text = "IP",
                           anchor = "center", justify = "left"
                           )
        modify = tkinter.Button(self, text = "Modificar",
                               anchor = "center", justify = "center",
                               overrelief="flat", command = self.modify
                               )
        add = tkinter.Button(self, text = "Añadir",
                               anchor = "center", justify = "center",
                               overrelief="flat", command = self.add
                               )
        global idx
        idx = tkinter.Entry(self)
        global xSeg
        xSeg = tkinter.Entry(self)
        global yMin
        yMin = tkinter.Entry(self)
        global ip
        ip = tkinter.Entry(self)
        global textSens
        textSens = tkinter.Text(self, height = 27, width = 94)
        exVen = tkinter.Button(self, text = "Salir",
                               anchor = "center", justify = "center",
                               overrelief="flat", command = self.exitVen
                               ) #If u want to pass arguments use command=lambda:

        #Posicionamiento de los widgets
        idxLB.place(x = 22, y = 10)
        xSegLB.place(x = 217, y = 10)
        yMinLB.place(x = 417, y = 10)
        ipLB.place(x = 614, y = 10)
        idx.place(x = 20, y = 30)
        xSeg.place(x = 215, y = 30)
        yMin.place(x = 415, y = 30)
        ip.place(x = 612, y = 30)
        textSens.place(x = 20, y = 60)
        modify.place(x = 70, y = (size[1] - 70))
        add.place(x = 200, y = (size[1] - 70))
        exVen.place(x = (size[0] - 70), y = (size[1] - 70))

        self.leer()

    def modify(*args):
        """
        sql("UPDATE Config_table SET Xseg = {} Ymin = {} IP = {} WHERE IDX = {}"
        .format(xSeg, yMin, ip, idx)
        """

    def add(*args):
        """
        sql("INSERT INTO Config_table(IDX, Xseg, Ymin, IP VALUES ({}, {}, {}, {})"
        .format(idx, xSeg, yMin, ip)
        """
    def leer(*args):
        #Contenido de la caja de texto --> Sensores
        mySens = [["1", "5", "10", "192.168.11.104"], [1, 5, 10, "192.168.11.107"]]
        #mySens = sql("SELECT * FROM Config_table")
        for x in range(len(mySens)):
            mySens[x].append("\n")
            textSens.insert(tkinter.END, mySens[x])

    def exitVen(*args):
        ven.destroy()

class Informe(ttk.Frame):    
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        tiempo = "12"

        tguardado_Infor = tkinter.Label(self,
                           text = "{}".format(tiempo),
                           anchor = "center", justify = "left"
                           )
        global t_Infor
        t_Infor = tkinter.Entry(self)
        actualizar = tkinter.Button(self, text = "Actualizar",
                               anchor = "center", justify = "center",
                               overrelief="flat", command = self.actualizar
                               )
        exVen = tkinter.Button(self, text = "Salir",
                               anchor = "center", justify = "center",
                               overrelief="flat", command = self.exitVen
                               ) #If u want to pass arguments use command=lambda:

        #Posicionamiento de los widgets
        tguardado_Infor.place(x = 320, y = 230)
        t_Infor.place(x = 320, y = 270)
        actualizar.place(x = 600, y = (size[1] - 70))
        exVen.place(x = (size[0] - 70), y = (size[1] - 70))

    def actualizar(*args):
        pass

    def exitVen(*args):
        ven.destroy()

class Alarmas(ttk.Frame):    
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        global textAlarms
        textAlarms = tkinter.Text(self, height = 30, width = 94)
        actualizar = tkinter.Button(self, text = "Actualizar",
                               anchor = "center", justify = "center",
                               overrelief="flat", command = self.actualizar
                               )
        exVen = tkinter.Button(self, text = "Salir",
                               anchor = "center", justify = "center",
                               overrelief="flat", command = self.exitVen
                               ) #If u want to pass arguments use command=lambda:

        #Posicionamiento de los widgets
        textAlarms.place(x = 20, y = 20)
        actualizar.place(x = 600, y = (size[1] - 70))
        exVen.place(x = (size[0] - 70), y = (size[1] - 70))

        self.actualizar()

    def actualizar(*args):
        """
        myAlarms = sql("SELECT * FROM Alarms_table"
        .format(xSeg, yMin, ip, idx)
        """
        #Contenido de la caja de texto --> Sensores
        myAlarms = [["Ha superado la Tension máxima", "\n\t20:00h 24/01/2020"],
                    ["Ha superado la Intensidad máxima", "\n\t10:05h 24/01/2020"]]
        #mySens = sql("SELECT * FROM Config_table")
        for x in range(len(myAlarms)):
            myAlarms[x].append("\n")
            textAlarms.insert(tkinter.END, myAlarms[x])

    def exitVen(*args):
        ven.destroy()

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
