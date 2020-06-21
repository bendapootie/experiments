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

struct Scores
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
    Sequence guess_;
    std::vector<Scores> scores_;
};

int main()
{
    printf("Num Sequences = %d\n", Sequence::GetNumSequences());
    for (int i = 0; i < Sequence::GetNumSequences(); i++)
    {
        Sequence s = Sequence::GetSequenceFromIndex(i);
        s.Print();
    }
}

/*
enum class GuessState
{
    Valid,
    Invalid
};

struct Guess
{
public:
    static const int kNumSlots = 4;
    Guess(unsigned char a_, unsigned char b_, unsigned char c_, unsigned char d_)
    {
        g_[0] = a_;
        g_[1] = b_;
        g_[2] = c_;
        g_[3] = d_;
        state_ = GuessState::Valid;
    }

    // Returns true if 'this' is possible given the guess and 'right_color/right_place'
    // of the passed in guess_
    bool IsPossbileSolution(const Guess& guess_) const
    {
        Guess test_guess(*this);

        // For each slot, figure out how many are right_place
        for (int slot = 0; slot < kNumSlots; slot++)
        {
            if (test_guess.g_[slot] == guess_.g_[slot])
            {
                test_guess.right_place_ += 1;
            }
        }
        return true;
    }

    union
    {
        unsigned int guess_;
        unsigned char g_[kNumSlots];
    };
    unsigned char right_color_ = 0;
    unsigned char right_place_ = 0;

    GuessState state_;
};

class Solutions
{
public:
    void GenerateAllSolutions()
    {
        guesses_.clear();
        for (int a = 0; a < 6; a++)
        {
            for (int b = 0; b < 6; b++)
            {
                for (int c = 0; c < 6; c++)
                {
                    for (int d = 0; d < 6; d++)
                    {
                        guesses_.push_back(Guess(a, b, c, d));
                    }
                }
            }
        }
    }

    void AddPossibleSolutions(const Guess& guess_, const Solutions& set_)
    {
        for (int i = 0; i < set_.guesses_.size(); i++)
        {
            const Guess& set_guess = set_.guesses_[i];
            if (set_guess.IsPossbileSolution(guess_))
            {
                guesses_.push_back(set_guess);
            }
        }
    }

private:
    std::vector<Guess> guesses_;
};
*/
/*
int main()
{
    Solutions solutions;
    solutions.GenerateAllSolutions();

    Solutions possible;
    Guess guess(0, 0, 0, 0);
    guess.right_color_ = 0;
    guess.right_place_ = 1;
    possible.AddPossibleSolutions(guess, solutions);
}
*/