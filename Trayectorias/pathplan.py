# 1 Inicializar el enjambre en el espacio solución. La
# 	posición inicial de origen. La velocidad inicial es
# 	aleatoria
# 2 Begin
# 3 While Hasta llegar al punto de destino
# 	o bajo algún criterio de financiación. do
# 4 Calcular Uact y Umini
# 5 Calcular Uαe
# 6 for i = 1 hata N do
# 7 Calcular la nueva Posición de la partículas,
# 	empleando la ecuación 14 Calcular la nueva
# 	velocidad de las partículas, empleando
# 	la ecuación 15
# 8 end
# 9 end
# 10 Establecer el trayecto final
# 11 end

# Fpro_i = (- alpha + beta Norm[v_i]**2) v_i
# Fint_i = -a (r_i - R) = -(a/N) Sum[r_i - r_j, {j,1,N}]
# Fesp_i = -k_f Grad[Uesp(r_i)]
# Fest_i = NormalDist[mu=0, sigma=sqrt(D)]


# Uobj(r) = kobj dist(r,robj)**2
# Uobs(r) = kobs dist(r,robs)**-1
# Uesp(r) = Uobj(r) + Sum[Uobs_m(r) ,{m,1,M}]
# Umin = Piecewise[{Uesp(R), iUmin>=Uesp(R)},{Umin, iUmin<Uesp(R)}]

# r_i(n) = r_i(n-1) + v_i dt
# v_i(n) = v_i(n-1) + (Fpro_i + Fint_i + Fesp_i + Fest_i) dt/m_i
# alpha_e(n) = alpha_e(n-1) + dt Piecewise[{tau_c, Umin >= Uesp(R)}, {-tau_c, Umin < Uesp(R)}]

# N=10, beta=0.2, alpha0=10, a=1, dt=0.1, tau_c=1.0, tau_d=0.1, k_f=30, k_u=0.1, 0<=alpha_e<=10

# Useful json functions:
# json.load() -> Similar to read()
# json.dump() -> Similar to write()

import numpy as np
#import matplotlib.pyplot as plt
import urllib.request
import json

result = json.load(urllib.request.urlopen('http://localhost:8000'))

print(result)
print(len(result))
print([val for sublist in result for val in sublist].count('RED'))
print(result[0][2] == 'RED')


# nx = int(input("Tamaño en X de la cancha: "))
# ny = int(input("Tamaño en Y de la cancha: "))

nx = 645
ny = 645


x = np.linspace(0,nx-1, nx, dtype = int)
y = np.linspace(0,ny-1, ny, dtype = int)
xx, yy = np.meshgrid(x, y)

print("xx = ", xx, "\nyy = ", yy)

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

# Nobj = int(input("Cantidad de objetivos: "))
# Nobs = int(input("Cantidad de obstáculos: "))

Nobj = [val for sublist in result for val in sublist].count('RED')
Nobs = len(result) - Nobj

if Nobj >= 0 and Nobs >= 0:
	
    Objx = np.zeros(Nobj)
    Objy = np.zeros(Nobj)
    Obsx = np.zeros(Nobs)
    Obsy = np.zeros(Nobs)
    
    for i in range(len(result)):
        # Objx[i] = int(input("Coordenada en X del objetivo #%d: "%i)) % nx
        # Objy[i] = int(input("Coordenada en Y del objetivo #%d: "%i)) % ny
        
        if(result[i][2] == 'RED'):
            
            Objx[i] = result[i][0][0]
            Objy[i] = result[i][0][1]
            
    for i in range(Nobs):
        
        # Obsx[i] = int(input("Coordenada en X del obstáculo #%d: "%i)) % nx
        # Obsy[i] = int(input("Coordenada en Y del obstáculo #%d: "%i)) % ny
        
        if(result[i][2] != 'RED'):
            Obsx[i] = result[i][0][0]
            Obsy[i] = result[i][0][1]
            
    # rx0 = int(input("Posición inicial del vehículo en X: "))
    # ry0 = int(input("Posición inicial del vehículo en Y: "))
        
    rx0 = 0
    ry0 = 0

    vx0, vy0 = (0,0)
    
    Uobj = np.zeros([nx,ny])
    Uobs = np.zeros([nx,ny])

    for i in range(Nobj):
        
        Uobj = Uobj + k_obj * ((xx - Objx[i])**2 + (yy - Objy[i])**2)
        
    for i in range(Nobs):
        
        Uobs = Uobs + k_obs * (np.sqrt((xx - Obsx[i])**2 + (yy - Obsy[i])**2))**-1

Uesp = Uobj + Uobs

Fespy, Fespx = np.gradient(-k_f * Uesp)
Fprox = (- alpha0 + beta * (vx0**2 + vy0**2)) * vx0
Fproy = (- alpha0 + beta * (vx0**2 + vy0**2)) * vy0


# Umin = Piecewise[{Uesp(R), iUmin>=Uesp(R)},{Umin, iUmin<Uesp(R)}]

# Fint_i = -a (r_i - R) = -(a/N) Sum[r_i - r_j, {j,1,N}]
# Fest_i = NormalDist[mu=0, sigma=sqrt(D)]


# r_i(n) = r_i(n-1) + v_i dt
vx = vx0 + (Fprox + Fespx) * dt/m
vy = vy0 + (Fproy + Fespy) * dt/m
# alpha_e(n) = alpha_e(n-1) + dt Piecewise[{tau_c, Umin >= Uesp(R)}, {-tau_c, Umin < Uesp(R)}]



print(Uesp)
# plt.imshow(Uesp)
# plt.colorbar()

print(vx)
print(vy)
# plt.quiver(xx,yy,vx,vy,Uesp)
# plt.contour(xx,yy,Uesp)


# r_i(n) = r_i(n-1) + v_i dt


# z = np.sin((xx**2 + yy**2)*0.1)
# h = plt.contourf(x,y,z)
# plt.show()



# King Crimson
# Indukti
# Supertramp
# Twelve Foot Ninja
# 3 Inches of Blood

