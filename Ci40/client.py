import os
from subprocess import PIPE, Popen
import time
import socket
import time


UDP_IP = 'fe80::28e9:3285:421c:bc82' # = 0.0.0.0 u IPv4
UDP_PORT = 5005
UDP_IP_LOCALHOST = "::1"
UDP_IP_CLIENT_PORT = 5006
SCALE_LSB = 0.03125
seconds = 3
macs = ["B0:91:22:F6:D5:03","54:6C:0E:52:F9:8F"]
programName = "gatttool"

#sock = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM) # UDP
#sock.bind((UDP_IP_LOCALHOST,UDP_IP_CLIENT_PORT))
#sock.sendto(b"hola",(UDP_IP,UDP_PORT))


while True:
    temperatures = []
    
    for mac in macs:
        
        os.system("gatttool -b "+ mac +" --char-write-req -a 0x0027 -n 01")
        
        time.sleep(1);
        
        programArgs = ["-b", mac, "--char-read", "-a", "0x0024"]
        command = [programName]
        command.extend(programArgs)
        
        output = Popen(command, stdout = PIPE).stdout.read().decode()
        split = output.split(" ")
        split = split[4:6]
        split.reverse()
        temp1 = ''.join(split)
        
        rawAmbTemp = int(temp1,16)
        it = int(rawAmbTemp >> 2)
        t = float(it * SCALE_LSB)
        temperatures.append(t)
        print("Temp from "+mac+" read."+str(t))
        time.sleep(1);

print("Temperaures. "+str(temperatures[0])+" and "+str(temperatures[1]))
