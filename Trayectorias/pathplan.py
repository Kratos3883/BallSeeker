import numpy as np
import matplotlib.pyplot as plt
import urllib.request
import json
import serial

'''
ser = serial.Serial(
	    port='/dev/rfcomm0', #'/dev/ttyUSB0',#'/dev/ttyACM0',
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

'''

N = 10
beta = 0.2
a = 1
dt = 0.1
tau_c = 1.0
tau_d = 0.1
k_f = 30
k_u = 0.1
Umin = 0
alpha0 = 0
k_obj = 0.2
k_obs = 7
k_obs_s = 0.1
m = 1
e_theta_range = 25
pwm_r_n_base = 52
pwm_v_b_base = 57

# nx = int(input("Tamaño en X de la cancha: "))
# ny = int(input("Tamaño en Y de la cancha: "))

nx = 150
ny = 150

x = np.linspace(0,nx-1, nx, dtype = int)
y = np.linspace(0,ny-1, ny, dtype = int)
xx, yy = np.meshgrid(x, y)

#print("xx = ", xx, "\nyy = ", yy)

# rx0 = int(input("Posición inicial del vehículo en X: "))
# ry0 = int(input("Posición inicial del vehículo en Y: "))
            
rx = 0
ry = 0
rx_prev = 0
ry_prev = 0

vx, vy = (0,0)
        

