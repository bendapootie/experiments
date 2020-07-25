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
	
	bool IsSolved() const
	{
		return domain_.Size() == 1;
	}
	
	void MakeGuess(const Sequence& guess_sequence)
	{
		guesses_.push_back(guess_sequence);
		Guess& new_guess = guesses_.back();
		new_guess.score_ = Score::CalculateScore(new_guess.guess_, solution_);
        //printf("Guess - ");
        //guess_sequence.Print();
        //printf(" score = %d,%d", new_guess.score_.right_position_, new_guess.score_.right_color_);

        // Update the possible domain given the new guess and score
        if (guesses_.size() == 1)
        {
            BuildDomain();
        }
        else
        {
            UpdateDomain(new_guess);
        }
        //printf(" domain(%d)", domain_.Size());
        //printf("\n");
    }
    
    // Given the current state of the game, returns the guess that will most dramatically reduce the domain size
    Sequence FindBestGuess() const
    {
    	// For every possible guess, see how much it will reduce the domain for each possible solution.
    	// Choose the guess that results in the smallest maximum domain size
    	Game temp_dupe;
    	const Game* reference_game = this;
    	printf("Game domain size = %d\n", domain_.Size());
    	if (domain_.Size() == 0)
    	{
    		temp_dupe = *this;
    		temp_dupe.BuildDomain();
    		reference_game = &temp_dupe;
    	}
    	printf("Ref game domain size = %d\n", reference_game->domain_.Size());
    	Sequence best_sequence;
    	int best_sequence_domain_size = Sequence::GetNumSequences() + 1;
    	for (int i = 0; i < Sequence::GetNumSequences(); i++)
    	{
    		Sequence guess = Sequence::GetSequenceFromIndex(i);
    		int max_domain_index = 0;
    		std::vector<Game> games;
    		for (int j = 0; j < reference_game->domain_.Size(); j++)
    		{
    			games.emplace_back(*reference_game);
    			Game& g = games.back();
    			g.solution_ = reference_game->domain_[j];
    			g.MakeGuess(guess);
    			if (g.domain_.Size() > games[max_domain_index].domain_.Size())
    			{
    				max_domain_index = j;
    			}
    		}
    		
    		assert(max_domain_index < games.size());
    		const Game& best_game = games[max_domain_index];
    		const Guess& best_guess = best_game.guesses_.back();
    		const int best_domain_size = best_game.domain_.Size();
    		
    		if (best_domain_size < best_sequence_domain_size)
    		{
    			best_sequence = best_guess.guess_;
    			best_sequence_domain_size = best_domain_size;
	    		printf("best guess - ");
	    		best_sequence.Print();
	    		printf(" Domain size = %d\n", best_sequence_domain_size);
    		}
    	}
    	return best_sequence;
    }
	
protected:
    // Builds domain based on what's in the guesses_ list
    void BuildDomain()
    {
        domain_.Clear();
        
        // If no guesses, add all sequences to domain
        if (guesses_.size() == 0)
        {
            for (int i = 0; i < Sequence::GetNumSequences(); i++)
            {
                domain_.Add(Sequence::GetSequenceFromIndex(i));
            }
        }

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

int main()
{
    printf("Num Sequences = %d\n", Sequence::GetNumSequences());
    Game g(Sequence(0, 0, 0, 0));
    g.MakeGuess(Sequence(0, 0, 1, 1));
    //g.MakeGuess(Sequence(5, 5, 2, 1));
    //g.MakeGuess(Sequence(4, 0, 0, 5));
    //g.MakeGuess(Sequence(1, 5, 5, 5));
    //g.MakeGuess(Sequence(4, 5, 5, 5));
    while (g.IsSolved() == false)
    {
    	Sequence next_guess = g.FindBestGuess();
    	next_guess.Print();
    	printf("\n");
    	g.MakeGuess(next_guess);
    }
}
