%MOTOR ROJO/NEGRO
K1=0.32;
R1=1.9;
L1=300e-6;
J1=19.3e-6;
B1=0.1;

Num1=[K1];
Den1=[L1*J1 R1*J1+B1*L1 B1*R1+K1^2];
TF_R_N= tf(Num1, Den1);
figure(1)
step(TF_R_N, stepDataOptions('StepAmplitude',1.1))
title('Step response of motor r/n');
ylabel('Angular velocity, \omega (rad/s)');
xlabel('Time (s)');

%MOTOR VERDE/BLANCO

K2=0.35;
R2=2.25;
L2=300e-6;
J2=19.3e-6;
B2=0.1;

Num2=[K2];
Den2=[L2*J2 (R2*J2+L2*B2) B2*R2+K2^2];
TF_V_B= tf(Num2, Den2);
figure(2)
step(TF_V_B, stepDataOptions('StepAmplitude',1.1))
title('Step response of motor v/b');
ylabel('Angular velocity, \omega (rad/s)');
xlabel('Time (s)');