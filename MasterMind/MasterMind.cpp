#include <cassert>
#include <stdio.h>
#include <vector>
#include <map>

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

    uint8 g_[kSequenceLength] = { kInvalidColor };
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

    bool operator != (const Score& rhs) const
    {
        return !(*this == rhs);
    }

    bool operator < (const Score& rhs) const
    {
        if (right_position_ < rhs.right_position_)
        {
            return true;
        }
        if (right_position_ == rhs.right_position_)
        {
            return right_color_ < rhs.right_color_;
        }
        return false;
    }
	
    int8 right_position_ = 0;
    int8 right_color_ = 0;
};

class Domain
{
public:
    int Size() const
    {
        return (int)sequences_.size();
    }

    void Clear()
    {
        sequences_.clear();
    }

    void Add(const Sequence& sequence)
    {
        sequences_.push_back(sequence);
    }

    // Removes the Sequence at the given index in constant time, but doesn't maintain list order
    void RemoveAtSwap(const int index)
    {
        assert(index >= 0 && index < sequences_.size());
        sequences_[index] = sequences_[sequences_.size() - 1];
        sequences_.pop_back();
    }

    const Sequence& operator[] (int index) const
    {
        return sequences_[index];
    }

protected:
    std::vector<Sequence> sequences_;
};

class Guess
{
public:
    Guess(const Sequence& guess) :
        guess_(guess)
    {
    }

    Guess(const Sequence& guess, const Score& score) :
        guess_(guess),
        score_(score)
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
	        	domain_.Add(s);
	        }
	    }
	    printf("Domain size = %d\n", (int)domain_.Size());
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
    Game()
    {
    }

	Game(const Sequence& solution) :
		solution_(solution)
	{
	}

	void MakeGuess(const Sequence& guess_sequence)
	{
		guesses_.push_back(guess_sequence);
		Guess& new_guess = guesses_.back();
		new_guess.score_ = Score::CalculateScore(new_guess.guess_, solution_);
        printf("Guess - ");
        guess_sequence.Print();
        printf(" score = %d,%d", new_guess.score_.right_position_, new_guess.score_.right_color_);

        // Update the possible domain given the new guess and score
        if (guesses_.size() == 1)
        {
            BuildDomain();
        }
        else
        {
            UpdateDomain(new_guess);
        }
        printf(" domain(%d)", domain_.Size());
        printf("\n");
    }
	
protected:
    // Builds domain based on what's in the guesses_ list
    void BuildDomain()
    {
        domain_.Clear();

        // Calculate the domain from the first guess
        if (guesses_.size() > 0)
        {
            const Guess& guess = guesses_[0].guess_;
            for (int i = 0; i < Sequence::GetNumSequences(); i++)
            {
                Sequence s = Sequence::GetSequenceFromIndex(i);
                Score score = Score::CalculateScore(guess.guess_, s);
                // Add possible sequences to domain
                if (score == guess.score_)
                {
                    domain_.Add(s);
                }
            }
        }

        // Remove non-relavent sequences from domain based on subsequent guesses
        for (int i = 1; i < guesses_.size(); i++)
        {
            UpdateDomain(guesses_[i]);
        }
    }

    // Removes impossible sequences from the domain based on a new guess/score
    void UpdateDomain(const Guess& new_guess)
    {
        for (int i = 0; i < domain_.Size(); i++)
        {
            const Sequence& s = domain_[i];
            Score score = Score::CalculateScore(new_guess.guess_, s);
            // Remove no longer possible sequenes from domain
            if (score != new_guess.score_)
            {
                domain_.RemoveAtSwap(i);
            }
        }
    }

protected:
	Sequence solution_;
	std::vector<Guess> guesses_;
    // What sequences are possible given the list of guesses
    Domain domain_;
};

void EvaluateGuess(const Sequence& guess)
{
    // A* problem space
    // - Decision is sequence to guess
    // - Heuristic is number of guesses; then domain size
    // - Domain size of 1 is success

    // Make a game for each potential solution
    std::vector<Game> games;
    for (int i = 0; i < Sequence::GetNumSequences(); i++)
    {
        games.push_back(Game(Sequence::GetSequenceFromIndex(i)));
    }

    // Evaluate how each game will go given the passed in guess
    for (auto it = games.begin(); it < games.end(); it++)
    {
        it->MakeGuess(guess);
    }

/*
    // Count how many solutions with each score there are
    std::map<Score, int> histogram;
    for (int j = 0; j < Sequence::GetNumSequences(); j++)
    {
        Sequence solution = Sequence::GetSequenceFromIndex(j);
        Score s = Score::CalculateScore(guess, solution);
        histogram[s]++;
    }
    guess.Print();
    printf(" - ");
    for (auto it = histogram.begin(); it != histogram.end(); it++)
    {
        printf("(%d,%d):%d ", it->first.right_position_, it->first.right_color_, it->second);
    }
    printf("\n");
*/
}

void FindBestGuess()
{
    for (int i = 0; i < Sequence::GetNumSequences(); i++)
    {
        Sequence guess = Sequence::GetSequenceFromIndex(i);
        EvaluateGuess(guess);
    }
}

int main()
{
    printf("Num Sequences = %d\n", Sequence::GetNumSequences());
    
    Game g(Sequence(2, 2, 3, 4));
    g.MakeGuess(Sequence(1, 3, 2, 0));
    g.MakeGuess(Sequence(2, 3, 4, 2));
    g.MakeGuess(Sequence(3, 1, 5, 2));

    //EvaluateGuess(Sequence(0, 1, 2, 3));

    /*
    Guess test_guess(Sequence(0,1,2,3));
    Game g(Sequence(2,4,5,5));
    g.MakeGuess(Sequence(1,2,3,4));
    */
    /*
    for (int i = 0; i < Sequence::GetNumSequences(); i++)
    {
        Sequence s = Sequence::GetSequenceFromIndex(i);
        Game g(s);
        s.Print();
    }
    */
}
