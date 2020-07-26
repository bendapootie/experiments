// Program to help generate BrainFuck code that outputs
// the square root of 2 to 1000 decimal places

#include <stdio.h>
#include <string>
#include <queue>
#include <assert.h>
#include <set>
#include <map>
#include <unordered_map>

const int kDefaultTapeSize = 30000;
typedef unsigned char uint8;
typedef unsigned int uint32;
typedef long long int64;

static_assert(sizeof(uint8) == 1, "Expected uint8 to be 1-byte");
static_assert(sizeof(uint32) == 4, "Expected uint32 to be 4-bytes");
static_assert(sizeof(int64) == 8, "Expected int64 to be 8-bytes");


// If 'USE_JUMP_TABLE' is defined, a simple jump table optimization will
// be generated at the memory cost of an integer per instruction
//#define USE_JUMP_TABLE

// (0) --> (255)
std::string BF_MAKE_255 = "-";

// (0) 0 --> 0 (48)
std::string BF_ASCII_0 = "-[>+<-----]>---";

// (0) 0 --> 0 (49)
std::string BF_ASCII_1 = "-[>+<-----]>--";

// (0) 0 --> 0 (50)
std::string BF_ASCII_2 = "-[>+<-----]>-";

// (0) 0 --> 0 (51)
std::string BF_ASCII_3 = "-[>+<-----]>";

// (0) 0 --> 0 (52)
std::string BF_ASCII_4 = "-[>+<-----]>+";

// (0) 0 --> 0 (53)
std::string BF_ASCII_5 = "-[>+<-----]>++";

// (0) 0 --> 0 (54)
std::string BF_ASCII_6 = "-[>+<-----]>+++";

// (0) 0 0 0 --> 0 0 0 (55)
std::string BF_ASCII_7 = ">-[++>+[+<]>]>-";

// (0) 0 0 0 --> 0 0 0 (56)
std::string BF_ASCII_8 = ">-[++>+[+<]>]>";

// (0) 0 0 0 --> 0 0 0 (57)
std::string BF_ASCII_9 = ">-[++>+[+<]>]>+";

// (x) 0 0 --> (0) x x
std::string BF_COPY_2X("[->+>+<<]");

// (x) 0 0 0 0 --> (0) x x x x
std::string BF_COPY_4X("[->+>+>+>+<<<<]");

// (0) 0 0 0 0 --> 0 (49) 49 49 49
std::string bf_code_1111 = "-----------[>+>+>+>+<<<<-----]>";

std::string BF_11 = BF_ASCII_1 + BF_COPY_2X;
std::string BF_55 = BF_ASCII_5 + BF_COPY_2X;
std::string BF_5555 = BF_ASCII_5 + BF_COPY_4X;

// 0 0 0 0 0 --> 0 (0) 49 49 49 49
std::string BF_33 = BF_ASCII_3 + BF_COPY_2X;

// 0 0 0 0 0 --> 0 (0) 49 49 49 49
std::string BF_1111 = BF_ASCII_1 + BF_COPY_4X;

// (x) 0 N N 0 --> (0) 0 N N N N ... N N N (N : x times) 0
std::string BF_MAKE_X_COPIES = "[->>[>]<" + BF_COPY_2X + ">>[-<<+>>]<[<]<]";

// 0 0 0 0 ... --> (0) 0 3 3 3 3 3 ... 3 3 3 0
std::string BF_LOTS_OF_3s = "-[->>[>]>-[<+>-----]<[<]<]";

// 0 (0) 0 0 ... --> (0) 0 4 4 4 4 4 ... 4 4 4 0
std::string BF_LOTS_OF_4s = "-[->>[>]+>-[<+>-----]<[<]<]";

// 0 (0) 0 0 ... --> (0) 0 5 5 5 5 5 ... 5 5 5 0
std::string BF_LOTS_OF_5s = "-[->>[>]++>-[<+>-----]<[<]<]";

// 0 (0) 0 0 ... --> (0) 0 6 6 6 6 6 ... 6 6 6 0
std::string BF_LOTS_OF_6s = "-[->>[>]+++>-[<+>-----]<[<]<]";


