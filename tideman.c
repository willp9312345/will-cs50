#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// Check circle - recursive function
bool check_circ(int start, int current);


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
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(candidates[i], name) == 0)
        {
            ranks[rank] = i;
            //printf("True");
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count - 1; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            if (!(preferences[ranks[i]][ranks[j]] > 0))
            {
                preferences[ranks[i]][ranks[j]] = 1;
            }
            else
            {
                preferences[ranks[i]][ranks[j]] += 1;
            }
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
   pair_count = 0;

    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i; j < candidate_count; j++)

            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count += 1;
            }
            else if (preferences[i][j] < preferences[j][i])
            {
                pairs[pair_count].loser = i;
                pairs[pair_count].winner = j;
                pair_count += 1;
            }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    int temp_win;
    int temp_lose;

    for (int j = 0; j < pair_count; j++)
    {
        int swaps = 0;
        for (int i = 0; i < pair_count - 1; i++)
        {
            int diff = preferences[pairs[i].winner][pairs[i].loser] - preferences[pairs[i].loser][pairs[i].winner];
            int next_diff = preferences[pairs[i + 1].winner][pairs[i + 1].loser] - preferences[pairs[i + 1].loser][pairs[i + 1].winner];

            if (diff < next_diff)
            {
                temp_win = pairs[i].winner;
                temp_lose = pairs[i].loser;
                pairs[i].winner = pairs[i + 1].winner;
                pairs[i].loser = pairs[i + 1].loser;
                pairs[i + 1].winner = temp_win;
                pairs[i + 1].loser = temp_lose;
                swaps += 1;
            }
        }
        if (swaps == 0)
        {
            return;
        }
    }
    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int i = 0; i < pair_count; i++)
    {
        if (check_circ(pairs[i].winner, pairs[i].loser) == false)
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }
    return;
}

// Print the winner of the election
void print_winner(void)
{
    bool outgoing;
    bool incoming;

    for (int i = 0; i < pair_count; i++)
    {
        outgoing = false;
        incoming = false;

        for (int j = 0; j < pair_count; j++)
        {
            if (locked[i][j] == true )
            {
                outgoing = true;
            }
            if (locked[j][i] == true )
            {
                incoming = true;
            }
        }
        if (outgoing == true && incoming == false)
        {
            printf("%s\n", candidates[i]);
        }
    }
    return;
}

bool check_circ(int start, int current)
{
    for (int i = 0; i < candidate_count; i++)
    {
        //check if current has any outgoing
        if (locked[current][i] == true)
        {
            if (i == start)
            {
                return true;
            }
            else
            {
                if (check_circ(start, i) == false)
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
        }

    }
    return false;
}