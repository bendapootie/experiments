const bool kHideAllDebug = true;

const bool kPrintfSpamNewBests = !kHideAllDebug && true;
const bool kPrintfSpamTrimUpdates = !kHideAllDebug && true;

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
#include <chrono>

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
std::string E = "2.7182818284590452353602874713526624977572470936999595749669676277240766303535475945713821785251664274274663919320030599218174135966290435729003342952605956307381323286279434907632338298807531952510190115738341879307021540891499348841675092447614606680822648001684774118537423454424371075390777449920695517027618386062613313845830007520449338265602976067371132007093287091274437470472306969772093101416928368190255151086574637721112523897844250569536967707854499699679468644549059879316368892300987931277361782154249992295763514822082698951936680331825288693984964651058209392398294887933203625094431173012381970684161403970198376793206832823764648042953118023287825098194558153017567173613320698112509961818815930416903515988885193458072738667385894228792284998920868058257492796104841984443634632449684875602336248270419786232090021609902353043699418491463140934317381436405462531520961836908887070167683964243781405927145635490613031072085103837505101157477041718986106873969655212671546889570350354";
std::string PHI = "1.6180339887498948482045868343656381177203091798057628621354486227052604628189024497072072041893911374847540880753868917521266338622235369317931800607667263544333890865959395829056383226613199282902678806752087668925017116962070322210432162695486262963136144381497587012203408058879544547492461856953648644492410443207713449470495658467885098743394422125448770664780915884607499887124007652170575179788341662562494075890697040002812104276217711177780531531714101170466659914669798731761356006708748071013179523689427521948435305678300228785699782977834784587822891109762500302696156170025046433824377648610283831268330372429267526311653392473167111211588186385133162038400522216579128667529465490681131715993432359734949850904094762132229810172610705961164562990981629055520852479035240602017279974717534277759277862561943208275051312181562855122248093947123414517022373580577278616008688382952304592647878017889921990270776903895321968198615143780314997411069260886742962267575605231727775203536139362";
std::string PI = "3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066470938446095505822317253594081284811174502841027019385211055596446229489549303819644288109756659334461284756482337867831652712019091456485669234603486104543266482133936072602491412737245870066063155881748815209209628292540917153643678925903600113305305488204665213841469519415116094330572703657595919530921861173819326117931051185480744623799627495673518857527248912279381830119491298336733624406566430860213949463952247371907021798609437027705392171762931767523846748184676694051320005681271452635608277857713427577896091736371787214684409012249534301465495853710507922796892589235420199561121290219608640344181598136297747713099605187072113499999983729780499510597317328160963185950244594553469083026425223082533446850352619311881710100031378387528865875332083814206171776691473035982534904287554687311595628638823537875937519577818577805321712268066130019278766111959092164201989";
std::string TAU = "6.2831853071795864769252867665590057683943387987502116419498891846156328125724179972560696506842341359642961730265646132941876892191011644634507188162569622349005682054038770422111192892458979098607639288576219513318668922569512964675735663305424038182912971338469206972209086532964267872145204982825474491740132126311763497630418419256585081834307287357851807200226610610976409330427682939038830232188661145407315191839061843722347638652235862102370961489247599254991347037715054497824558763660238982596673467248813132861720427898927904494743814043597218874055410784343525863535047693496369353388102640011362542905271216555715426855155792183472743574429368818024499068602930991707421015845593785178470840399122242580439217280688363196272595495426199210374144226999999967459560999021194634656321926371900489189106938166052850446165066893700705238623763420200062756775057731750664167628412343553382946071965069808575109374623191257277647075751875039155637155610643424536132260038557532223918184328403978";
std::string CONWAYS_CONSTANT = "1.3035772690342963912570991121525518907307025046594048757548613906285508878524615571268157668644252255534713930470949026839628498935515543473758248566910889777702165760066666136181957581499714162117477679231324299325720143046277132953880169252753163112124160278974455631122119289176594544284422105034839844304843503659788616365686961229330052179678121274207274283384185853897804629753621113325713155961142841646364930650685182742924915750807604716554873289903140679626277521098230185009804867680417112703692827704555699254104725325529600239674161517613007576995239414709209627297796525283812895292399842146646970063628087185984737643242596375377861487149180417794388256454962961267267631703413998923868380017499339163832550419738743376404644230963416377342999964027953477812315036938111361969193101105512224328065334872949851635350180008618980044269569620390894963960410593894526262897484805720049895205894083154511527464184151251802473020019379677783662140660911047529322875883638588769569619942051649";