std::string SQRT_2 = "1.4142135623730950488016887242096980785696718753769480731766797379907324784621070388503875343276415727350138462309122970249248360558507372126441214970999358314132226659275055927557999505011527820605714701095599716059702745345968620147285174186408891986095523292304843087143214508397626036279952514079896872533965463318088296406206152583523950547457502877599617298355752203375318570113543746034084988471603868999706990048150305440277903164542478230684929369186215805784631115966687130130156185689872372352885092648612494977154218334204285686060146824720771435854874155657069677653720226485447015858801620758474922657226002085584466521458398893944370926591800311388246468157082630100594858704003186480342194897278290641045072636881313739855256117322040245091227700226941127573627280495738108967504018369868368450725799364729060762996941380475654823728997180326802474420629269124859052181004459842150591120249441341728531478105803603371077309182869314710171111683916581726889419758716582152128229518488472";

class Hash
{
public:
	Hash()
	{
	}

	bool operator < (const Hash& other) const
	{
		return _hash < other._hash;
	}

	void Add(const void* ptr, const int size)
	{
		const uint8* p = static_cast<const uint8*>(ptr);
		for (int i = 0; i < size; i++, p++)
		{ 
			_hash = ((_hash << 5) + _hash) + (*p);
		}
	}

	void Add(const std::string& s)
	{
		Add(s.c_str(), (int)s.size());
	}

	template <class T>
	void Add(const T& n)
	{
		Add(&n, sizeof(n));
	}

	int64 GetValue() const
	{
		return _hash;
	}

