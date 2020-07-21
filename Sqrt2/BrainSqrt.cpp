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
#include <queue>
#include <assert.h>
#include <set>

// If 'USE_JUMP_TABLE' is defined, a simple jump table optimization will
// be generated at the memory cost of an integer per instruction
//#define USE_JUMP_TABLE

const int kDefaultTapeSize = 30000;
typedef unsigned char uint8;
typedef unsigned int uint32;

class BFVM
{
public:
	enum class OutputStyle
	{
		None = 0,
		StdOut = 1 << 0,
		InternalBuffer = 1 << 1,
		StdOutAndInteralBuffer = OutputStyle::StdOut | OutputStyle::InternalBuffer
	};

public:
	BFVM()
	{
	}

	BFVM(const char* instructions, int tape_size = kDefaultTapeSize, OutputStyle output_style = OutputStyle::StdOut)
	{
		Initialize(instructions, tape_size, output_style);
	}

	BFVM(const BFVM& other)
	{
		Clone(other);
	}

	~BFVM()
	{
		if (_tape != nullptr)
		{
			delete[] _tape;
		}
	}

	void Clone(const BFVM& other)
	{
		// Create tape buffer and copy it from the passed in VM
		_tape_size = other._tape_size;
		_tape = new uint8[_tape_size];
		if (other._tape != nullptr)
		{
			memcpy(_tape, other._tape, _tape_size);
		}
		else
		{
			memset(_tape, 0, sizeof(*_tape) * _tape_size);
		}
		_tp = other._tp;

		// Copy instruction buffer
		_instructions = other._instructions;
		_ip = other._ip;

		_output = other._output;
		_output_style = other._output_style;

#ifdef USE_JUMP_TABLE
		// Copy jump table
		_jump_table = new int[_instructions.size()];
		memcpy(_jump_table, other._jump_table, sizeof(*_jump_table) * _instructions.size());
#endif // #ifdef USE_JUMPTABLE
	}

	void Initialize(const char* instructions, int tape_size = kDefaultTapeSize, OutputStyle output_style = OutputStyle::StdOut)
	{
		// Create tape buffer and initialize to zero
		_tape_size = tape_size;
		if (_tape != nullptr)
		{
			delete[] _tape;
		}
		_tape = new uint8[_tape_size];
		memset(_tape, 0, sizeof(*_tape) * _tape_size);
		_tp = 0;

		// Copy instruction buffer
		_instructions = instructions;
		_ip = 0;

		_output = "";
		_output_style = output_style;

#ifdef USE_JUMP_TABLE
		// Initialize jump table to empty
		_jump_table = new int[_instructions.size()];
		memset(_jump_table, 0, sizeof(*_jump_table) * _instructions.size());
#endif // #ifdef USE_JUMP_TABLE
	}

