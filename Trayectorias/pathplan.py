import numpy as np
import matplotlib.pyplot as plt
import urllib.request
import json
import serial


# Configurar comunicación por Bluetooth
ser = serial.Serial(
	    port='/dev/rfcomm0',
	    baudrate=9600,
	    parity=serial.PARITY_NONE,
	    stopbits=serial.STOPBITS_ONE,
	    bytesize=serial.EIGHTBITS
	)

try:
    if ser.isOpen():
        ser.close()
    ser.open()
except Exception:
    print('Error abriendo el puerto')



# Constantes y valores iniciales
dt = 0.1
k_f = 30
k_obj = 0.2
k_obs = 2500
m = 1
e_theta_range = 40
pwm_r_n_base = 40
pwm_v_b_base = 40
caught_flag = False
get_away_flag = False

nx = 150
ny = 150

x = np.linspace(0,nx-1, nx, dtype = int)
y = np.linspace(0,ny-1, ny, dtype = int)
xx, yy = np.meshgrid(x, y)
            
rx = 0
ry = 0
rx_rear = 0
ry_rear = 0
rx_front = 0
ry_front = 0
rx_prev = 0
ry_prev = 0

vx, vy = (0,0)
        

while 1:
    # Leer el servidor
    result = json.load(urllib.request.urlopen('http://localhost:8000'))

    car_points = 0
    
    # Ubicar carro
    for i in range(len(result)):
        if result[i][2] == 'GREEN':
            rx_rear = result[i][0][0]*100
            ry_rear = result[i][0][1]*100
            car_points = car_points+1

        if result[i][2] == 'YELLOW':
            rx_front = result[i][0][0]*100
            ry_front = result[i][0][1]*100
            car_points = car_points+1

    # Cantidad de obstáculos y objetivos
    Nobj = [val for sublist in result for val in sublist].count('RED')
    Nobs = len(result) - Nobj - car_points

    # Si se ubicó el carro, calcular centroide, orientación y velocidad
    if car_points == 2:
        car_theta = np.arctan2( (ry_front-ry_rear) , (rx_front-rx_rear))
        rx_prev = rx
        ry_prev = ry
        rx = (rx_front+rx_rear)/2
        ry = (ry_front+ry_rear)/2
        vx_real = abs(rx-rx_prev)
        vy_real = abs(ry-ry_prev)

    
    if Nobj >= 0 and Nobs >= 0:
	    
        Objx = np.zeros(Nobj)
        Objy = np.zeros(Nobj)
        Obsx = np.zeros(Nobs)
        Obsy = np.zeros(Nobs)
        Obsi = 0
        Obji = 0

        # Extraer posición de los objetivos
        # Se ignoran los que estén en la meta
        for i in range(len(result)):
            
            if result[i][2] == 'RED' and result[i][0][0]*100 > 15:
                
                Objx[Obji] = result[i][0][0]*100
                Objy[Obji] = result[i][0][1]*100
                Obji = Obji +1
         
            if result[i][2] == 'RED' and result[i][0][0]*100 <= 15:

                Nobj = Nobj -1

        # Distancia entre los obstáculos y el carro
        Dobjx = Objx - rx
        Dobjy = Objy - ry

        Dobj = np.sqrt(Dobjx**2 + Dobjy**2) 

        # Extraer posición de los obstáculos
        for i in range(len(result)):
            
            if(result[i][2] != 'RED' and result[i][2] !='GREEN' and result[i][2] !='YELLOW'):
                Obsx[Obsi] = result[i][0][0]*100
                Obsy[Obsi] = result[i][0][1]*100

                Obsi = Obsi +1

        # Distancia entre el carro y los obstáculos
        Dobsx = Obsx - rx
        Dobsy = Obsy - ry

        Dobs = np.sqrt(Dobsx**2 + Dobsy**2) 

        Uobj = np.zeros([nx,ny])
        Uobs = np.zeros([nx,ny])
        
        # Calcular potencial debido al objetivo más cercano
        for i in range(Nobj):

            if i == np.argmin(Dobjx**2 + Dobjy**2):
            
                r_obj = np.sqrt((xx - Objx[i])**2 + (yy - Objy[i])**2)
                Uobj = Uobj + k_obj * (r_obj)**2

        # Calcular potencial debido a los obstáculos
        for i in range(Nobs):
            
            r_obs = np.sqrt((xx - Obsx[i])**2 + (yy - Obsy[i])**2)
            Uobs = Uobs + k_obs * (r_obs)**-1

    # Potencial total
    Uesp = Uobj + Uobs

    # Campo de fuerzas
    Fespy, Fespx = np.gradient(-k_f * Uesp)

    # Campo de velocidades
    vx = Fespx * dt/m # Ventana de integracion de tamaño 1
    vy = Fespy * dt/m

    #plt.quiver(xx,yy,vx,vy,Uesp)
    #plt.contour(xx,yy,Uesp)
    #plt.show()

    # Control angular
    if car_points == 2 and (not get_away_flag):

        # Si ya se capturó la pelota, orientarse hacia la meta
        # Si no, orientarse hacia la pelota
        if caught_flag:
            v_theta = np.pi
        else:
            v_theta = np.arctan2( vy[int(ry)][int(rx)] , vx[int(ry)][int(rx)])

        e_theta = v_theta - car_theta

        e_theta = e_theta *e_theta_range/(np.pi) + e_theta_range
    
        e_theta = e_theta % (2*e_theta_range)
        e_theta = e_theta - e_theta_range

        # PWM de cada motor
        pwm_r_n = int(pwm_r_n_base + e_theta)
        pwm_v_b = int(pwm_v_b_base - e_theta)

        # Si hay pelotas por buscar atraparla con las pinzas
        if ser.isOpen() and ( np.all(Dobj > 15) or np.abs(e_theta) > e_theta_range/4) and Nobj > 0 and (not caught_flag):
            ser.write(b'\xFF')
            ser.write(pwm_r_n.to_bytes(1, 'little', signed = False))
            ser.write(pwm_v_b.to_bytes(1, 'little', signed = False))

        # Detenerse si no hay pelotas por buscar o si se acaba de atrapar una
        elif ser.isOpen() and ( (np.any(Dobj < 15) and Nobj > 0) or Nobj == 0) and (not caught_flag):
            ser.write(b'\xFF')
            ser.write((0x80).to_bytes(1, 'little', signed = False))
            ser.write((0x80).to_bytes(1, 'little', signed = False))
            
            if np.any(Dobj < 15) and Nobj > 0:
                caught_flag = True

        # Llevar la pelota capturada a la meta
        elif ser.isOpen() and caught_flag:
            ser.write(b'\xFF')
            ser.write(pwm_r_n.to_bytes(1, 'little', signed = False))
            ser.write(pwm_v_b.to_bytes(1, 'little', signed = False))

            if np.any(Objx < 15):
                caught_flag = False
                get_away_flag = True
        
    # Si no se encontró el carro, girar con la esperanza de que vuelva a la cancha
    elif car_points != 2 and (not get_away_flag):

        pwm_r_n = int(pwm_r_n_base + 10)
        pwm_v_b = int(pwm_v_b_base - 10)

        ser.write(b'\xFF')
        ser.write(pwm_r_n.to_bytes(1, 'little', signed = False))
        ser.write(pwm_v_b.to_bytes(1, 'little', signed = False))

    # Retirarse de la meta después de dejar una pelota en ella
    elif get_away_flag:

        pwm_r_n = int(1.5*pwm_r_n_base + 0x80)
        pwm_v_b = int(1.5*pwm_v_b_base + 0x80)

        ser.write(b'\xFF')
        ser.write(pwm_r_n.to_bytes(1, 'little', signed = False))
        ser.write(pwm_v_b.to_bytes(1, 'little', signed = False))

        if rx_front > 30:
            get_away_flag = False

