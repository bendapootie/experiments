// Program to help generate BrainFuck code that outputs
// the square root of 2 to 1000 decimal places

// Next plan - A*
// 1. Set up the tape with something reasonable (all '4's, alternating '2's and '7's?)
// 2. A* this bitch!
//   A. State is tape state (data + pointer) and list of instructions taken to get there
//   B. Each step find a handful of potential next steps (change current data, 1 left, 1 right, 2 left, etc)
//   C. Heuristic is instruction length
// 3. Repeat with a bunch of different starting tape states to find optimal answer

#include <stdio.h>
#include <string>
#include <map>

// If 'USE_JUMP_TABLE' is defined, a simple jump table optimization will
// be generated at the memory cost of an integer per instruction
//#define USE_JUMP_TABLE

const int kDefaultTapeSize = 30000;
typedef unsigned char uint8;

class BFVM
{
public:
	BFVM(const char* instructions, int tape_size = kDefaultTapeSize)
	{
		// Create tape buffer and initialize to zero
		_tape_size = tape_size;
		_tape = new uint8[_tape_size];
		memset(_tape, 0, sizeof(*_tape) * _tape_size);
		_dp = _tape;

		// Copy instruction buffer
		_instruction_size = strlen(instructions) + 1;
		_instructions = new char[_instruction_size];
		strcpy_s(_instructions, _instruction_size, instructions);
		_ip = _instructions;

#ifdef USE_JUMP_TABLE
		// Initialize jump table to empty
		_jump_table = new int[_instruction_size];
		memset(_jump_table, 0, sizeof(*_jump_table) * _instruction_size);
#endif // #ifdef USE_JUMP_TABLE
	}

	// Executes one instruction
	// Returns 'false' if reached the end of the instruction list
	// Returns 'true' otherwise
	bool Run(int num_steps = 0)
	{
		int steps_taken = 0;
		int search_dir = 1;
		while (steps_taken != num_steps || num_steps == 0)
		{
			switch (*_ip)
			{
			case '>':
				++_dp;
				steps_taken++;
				break;

			case '<':
				--_dp;
				steps_taken++;
				break;

			case '+':
				++(*_dp);
				steps_taken++;
				break;

			case '-':
				--(*_dp);
				steps_taken++;
				break;

			case '.':
				putchar(*_dp);
				steps_taken++;
				break;

			case ',':
				*_dp = getchar();
				steps_taken++;
				break;

			case ']':
				search_dir = -1;
				// fall through...
			case '[':
				// if data is zero, jump forward to matching ']'
				if ((*_dp != 0 && *_ip == ']') || (*_dp == 0  && *_ip =='['))
				{
					int index = (int)(_ip - _instructions);
#ifdef USE_JUMP_TABLE
					int& jump_delta = _jump_table[index];
#else // #ifdef USE_JUMP_TABLE
					int jump_delta = 0;
#endif // #ifdef USE_JUMP_TABLE
					if (jump_delta != 0)
					{
						_ip += jump_delta;
					}
					else
					{
						int depth = 1;
						char* search = _ip;
						do
						{
							search += search_dir;
							if (*search == ']')
							{
								depth -= search_dir;
							}
							else if (*search == '[')
							{
								depth += search_dir;
							}
						} while (depth != 0);

						jump_delta = (int)(search - _ip);
						_ip += jump_delta;
					}
				}
				search_dir = 1;
				steps_taken++;
				break;

			case 0:
				// Reached end of the input
				return false;

			case '#':
				// todo - support breakpoints?
				_ip = _ip;
				break;

			default:
				// ignore all other characters
				break;
			}
			// increment instruction pointer
			_ip++;
		}
		return true;
	}

	static void Test(std::string bf_code)
	{
		BFVM vm(bf_code.c_str());
		//BFVM vm("[+++.]>>>-[>+<-----]>+.");	// Write an ascii '4'
		for (; vm.Run(););
	}

private:
	uint8* _tape = nullptr;
	uint8* _dp = nullptr;	// data pointer
	size_t _tape_size = 0;

	char* _instructions = nullptr;
	char* _ip = nullptr; // instruction pointer
	size_t _instruction_size = 0;

#ifdef USE_JUMP_TABLE
	// lookup to map index of '[' and ']' to the delta to find its counterpart
	int* _jump_table = nullptr;
#endif // #ifdef USE_JUMP_TABLE
};

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

std::string FirstPassSqrt2()
{
#define BF(s) bf += s
	const char* s = "1.4142135623730950488016887242096980785696718753769480731766797379907324784621070388503875343276415727350138462309122970249248360558507372126441214970999358314132226659275055927557999505011527820605714701095599716059702745345968620147285174186408891986095523292304843087143214508397626036279952514079896872533965463318088296406206152583523950547457502877599617298355752203375318570113543746034084988471603868999706990048150305440277903164542478230684929369186215805784631115966687130130156185689872372352885092648612494977154218334204285686060146824720771435854874155657069677653720226485447015858801620758474922657226002085584466521458398893944370926591800311388246468157082630100594858704003186480342194897278290641045072636881313739855256117322040245091227700226941127573627280495738108967504018369868368450725799364729060762996941380475654823728997180326802474420629269124859052181004459842150591120249441341728531478105803603371077309182869314710171111683916581726889419758716582152128229518488472";
	std::string bf = "";

	// Fill data stream with ascii 4 and leave pointer at right
	// put ascii '4' in data
	BF(">>>-[>+<-----]>+");
	// 0 0 0 0 (52) 0 0 0

	// loop 3 times
	BF("<<<+++[");
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
		c = *s;
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
	printf("size = %d\n", (int)bf.size());

	return bf;
#undef BF
}

int main(int argc, char *argv[])
{
	std::string bf_sqrt2 = FirstPassSqrt2();
	BFVM::Test(bf_sqrt2);
}
