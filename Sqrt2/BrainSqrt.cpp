// Program to help generate BrainFuck code that outputs
// the square root of 2 to 1000 decimal places

#include <stdio.h>
#include <time.h>
#include <cstdlib>
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

std::string BF_LOTS_OF_0s = "-[->>[>]--->-[<+>-----]<[<]<]";
std::string BF_LOTS_OF_1s = "-[->>[>]-->-[<+>-----]<[<]<]";
std::string BF_LOTS_OF_2s = "-[->>[>]->-[<+>-----]<[<]<]";

// 0 0 0 0 ... --> (0) 0 3 3 3 3 3 ... 3 3 3 0
std::string BF_LOTS_OF_3s = "-[->>[>]>-[<+>-----]<[<]<]";

// 0 (0) 0 0 ... --> (0) 0 4 4 4 4 4 ... 4 4 4 0
std::string BF_LOTS_OF_4s = "-[->>[>]+>-[<+>-----]<[<]<]";

// 0 (0) 0 0 ... --> (0) 0 5 5 5 5 5 ... 5 5 5 0
std::string BF_LOTS_OF_5s = "-[->>[>]++>-[<+>-----]<[<]<]";

// 0 (0) 0 0 ... --> (0) 0 6 6 6 6 6 ... 6 6 6 0
std::string BF_LOTS_OF_6s = "-[->>[>]+++>-[<+>-----]<[<]<]";
std::string BF_LOTS_OF_7s = "-[->>[>]++++>-[<+>-----]<[<]<]";
std::string BF_LOTS_OF_8s = "-[->>[>]+++++>-[<+>-----]<[<]<]";
std::string BF_LOTS_OF_9s = "-[->>[>]++++++>-[<+>-----]<[<]<]";


std::string SQRT_2 = "1.4142135623730950488016887242096980785696718753769480731766797379907324784621070388503875343276415727350138462309122970249248360558507372126441214970999358314132226659275055927557999505011527820605714701095599716059702745345968620147285174186408891986095523292304843087143214508397626036279952514079896872533965463318088296406206152583523950547457502877599617298355752203375318570113543746034084988471603868999706990048150305440277903164542478230684929369186215805784631115966687130130156185689872372352885092648612494977154218334204285686060146824720771435854874155657069677653720226485447015858801620758474922657226002085584466521458398893944370926591800311388246468157082630100594858704003186480342194897278290641045072636881313739855256117322040245091227700226941127573627280495738108967504018369868368450725799364729060762996941380475654823728997180326802474420629269124859052181004459842150591120249441341728531478105803603371077309182869314710171111683916581726889419758716582152128229518488472";

float RandomFloat()
{
	return (float(rand())/float(RAND_MAX));
}

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
private:
	void InitAll(const uint8* tape, size_t tape_size, size_t tp,
		const char* instructions, size_t ip,
		const char* output, OutputStyle output_style)
	{
		if (_tape_size != tape_size)
		{
			// Free old tape data if it can't be reused
			if (_tape != nullptr)
			{
				delete []_tape;
				_tape = nullptr;
			}
			
			// Allocate new tape
			if (tape_size > 0)
			{
				_tape = new uint8[tape_size];
			}
		}
		
		// Copy data from other tape or zero out if source is null
		_tape_size = tape_size;
		if (_tape_size > 0)
		{
			if (tape != nullptr)
			{
				memcpy(_tape, tape, _tape_size);
			}
			else
			{
				memset(_tape, 0, sizeof(*_tape) * _tape_size);
			}
		}
		
		// Set the tape pointer
		_tp = tp;
		
		_instructions = instructions != nullptr ? instructions : "";
		_ip = ip;			// instruction pointer index

		_output = output;
		_output_style = output_style;
	}

