#include <stdio.h>
#define k 251
#define w 10000
int n[k]={1};
int o[2*k];
int i;

int s()
{
	for(int b=0;b<2*k;o[b++]=0);
	for(int b=i;b>=0;b--)
	{
		int c=0;
		for(int a=i;a>=0;a--)
		{
			int v=o[a+b]+n[a]*n[b]+c;
			o[a+b]=v%w;
			c=v/w;
		}
		o[b-1]+=c;
	}
	return o[0];
}

int main()
{
	printf("1.");
	for(i=1;i<k;i++)
	{
		int a=0;
		for(int b=w;a+1<b;)
		{
			int m=(a+b)/2;
			n[i]=m;
			int r=s();
			(r>1?b:a)=m;
		}
		n[i]=a;
		printf("%04d",n[i]);
	}
	return 0;
}