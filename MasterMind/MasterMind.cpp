#include <cassert>
#include <stdio.h>
#include <vector>

typedef char int8;
typedef unsigned char uint8;

static const int kSequenceLength = 4;
static const int kNumColors = 6;
static const unsigned char kInvalidColor = -1;

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
	static Score CalculateScore(const Sequence& guess, const Sequence& solution)
	{
		Score out_score;
		Sequence temp_guess(guess);
        Sequence temp_solution(solution);

        // Find parts of the guess in the right position
		for (int i = 0; i < kSequenceLength; i++)
		{
			if (temp_guess.g_[i] == temp_solution.g_[i])
			{
				temp_guess.g_[i] = kInvalidColor;
                temp_solution.g_[i] = kInvalidColor;
				out_score.right_position_++;
			}
		}

        // Find parts that are the right color but wrong position
        for (int i = 0; i < kSequenceLength; i++)
        {
            if (temp_guess.g_[i] != kInvalidColor)
            {
                for (int j = 0; j < kSequenceLength; j++)
                {
                    if (temp_guess.g_[i] == temp_solution.g_[j])
                    {
                        temp_guess.g_[i] = kInvalidColor;
                        temp_solution.g_[j] = kInvalidColor;
                        out_score.right_color_++;
                        break;
                    }
                }
            }
        }

		return out_score;
	}
	
	bool operator == (const Score& rhs) const
	{
		return (right_position_ == rhs.right_position_) && (right_color_ == rhs.right_color_);
	}
	
    int8 right_position_ = 0;
    int8 right_color_ = 0;
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
	
	void CalculateDomain()
	{
	    for (int i = 0; i < Sequence::GetNumSequences(); i++)
	    {
	        Sequence s = Sequence::GetSequenceFromIndex(i);
	        Score score = Score::CalculateScore(guess_, s);
	        if (score == score_)
	        {
	        	domain_.sequences_.push_back(s);
	        }
	    }
	    printf("Domain size = %d\n", domain_.sequences_.size());
	}
	
    // what sequence was guessed
    Sequence guess_;
    // if not empty, what sequences are possible
    Domain domain_;
    Score score_;
};

class Game
{
public:
	Game(const Sequence& solution) :
		solution_(solution)
	{
	}
	
	void MakeGuess(const Sequence& guess_sequence)
	{
		guesses_.push_back(guess_sequence);
		Guess& new_guess = guesses_.back();
		new_guess.score_ = Score::CalculateScore(new_guess.guess_, solution_);
		new_guess.CalculateDomain();
		printf("score = %d,%d", new_guess.score_.right_position_, new_guess.score_.right_color_);
	}
		
protected:
	Sequence solution_;
	std::vector<Guess> guesses_;
};

int main()
{
    printf("Num Sequences = %d\n", Sequence::GetNumSequences());
    
    Guess test_guess(Sequence(0,1,2,3));
    Game g(Sequence(2,4,5,5));
    g.MakeGuess(Sequence(1,2,3,4));
    /*
    for (int i = 0; i < Sequence::GetNumSequences(); i++)
    {
        Sequence s = Sequence::GetSequenceFromIndex(i);
        Game g(s);
        s.Print();
    }
    */
}