public:
	void Clone(const BFVM& other)
	{
		InitAll(
			other._tape,
			other._tape_size,
			other._tp,
			other._instructions.c_str(),
			other._ip,
			other._output.c_str(),
			other._output_style);
	}

	void Initialize(const char* instructions, int tape_size = kDefaultTapeSize, OutputStyle output_style = OutputStyle::StdOut)
	{
		InitAll(
			nullptr, tape_size, 0,
			instructions, 0,
			"", output_style
		);
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

					int jump_delta = (int)(search - &raw_instructions[_ip]);
					_ip += jump_delta;
				}
				search_dir = 1;
				steps_taken++;
				break;

			case 0:
				// Reached end of the input
				return false;

			case '#':
				// todo - support breakpoints?
				_ip += 0;
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
		assert(out == "4");
	}

private:
	uint8* _tape = nullptr;
	size_t _tape_size = 0;
	size_t _tp = 0;			// tape pointer index

	std::string _instructions;
	size_t _ip = 0;			// instruction pointer index

	std::string _output;
	OutputStyle _output_style = OutputStyle::StdOut;
};


template <class T>
class VmPool
{
public:
	VmPool()
	{
	}

	~VmPool()
	{
	}

	// Returns a VM that is in a consistant state, but not necessarily initialized
	T* CloneVM(const T& vm)
	{
		T* new_vm = nullptr;
		if (_available.empty())
		{
			// Create a new instance
			new_vm = new T();
			_num_allocated++;
		}
		else
		{
			new_vm = _available.back();
			_available.pop_back();
		}
		assert(new_vm != nullptr);

		new_vm->Clone(vm);
		return new_vm;
	}

	// Marks passed in vm as unused and available for reallocation
	void FreeVM(T* vm)
	{
		_available.push_back(vm);
	}

	// Marks all allocated VMs as unused
	void FreeAll()
	{
		// TODO: Implement this function!
		assert(false);
	}

protected:
	int _num_allocated = 0;
	std::vector<T*> _available;
};

