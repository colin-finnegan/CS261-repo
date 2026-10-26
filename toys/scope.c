#include <stdio.h>

int  a=200 ,  x = 50 ;

int sum( int a , int b )
{
	static int counter = 0;	
	int c;
	counter++ ;        c = a + b ;
	printf( "counter in sum = %d\n" , counter ) ;
	return c ;
}

void main()
{
	int x=10 ;
	printf( "sum1=%d , sum2=%d , x=%d , twice global x=%d\n" ,
   	sum(10,30) , sum(60,20) , x , 2 * x ) ;
}
