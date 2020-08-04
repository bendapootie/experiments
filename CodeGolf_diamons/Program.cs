// 204
//using System;class x{static void Main(){int t=0,x,y,n;for(;t++<9;){for(y=0;++y<t*2;){for(x=-10;++x<10;){n=t-Math.Abs(y-t)-(x<0?-x:x);Console.Write((n>0?""+n:" ")+(x==9?("\n"+(y==t*2-1?"\n":"")):""));}}}}}

/* 204
using System;class x{static void Main(){
int t=0,x,y,n;
for(;t++<9;){
for(y=0;++y<t*2;){
for(x=-10;++x<10;){
n=t-Math.Abs(y-t)-(x<0?-x:x);
Console.Write((n>0?""+n:" ")+(x==9?("\n"+(y==t*2-1?"\n":"")):""));
}}}}}
*/

#if FALSE
using System;class x{static void Main(){
int t=0,x,y,n;
for(;t++<9;){
for(y=0;++y<t*2;){
for(x=-10;++x<10;){
n=t-Math.Abs(y-t)-(x<0?-x:x);
Console.Write((n>0?""+n:" ")+(x==9?("\n"+(y==t*2-1?"\n":"")):""));
}}}}}
#endif


using System;
class x
{
    static void S(int s)
    {
        while (s-- > 0)
        {
            Console.Write(" ");
        }
    }
    static void L(int n)
    {
        for (int a = 1; a <= n*2-1; a++)
        {
            Console.Write(n-Math.Abs(a-n));
        }
        Console.WriteLine();
    }

    static void Main()
    {
        S(9);
        L(1);
        L(0);

        S(9); L(1);
        S(8); L(2);
        S(9); L(1);
        L(0);
    }
}