template <class T>
class PriorityQueue : public std::priority_queue<T> 
{
public:
	const std::vector<T>& GetRawList() const { return this->c; }
};

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
	
	void DebugTape(int chars_per_line = 20) const
	{
		printf("\n");
		int line_index = 0;
		bool line_has_non_zero = false;
		do
		{
			line_has_non_zero = false;
			printf("%4d :", line_index);
			for (int i = line_index; i < line_index + chars_per_line; i++)
			{
				uint8 c = _tape[i];
				// Figure out how to show where the tape pointer is
				const char* tape_open = "(";
				const char* tape_close = ")";
				const char* prefix = (i == _tp) ? tape_open : (i != line_index && i == _tp + 1 ? tape_close : " ");
				const char* suffix = (i + 1 != line_index + chars_per_line) ? "" : (i == _tp ? tape_close : " ");
				printf("%s", prefix);
				if (c >= 32 && c <= 126)
				{
					printf("'%c", c);
				}
				else
				{
					printf("%02x", c);
				}
				printf("%s", suffix);

				// Track if any non-zeros are found in this line
				line_has_non_zero |= (c != 0);
			}
			line_index += chars_per_line;
			printf("\n");
		} while (line_has_non_zero);
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
		BFVM vm("[+++.]>>>-[>+<-----]>+.", 16, OutputStyle::InternalBuffer);	// Write an ascii '4'
		vm.Run();
		assert(vm.GetOutput().back() == '4');

		/*
		int steps_per_run = 0;
		std::vector<std::string> bf_chunks;
		std::string s = "-[->>[>]>-[<+>-----]<[<]<]>>[->++>++++>][<]>[<]><<<<<-.>>---------.<<<<-.>------.<.<.-.++.<<.+.<.+.<.>.---.";
		bf_chunks.push_back(s);

		BFVM b("", 500, BFVM::OutputStyle::StdOutAndInteralBuffer);
		for (int i = 0; i <bf_chunks.size(); i++)
		{
			printf("%s\n", bf_chunks[i].c_str());
			b.GetMutableInstructions() += bf_chunks[i];

			bool still_running = false;
			do
			{
				still_running = b.Run(steps_per_run);
				b.DebugTape(20);
			} while (still_running);
		}
		printf("%s\n", b.GetInstructions().c_str());
		//*/
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

	void ComputeScore(int target_output_size, const float target_instructions_per_output, const float random_score)
	{
		assert(_vm != nullptr);
		
		float num_setup_instructions = 40.f;
		
		_score = RawComputeScore(
			num_setup_instructions,
			(int)_vm->GetInstructions().size(),
			(int)_vm->GetOutput().size(),
			target_output_size,
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

class BF_Helper
{
public:
	static AStarNode GenerateTestNodeWithInstructions(const AStarNode& previous, const std::string& instructions)
	{
		AStarNode out_new_node = BF_Helper::CloneNode(previous._vm);
		BFVM& vm_ref = *out_new_node._vm;

		vm_ref.GetMutableInstructions() += instructions;

		vm_ref.Run();
		return out_new_node;
	}

	static AStarNode GenerateTestNode(const AStarNode& previous, char next_output, int delta_pos)
	{
		// TODO: Verify we're not going to go off the tape (avoid BF code that wraps around)
		assert((int)previous._vm->GetTapePointer() >= -delta_pos);

		AStarNode out_new_node;

		// early-out if data at target index is too far away to consider
		uint8 data = previous._vm->GetData(delta_pos);
		if (((next_output > data) && (next_output - data > 10)) ||
			((next_output < data) && (data - next_output > 10)))
		{
			// data is too far away to consider this node
			assert(out_new_node._vm == nullptr);
			return out_new_node;
		}

		out_new_node = BF_Helper::CloneNode(previous._vm);
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

		return out_new_node;
	}

	// TODO: The existance of this function makes me sad :(
	static AStarNode CloneNode(const AStarNode& other)
	{
		BFVM* new_vm = VM_POOL.CloneVM(*other._vm);
		AStarNode new_node(new_vm);
		new_node._score = other._score;
		return new_node;
	}
};

enum class EndCondition
{
	FirstSolutionFound,
	SearchExhausted
};

struct TestParams
{
	int search_depth = 0;
	std::string starting_instructions;
	int vm_tape_size = kDefaultTapeSize;
	std::string target_output_string;
	EndCondition end_condition = EndCondition::FirstSolutionFound;
	int64 max_nodes_to_process = 0;

	// AStar specific params?
	// Parameters to control if and how _target_instructions_per_output is adjusted over time
	float target_ipo = 2.6f;
	// How much to increase _current_instructions_per_output if a trim happens without any increase in output length
	float ipo_increment_per_trim = 0.f;	// 0.0001f;
	// How big of a random factor to add to each score for variability
	float random_score = 0.0f;			// 0.001f;

	// parameters for queue trimming.
	// If queue ever gets bigger than 'threshold', the bottom scoring values will be trimmed
	// 'amount' is the percentage of the queue to trim off (0.8 = trim 80% of queue; 0.5 = trim 50%)
	int queue_trim_size_threshold = 0;	// 0 = don't ever trim
	float queue_trim_amount = 0.5f;		// 0.1 = 
};

// Base class to put common functionality
class SearchBase
{
public:
	SearchBase()
	{
	}

	virtual ~SearchBase()
	{
	}

	virtual const char* GetSearchStyleName() const
	{
		return "SearchBase";
	}

	const std::vector<std::string>& GetBestSolutions() const
	{
		return _best_solutions;
	}

	int64 GetNumNodesProcessed() const
	{
		return _num_nodes_processed;
	}

	const float GetRunTimeSeconds() const
	{
		return _run_time_seconds;
	}

	virtual void Run(const TestParams& params)
	{
		// Todo: Make pure virtual?
		assert(false);
	}

	void PrintResults() const
	{
		printf("%s Run Time - %0.4f  (%lld nodes)\n", GetSearchStyleName(), GetRunTimeSeconds(), _num_nodes_processed);
		auto best_solutions = GetBestSolutions();
		if (best_solutions.empty())
		{
			printf("No solution found\n");
		}
		else
		{
			printf("Best Solutions (%d) - %d chars\n", (int)best_solutions.size(), (int)best_solutions[0].size());
			for (int i = 0; i < best_solutions.size(); i++)
			{
				printf("%s\n", best_solutions[i].c_str());
			}
		}
		printf("\n");
	}

protected:
	TestParams _params;
	std::vector<std::string> _best_solutions;
	int64 _num_nodes_processed = 0;
	float _run_time_seconds = 0.f;
};

class AStar : public SearchBase
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

	virtual const char* GetSearchStyleName() const override
	{
		return "AStar";
	}

	const AStarNode* GetTopNode() const
	{
		if (!_queue.empty())
		{
			return &_queue.top();
		}
		return nullptr;
	}
	
	bool IsFinished() const
	{
		switch (_params.end_condition)
		{
		case EndCondition::FirstSolutionFound:
			return GetBestSolutions().empty() == false;
		case EndCondition::SearchExhausted:
			return _queue.empty() ||
				((_params.max_nodes_to_process > 0) && (_num_nodes_processed >= _params.max_nodes_to_process));
		}
		
		assert(false);	// Unhandled end condition!
		return true;
	}

	bool Succeeded() const
	{
		return IsFinished() && !GetBestSolutions().empty();
	}

	virtual void Run(const TestParams& params) override
	{
		// Save off the starting params
		_params = params;
		// Todo: Put validation of _params somewhere more central. In TestParams struct?
		assert(_params.search_depth <= _params.target_output_string.size());
		_current_instructions_per_output = _params.target_ipo;

		// Initialize the 'seed' vm
		_seed_vm.Initialize(_params.starting_instructions.c_str(), _params.vm_tape_size, BFVM::OutputStyle::InternalBuffer);
		_seed_vm.Run();

		// Cache off target output size. This tells us when we're done searching
		_target_output_size = (int)_seed_vm.GetOutput().size() + _params.search_depth;

		SetInitialState(_seed_vm);

		// Start timer
		auto start = std::chrono::high_resolution_clock::now();
		{
			while (!IsFinished())
			{
				ProcessNextNode();
			}
		}
		// End timer
		auto end = std::chrono::high_resolution_clock::now();
		_run_time_seconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000000.0f;
	}

	// Pop the Node with the lowest score and add all its potential next steps
	void ProcessNextNode()
	{
		// Pop the highest scoring node
		const AStarNode top = _queue.top();
		_queue.pop();
		
		const int top_output_size = (int)top._vm->GetOutput().size();
		if ((_length_to_best_solution[top_output_size].size() == 0) ||
			(_length_to_best_solution[top_output_size].size() > top._vm->GetInstructions().size()))
		{
			_length_to_best_solution[top_output_size] = top._vm->GetInstructions();
			_print_all_bests_next_trim = true;
		}

		// Check if top is a solution
		if (top_output_size == _target_output_size)
		{
			const BFVM& solution_vm = *top._vm;
			// Found solution! Compare it with previous best
			const int solution_instruction_size = (int)solution_vm.GetInstructions().size();
			bool print_solution = false;
			bool _debug_output_progress = false;
			if (_best_solutions.empty() || (solution_instruction_size == _best_solutions[0].size()))
			{
				// This is either the first solution or another one of the same size
				_best_solutions.push_back(solution_vm.GetInstructions());
				print_solution = true;
			}
			else if (solution_instruction_size < _best_solutions[0].size())
			{
				_best_solutions.clear();
				_best_solutions.push_back(solution_vm.GetInstructions());
				print_solution = true;
			}
			if (print_solution && _debug_output_progress)
			{
				printf("New Best - %d/%d : %s\n", (int)solution_vm.GetInstructions().size(), (int)solution_vm.GetOutput().size(), solution_vm.GetInstructions().c_str());
			}

			// Free up the VM and early-out
			VM_POOL.FreeVM(top._vm);
			return;
		}

		// Find the next character to output
		// TODO: If the output string is complete, can we stop?!?
		char next_output_char;
		{
			const std::string& top_output = top._vm->GetOutput();
			// Make sure we're not searching beyond the seed output size + search depth
			assert(top_output.size() < _target_output_size);

			next_output_char = _params.target_output_string[top_output.size()];
		}

		bool use_big_shift_nodes = false;
		if (use_big_shift_nodes)
		{
			TryAddNodeToQueue(BF_Helper::GenerateTestNodeWithInstructions(top, "[<]>"));
			TryAddNodeToQueue(BF_Helper::GenerateTestNodeWithInstructions(top, "[>]<"));
		}

		for (int i = -5; i <= 5; i++)
		{
			if ((int)top._vm->GetTapePointer() >= -i)
			{
				AStarNode next_node = BF_Helper::GenerateTestNode(top, next_output_char, i);
				TryAddNodeToQueue(next_node);
			}
		}

		// DIRTY: We're done with the VM associated with the top node; clean it up
		// TODO: Replace this with refcounting?
		VM_POOL.FreeVM(top._vm);

		_num_nodes_processed++;

		// Trim queue if it gets too big
		if ((_params.queue_trim_size_threshold > 0) && ((int)_queue.size() >= _params.queue_trim_size_threshold))
		{
			TrimQueue();
		}
	}

protected:
	void SetInitialState(const BFVM& vm)
	{
		// Clone the passed in VM and push it on the A* search queue
		BFVM* new_vm = VM_POOL.CloneVM(vm);
		AStarNode new_node(new_vm);
		bool added = TryAddNodeToQueue(new_node);
		assert(added);
	}

	bool TryAddNodeToQueue(AStarNode node)
	{
		// don't use node if its vm is null'
		if (node._vm == nullptr)
		{
			return false;
		}
		
		if (!_best_solutions.empty())
		{
			if (node._vm->GetInstructions().size() >= _best_solutions[0].size())
			{
				// new node is worse than knows solution; punt
				VM_POOL.FreeVM(node._vm);
				node._vm = nullptr;
				return false;
			}
		}
		
		node.ComputeScore(_target_output_size, _current_instructions_per_output, _params.random_score);

		if (_check_hash == false)
		{
			_queue.push(node);
		}
		else
		{
			Hash h = node._vm->CalculateHash();
			float previous_best = _hash_to_best_score[h.GetValue()];
			// If "previous_best" is exactly 0.0, we assume it's just the default value and still add the node
			if ((previous_best == 0.0f) || (node._score > previous_best))
			{
				_hash_to_best_score[h.GetValue()] = node._score;
				_queue.push(node);
			}
			else
			{
				// Hash collided, don't add to queue and free up VM from pool
				VM_POOL.FreeVM(node._vm);
				node._vm = nullptr;
			}
		}
		return true;
	}
	
	void TrimQueue()
	{
		static bool kTrimEveryOther = false;
		static bool kTrimPerOutputLength = true;
		assert(_params.queue_trim_amount >= 0.f && _params.queue_trim_amount <= 1.f);

		static bool print_stats = false;
		if (print_stats)
		{
			PrintDebugQueueStats();
		}

		if (_print_all_bests_next_trim && !kHideAllDebug)
		{
			PrintDebugAllBests();
			_print_all_bests_next_trim = false;
		}

		// If the output size of the best node hasn't gotten longer, adjust the scoring function parameters
		int top_output_size = (int)_queue.top()._vm->GetOutput().size();
		if (top_output_size <= _longest_top_output_length)
		{
			// Output didn't get longer, increase scoring function parameters
			_current_instructions_per_output += _params.ipo_increment_per_trim;
		}
		else
		{
			// Track output length of top node
			_longest_top_output_length = top_output_size;
			// Output improved, reset scoring function parameters
			_current_instructions_per_output = _params.target_ipo;
		}

		int num_to_keep = (int)(_params.queue_trim_size_threshold * _params.queue_trim_amount);
		std::map<int, int> size_to_num_to_keep;

		if (kTrimPerOutputLength)
		{
			int kKeepAllThreshold = 500;

			// First, count how many nodes there are per output length
			auto list = _queue.GetRawList();
			for (int i = 0; i < list.size(); i++)
			{
				int size = (int)list[i]._vm->GetOutput().size();
				size_to_num_to_keep[size]++;
			}

			// Then, scale the num to keep by the trim amount
			// also, recalculate num_to_keep
			num_to_keep = 0;
			for (auto it : size_to_num_to_keep)
			{
				// Only trim this size bucket if there are more than the threshold allows
				if (it.second > kKeepAllThreshold)
				{
					int num_for_size = (int)std::ceil(it.second * _params.queue_trim_amount);
					num_to_keep += num_for_size;
					size_to_num_to_keep[it.first] = num_for_size;
				}
			}
		}

		std::vector<AStarNode> nodes_to_keep;
		nodes_to_keep.reserve(num_to_keep);
		if (kPrintfSpamTrimUpdates)
		{
			printf("Iterations:%d - queue size:%d", (int)_num_nodes_processed, (int)_queue.size());
		}

		if (kTrimEveryOther)
		{
			int64 num_processed = 0;
			while (!_queue.empty())
			{
				num_processed++;
				const AStarNode& top = _queue.top();

				// Decide if top should go in the keep list or be freed
				float current_trim_amount = nodes_to_keep.size() / (float)num_processed;
				if (current_trim_amount < _params.queue_trim_amount)
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
			while (nodes_to_keep.size() < num_to_keep)
			{
				AStarNode top = _queue.top();
				if (kTrimPerOutputLength)
				{
					int size = (int)top._vm->GetOutput().size();
					if (size_to_num_to_keep[size] > 0)
					{
						// Keep the best 'x' scoring nodes for each size
						nodes_to_keep.push_back(top);
						size_to_num_to_keep[size]--;
					}
					else
					{
						// We already have enough of this size, just release the vm
						VM_POOL.FreeVM(top._vm);
					}
				}
				else
				{
					// Keep the best 'x' scoring nodes from the queue
					nodes_to_keep.push_back(top);
				}
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

		if (kPrintfSpamTrimUpdates)
		{
			printf("->%d", (int)_queue.size());
			printf(" : i/c=%d/%d : %0.2f\n", (int)_queue.top()._vm->GetInstructions().size(), (int)_queue.top()._vm->GetOutput().size(), _current_instructions_per_output);
		}
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
	
	void PrintDebugQueueStats()
	{
		std::map<int, int> count;
		auto list = _queue.GetRawList();
		for (int i = 0; i < list.size(); i++)
		{
			int size = (int)list[i]._vm->GetOutput().size();
			count[size]++;
		}
		for (auto it = count.begin(); it != count.end(); it++)
		{
			printf("(%d:%d)", it->first, it->second);
		}
		printf("\n");
	}

	void PrintDebugAllBests()
	{
		int previous_key = -1;
		for (auto it = _length_to_best_solution.rbegin(); it != _length_to_best_solution.rend(); it++)
		{
			std::string& previous_string = _length_to_best_solution[previous_key];
			if (previous_key == -1 || strncmp(previous_string.c_str(), it->second.c_str(), it->second.size()) != 0)
			{
				printf("%d : %s\n", it->first, it->second.c_str());
			}
			previous_key = it->first;
		}
	}

protected:
	BFVM _seed_vm;
	int _target_output_size = 0;

	//std::priority_queue<AStarNode> _queue;
	PriorityQueue<AStarNode> _queue;
	std::unordered_map<int64, float> _hash_to_best_score;
	bool _check_hash = true;

	float _current_instructions_per_output = 0.f;

	std::map<int, std::string> _length_to_best_solution;
	bool _print_all_bests_next_trim = false;

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

void AnalyzeInstructions(std::string initial_state, std::string instructions)
{
	int chars_per_line = 20;

	printf("Initial state ------------------------------------\n");
	printf("%s\n", initial_state.c_str());
	BFVM vm(initial_state.c_str(), kDefaultTapeSize, BFVM::OutputStyle::InternalBuffer);
	vm.Run();
	vm.DebugTape(chars_per_line);

	printf("Instructions -------------------------------------\n");
	printf("%s\n", instructions.c_str());
	vm.Initialize(instructions.c_str(), kDefaultTapeSize, BFVM::OutputStyle::InternalBuffer);
	vm.Run();
	printf("Output -----\n%s\n", vm.GetOutput().c_str());
	vm.DebugTape(chars_per_line);

	bool analyze_output = true;
	if (analyze_output)
	{
		std::string non_initial_instructions = instructions.substr(initial_state.size());
		const char* op = vm.GetOutput().c_str();
		const char* ip = non_initial_instructions.c_str();
		while (*op)
		{
			printf("'%c' - ", *op);
			std::string inst = "";
			do
			{
				inst += *ip;
			} while (*ip++ != '.');
			printf("%d - %s\n", (int)inst.size(), inst.c_str());
			op++;
		}
	}
}

class BruteForce : public SearchBase
{
public:
	BruteForce() {}

	virtual const char* GetSearchStyleName() const override
	{
		return "Brute Force";
	}

	virtual void Run(const TestParams& params) override
	{
		// Save off the starting params
		_params = params;
		// Todo: Put validation of _params somewhere more central. In TestParams struct?
		assert(_params.search_depth <= _params.target_output_string.size());

		// Create and seed a vm
		BFVM vm(params.starting_instructions.c_str(), params.vm_tape_size, BFVM::OutputStyle::InternalBuffer);
		vm.Run();

		SetInitialState(vm);

		// Start timer
		auto start = std::chrono::high_resolution_clock::now();
		{
			_cutoff_instruction_count = CalculateInitialInstructionCutoffCount(*_initial_state, params.search_depth);
			//printf("Cutoff Instruction Count = %d\n", _cutoff_instruction_count);
			_num_nodes_processed = 0;
			ProcessNode(*_initial_state, params.search_depth);
		}
		// End timer
		auto end = std::chrono::high_resolution_clock::now();
		_run_time_seconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000000.0f;
	}

	void ProcessNode(BFVM& node, int depth)
	{
		_num_nodes_processed++;
		if (depth == 0)
		{
			const int node_instruction_size = (int)node.GetInstructions().size();
			bool print_solution = false;
			if (_best_solutions.empty() || (node_instruction_size == _best_solutions[0].size()))
			{
				_best_solutions.push_back(node.GetInstructions());
				print_solution = true;
			}
			else if (node_instruction_size < _best_solutions[0].size())
			{
				_best_solutions.clear();
				_best_solutions.push_back(node.GetInstructions());
				print_solution = true;
			}
			if ((_cutoff_instruction_count == 0) || (node_instruction_size < _cutoff_instruction_count))
			{
				_cutoff_instruction_count = node_instruction_size;
			}
			if (print_solution && _debug_output_progress)
			{
				printf("New Best - %d/%d : %s\n", (int)node.GetInstructions().size(), (int)node.GetOutput().size(), node.GetInstructions().c_str());
			}
			return;
		}

		char next_output_char = _params.target_output_string[node.GetOutput().size()];

		for (int index = 0; index <= 10; index++)
		{
			if (ShouldEarlyOut())
			{
				break;
			}
			const int i = ((index + 1) >> 1) * ((index & 1) ? 1 : -1);
			if ((int)node.GetTapePointer() >= -i)
			{
				AStarNode next_node = BF_Helper::GenerateTestNode(&node, next_output_char, i);
				if (next_node._vm != nullptr)
				{
					// Skip nodes that have or will exceeded the instruction count cutoff
					if ((_cutoff_instruction_count == 0) ||
						((int)next_node._vm->GetInstructions().size() <= _cutoff_instruction_count - (depth - 1)))
					{
						ProcessNode(*next_node._vm, depth - 1);
					}
					VM_POOL.FreeVM(next_node._vm);
				}
			}
		}
	}

	bool IsFinished() const
	{
		return true;
	}

protected:
	void SetInitialState(const BFVM& vm)
	{
		// Clone the passed in VM
		if (_initial_state != nullptr)
		{
			VM_POOL.FreeVM(_initial_state);
		}
		_initial_state = VM_POOL.CloneVM(vm);
	}

	bool ShouldEarlyOut() const
	{
		switch (_params.end_condition)
		{
		case EndCondition::FirstSolutionFound:
			return !_best_solutions.empty();
		case EndCondition::SearchExhausted:
			return (_params.max_nodes_to_process > 0) && (_num_nodes_processed >= _params.max_nodes_to_process);
		}
		assert(false);	// Unhandled end condition!
		return true;
	}

	int CalculateInitialInstructionCutoffCount(const BFVM& node, const int depth)
	{
		// Copy all test Params from the BruteForce search except always use the first solution
		TestParams astar_params(_params);
		astar_params.end_condition = EndCondition::FirstSolutionFound;
		astar_params.max_nodes_to_process = 0;	// no limit
		astar_params.target_ipo = 2.6f;

		astar_params.queue_trim_size_threshold = 200 * 1000;	// 0 = don't ever trim
		astar_params.queue_trim_amount = 0.5f;

		AStar a;
		a.Run(astar_params);

		int instruction_cutoff_count = 0;
		if (!a.GetBestSolutions().empty())
		{
			const std::string& str = a.GetBestSolutions()[0];
			instruction_cutoff_count = (int)str.size();
		}
		else
		{
			assert(false);	// try to not let this happen
		}
		return instruction_cutoff_count;
	}

protected:
	BFVM* _initial_state = nullptr;

	int _cutoff_instruction_count = 0;
	bool _debug_output_progress = false;
};

void TestAStar()
{
	// Good looking starting patterns
	//{ 257, 371, 713, 137, 319, 852, 471, 183, 528, 570, 441, 318, 409, 147, 742, 802, 681, 258, 481, 816, 462, 509, 580, 806, 804, 484, 168, 169, 414, 609, 294, 263, 27, 328, 714, 185, 361, 72, 406, 550, 780, 270, 480, 832, 814, 249, 841, 492, 770, 144, 807, 616, 419, 16, 390, 640, 28, 539, 831, 429, 17, 38, 81, 184};

	TestParams t;
	t.search_depth = 16;	// 1002 = 1000 decimal places + "1."
	t.starting_instructions = BuildBFPattern(257);
	t.vm_tape_size = 400;
	t.target_output_string = SQRT_2;
	t.end_condition = EndCondition::SearchExhausted;
	t.max_nodes_to_process = 1000 * 1000;	// 0 = no limit

	// AStar specific parameters
	t.target_ipo = 2.6f;		// Instructions per output
	t.random_score = 0.0f; // 0.001f;

	t.ipo_increment_per_trim = 0.f;	// 0.0001f;
	t.queue_trim_size_threshold = 200 * 1000;	// 0 = don't ever trim
	t.queue_trim_amount = 0.5f;


	AStar a;
	a.Run(t);

	a.PrintResults();
}

void TestBruteForce()
{
	TestParams t;
	t.search_depth = 15;
	t.starting_instructions = BuildBFPattern(257);
	t.vm_tape_size = 400;
	t.target_output_string = SQRT_2;
	t.end_condition = EndCondition::SearchExhausted;
	t.max_nodes_to_process = 0;	// 0 = no limit

	BruteForce b;
	b.Run(t);

	b.PrintResults();
}

std::string TrimLastOutputs(const std::string& instructions, const int num_outputs_tp_trim)
{
	int num_to_find = num_outputs_tp_trim;
	int i = (int)instructions.size() - 1;
	while ((num_to_find > 0) || (instructions[i] != '.'))
	{
		if (instructions[i] == '.')
		{
			num_to_find--;
		}
		i--;
	}
	return instructions.substr(0, (int64)(i) + 1);
}

void IterativeSolution()
{
	const bool kUseBruteForce = false;

	const float kOutputIterationScalar = 0.05f;
	const int kNumRandomIterations = 50;

	TestParams t;
	t.search_depth = 250;
	t.starting_instructions = BuildBFPattern(257);
	t.vm_tape_size = 400;
	t.target_output_string = SQRT_2;
	t.end_condition = kUseBruteForce ? EndCondition::SearchExhausted : EndCondition::FirstSolutionFound;
	t.max_nodes_to_process = 0;	// 0 = no limit

	// AStar-specific params
	t.target_ipo = 2.5f;
	t.ipo_increment_per_trim = 0.f;	// 0.0001f;
	t.random_score = 0.0f;		// 0.001f;
	t.queue_trim_size_threshold = 500 * 1000;	// 0 = don't ever trim
	t.queue_trim_amount = 0.5f;		// 0.1 = 

	std::vector<TestParams> scenarios_to_test;

	scenarios_to_test.push_back(t);

	int target_output_size = 0;
	// Num output chars to trim per iteration
	const int output_trim = (int)(t.search_depth * (1.0f - kOutputIterationScalar));
	int iteration = 0;

	while (target_output_size < (int)t.target_output_string.size())
	{
		target_output_size = std::min(target_output_size + t.search_depth, (int)t.target_output_string.size());
		printf("Iteration %d  : Target output %d\n", ++iteration, target_output_size);
		std::set<std::string> next_scenarios;
		int min_length = 0;
		for (int i = 0; i < scenarios_to_test.size(); i++)
		{
			int num_random_iterations = (t.random_score != 0.0f) ? kNumRandomIterations : 1;
			for (int random_iteration = 0; random_iteration < num_random_iterations; random_iteration++)
			{
				SearchBase* search = nullptr;
				if (kUseBruteForce)
				{
					search = new BruteForce();
				}
				else
				{
					search = new AStar();
				}

				search->Run(scenarios_to_test[i]);
				search->PrintResults();

				for (std::string s : search->GetBestSolutions())
				{
					std::string next_input = TrimLastOutputs(s, output_trim);
					min_length = (min_length == 0 || min_length > (int)next_input.size()) ? (int)next_input.size() : min_length;
					next_scenarios.insert(next_input);
				}

				delete search;
			}
		}

		scenarios_to_test.clear();
		// Todo: Move this completion check somewhere better
		if (target_output_size < (int)t.target_output_string.size())
		{
			target_output_size -= output_trim;

			for (std::string s : next_scenarios)
			{
				if (s.size() <= min_length)
				{
					t.starting_instructions = s;
					scenarios_to_test.push_back(t);
					if (target_output_size + scenarios_to_test.back().search_depth > (int)t.target_output_string.size())
					{
						scenarios_to_test.back().search_depth = (int)t.target_output_string.size() - target_output_size;
					}
				}
			}
		}
	}
}

int main(int argc, char *argv[])
{
	//int seed = (int)time(nullptr);
	//printf("seed = %ld\n", seed);
	//srand(seed);
	
	BFVM::TestVM();
	//TestAStar();
	//TestBruteForce();
	IterativeSolution();

	/*
	//AnalyzeInstructions(
		"-[->>[>]>-[<+>-----]<[<]<]>>[->++>++++>]",
		"-[->>[>]>-[<+>-----]<[<]<]>>[->++>++++>]<<<-.---.<<-.<-.>.<+.-.++.<<.+.<.+.<.>.---.<++.<.<--."
	);
	//*/
}
