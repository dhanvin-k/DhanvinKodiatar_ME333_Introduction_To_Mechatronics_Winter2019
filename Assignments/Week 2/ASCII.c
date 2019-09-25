#include <stdio.h>
	int main(){
		unsigned char i;
		
		for(i=33;i<128;i++)
		{
			if(i==127){
				printf("%d del\n",i);}	// displays 'del' at ASCII value 127
										// instead of a blank space
			
			else{
				printf("%d %c\n",i,i);}
				
		}
		return 0;}