VmPool<BFVM> VM_POOL;


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

	void ComputeScore(const std::string& target_output, const float target_instructions_per_output, const float random_score)
	{
		assert(_vm != nullptr);
		
		float num_setup_instructions = 40.f;
		
		_score = RawComputeScore(
			num_setup_instructions,
			_vm->GetInstructions().size(),
			_vm->GetOutput().size(),
			target_output.size(),
			target_instructions_per_output,
			random_score
		);
	}
	
	static float RawComputeScore(float setup_instructions, int solved_num_instructions, int solved_output_size, const int target_output_size, const float target_instructions_per_output, const float random_score)
	{
		float score = 0.f;
		const float countable_instructions = solved_num_instructions - setup_instructions;
		float ipo_solved = (float)countable_instructions / (float)solved_output_size;
		float solved_alpha = (float)solved_output_size / (float)target_output_size;
		score = solved_alpha * ipo_solved + (1.f - solved_alpha) * target_instructions_per_output;
		score = -score;
		
		if (random_score > 0.f)
		{
			score += RandomFloat() * random_score;
		}
		return score;
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
		// Free up any resources held by _queue
		EmptyQueue();
	}

	const AStarNode* GetTopNode() const
	{
		if (!_queue.empty())
		{
			return &_queue.top();
		}
		return nullptr;
	}
	
	const AStarNode* GetBestResult() const
	{
		if (_best_solution._vm != nullptr)
		{
			return &_best_solution;
		}
		return nullptr;
	}

	bool IsFinished() const
	{
		return _queue.empty() ||
			((_max_num_nodes > 0) && (_num_nodes_processed > _max_num_nodes));
	}

	bool Succeeded() const
	{
		return IsFinished() && ((_max_num_nodes == 0) || (_num_nodes_processed < _max_num_nodes));
	}

	int64 GetNumNodesProcessed() const
	{
		return _num_nodes_processed;
	}

	void SetMaxNumNodesToProcess(int64 max_num_nodes)
	{
		_max_num_nodes = max_num_nodes;
	}

	// Sets parameters for queue trimming.
	// If queue ever gets bigger than 'threshold', the bottom scoring values will be trimmed
	// 'amount' is the percentage of the queue to trim off (0.8 = trim 80% of queue; 0.5 = trim 50%)
	void SetQueueTrimSettings(int queue_trim_size_threshold, float queue_trim_amount)
	{
		assert(queue_trim_size_threshold >= 0);
		assert(queue_trim_amount >= 0.f && queue_trim_amount <= 1.f);
		_queue_trim_size_threshold = queue_trim_size_threshold;
		_queue_trim_amount = queue_trim_amount;
	}

	void SetTargetOutput(std::string target_output)
	{
		_target_output = target_output;
	}
	
	void SetInstructionsPerOutputSettings(const float base_ipo, const float ipo_increment_per_trim = 0.f, const float random_score =0.f)
	{
		_target_instructions_per_output = base_ipo;
		_current_instructions_per_output = _target_instructions_per_output;
		_ipo_increment_per_trim = ipo_increment_per_trim;
		_random_score = random_score;
	}

	void SetInitialState(const BFVM& vm)
	{
		// Clone the passed in VM and push it on the A* search queue
		BFVM* new_vm = VM_POOL.CloneVM(vm);
		AStarNode new_node(new_vm);
		_queue.push(new_node);
	}

	// Pop the Node with the lowest score and add all its potential next steps
	void ProcessNextNode()
	{
		// Pop the highest scoring node
		const AStarNode top = _queue.top();
		_queue.pop();
		
		// Check if top is a solution
		if (top._vm->GetOutput().size() == _target_output.size())
		{
			// Found solution! Compare it with previous best
			if (_best_solution._vm == nullptr)
			{
				_best_solution = top;
				return;
			}
			if (top._vm->GetInstructions().size() < _best_solution._vm->GetInstructions().size())
			{
				printf("New best! - %d\n", (int)top._vm->GetInstructions().size());
				printf("%s", top._vm->GetInstructions().c_str());
				VM_POOL.FreeVM(_best_solution._vm);
				_best_solution = top;
			}
			else
			{
				// not a new best
				VM_POOL.FreeVM(top._vm);
			}
			
			// quit out... nothing else to do
			return;
		}

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
					next_node.ComputeScore(_target_output, _current_instructions_per_output, _random_score);

					if (_check_hash == false)
					{
						_queue.push(next_node);
					}
					else
					{
						Hash h = next_node._vm->CalculateHash();
						float previous_best = _hash_to_best_score[h.GetValue()];
						// If "previous_best" is exactly 0.0, we assume it's just the default value and still add the node
						if ((previous_best == 0.0f) || (next_node._score > previous_best))
						{
							_hash_to_best_score[h.GetValue()] = next_node._score;
							_queue.push(next_node);
						}
						else
						{
							// Hash collided, don't add to queue and free up VM from pool
							VM_POOL.FreeVM(next_node._vm);
						}
					}
				}
			}
		}

		// DIRTY: We're done with the VM associated with the top node; clean it up
		// TODO: Replace this with refcounting?
		VM_POOL.FreeVM(top._vm);

		_num_nodes_processed++;

		// Trim queue if it gets too big
		if ((_queue_trim_size_threshold > 0) && ((int)_queue.size() >= _queue_trim_size_threshold))
		{
			TrimQueue();
		}
	}

