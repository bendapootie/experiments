// 245 Characters
w=10000;n[251]={1};o[999];main(i,x,y,q,t,v,c){printf("1.");for(;i<251;printf("%04d",n[i++]=x))for(x=0,y=w;x+1<y;){n[i]=x+y>>1;memset(o,0,w/4);for(q=i,c=0;q>=0;o[q---1]+=c)for(t=i;t>=0;v=o[t+q]+n[t]*n[q]+c,o[t--+q]=v%w,c=v/w);*(&x-(2&*o))=n[i];}}







w=10000;		// w = Numerical base used for calculation (ie. 10 would compute one digit at a time; 100 would compute 2)
n[251]={1};		// n = Array of ints that represents the current guess n[0] = 1; n[1...] are the calculated values to the right of the decimal;
				//     n[1] with a "w" of 10000 will be "4142"; n[2] = "1356"
o[999];			// o = Output array for checking if n*n is greater or less than 2.
				//     Foc computation, it needs to be at least twice the size of n, but 999 allows an code reduction call to memset

// i = main loop variable; tells which digit is being calculated - Assume initialized to "1"
// x, y = min and max values for binary search to find next digit
// q = loop variable to perform O(n^2) long multiplication of "n"
// t = second loop variable for long multiplication of "n"
// v = value of two digits multiplied together
// c = carry value if multiplication doesn't fit in target int
main(i,x,y,q,t,v,c)
{
	printf("1.");
	for(;i<251;printf("%04d",n[i++]=x))
		// Do a binary search to find the next digit
		// Range is [x..y] and we're done when "x" makes the square of the number less than 2 and "y" would be greater
		for(x=0,y=w;x+1<y;)
		{
			// Binary search of range - Set the next digit to the middle of the range (rounded down)
			// ">>1" has lower precidence than "/2" to avoid perentheses
			n[i]=x+y>>1;

			// Clear output number
			// "o" array needs to be at least 502 ints, or 2008 bytes and "w/4" is one byte
			// shorter than "2008".
			memset(o,0,w/4);
			for(q=i,c=0;q>=0;o[q---1]+=c)
				for(t=i;t>=0;v=o[t+q]+n[t]*n[q]+c,o[t--+q]=v%w,c=v/w);
			
			// Binary search
			// Check the first digit of "o", which is the current value of "n^2"
			// This digit will only ever be "1,2 or 3". The lergest it could ever is 1.999^2, or 3.99...
			// We take advantage of this to use "2&" to check if the "2-bit" is set. If it is, that means
			// the guess was too large and the upper bound of the binary search, "y", needs to be brought
			// down. If that bit is not set, the lower bound, "x", needs to be raised to the tested value.
			// This compiler doesn't support the trinary operator as an l-value, so to avoid a verbose if-else
			// we take advantage of "x" and "y" being near each other in memory to compute the l-value.
			// If the "2-bit" of the first digit of the answer, "o[0]", or "*o" is set, the l-value will
			// be "&x" minus 2, which is exactly where "y" is. If that bit is zero, the left size reduces to
			// "*(&x)", or simply, "x"
			*(&x-(2&*o))=n[i];
		}
}