	// Executes one instruction
	// Returns 'false' if reached the end of the instruction list
	// Returns 'true' otherwise
	bool Run(int num_steps = 0)
	{
		// The raw char array allows faster access, especially in debug
		const char* raw_instructions = _instructions.c_str();

		int steps_taken = 0;
		int search_dir = 1;
		while (steps_taken != num_steps || num_steps == 0)
		{
			assert(_ip <= _instructions.size());
			switch (raw_instructions[_ip])
			{
			case '>':
				_tp++;
				assert(_tp < _tape_size);
				steps_taken++;
				break;

			case '<':
				_tp--;
				assert(_tp < _tape_size);
				steps_taken++;
				break;

			case '+':
				_tape[_tp]++;
				steps_taken++;
				break;

			case '-':
				_tape[_tp]--;
				steps_taken++;
				break;

			case '.':
				if (int(_output_style) & int(OutputStyle::StdOut))
				{
					putchar(_tape[_tp]);
				}
				if (int(_output_style) & int(OutputStyle::InternalBuffer))
				{
					_output += _tape[_tp];
				}
				steps_taken++;
				break;

			case ',':
				_tape[_tp] = getchar();
				steps_taken++;
				break;

			case ']':
				search_dir = -1;
				// fall through...
			case '[':
				// if data is zero, jump forward to matching ']'
				if ((_tape[_tp] != 0 && raw_instructions[_ip] == ']') ||
					(_tape[_tp] == 0  && raw_instructions[_ip] =='['))
				{
#ifdef USE_JUMP_TABLE
					int& jump_delta = _jump_table[_ip];
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
						const char* search = &raw_instructions[_ip];
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

						jump_delta = (int)(search - &raw_instructions[_ip]);
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

	const std::string& GetOutput() const
	{
		return _output;
	}

	// Returns the data value currently pointed to by the tape pointer, with an optional offset
	uint8 GetData(int offset = 0) const
	{
		return _tape[_tp + offset];
	}

	const uint8* GetTape() const
	{
		return _tape;
	}

	const size_t GetTapeSize() const
	{
		return _tape_size;
	}

	const size_t GetTapePointer() const
	{
		return _tp;
	}

	// Returns the instruction currently pointed to by the instruction pointer, with an optional offset
	char GetInstruction(int offset = 0) const
	{
		return _instructions[_ip + offset];
	}

	const std::string& GetInstructions() const
	{
		return _instructions;
	}

	std::string& GetMutableInstructions()
	{
		return _instructions;
	}

	size_t GetInstructionPointer() const
	{
		return _ip;
	}

	static void TestVM()
	{
		//BFVM vm(FirstPassSqrt2().c_str());
		//vm.Run();

		BFVM vm("[+++.]>>>-[>+<-----]>+.", 16, OutputStyle::InternalBuffer);	// Write an ascii '4'
		vm.Run();
		assert(vm.GetOutput().back() == '4');
	}

	static void TestClass()
	{
		BFVM vm_1("[+++.]>>>-[>+<-----]>+.", kDefaultTapeSize, OutputStyle::InternalBuffer);	// Write an ascii '4'
		BFVM vm_2(vm_1);
		vm_1.Run();
		const std::string& out = vm_2.GetOutput();
	}

private:
	uint8* _tape = nullptr;
	size_t _tape_size = 0;
	size_t _tp = 0;			// tape pointer index

	std::string _instructions;
	size_t _ip = 0;			// instruction pointer index

	std::string _output;
	OutputStyle _output_style = OutputStyle::StdOut;

#ifdef USE_JUMP_TABLE
	// lookup to map index of '[' and ']' to the delta to find its counterpart
	// TODO: Figure out what to do with jump table if size of _instructions grows
	int* _jump_table = nullptr;
#endif // #ifdef USE_JUMP_TABLE
};


class AStarNode
{
public:
	AStarNode()
	{
	}

	AStarNode(BFVM* vm) :
		_vm(vm)
	{
	}

	~AStarNode()
	{
	}

	bool operator < (const AStarNode& rhs) const
	{
		return _score < rhs._score;
	}

	void ComputeScore(const std::string target_output)
	{
		assert(_vm != nullptr);

		// Estimated Cost = (Num Instructions Used) + (Num Output Characters Remaining) * (Magic Scalar)
		float cost = _vm->GetInstructions().size() + (target_output.size() - _vm->GetOutput().size()) * kEstimatedInstructionsPerOutput;

		// Negate cost to get score because we want to smallest cost to have the biggest score
		_score = -cost;
	}

public:
	static const float kEstimatedInstructionsPerOutput;
	float _score = 0.0f;
	BFVM* _vm = nullptr;
};

const float AStarNode::kEstimatedInstructionsPerOutput = 4.0f;


class AStar
{
public:
	AStar()
	{
	}

	~AStar()
	{
		// Free up all the memory from the VMs allocated in the cache
		for (BFVM* vm : _vm_cache)
		{
			delete vm;
		}
		_vm_cache.clear();
	}

	const AStarNode& GetTopNode() const
	{
		return _queue.top();
	}

	bool IsFinished() const
	{
		return _queue.empty() || _queue.top()._vm->GetOutput().size() == _target_output.size();
	}

	void SetTargetOutput(std::string target_output)
	{
		_target_output = target_output;
	}

	void SetInitialState(const BFVM& vm)
	{
		// Clone the passed in VM and push it on the A* search queue
		BFVM* new_vm = GetNewVM();
		new_vm->Clone(vm);

		AStarNode new_node(new_vm);
		AddNode(new_node);
	}

	// Pop the Node with the lowest score and add all its potential next steps
	void ProcessNextNode()
	{
		// Pop the highest scoring node
		const AStarNode top = _queue.top();
		_queue.pop();

		// Find the next character to output
		// TODO: If the output string is complete, can we stop?!?
		char next_output_char;
		{
			const std::string& top_output = top._vm->GetOutput();
			assert(top_output.size() < _target_output.size());

			next_output_char = _target_output[top_output.size()];
		}

		for (int i = -5; i <= 5; i++)
		{
			if ((int)top._vm->GetTapePointer() >= -i)
			{
				AStarNode next_node = GenerateTestNode(top, next_output_char, i);
				AddNode(next_node);
			}
		}

		// DIRTY: We're done with the VM associated with the top node; clean it up
		// TODO: Replace this with refcounting?
		FreeVM(top._vm);
	}

protected:
	BFVM* GetNewVM()
	{
		BFVM* new_vm = new BFVM();
		_vm_cache.insert(new_vm);
		return new_vm;
	}

	void FreeVM(BFVM* vm)
	{
		auto it = _vm_cache.find(vm);
		delete* it;
		_vm_cache.erase(it);
	}

	AStarNode GenerateTestNode(const AStarNode& previous, char next_output, int delta_pos)
	{
		// TODO: Verify we're not going to go off the tape (avoid BF code that wraps around)
		assert((int)previous._vm->GetTapePointer() >= -delta_pos);

		AStarNode new_node = CloneNode(previous._vm);
		BFVM& vm_ref = *new_node._vm;

		// Add instructions to move the data pointer based on 'delta_pos'
		std::string& instructions_ref = vm_ref.GetMutableInstructions();
		int increment = (delta_pos > 0) ? 1 : -1;
		for (int i = 0; i != delta_pos; i += increment)
		{
			instructions_ref += (delta_pos > 0) ? '>' : '<';
		}

		// Add instructions to modify data at this position in the tape to match 'next_output'
		// TODO: Support wraparound?
		uint8 data = vm_ref.GetData(delta_pos);
		increment = (next_output > data) ? 1 : -1;
		for (int i = data; i != next_output; i += increment)
		{
			instructions_ref += (next_output > data) ? '+' : '-';
		}

		// Add instruction to print the character
		instructions_ref += '.';

		// Execute instructions that were just generated
		vm_ref.Run();

		assert(vm_ref.GetOutput().back() == next_output);

		return new_node;
	}

	// TODO: The existance of this function makes me sad :(
	AStarNode CloneNode(const AStarNode& other)
	{
		BFVM* new_vm = GetNewVM();
		new_vm->Clone(*other._vm);
		AStarNode new_node(new_vm);
		new_node._score = other._score;
		return new_node;
	}

	void AddNode(AStarNode& node)
	{
		node.ComputeScore(_target_output);
		_queue.push(node);
	}
	
protected:
	std::set<BFVM*> _vm_cache;
	std::priority_queue<AStarNode> _queue;
	std::string _target_output;
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

void TestAStar()
{
	const char* bf_code_4 = ">>>-[>+<-----]>+";
	const char* bf_code_lots_of_4s = ">>>-[>+<-----]>+<<<+++[>>-[[>]<[->+>+<<]>>[-<<+>>]<[<]>-]<<-]>>>";
	const char* bf_code_lots_of_5s = ">>>-[>+<-----]>++<<<+++[>>-[[>]<[->+>+<<]>>[-<<+>>]<[<]>-]<<-]>>>";
	BFVM vm(bf_code_lots_of_5s, 10000, BFVM::OutputStyle::StdOutAndInteralBuffer);
	vm.Run();
	AStar a;

	std::string target_output = "1.4142135623730950488016887242096980785696718753769480731766797379907324784621070388503875343276415727350138462309122970249248360558507372126441214970999358314132226659275055927557999505011527820605714701095599716059702745345968620147285174186408891986095523292304843087143214508397626036279952514079896872533965463318088296406206152583523950547457502877599617298355752203375318570113543746034084988471603868999706990048150305440277903164542478230684929369186215805784631115966687130130156185689872372352885092648612494977154218334204285686060146824720771435854874155657069677653720226485447015858801620758474922657226002085584466521458398893944370926591800311388246468157082630100594858704003186480342194897278290641045072636881313739855256117322040245091227700226941127573627280495738108967504018369868368450725799364729060762996941380475654823728997180326802474420629269124859052181004459842150591120249441341728531478105803603371077309182869314710171111683916581726889419758716582152128229518488472";
	target_output = target_output.substr(0, 2 + 1000);
	a.SetTargetOutput(target_output);

	a.SetInitialState(vm);
	do
	{
		a.ProcessNextNode();
	} while (!a.IsFinished());

	const AStarNode& solution = a.GetTopNode();
	const std::string& str = solution._vm->GetInstructions();
	printf("\nSolution...\n%s\n%d characters\n", str.c_str(), int(str.size()));
}

int main(int argc, char *argv[])
{
	BFVM::TestVM();
	//BFVM::TestClass();
	TestAStar();
	//FirstPassSqrt2();
}
