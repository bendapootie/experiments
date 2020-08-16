// Code golf program to output abbreviation given state name
// https://code.golf/united-states#c

/*
---- First two letters
Alabama	AL
Arkansas	AR
California	CA
Colorado	CO
Delaware	DE
Florida	FL
Idaho	ID
Illinois	IL
Indiana	IN
Massachusetts	MA
Michigan	MI
Nebraska	NE
Ohio	OH
Oklahoma	OK
Oregon	OR
Utah	UT
Washington	WA
Wisconsin	WI
Wyoming	WY

---- First and last
Connecticut	CT
Georgia	GA
Hawaii	HI
Iowa	IA
Kansas	KS
Kentucky	KY
Louisiana	LA
Maine	ME
Maryland	MD
Pennsylvania	PA
Vermont	VT
Virginia	VA

---- First letters of first and last words
District of Columbia	DC
New Hampshire	NH
New Jersey	NJ
New Mexico	NM
New York	NY
North Carolina	NC
North Dakota	ND
Rhode Island	RI
South Carolina	SC
South Dakota	SD
West Virginia	WV

????
Alaska	AK
Arizona	AZ
Minnesota	MN
Mississippi	MS
Missouri	MO
Montana	MT
Nevada	NV
Tennessee	TN
Texas	TX
*/

#include <stdio.h>

void P(char* s)
{
	char* p=s;
	printf("%c",*p);
	while (*++p)
	{
		if (*p>64&*p<95)
		{
			printf("%c\n",*p);
			return;
		}
	}
}

int main(int c,char** v)
{
	P("Rhode Island");
	P("Alaska");
	for(;*++v;)
		P(*v);
}
