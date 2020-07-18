// Program to help generate BrainFuck code that outputs
// the square root of 2 to 1000 decimal places

#include <stdio.h>
#include <string>

#define BF(s) bf += s

std::string CalcDelta(int delta)
{
	std::string w;
	while(delta > 0)
	{
		delta--;
		w+="+";
	}
	while(delta < 0)
	{
		delta++;
		w+="-";
	}
	return w;
}

int main(int argc, char *argv[])
{
	const char* s = "1.4142135623730950488016887242096980785696718753769480731766797379907324784621070388503875343276415727350138462309122970249248360558507372126441214970999358314132226659275055927557999505011527820605714701095599716059702745345968620147285174186408891986095523292304843087143214508397626036279952514079896872533965463318088296406206152583523950547457502877599617298355752203375318570113543746034084988471603868999706990048150305440277903164542478230684929369186215805784631115966687130130156185689872372352885092648612494977154218334204285686060146824720771435854874155657069677653720226485447015858801620758474922657226002085584466521458398893944370926591800311388246468157082630100594858704003186480342194897278290641045072636881313739855256117322040245091227700226941127573627280495738108967504018369868368450725799364729060762996941380475654823728997180326802474420629269124859052181004459842150591120249441341728531478105803603371077309182869314710171111683916581726889419758716582152128229518488472";
	std::string bf = "";

	// Fill data stream with ascii 4 and leave pointer at right
	// put ascii '4' in data
	BF(">>>-[>+<-----]>+");
	// 0 0 0 0 (52) 0 0 0

	// loop 85 times
	BF("<<<<-[>+<---]>[");
	// 0 (85) 0 0 52 0 0 0
	{
		// loop 255 times
		BF(">>-[");
		// 0 255 0 (255) 52 0 0 0
		{
			// find next 0 in data
			BF("[>]");
			// 0 255 0 255 52 (0) 0 0

			// copy previous value into two spots
			BF("<[->+>+<<]");
			// 0 255 0 255 (0) 52 52 0 0 

			// copy back to original
			BF(">>[-<<+>>]<");
			// 0 255 0 255 52 (52) 0 0

			// move data pointer back to loop counter and decrement it
			BF("[<]>-");
			// 0 255 0 (254) 52 52 0 0
		}
		// end of loop
		BF("]<<-");
		// 0 (254) 0 0 52 52 52 52 52 52 52 etc 52 52 0 0 0
	}
	// end of loop
	BF("]>>>");
	// 0 0 0 0 (52) 52 52 52 52 52 52 etc 52 52 0 0 0


	//BF("\n\n");

	const char fill_char = '4';
	char c = fill_char;
	int num_n = 0;
	int num_d = 0;
	while (*s)
	{
		std::string w = CalcDelta(*s - c);
		std::string n = ">" + CalcDelta(*s - fill_char);
		c=*s;
		if (w.size() < n.size())
		{
			BF(w.c_str());
			num_d++;
		}
		else
		{
			BF(n.c_str());
			num_n++;
		}
		BF(".");
		s++;
	}
	printf("%s\n\n", bf.c_str());
	printf("size = %d", (int)bf.size());
}