while 1:
    result = json.load(urllib.request.urlopen('http://localhost:8000'))
    
    #print(result)
    #print(len(result))
    #print([val for sublist in result for val in sublist].count('RED'))
    #print(result[0][2] == 'RED')
    
    # Nobj = int(input("Cantidad de objetivos: "))
    # Nobs = int(input("Cantidad de obstáculos: "))
    

    car_points = 0

    for i in range(len(result)):
        if result[i][2] == 'GREEN':
            rx_rear = result[i][0][0]*100
            ry_rear = result[i][0][1]*100
            car_points = car_points+1

        if result[i][2] == 'YELLOW':
            rx_front = result[i][0][0]*100
            ry_front = result[i][0][1]*100
            car_points = car_points+1

    Nobj = [val for sublist in result for val in sublist].count('RED')
    Nobs = len(result) - Nobj - car_points

    if car_points == 2:
        car_theta = np.arctan2( (ry_front-ry_rear) , (rx_front-rx_rear))
        #print('car_theta = {}'.format(car_theta))
        rx_prev = rx
        ry_prev = ry
        rx = (rx_front+rx_rear)/2
        ry = (ry_front+ry_rear)/2
        #print('(rx,ry) = ({},{})'.format(rx,ry))
        vx_real = abs(rx-rx_prev)
        vy_real = abs(ry-ry_prev)

    
    if Nobj >= 0 and Nobs >= 0:
	    
        Objx = np.zeros(Nobj)
        Objy = np.zeros(Nobj)
        Obsx = np.zeros(Nobs)
        Obsy = np.zeros(Nobs)
        Obsi = 0
        Obji = 0
        
        for i in range(len(result)):
            # Objx[i] = int(input("Coordenada en X del objetivo #%d: "%i)) % nx
            # Objy[i] = int(input("Coordenada en Y del objetivo #%d: "%i)) % ny

            #print('i = {}'.format(i))
            
            if(result[i][2] == 'RED'):
                
                Objx[Obji] = result[i][0][0]*100
                Objy[Obji] = result[i][0][1]*100
                #print('(Objx,Objy) = ({},{})'.format(Objx,Objy))
                Obji = Obji +1
         
        Dobjx = Objx - rx
        Dobjy = Objy - ry

        Dobj = np.sqrt(Dobjx**2 + Dobjy**2) 

        for i in range(len(result)):
            
            # Obsx[i] = int(input("Coordenada en X del obstáculo #%d: "%i)) % nx
            # Obsy[i] = int(input("Coordenada en Y del obstáculo #%d: "%i)) % ny
            
            if(result[i][2] != 'RED' and result[i][2] !='GREEN' and result[i][2] !='YELLOW'):
                Obsx[Obsi] = result[i][0][0]*100
                Obsy[Obsi] = result[i][0][1]*100

                Obsi = Obsi +1

        print('(Obsx,Obsy) = ({},{})'.format(Obsx,Obsy))
                
        Dobsx = Obsx - rx
        Dobsy = Obsy - ry

        Dobs = np.sqrt(Dobsx**2 + Dobsy**2) 

        Uobj = np.zeros([nx,ny])
        Uobs = np.zeros([nx,ny])
        
        r_obj = 0
        
        for i in range(Nobj):

            if i == np.argmin(Dobjx**2 + Dobjy**2):
            
                r_obj = np.sqrt((xx - Objx[i])**2 + (yy - Objy[i])**2)
                Uobj = Uobj + k_obj * (r_obj)**2

        for i in range(Nobs):
            
            r_obs = np.sqrt((xx - (rx - rx_front) - Obsx[i])**2 + (yy - (ry - ry_front) - Obsy[i])**2)

            obs_z = r_obs > 20 #and r_obs > r_obj_min
            obs_z = obs_z.astype(int)

            r_obs_z = np.multiply(r_obs, obs_z)

            Uobs = Uobs + k_obs * (r_obs_z+0.02)**-1

    Uesp = Uobj + Uobs

    for i in range(10):
        for j in range(42):
            for k in range(42):
                if j - 21 > 0 and k - 21 > 0 and j + 21 < nx -1 and k + 21 < ny -1:
                    Uesp[k-21][j-21] = 0.25 * (Uesp[k-22][j-22] + Uesp[k-22][j-20] + Uesp[k-20][j-22] + Uesp[k-20][j-20])

    
    Fespy, Fespx = np.gradient(-k_f * Uesp)

    vx = Fespx * dt/m # Ventana de integracion de tamano 1
    vy = Fespy * dt/m

    #print('(vx,vy) = ({},{})'.format(vx[int(ry)][int(rx)],vy[int(ry)][int(rx)]))
    
    
    plt.quiver(xx,yy,vx,vy,Uesp)
    plt.contour(xx,yy,Uesp)
    plt.show()


    if car_points == 2:

        v_theta = np.arctan2( vy[int(ry)][int(rx)] , vx[int(ry)][int(rx)])
        #print('v_theta = {}'.format(v_theta))

        e_theta = v_theta - car_theta

        e_theta = e_theta *e_theta_range/(np.pi) + e_theta_range
    
        e_theta = e_theta % (2*e_theta_range)
        e_theta = e_theta - e_theta_range

        #if np.any( Dobs < 25):
            #e_theta = (e_theta_range/2) * ( e_theta / np.abs(e_theta))

        pwm_r_n = int(pwm_r_n_base + e_theta)
        pwm_v_b = int(pwm_v_b_base - e_theta)

        #print('e_theta = {}'.format(e_theta))
        #print('PWM = ({},{})'.format(pwm_r_n, pwm_v_b))
    
        #print('Dobj = {}'.format(Dobj))

        print('Dobs = {}'.format(Dobs))
    
        '''
        if ser.isOpen() and np.all(Dobj > 15) and Nobj > 0:
            ser.write(b'\xFF')
            #print(b'\xFF')
            ser.write(pwm_r_n.to_bytes(1, 'little', signed = False))
            #print(pwm_r_n.to_bytes(1, 'little', signed = False))
            ser.write(pwm_v_b.to_bytes(1, 'little', signed = False))
            #print(pwm_v_b.to_bytes(1, 'little', signed = False))

        elif ser.isOpen() and ( (np.any(Dobj < 15) and Nobj > 0) or Nobj == 0):
            ser.write(b'\xFF')
            #print(b'\xFF')
            ser.write((0x80).to_bytes(1, 'little', signed = False))
            #print((0x80).to_bytes(1, 'little', signed = False))
            ser.write((0x80).to_bytes(1, 'little', signed = False))
            #print((0x80).to_bytes(1, 'little', signed = False))
        '''
