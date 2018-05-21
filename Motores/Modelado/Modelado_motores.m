%MOTOR ROJO/NEGRO
K1=0.29;
R1=1.11;
L1=286e-6;
J1=19.3e-6;

Num1=[K1];
Den1=[L1*J1 R1*J1 K1^2];
TF1= tf(Num1, Den1);
figure(1)
plot(step(TF1))

%MOTOR VERDE/BLANCO

K2=0.29;
R2=1.09;
L2=339e-6;
J2=19.3e-6;

Num2=[K2];
Den2=[L2*J2 R2*J2 K2^2];
TF2= tf(Num2, Den2);
figure(2)
plot(step(TF2))