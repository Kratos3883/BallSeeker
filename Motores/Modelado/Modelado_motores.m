%MOTOR ROJO/NEGRO
K1=0.29;
R1=1.11;
L1=286e-6;
J1=19.3e-6;
B1=0.02;

Num1=[K1];
Den1=[L1*J1 R1*J1+B1*L1 B1*R1+K1^2];
TF_R_N= tf(Num1, Den1);
figure(1)
plot(step(TF_R_N))
title('Step response of motor r/n');
ylabel('Angular velocity, \omega (rad/s)');
xlabel('Time (s)');

%MOTOR VERDE/BLANCO

K2=0.29;
R2=1.09;
L2=339e-6;
J2=19.3e-6;
B2=0.02;

Num2=[K2];
Den2=[L2*J2 (R2*J2+L2*B2) B2*R2+K2^2];
TF_V_B= tf(Num2, Den2);
figure(2)
plot(step(TF_V_B))
title('Step response of motor v/b');
ylabel('Angular velocity, \omega (rad/s)');
xlabel('Time (s)');