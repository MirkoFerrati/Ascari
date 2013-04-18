//written by Alessandro Settimi
#include "task_assignment.h"
#include <agent.h>


double task_assignment::dubins_dist(double Ax,double Ay, double theta, double Tx, double Ty, double r)
{
	//calcolo le distanze di dubins dal target e ritorno la direzione per la
	//velocità angolare da dare per andare sulla traiettoria più corta
	
	double pi=M_PI;

	double ct_pi2=cos(theta-pi/2);
	double st_pi2=sin(theta-pi/2);

	double ct_PI2=cos(theta+pi/2);
	double st_PI2=sin(theta+pi/2);

	double C1x=ct_pi2*(Ax+r)-st_pi2*Ay-ct_pi2*Ax+st_pi2*Ay+Ax;
	double C1y=st_pi2*(Ax+r)+ct_pi2*Ay-st_pi2*Ax-ct_pi2*Ay+Ay;

	double C2x=ct_PI2*(Ax+r)-st_PI2*Ay-ct_PI2*Ax+st_PI2*Ay+Ax;
	double C2y=st_PI2*(Ax+r)+ct_PI2*Ay-st_PI2*Ax-ct_PI2*Ay+Ay;



	double a1=C1x*C1x+Tx*Tx-2*C1x*Tx-r*r;
	double b1=-2*C1x*C1y+2*C1x*Ty+2*Tx*C1y-2*Tx*Ty;
	double c1=C1y*C1y+Ty*Ty-2*C1y*Ty-r*r;

	double m1_1,m2_1,m1_2,m2_2;
	double Bx1_C1,By1_C1,Bx2_C1,By2_C1,Bx1_C2,By1_C2,Bx2_C2,By2_C2;
	double linear_distance_T_B1_C1,linear_distance_T_B2_C1,linear_distance_T_B1_C2,linear_distance_T_B2_C2;
	
	if (b1*b1-4*a1*c1>=0)
	{
	    m1_1=(-b1+sqrt(b1*b1-4*a1*c1))/(2*a1);

	    Bx1_C1=cos(atan2(-2*a1,-b1+sqrt(b1*b1-4*a1*c1)))*(C1x+r)-sin(atan2(-2*a1,-b1+sqrt(b1*b1-4*a1*c1)))*C1y-cos(atan2(-2*a1,-b1+sqrt(b1*b1-4*a1*c1)))*C1x+sin(atan2(-2*a1,-b1+sqrt(b1*b1-4*a1*c1)))*C1y+C1x;
	    By1_C1=sin(atan2(-2*a1,-b1+sqrt(b1*b1-4*a1*c1)))*(C1x+r)+cos(atan2(-2*a1,-b1+sqrt(b1*b1-4*a1*c1)))*C1y-sin(atan2(-2*a1,-b1+sqrt(b1*b1-4*a1*c1)))*C1x-cos(atan2(-2*a1,-b1+sqrt(b1*b1-4*a1*c1)))*C1y+C1y;

	    if( fabs(By1_C1-Ty-m1_1*(Bx1_C1-Tx)) > 0.1)
	    {
		Bx1_C1=cos(atan2(-2*a1,-b1+sqrt(b1*b1-4*a1*c1))+pi)*(C1x+r)-sin(atan2(-2*a1,-b1+sqrt(b1*b1-4*a1*c1))+pi)*C1y-cos(atan2(-2*a1,-b1+sqrt(b1*b1-4*a1*c1))+pi)*C1x+sin(atan2(-2*a1,-b1+sqrt(b1*b1-4*a1*c1))+pi)*C1y+C1x;
		By1_C1=sin(atan2(-2*a1,-b1+sqrt(b1*b1-4*a1*c1))+pi)*(C1x+r)+cos(atan2(-2*a1,-b1+sqrt(b1*b1-4*a1*c1))+pi)*C1y-sin(atan2(-2*a1,-b1+sqrt(b1*b1-4*a1*c1))+pi)*C1x-cos(atan2(-2*a1,-b1+sqrt(b1*b1-4*a1*c1))+pi)*C1y+C1y;
	    }

	    linear_distance_T_B1_C1=norm2(Tx,Ty,Bx1_C1,By1_C1);
	}
	else
	{
	    m1_1=INF;
	    Bx1_C1=INF;
	    By1_C1=INF;
	    linear_distance_T_B1_C1=INF;
	}


	if (b1*b1-4*a1*c1>=0)
	{
	    m2_1=(-b1-sqrt(b1*b1-4*a1*c1))/(2*a1);

	    Bx2_C1=cos(atan2(-2*a1,-b1-sqrt(b1*b1-4*a1*c1)))*(C1x+r)-sin(atan2(-2*a1,-b1-sqrt(b1*b1-4*a1*c1)))*C1y-cos(atan2(-2*a1,-b1-sqrt(b1*b1-4*a1*c1)))*C1x+sin(atan2(-2*a1,-b1-sqrt(b1*b1-4*a1*c1)))*C1y+C1x;
	    By2_C1=sin(atan2(-2*a1,-b1-sqrt(b1*b1-4*a1*c1)))*(C1x+r)+cos(atan2(-2*a1,-b1-sqrt(b1*b1-4*a1*c1)))*C1y-sin(atan2(-2*a1,-b1-sqrt(b1*b1-4*a1*c1)))*C1x-cos(atan2(-2*a1,-b1-sqrt(b1*b1-4*a1*c1)))*C1y+C1y;

	    if (fabs(By2_C1-Ty-m2_1*(Bx2_C1-Tx)) > 0.1)
	    {

		Bx2_C1=cos(atan2(-2*a1,-b1-sqrt(b1*b1-4*a1*c1))+pi)*(C1x+r)-sin(atan2(-2*a1,-b1-sqrt(b1*b1-4*a1*c1))+pi)*C1y-cos(atan2(-2*a1,-b1-sqrt(b1*b1-4*a1*c1))+pi)*C1x+sin(atan2(-2*a1,-b1-sqrt(b1*b1-4*a1*c1))+pi)*C1y+C1x;
		By2_C1=sin(atan2(-2*a1,-b1-sqrt(b1*b1-4*a1*c1))+pi)*(C1x+r)+cos(atan2(-2*a1,-b1-sqrt(b1*b1-4*a1*c1))+pi)*C1y-sin(atan2(-2*a1,-b1-sqrt(b1*b1-4*a1*c1))+pi)*C1x-cos(atan2(-2*a1,-b1-sqrt(b1*b1-4*a1*c1))+pi)*C1y+C1y;
	    }

	    linear_distance_T_B2_C1=norm2(Tx,Ty,Bx2_C1,By2_C1);
	}
	else
	{
	    m2_1=INF;
	    Bx2_C1=INF;
	    By2_C1=INF;
	    linear_distance_T_B2_C1=INF;
	}


	double a2=C2x*C2x+Tx*Tx-2*C2x*Tx-r*r;
	double b2=-2*C2x*C2y+2*C2x*Ty+2*Tx*C2y-2*Tx*Ty;
	double c2=C2y*C2y+Ty*Ty-2*C2y*Ty-r*r;


	if (b2*b2-4*a2*c2>=0)
	{

	    m1_2=(-b2+sqrt(b2*b2-4*a2*c2))/(2*a2);

	    Bx1_C2=cos(atan2(-2*a2,-b2+sqrt(b2*b2-4*a2*c2)))*(C2x+r)-sin(atan2(-2*a2,-b2+sqrt(b2*b2-4*a2*c2)))*C2y-cos(atan2(-2*a2,-b2+sqrt(b2*b2-4*a2*c2)))*C2x+sin(atan2(-2*a2,-b2+sqrt(b2*b2-4*a2*c2)))*C2y+C2x;
	    By1_C2=sin(atan2(-2*a2,-b2+sqrt(b2*b2-4*a2*c2)))*(C2x+r)+cos(atan2(-2*a2,-b2+sqrt(b2*b2-4*a2*c2)))*C2y-sin(atan2(-2*a2,-b2+sqrt(b2*b2-4*a2*c2)))*C2x-cos(atan2(-2*a2,-b2+sqrt(b2*b2-4*a2*c2)))*C2y+C2y;

	    if (fabs(By1_C2-Ty-m1_2*(Bx1_C2-Tx)) > 0.1)
	    {
		Bx1_C2=cos(atan2(-2*a2,-b2+sqrt(b2*b2-4*a2*c2))+pi)*(C2x+r)-sin(atan2(-2*a2,-b2+sqrt(b2*b2-4*a2*c2))+pi)*C2y-cos(atan2(-2*a2,-b2+sqrt(b2*b2-4*a2*c2))+pi)*C2x+sin(atan2(-2*a2,-b2+sqrt(b2*b2-4*a2*c2))+pi)*C2y+C2x;
		By1_C2=sin(atan2(-2*a2,-b2+sqrt(b2*b2-4*a2*c2))+pi)*(C2x+r)+cos(atan2(-2*a2,-b2+sqrt(b2*b2-4*a2*c2))+pi)*C2y-sin(atan2(-2*a2,-b2+sqrt(b2*b2-4*a2*c2))+pi)*C2x-cos(atan2(-2*a2,-b2+sqrt(b2*b2-4*a2*c2))+pi)*C2y+C2y;
	    }

	    linear_distance_T_B1_C2=norm2(Tx,Ty,Bx1_C2,By1_C2);
	}
	else
	{
	    m1_2=INF;
	    Bx1_C2=INF;
	    By1_C2=INF;
	    linear_distance_T_B1_C2=INF;
	}


	if (b2*b2-4*a2*c2>=0)
	{
	    m2_2=(-b2-sqrt(b2*b2-4*a2*c2))/(2*a2);

	    Bx2_C2=cos(atan2(-2*a2,-b2-sqrt(b2*b2-4*a2*c2)))*(C2x+r)-sin(atan2(-2*a2,-b2-sqrt(b2*b2-4*a2*c2)))*C2y-cos(atan2(-2*a2,-b2-sqrt(b2*b2-4*a2*c2)))*C2x+sin(atan2(-2*a2,-b2-sqrt(b2*b2-4*a2*c2)))*C2y+C2x;
	    By2_C2=sin(atan2(-2*a2,-b2-sqrt(b2*b2-4*a2*c2)))*(C2x+r)+cos(atan2(-2*a2,-b2-sqrt(b2*b2-4*a2*c2)))*C2y-sin(atan2(-2*a2,-b2-sqrt(b2*b2-4*a2*c2)))*C2x-cos(atan2(-2*a2,-b2-sqrt(b2*b2-4*a2*c2)))*C2y+C2y;

	    if (fabs(By2_C2-Ty-m2_2*(Bx2_C2-Tx)) > 0.1)
	    {
		Bx2_C2=cos(atan2(-2*a2,-b2-sqrt(b2*b2-4*a2*c2))+pi)*(C2x+r)-sin(atan2(-2*a2,-b2-sqrt(b2*b2-4*a2*c2))+pi)*C2y-cos(atan2(-2*a2,-b2-sqrt(b2*b2-4*a2*c2))+pi)*C2x+sin(atan2(-2*a2,-b2-sqrt(b2*b2-4*a2*c2))+pi)*C2y+C2x;
		By2_C2=sin(atan2(-2*a2,-b2-sqrt(b2*b2-4*a2*c2))+pi)*(C2x+r)+cos(atan2(-2*a2,-b2-sqrt(b2*b2-4*a2*c2))+pi)*C2y-sin(atan2(-2*a2,-b2-sqrt(b2*b2-4*a2*c2))+pi)*C2x-cos(atan2(-2*a2,-b2-sqrt(b2*b2-4*a2*c2))+pi)*C2y+C2y;
	    }

	    linear_distance_T_B2_C2=norm2(Tx,Ty,Bx2_C2,By2_C2);
	}
	else
	{
	    m2_2=INF;
	    Bx2_C2=INF;
	    By2_C2=INF;
	    linear_distance_T_B2_C2=INF;
	}

	double alfa11,alfa21,alfa12,alfa22;


	if (Ax!=C1x && Ax!=C2x)
	{
	    double app1=(Ay-C1y)/(Ax-C1x);
	    double app2=(Ay-C2y)/(Ax-C2x);
	    
	    if (linear_distance_T_B1_C1 < INF)alfa11=atan2(By1_C1 -C1y -Bx1_C1*(app1) +C1x*(app1) , Bx1_C1 -C1x -By1_C1*(-app1) +C1y*(-app1));
	    else alfa11=INF;

	    
	    if (linear_distance_T_B2_C1 < INF)alfa21=atan2(By2_C1 -C1y -Bx2_C1*(app1) +C1x*(app1) , Bx2_C1 -C1x -By2_C1*(-app1) +C1y*(-app1));
	    else alfa21=INF;

	    
	    if (linear_distance_T_B1_C2 < INF)alfa12=atan2(By1_C2 -C2y -Bx1_C2*(app2) +C2x*(app2) , Bx1_C2 -C2x -By1_C2*(-app2) +C2y*(-app2));
	    else alfa12=INF;

	    
	    if (linear_distance_T_B2_C2 < INF)alfa22=atan2(By2_C2 -C2y -Bx2_C2*(app2) +C2x*(app2) , Bx2_C2 -C2x -By2_C2*(-app2) +C2y*(-app2));
	    else alfa22=INF;
	}
	else
	{ 
	    if (linear_distance_T_B1_C1 < INF)alfa11=atan2(Bx1_C1-C1x,C1y-By1_C1);    
	    else alfa11=INF;
	    
	    if (linear_distance_T_B2_C1 < INF)alfa21=atan2(Bx2_C1-C1x,C1y-By2_C1);    
	    else alfa21=INF;
	    
	    if (linear_distance_T_B1_C2 < INF)alfa12=atan2(Bx1_C2-C2x,C2y-By1_C2);
	    else alfa12=INF;
	    
	    if (linear_distance_T_B2_C2 < INF)alfa22=atan2(Bx2_C2-C2x,C2y-By2_C2);
	    else alfa22=INF;
	}

	if (alfa11 > 0 && alfa11<INF) alfa11 = alfa11-2*pi;

	if (alfa21 > 0 && alfa21<INF) alfa21 = alfa21-2*pi;

	if (alfa12 < 0 && alfa12<INF) alfa12 = alfa12+2*pi;

	if (alfa22 < 0 && alfa22<INF) alfa22 = alfa22+2*pi;

	if (alfa11 < -2*pi && alfa11<INF) alfa11 = alfa11+2*pi;

	if (alfa21 < -2*pi && alfa21<INF) alfa21 = alfa21+2*pi;

	if (alfa12 > 2*pi && alfa12<INF) alfa12 = alfa12-2*pi;

	if (alfa22 > 2*pi && alfa22<INF) alfa22 = alfa22-2*pi;

	double v11[2],v21[2],v12[2],v22[2];
	
	if (alfa11 < INF)
	{
	    v11[0]=cos(alfa11)*(Ax)-sin(alfa11)*Ay-cos(alfa11)*C1x+sin(alfa11)*C1y+C1x;
	    v11[1]=sin(alfa11)*(Ax)+cos(alfa11)*Ay-sin(alfa11)*C1x-cos(alfa11)*C1y+C1y;

	    if (fabs(v11[1]-Ty-m1_1*(v11[0]-Tx)) > 0.1)
	    {

		alfa11 = alfa11 + pi;
		v11[0]=cos(alfa11)*(Ax)-sin(alfa11)*Ay-cos(alfa11)*C1x+sin(alfa11)*C1y+C1x;
		v11[1]=sin(alfa11)*(Ax)+cos(alfa11)*Ay-sin(alfa11)*C1x-cos(alfa11)*C1y+C1y;
	    }
	}


	if (alfa21 < INF)
	{
	    v21[0]=cos(alfa21)*(Ax)-sin(alfa21)*Ay-cos(alfa21)*C1x+sin(alfa21)*C1y+C1x;
	    v21[1]=sin(alfa21)*(Ax)+cos(alfa21)*Ay-sin(alfa21)*C1x-cos(alfa21)*C1y+C1y;
	    
	    if (fabs(v21[1]-Ty-m2_1*(v21[0]-Tx)) > 0.1)
	    {
		alfa21 = alfa21 + pi;
		v21[0]=cos(alfa21)*(Ax)-sin(alfa21)*Ay-cos(alfa21)*C1x+sin(alfa21)*C1y+C1x;
		v21[1]=sin(alfa21)*(Ax)+cos(alfa21)*Ay-sin(alfa21)*C1x-cos(alfa21)*C1y+C1y;
	    }
	}


	if (alfa12 < INF)
	{
	    v12[0]=cos(alfa12)*(Ax)-sin(alfa12)*Ay-cos(alfa12)*C2x+sin(alfa12)*C2y+C2x;
	    v12[1]=sin(alfa12)*(Ax)+cos(alfa12)*Ay-sin(alfa12)*C2x-cos(alfa12)*C2y+C2y;

	    if (fabs(v12[1]-Ty-m1_2*(v12[0]-Tx)) > 0.1)
	    {
		alfa12 = alfa12 + pi;
		v12[0]=cos(alfa12)*(Ax)-sin(alfa12)*Ay-cos(alfa12)*C2x+sin(alfa12)*C2y+C2x;
		v12[1]=sin(alfa12)*(Ax)+cos(alfa12)*Ay-sin(alfa12)*C2x-cos(alfa12)*C2y+C2y;
	    }
	}


	if (alfa22 < INF)
	{
	    v22[0]=cos(alfa22)*(Ax)-sin(alfa22)*Ay-cos(alfa22)*C2x+sin(alfa22)*C2y+C2x;
	    v22[1]=sin(alfa22)*(Ax)+cos(alfa22)*Ay-sin(alfa22)*C2x-cos(alfa22)*C2y+C2y;

	    if (fabs(v22[1]-Ty-m2_2*(v22[0]-Tx)) > 0.1)
	    {
		alfa22 = alfa22 + pi;
		v22[0]=cos(alfa22)*(Ax)-sin(alfa22)*Ay-cos(alfa22)*C2x+sin(alfa22)*C2y+C2x;
		v22[1]=sin(alfa22)*(Ax)+cos(alfa22)*Ay-sin(alfa22)*C2x-cos(alfa22)*C2y+C2y;
	    }
	}

		if (alfa11 > 0 && alfa11<INF) alfa11 = alfa11-2*pi;

	if (alfa21 > 0 && alfa21<INF) alfa21 = alfa21-2*pi;

	if (alfa12 < 0 && alfa12<INF) alfa12 = alfa12+2*pi;

	if (alfa22 < 0 && alfa22<INF) alfa22 = alfa22+2*pi;

	if (alfa11 < -2*pi && alfa11<INF) alfa11 = alfa11+2*pi;

	if (alfa21 < -2*pi && alfa21<INF) alfa21 = alfa21+2*pi;

	if (alfa12 > 2*pi && alfa12<INF) alfa12 = alfa12-2*pi;

	if (alfa22 > 2*pi && alfa22<INF) alfa22 = alfa22-2*pi;


	double linear_distance[4]={linear_distance_T_B1_C1,linear_distance_T_B2_C1,linear_distance_T_B1_C2,linear_distance_T_B2_C2};

	double circle_distance[4]={alfa11*r,alfa21*r,alfa12*r,alfa22*r};

	double all_dist[4]={linear_distance[0]+fabs(circle_distance[0]),linear_distance[1]+fabs(circle_distance[1]),linear_distance[2]+fabs(circle_distance[2]),linear_distance[3]+fabs(circle_distance[3])};

	double dist[2];
	int direction=0;


	double new_theta11=theta+alfa11;

	if ((theta+alfa11)>pi) new_theta11=(theta+alfa11)-2*pi;

	if ((theta+alfa11)<-pi) new_theta11=(theta+alfa11)+2*pi;

	if (fabs(atan2(Ty-By1_C1,Tx-Bx1_C1) - new_theta11) < 1) dist[0]=all_dist[0];
	else dist[0]=all_dist[1];

	
	double new_theta12=theta+alfa12;

	if ((theta+alfa12)>pi) new_theta12=(theta+alfa12)-2*pi;

	if ((theta+alfa12)<-pi) new_theta12=(theta+alfa12)+2*pi;


	if (fabs(atan2(Ty-By1_C2,Tx-Bx1_C2) - new_theta12) < 1) dist[1]=all_dist[2]; 
	else dist[1]=all_dist[3];   

	if (dist[0]==INF) dist[0]=norm2(Tx,Ty,Ax,Ay);

	if (dist[1]==INF) dist[1]=norm2(Tx,Ty,Ax,Ay);

	if (dist[0] < dist [1]) direction=-1;
	if (dist[0]==INF && fabs(r-norm2(Tx,Ty,C1x,C1y)) < 0.1 ) direction=-1;
	if (dist[0] > dist [1]) direction=1;
	if (dist[1]==INF && fabs(r-norm2(Tx,Ty,C2x,C2y)) < 0.1 ) direction=1;

	
	
 	std::cout<<"DUBINS_DIST: ["<<dist[0]<<' '<<dist[1]<<']'<<"=> direction = "<<direction<<std::endl;
	
	return direction;
}