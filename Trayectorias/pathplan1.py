import numpy as np
import matplotlib.pyplot as plt
import urllib.request
import json
import serial

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
m = 1

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
        print('car_theta = {}'.format(car_theta))
        rx_prev = rx
        ry_prev = ry
        rx = (rx_front+rx_rear)/2
        ry = (ry_front+ry_rear)/2
        print('(rx,ry) = ({},{})'.format(rx,ry))
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
                print('(Objx,Objy) = ({},{})'.format(Objx,Objy))
                Obji = Obji +1
         
        Dobjx = Objx - rx
        Dobjy = Objy - ry

        for i in range(Nobs):
            
            # Obsx[i] = int(input("Coordenada en X del obstáculo #%d: "%i)) % nx
            # Obsy[i] = int(input("Coordenada en Y del obstáculo #%d: "%i)) % ny
            
            if(result[i][2] != 'RED' and result[i][2] !='GREEN' and result[i][2] !='YELLOW'):
                Obsx[Obsi] = result[i][0][0]*100
                Obsy[Obsi] = result[i][0][1]*100
                print('(Obsx,Obsy) = ({},{})'.format(Obsx,Obsy))

                Obsi = Obsi +1
                
        Dobsx = Obsx - rx
        Dobsy = Obsy - ry

        Uobj = np.zeros([nx,ny])
        Uobs = np.zeros([nx,ny])
    
        for i in range(Nobj):
            
            Uobj = Uobj + k_obj * ((xx - Objx[i])**2 + (yy - Objy[i])**2)
            
        for i in range(Nobs):
            
            Uobs = Uobs + k_obs * (np.sqrt((xx - Obsx[i])**2 + (yy - Obsy[i])**2))**-1
    
    Uesp = Uobj + Uobs
    
    Fespy, Fespx = np.gradient(-k_f * Uesp)
    #Fprox = (- alpha0 + beta * (vx**2 + vy**2)) * vx
    #Fproy = (- alpha0 + beta * (vx**2 + vy**2)) * vy
    Fprox = 0
    Fproy = 0
    
    
    # Umin = Piecewise[{Uesp(R), iUmin>=Uesp(R)},{Umin, iUmin<Uesp(R)}]
    
    # Fint_i = -a (r_i - R) = -(a/N) Sum[r_i - r_j, {j,1,N}]
    # Fest_i = NormalDist[mu=0, sigma=sqrt(D)]
    
    
    # r_i(n) = r_i(n-1) + v_i dt
    vx = (Fprox + Fespx) * dt/m #vx + (Fprox + Fespx) * dt/m
    vy = (Fproy + Fespy) * dt/m #vy + (Fproy + Fespy) * dt/m

    print('(vx,vy) = ({},{})'.format(vx[int(ry)][int(rx)],vy[int(ry)][int(rx)]))
    
    for i in range(nx):
        for j in range(ny):
            if vx[i][j] > 1000:
                vx[i][j] = 1000
            elif vx[i][j] < -1000:
                vx[i][j] = -1000

    for i in range(nx):
        for j in range(ny):
            if vy[i][j] > 1000:
                vy[i][j] = 1000
            elif vy[i][j] < -1000:
                vy[i][j] = -1000

    # alpha_e(n) = alpha_e(n-1) + dt Piecewise[{tau_c, Umin >= Uesp(R)}, {-tau_c, Umin < Uesp(R)}]
    
    
    
    #print(Uesp)
    # plt.imshow(Uesp)
    # plt.colorbar()
    
    #print('vx = {}'.format(vx))
    #print('vy = {}'.format(vy))
    #print('vx[int(rx)] = {}'.format(vx[int(rx)]))
    #print('vy[int(rx)] = {}'.format(vy[int(rx)]))
    #print('xx = {}'.format(xx))
    #print('yy = {}'.format(yy))



    if car_points == 2:

        v_theta = np.arctan2( vy[int(ry)][int(rx)] , vx[int(ry)][int(rx)])
        print('v_theta = {}'.format(v_theta))

        e_theta = v_theta - car_theta
    
        '''
        if e_theta > np.pi:
            e_theta = e_theta - 2*np.pi
        elif e_theta < -np.pi:
            e_theta = 2*np.pi + e_theta
        '''

        e_theta = e_theta*40/(np.pi) + 40
    
        e_theta = int(e_theta)

        e_theta = e_theta % 80

        '''
        if e_theta < 32:
            e_theta = 0
        elif e_theta > 45:
            e_theta = 80
        '''


        e_theta = int(e_theta)

        print('e_theta = {}'.format(e_theta))
    
        e_theta = e_theta.to_bytes(1, 'little', signed = False)
    
        print(e_theta)
    
        #plt.quiver(xx,yy,vx,vy,Uesp)
        #plt.contour(xx,yy,Uesp)
        
        
        # r_i(n) = r_i(n-1) + v_i dt
        
        
        # z = np.sin((xx**2 + yy**2)*0.1)
        # h = plt.contourf(x,y,z)
        #plt.show()
        Dobj = np.sqrt(Dobjx**2 + Dobjy**2) 
        print('Dobj = {}'.format(Dobj))

        if ser.isOpen() and np.all(Dobj > 15) and Nobj > 0:
            ser.write(b'\xFF')
            ser.write(e_theta)
            ser.write(b'\x00')

        #elif ser.isOpen() and np.any(Dobj) <= 15 and np.all(Dobj >= 12 ) and Nobj > 0:
            #ser.write(b'\xFF')
            #ser.write(e_theta)
            #ser.write(b'\x01')
        
        elif ser.isOpen() and np.any(Dobj < 15) and Nobj > 0:
            ser.write(b'\xFF')
            ser.write(e_theta)
            ser.write(b'\x01')

        elif ser.isOpen() and Nobj == 0:
            ser.write(b'\xFF')
            ser.write(e_theta)
            ser.write(b'\x03')
    

