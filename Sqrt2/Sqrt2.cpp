#include <stdio.h>
const int k=1001;
int n[k]={1};
int o[2*k];

void p(int* n, int d)
{
	for(int i=0;i<d;i++)
		printf("%d",n[i]);
	printf("\n");
}

int s()
{
	for(int b=0;b<2*k;o[b++]=0);
	for(int b=k-1;b>=0;b--)
	{
		int c=0;
		for(int a=k-1;a>=0;a--)
		{
			int v=o[a+b]+n[a]*n[b]+c;
			o[a+b]=v%10;
			c=v/10;
		}
		o[b-1]+=c;
	}
	return o[0];
}

int main()
{
	for(int i=1;i<k;i++)
	{
		for(n[i]=9;s()>1;n[i]--)
		{
		}
	}
	p(n,k);
	return 0;
}