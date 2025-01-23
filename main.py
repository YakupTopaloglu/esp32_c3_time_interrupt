import serial
import openpyxl
from datetime import datetime

ser = serial.Serial("COM10",baudrate=9600,timeout=1)#kart bağlantı

dosya = openpyxl.load_workbook("./excel.xlsx") #excel dosya açma
sayfa = dosya.active

current_time = datetime.now().strftime("%Y-%m-%d %H-%M-%S")#excel sayfa adı yıl ay gün saat
sayfa.title = current_time


with open("./dosya.txt","r",encoding="utf-8") as file:
    satir_sayisi = file.read()

i=int(satir_sayisi)

try:
    while True:
        data = ser.readline().decode("ascii").strip()
        
        if data:
            try:
                weatness_value = data.split()
                print(" FREKANS ",data)
                dosya.save("./excel.xlsx")
            except ValueError:
                print("unexpected value:",data)
except KeyboardInterrupt:
    print("program kapatıldı")
    with open("dosya.txt","w",encoding="utf-8") as dosya:
        dosya.write(str(i))