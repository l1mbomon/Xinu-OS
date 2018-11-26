#include <math.h>

double pow(double x, double y)
{
	int i;
	double res = 1;

	if(y>0)
	{
		for(i=1;i<=y;i++)
			res=res*x;
		return res;
	}
	else if(y<0)
	{
		for(i=0;i>y;i--)
			res=res/x;
		return res;
	}
	return res;
}

double log(double x)
{
	if(x<0)
		return -1;
		
	double res = 0;
	int i;
	
	for(i=0; i<20; i++)
	{		
		res = res + (pow(-1,i)*pow((x-1),(i+1))/(i+1));
	}
	return res;

}

double expdev(double lambda)
{
	double temp,res;
	
	do{
		temp = (double) rand()/RAND_MAX;
	}while(temp == 0);

	res = -log(temp)/lambda;

	return res;
}