	int64 _hash = 5381;
};



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
				if (_tp >= _tape_size)
				{
					_tp = 0;
				}
				steps_taken++;
				break;

			case '<':
				_tp--;
				// _tp is unsigned and will be very large if it tries to go negative
				if (_tp >= _tape_size)
				{
					_tp = _tape_size - 1;
				}
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
	
	Hash CalculateHash()
	{
		// Build hash based on these elements. Maybe let caller decide which ones to include?
		Hash h;
		h.Add(_tape, (int)_tape_size);	// tape data
		h.Add(_tp);						// tape pointer

		h.Add(_instructions.size());	// size of instructions
		//h.Add(_instructions);			// instructions
		h.Add(_ip);						// instruction pointer

		h.Add(_output);					// output
		return h;
	}
	
	void DebugTape(int min = 0, int count = 100) const
	{
		int chars_per_line = 10;
		for (int i = 0; i < count; i++)
		{
			printf("%3d%s%1c%s%s",
				_tape[i],
				i == _tp ? "(" : "'",
				_tape[i],
				i == _tp ? ")" : "'",
				i%chars_per_line == chars_per_line - 1?"\n":" ");
		}
		printf("\n");
	}
	
	const std::string& GetOutput() const
	{
		return _output;
	}

	// Returns the data value currently pointed to by the tape pointer, with an optional offset
	uint8 GetData(int offset = 0) const
	{
		return _tape[(_tp + offset) % _tape_size];
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

	void ComputeScore(const std::string target_output, const float target_instructions_per_output)
	{
		assert(_vm != nullptr);
		
		float score = 0.0f;
		if (target_instructions_per_output > 0.0f)
		{
			// A clear target was given, use that to determine score
			// score = number of instructions remaining per output char
			// Note - This approach leans into targeting a specific instruction length
			const float target_num_instructions = target_output.size() * target_instructions_per_output;
		
			score = (target_num_instructions - _vm->GetInstructions().size()) / (target_output.size() - _vm->GetOutput().size());
		}
		else
		{
			// No clear target was given, do something generic
			// Estimated Cost = (Num Instructions Used) + (Num Output Characters Remaining) * (Magic Scalar)
			float cost = _vm->GetInstructions().size() + (target_output.size() - _vm->GetOutput().size()) * 3.0f;
			// Negate cost to get score because we want to smallest cost to have the biggest score
			score = -cost;
		}
	
		_score = score;
	}

public:
	static const float kEstimatedInstructionsPerOutput;
	float _score = 0.0f;
	BFVM* _vm = nullptr;
};


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
		return _queue.empty() ||
			(_queue.top()._vm->GetOutput().size() == _target_output.size()) ||
			((_max_num_nodes > 0) && (_num_nodes_processed > _max_num_nodes));
	}

	bool Succeeded() const
	{
		return IsFinished() && ((_max_num_nodes == 0) || (_num_nodes_processed < _max_num_nodes));
	}

	int GetNumNodesProcessed() const
	{
		return _num_nodes_processed;
	}

	void SetMaxNumNodesToProcess(int max_num_nodes)
	{
		_max_num_nodes = max_num_nodes;
	}

	void SetTargetOutput(std::string target_output)
	{
		_target_output = target_output;
	}
	
	void SetTargetInstructionsPerOutput(const float s)
	{
		_target_instructions_per_output = s;
	}

	void SetInitialState(const BFVM& vm)
	{
		// Clone the passed in VM and push it on the A* search queue
		BFVM* new_vm = GetNewVM();
		new_vm->Clone(vm);

		AStarNode new_node(new_vm);
		_queue.push(new_node);
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
				AStarNode next_node;
				bool use_node = GenerateTestNode(top, next_output_char, i, next_node);
				if (use_node)
				{
					next_node.ComputeScore(_target_output, _target_instructions_per_output);

					Hash h = next_node._vm->CalculateHash();
					float previous_best = _hash_to_best_score[h.GetValue()];
					if (next_node._score > previous_best)
					{
						_hash_to_best_score[h.GetValue()] = next_node._score;
						_queue.push(next_node);
					}
					else
					{
						//printf("Hash collided - %llx! - %0.4f : %0.4f\n", h.GetValue(), previous_best, next_node._score);
					}
				}
			}
		}

		// DIRTY: We're done with the VM associated with the top node; clean it up
		// TODO: Replace this with refcounting?
		FreeVM(top._vm);

		_num_nodes_processed++;
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

	bool GenerateTestNode(const AStarNode& previous, char next_output, int delta_pos, AStarNode& out_new_node)
	{
		// TODO: Verify we're not going to go off the tape (avoid BF code that wraps around)
		assert((int)previous._vm->GetTapePointer() >= -delta_pos);

		out_new_node = CloneNode(previous._vm);
		BFVM& vm_ref = *out_new_node._vm;

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
		if (((next_output > data) && (next_output - data > 10)) ||
			((next_output < data) && (data - next_output > 10)))
		{
			// data is too far away to consider this node
			return false;
		}
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

		return true;
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
	
protected:
	std::set<BFVM*> _vm_cache;
	std::priority_queue<AStarNode> _queue;
	std::string _target_output;
	std::unordered_map<int64, float> _hash_to_best_score;
	int _num_nodes_processed = 0;
	int _max_num_nodes = 0;
	float _target_instructions_per_output = 0.0f;
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
	const char* s = SQRT_2.c_str();
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

	// Make 4 2 5 8 2 5 8 2 5 8 etc
	BF(">[-->+>++++>]<[<]");
	// 0 0 0 (0) 52 50 53 56 50 53 56 etc

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

std::string GetBFAsciiCodeForDigit(int digit)
{
	switch (digit)
	{
	case 0: return BF_ASCII_0;
	case 1:	return BF_ASCII_1;
	case 2:	return BF_ASCII_2;
	case 3:	return BF_ASCII_3;
	case 4:	return BF_ASCII_4;
	case 5:	return BF_ASCII_5;
	case 6:	return BF_ASCII_6;
	case 7:	return BF_ASCII_7;
	case 8:	return BF_ASCII_8;
	case 9:	return BF_ASCII_9;
	}
	assert(false);
	return "";
}

// Returns the BF code to generate a tape with a pattern 255 long of the repeating sequence given
// ie. passing in "38" will return code to generate '3' '8' '3' '8' '3' '8' ..... '3' '8'
std::string BuildBFPattern(int pattern)
{
	// Get list of digits in pattern (in reverse order)
	std::vector<int> digits;
	int n = pattern;
	int digit_sum = 0;
	while (n > 0)
	{
		digits.push_back(n % 10);
		digit_sum += n % 10;
		n = n / 10;
	}

	// Rather than being smart, just build ten versions of the code to
	// generate the pattern and pick the one that's the shortest
	std::string output_string = "";
	for (int base_digit = 0; base_digit < 10; base_digit++)
	{
		// Start with code to make 255 copies of the average value
		std::string potential_output_string = "";
		switch (base_digit)
		{
		case 3:
			potential_output_string = BF_LOTS_OF_3s;
			break;
		case 4:
			potential_output_string = BF_LOTS_OF_4s;
			break;
		case 5:
			potential_output_string = BF_LOTS_OF_5s;
			break;
		case 6:
			potential_output_string = BF_LOTS_OF_6s;
			break;
		default:
			potential_output_string = GetBFAsciiCodeForDigit(base_digit) + "<<" + BF_MAKE_255 + BF_MAKE_X_COPIES;
		}

		// Loop through list backwards since the list is backwards
		// and build the pattern based on difference from the average
		potential_output_string += ">>[";
		for (int i = (int)digits.size() - 1; i >= 0; i--)
		{
			int increment = digits[i] > base_digit ? 1 : -1;
			for (int j = base_digit; j != digits[i]; j += increment)
			{
				potential_output_string += digits[i] > base_digit ? "+" : "-";
			}
			potential_output_string += ">";
		}
		potential_output_string += "]";

		if (output_string.size() == 0 || (potential_output_string.size() < output_string.size()))
		{
			output_string = potential_output_string;
		}
	}

	return output_string;
}

void TestAStar()
{
	const int kMaxNodesToProcess = 100000;
	float target_instructions_per_output = 2.5f;
	int decimal_places_to_compute = 1000;
	int vm_tape_size = 500;

	printf("Pattern,Steps,Score,Output Size,Num Instructions\n");

	int best_starting_pattern = 0;
	std::string best_solution = "";
	for (int starting_pattern = 1; starting_pattern <= 999; starting_pattern++)
	{
		std::string starting_pattern_code = BuildBFPattern(starting_pattern);

		BFVM vm(starting_pattern_code.c_str(), vm_tape_size, BFVM::OutputStyle::InternalBuffer);
		vm.Run();
		AStar a;
		a.SetTargetInstructionsPerOutput(target_instructions_per_output);

		std::string target_output = SQRT_2.substr(0, (size_t)(decimal_places_to_compute + 2));
		a.SetTargetOutput(target_output);
		a.SetMaxNumNodesToProcess(kMaxNodesToProcess);

		a.SetInitialState(vm);
		do
		{
			a.ProcessNextNode();
		} while (!a.IsFinished());

		const AStarNode& top = a.GetTopNode();
		// Pattern, Steps, Score, Output Size, Num Instructions
		printf("%d,%d,%0.4f,%d,%d",
			starting_pattern,
			a.GetNumNodesProcessed(),
			top._score,
			(int)top._vm->GetOutput().size(),
			(int)top._vm->GetInstructions().size()
		);

		if (a.Succeeded())
		{
			const AStarNode& solution = a.GetTopNode();
			const std::string& str = solution._vm->GetInstructions();

			if (best_solution.size() == 0 || (str.size() < best_solution.size()))
			{
				best_solution = str;
				best_starting_pattern = starting_pattern;
				printf(",<-- NEW BEST!");
			}
		}
		printf("\n");
	}

	printf("\n\nBest Solution...\n%s\n%d characters\nStarting Pattern - %d",
		best_solution.c_str(),
		int(best_solution.size()),
		best_starting_pattern);
}

int main(int argc, char *argv[])
{
	BFVM::TestVM();
	//BFVM::TestClass();
	TestAStar();
	//FirstPassSqrt2();
	
	/*
	std::vector<std::string> bf_chunks;
	bf_chunks.push_back(BF_LOTS_OF_6s);

	BFVM b("", 500);
	for (int i = 0; i <bf_chunks.size(); i++)
	{
		printf("%s\n", bf_chunks[i].c_str());
		b.GetMutableInstructions() += bf_chunks[i];
		b.Run();
		b.DebugTape(0, 300);
	}
	printf("%s\n", b.GetInstructions().c_str());
	/*
	b.GetMutableInstructions() += "<<<+++[>>-[[>]<[->+>+<<]>>[-<<+>>]<[<]>-]<<-]>>>";
	b.Run();
	b.DebugTape(0, 20);
	b.GetMutableInstructions() += ">[-->+>++++>]<[<]";
	b.Run();
	b.DebugTape(0, 20);
	*/
}