protected:
	void TrimQueue()
	{
		static bool kTrimEveryOther = false;

		// If the output size of the best node hasn't gotten longer, adjust the scoring function parameters
		int64 top_output_size = _queue.top()._vm->GetOutput().size();
		if (top_output_size <= _longest_top_output_length)
		{
			// Output didn't get longer, increase scoring function parameters
			_current_instructions_per_output += _ipo_increment_per_trim;
		}
		else
		{
			// Track output length of top node
			_longest_top_output_length = top_output_size;
			// Output improved, reset scoring function parameters
			_current_instructions_per_output = _target_instructions_per_output;
		}

		int num_to_keep = (int)(_queue_trim_size_threshold * _queue_trim_amount);
		std::vector<AStarNode> nodes_to_keep;
		nodes_to_keep.reserve(num_to_keep);
		printf("Iterations:%d - queue size:%d", (int)_num_nodes_processed, (int)_queue.size());

		if (kTrimEveryOther)
		{
			int64 num_processed = 0;
			while (!_queue.empty())
			{
				num_processed++;
				const AStarNode& top = _queue.top();

				// Decide if top should go in the keep list or be freed
				float current_trim_amount = nodes_to_keep.size() / (float)num_processed;
				if (current_trim_amount < _queue_trim_amount)
				{
					nodes_to_keep.push_back(top);
				}
				else
				{
					VM_POOL.FreeVM(top._vm);
				}
				_queue.pop();
			}
		}
		else
		{
			// Save off 'num_to_keep' nodes to be readded
			for (int i = 0; i < num_to_keep; i++)
			{
				nodes_to_keep.push_back(_queue.top());
				_queue.pop();
			}

			// Clean out all remaining nodes
			EmptyQueue();
		}

		// Add the saved nodes back in
		for (int i = 0; i < (int)nodes_to_keep.size(); i++)
		{
			_queue.push(nodes_to_keep[i]);
		}

		// Clear the node hash since all the nodes being kept are unique, and to free up memory
		if (_check_hash)
		{
			_hash_to_best_score.clear();
		}

		printf("->%d", (int)_queue.size());

		printf(" : i/c=%d/%d : %0.2f\n", (int)_queue.top()._vm->GetInstructions().size(), (int)_queue.top()._vm->GetOutput().size(), _current_instructions_per_output);
	}

	bool GenerateTestNode(const AStarNode& previous, char next_output, int delta_pos, AStarNode& out_new_node)
	{
		// TODO: Verify we're not going to go off the tape (avoid BF code that wraps around)
		assert((int)previous._vm->GetTapePointer() >= -delta_pos);

		// early-out if data at target index is too far away to consider
		uint8 data = previous._vm->GetData(delta_pos);
		if (((next_output > data) && (next_output - data > 10)) ||
			((next_output < data) && (data - next_output > 10)))
		{
			// data is too far away to consider this node
			return false;
		}

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
		BFVM* new_vm = VM_POOL.CloneVM(*other._vm);
		AStarNode new_node(new_vm);
		new_node._score = other._score;
		return new_node;
	}

	// Empties out the queue and frees up all associated VMs
	void EmptyQueue()
	{
		// TODO: Replace with single call to VM_POOL.FreeAll()
		while (_queue.size() > 0)
		{
			const AStarNode& top = _queue.top();
			VM_POOL.FreeVM(top._vm);
			_queue.pop();
		}
	}

