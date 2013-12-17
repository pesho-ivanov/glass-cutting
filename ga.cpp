#include<cstdio>
#include<cassert>
#include<vector>
#include<algorithm>
using namespace std;

// big plate
struct Bin {
  int id;
  int h, w;

  Bin(int _id, int _h, int _w)
    : id(_id), h(_h), w(_w) {}
};

// glass
struct Item {
  int id;
  int h, w;

  Item(int _id, int _h, int _w)
    : id(_id), h(_h), w(_w) {}
};

// GA individual as defined by (+rotation & multibins)
// HEURISTICS FOR LARGE STRIP PACKING PROBLEMS
// WITH GUILLOTINE PATTERNS: AN EMPIRICAL STUDY
// Christine L. Mumford-Valenzuela, Pearl Y. Wang, Janis Vick
struct State {
  vector<int>  perm;            // [0, n)
  vector<bool> rots;            // 'false' for not rotated
  vector<bool> optype;          // 'false' for vertical; 'true' for horizontal
  vector<int>  chainlen;        // chain len field

  State(int n, bool rand_perm=true, bool rand_rots=true,
      bool rand_optype=true, bool rand_chainlen=true) {
    perm.resize(n);
    rots.resize(n, 0);
    optype.resize(n, 0);
    chainlen.resize(n, 1);

    for (int i=0; i<n; i++)
      perm[i] = i;

    if (rand_perm)
      random_shuffle(perm.begin(), perm.end());

    if (rand_rots)
      for (int i=0; i<n; i++)
        rots[i] = rand() % 2;

    if (rand_optype)
      for (int i=0; i<n; i++)
        optype[i] = rand() % 2;

    if (rand_chainlen)
      for (int i=0; i<n; i++)
        chainlen[i] = rand() % 3;
  }
}; 

// a tree is fully determined by a state
struct Tree {
  enum { HORIZONTAL = -2, VERTICAL = -1 };

  vector<int> postfix; // includes HORIZONTAL, VERTICAL, 0, ..., n-1
  vector<bool> rots;

  Tree(State &state) {
    int n = state.perm.size();
    vector<int> curr_operands;

    rots = state.rots;

    fprintf(stderr, "rots:");
    for (int i=0; i<rots.size(); i++)
      fprintf(stderr, " %d", int(rots[i]));
    fprintf(stderr, "\n");

    for (int i=0; i<n; i++) {
      postfix.push_back(state.perm[i]);
      curr_operands.push_back(state.perm[i]);
      int op = state.optype[i] ? HORIZONTAL : VERTICAL;

      for (int j=0; j<state.chainlen[i]; j++)
        if (curr_operands.size() > 1) {
          postfix.push_back(op);
          op = op == HORIZONTAL ? VERTICAL : HORIZONTAL;
          curr_operands.pop_back();
        }
    }

    int op = HORIZONTAL;
    while (curr_operands.size() > 1) {
        postfix.push_back(op);
        op = op == HORIZONTAL ? VERTICAL : HORIZONTAL;
        curr_operands.pop_back();
    }

    for (int i=0; i<postfix.size(); i++)
      fprintf(stderr, " %d", postfix[i]);
    fprintf(stderr, "\n");
  }
};

// not to be changed after read
vector<Bin> bins;
vector<Item> items;

// Hopper format
void read_bins(char *infile) {
  FILE *in = fopen(infile, "r");
  int n, curr_id=0;

  fscanf(in, "%d,,", &n);
  bins.reserve(n);

  while(bins.size() < n) {
    int id, h, w, num;

    fscanf(in, "%d,%d,%d,%d", &id, &h, &w, &num);
    //fprintf(stderr, "%d %d %d %d\n", id, h, w, num);

    for(int j=0; j<num; j++) {
      Bin bin(curr_id++, h, w);
      bins.push_back(bin);
    }
  }

  assert(bins.size() == n);
}

// Hopper format
void read_items(char *infile) {
  FILE *in = fopen(infile, "r");
  int m;
  int id, h, w;

  //fscanf(in, "%d", &m);
  //items.reserve(m);

  while (fscanf(in, "%d,%d,%d", &id, &h, &w) != EOF) {
    //fprintf(stderr, "%d %d %d\n", id, h, w);

    Item item(items.size(), h, w);
    items.push_back(item);
  }

  assert(!items.empty());
}

int fitness(const State &st) {
 
}

void ga() {
  State curr_state(items.size());
  Tree curr_tree(curr_state);
}

int main(int argn, char *args[])
{
  if (argn != 3) {
    fprintf(stderr, "args: <bins_file> <items_file>\n");
  }
  
  read_bins(args[1]);
  read_items(args[2]);

  ga();
  
  return 0;
}
