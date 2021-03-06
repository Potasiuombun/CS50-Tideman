#include <cs50.h>
#include <stdio.h>
#include <string.h>
// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool cycle(int original_winner, int winner, int loser);
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);


int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    // A, B, C -> candidate[0] = A, candidate[1] = B, candidate[2] = C
    //
    for (int i = 0; i < candidate_count; i++)
    {
        // Checks if the voter chooses a valid name for the candidate and if they do update ranks[rank] with the index
        if (strcmp(name, candidates[i]) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (i < j)
            preferences[ranks[i]][ranks[j]]++;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i+1; j < candidate_count; j++)
        {
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;
            }
            else if (preferences[j][i] > preferences[i][j])
            {
                pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;
                pair_count++;
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    // TODO
  int swap_count = -1;
  pair temp[pair_count];
  while(swap_count != 0)
  {
      swap_count = 0;
      for (int i = 0; i < pair_count; i++)
      {
          for (int j = i + 1; j < pair_count; j++)
          {
              if (preferences[pairs[i].winner][pairs[i].loser] < preferences[pairs[j].winner][pairs[j].loser])
              {
                    temp[i].winner = pairs[j].winner;
                    temp[i].loser = pairs[j].loser;

                    pairs[j].winner = pairs[i].winner;
                    pairs[j].loser = pairs[i].loser;

                    pairs[i].winner = temp[i].winner;
                    pairs[i].loser = temp[i].loser;
                    swap_count++;
              }
          }
      }
  }
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{   
    
    for (int i = 0; i < pair_count; i++)
    {   
    
        if (!cycle(pairs[i].winner, pairs[i].winner, pairs[i].loser))
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
       
    }
    return;
}

// Print the winner of the election
void print_winner(void)
{   int winner[candidate_count];
    
    for (int i = 0; i < candidate_count; i++)
    {
        winner[i] = 0;
    }
    for (int i = 0; i < candidate_count; i++)
    {   
        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[i][j] == false)
            {   
                winner[j]++;
            }
        }
    }
    
    int max = 0;
    int true_winner;
    for (int i = 0; i < candidate_count; i++)
    {
        if (winner[i] > max)
        {
            max = winner[i];
            true_winner = i;
        }
    }
    printf("%s\n", candidates[true_winner]);
}

bool cycle(int original_winner, int winner, int loser)
{   // IF c == c
    if (locked[loser][original_winner] == true)
    {
        return true;
    }
    
    for (int j = 0; j < candidate_count; j++)
    {   // Check if the loser wins in any locked pairs
        if(locked[loser][j])
        {
           if(cycle(original_winner,loser,j))
           {
               return true;
           }
        }
    }
    
    return false;
}