protected:
	std::priority_queue<AStarNode> _queue;
	std::string _target_output;
	std::unordered_map<int64, float> _hash_to_best_score;
	AStarNode _best_solution;
	int64 _num_nodes_processed = 0;
	int64 _max_num_nodes = 0;
	int _queue_trim_size_threshold = 0;
	float _queue_trim_amount = 0.0f;
	bool _check_hash = true;

	// Parameters to control if and how _target_instructions_per_output is adjusted over time
	float _target_instructions_per_output = 0.0f;
	// Value to use in scoring function that can change over time
	float _current_instructions_per_output = 0.0f;
	// How much to increase _current_instructions_per_output if a trim happens without any increase in output length
	float _ipo_increment_per_trim = 0.0f;
	// How big of a random factor to add to each score for variability
	float _random_score = 0.f;
	// Keeps track of how long the output was at the last trim to see if we're making progress
	int _longest_top_output_length = 0;
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
		case 0: potential_output_string = BF_LOTS_OF_0s; break;
		case 1: potential_output_string = BF_LOTS_OF_1s; break;
		case 2: potential_output_string = BF_LOTS_OF_2s; break;
		case 3: potential_output_string = BF_LOTS_OF_3s; break;
		case 4: potential_output_string = BF_LOTS_OF_4s; break;
		case 5: potential_output_string = BF_LOTS_OF_5s; break;
		case 6: potential_output_string = BF_LOTS_OF_6s; break;
		case 7: potential_output_string = BF_LOTS_OF_7s; break;
		case 8: potential_output_string = BF_LOTS_OF_8s; break;
		case 9: potential_output_string = BF_LOTS_OF_9s; break;
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
	float ipo = 2.5;		// Instructions per output
	float ipo_increment_per_trim = 0.0001f;
	float random_score = 0.f;
	int queue_trim_size_threshold = 200 * 1000;	// 0 = don't ever trim
	float queue_trim_amount = 0.5f;
	const int kMaxNodesToProcess = 0 * 500 * 1000;	// 0 = no limit
	int decimal_places_to_compute = 1000;
	int vm_tape_size = 500;

	std::vector<int> good_patterns = { 257 };// { 257, 371, 713, 137, 319, 852, 471, 183, 528, 570, 441, 318, 409, 147, 742, 802, 681, 258, 481, 816, 462, 509, 580, 806, 804, 484, 168, 169, 414, 609, 294, 263, 27, 328, 714, 185, 361, 72, 406, 550, 780, 270, 480, 832, 814, 249, 841, 492, 770, 144, 807, 616, 419, 16, 390, 640, 28, 539, 831, 429, 17, 38, 81, 184};

	printf("Seconds,Pattern,Steps,Score,Output Size,Num Instructions\n");

	int best_starting_pattern = 0;
	std::string best_solution = "";
	for (int starting_pattern : good_patterns)
	{
		std::string starting_pattern_code = BuildBFPattern(starting_pattern);

		BFVM vm(starting_pattern_code.c_str(), vm_tape_size, BFVM::OutputStyle::InternalBuffer);
		vm.Run();
		AStar a;
		a.SetInstructionsPerOutputSettings(ipo, ipo_increment_per_trim, random_score);
		a.SetQueueTrimSettings(queue_trim_size_threshold, queue_trim_amount);

		std::string target_output = SQRT_2.substr(0, 2 + (size_t)decimal_places_to_compute);
		a.SetTargetOutput(target_output);
		a.SetMaxNumNodesToProcess(kMaxNodesToProcess);

		// Start timer
		time_t start_time = time(nullptr);
		a.SetInitialState(vm);
		do
		{
			a.ProcessNextNode();
		} while (!a.IsFinished());

		// End timer
		time_t end_time = time(nullptr);
		int elapsed_seconds = (int)difftime(end_time, start_time);

		const AStarNode* top = a.GetTopNode();
		// Pattern, Steps, Score, Output Size, Num Instructions
		printf("%d,%d,%d,%0.4f,%d,%d",
			elapsed_seconds,
			starting_pattern,
			(int)a.GetNumNodesProcessed(),
			top->_score,
			(int)top->_vm->GetOutput().size(),
			(int)top->_vm->GetInstructions().size()
		);

		if (a.Succeeded())
		{
			const AStarNode* solution = a.GetTopNode();
			const std::string& str = solution->_vm->GetInstructions();

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
	time_t seed = time(nullptr);
	printf("seed = %ld\n", seed);
	srand(seed);
	
	BFVM::TestVM();
	//BFVM::TestClass();
	TestAStar();
	//FirstPassSqrt2();
	
	/*
	std::vector<std::string> bf_chunks;
	bf_chunks.push_back(BF_LOTS_OF_1s);

	BFVM b("", 500);
	for (int i = 0; i <bf_chunks.size(); i++)
	{
		printf("%s\n", bf_chunks[i].c_str());
		b.GetMutableInstructions() += bf_chunks[i];
		b.Run();
		b.DebugTape(0, 300);
	}
	printf("%s\n", b.GetInstructions().c_str());
	*/
}
