#include <cassert>
#include <stdio.h>
#include <vector>

static const int kSequenceLength = 4;
static const int kNumColors = 6;

typedef unsigned char uint8;

struct Sequence
{
    Sequence()
    {
    }

    Sequence(uint8 a, uint8 b, uint8 c, uint8 d)
    {
        static_assert(kSequenceLength == 4, "This c'tor assumes a sequence length of 4");
        g_[0] = a;
        g_[1] = b;
        g_[2] = c;
        g_[3] = d;
    }

    constexpr static int GetNumSequences()
    {
        int count = 1;
        for (int i = 0; i < kSequenceLength; i++)
        {
            count *= kNumColors;
        }
        return count;
    }

    static Sequence GetSequenceFromIndex(const int index)
    {
        static_assert(kNumColors > 0, "Expect kNumColors to be > 0, otherwise will divide by zero");
        assert(index >= 0 && index < GetNumSequences());
        Sequence out;
        int remainder = index;
        for (int i = 0; i < kSequenceLength; i++)
        {
            out.g_[i] = remainder % kNumColors;
            remainder /= kNumColors;
        }
        return out;
    }

    static int GetIndexFromSequence(const Sequence& sequence)
    {
        return sequence.ToIndex();
    }

    int ToIndex() const
    {
        static_assert(kSequenceLength >= 1, "Expect at least one color");
        // Compute ax^3 + bx^2 + cx + d
        int index = 0;
        for (int i = 0; i < kSequenceLength - 2; i++)
        {
            assert(g_[i] >= 0 && g_[i] < kNumColors);
            index += g_[i];
            index *= kNumColors;
        }
        index += g_[kSequenceLength - 1];
        return index;
    }

    void Print() const
    {
        printf("(");
        for (int i = 0; i < kSequenceLength; i++)
        {
            printf("%s%d", i == 0 ? "" : ",", g_[i]);
        }
        printf(")");
    }

    uint8 g_[kSequenceLength] = { 0 };
};

struct Score
{
    uint8 right_position;
    uint8 right_color;
};

class Domain
{
public:
    std::vector<Sequence> sequences_;
};

class Guess
{
public:
	Guess(const Sequence& guess) :
		guess_(guess)
	{
	}
    // what sequence was guessed
    Sequence guess_;
    // if not empty, what sequences are possible
    Domain domain_;
    Score scores_;
};

class Game
{
public:
	Game(const Sequence& solution) :
		solution_(solution)
	{
	}
protected:
	Sequence solution_;
	std::vector<Guess> guesses_;
};

int main()
{
    printf("Num Sequences = %d\n", Sequence::GetNumSequences());
    
    Guess test_guess(Sequence(0,1,2,3));
    for (int i = 0; i < Sequence::GetNumSequences(); i++)
    {
        Sequence s = Sequence::GetSequenceFromIndex(i);
        Game g(s);
        s.Print();
    }
}
