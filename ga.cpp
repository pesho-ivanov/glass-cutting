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

// not to be changed after read
vector<Bin> bins;
vector<Item> items;

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

  struct Node {
    Item item; // if leaf
    Node *first, *second; // children
    int cut; // if not leaf

    Node(Item _item, Node *_first, Node *_second, int _cut=0)
      : item(_item), first(_first), second(_second), cut(_cut) {}
  };

  int n;
  vector<int> postfix; // includes HORIZONTAL, VERTICAL, 0, ..., n-1
  vector<bool> rots;
  Node *root;
  int h, w;

  Tree(State &state) {
    n = state.perm.size();
    build_postfix(state);
    build_tree();
    
    pair<int, int> size = print_tree(root, 0, 0);
    w = size.first;
    h = size.second;
    fprintf(stderr, "size: (%d, %d)\n", w, h);
  }

  int fitness() {
    return w*h;
  }

  void build_postfix(State &state) {
    vector<int> curr_operands;

    rots = state.rots;

    //fprintf(stderr, "rots:");
    //for (int i=0; i<rots.size(); i++)
    //  fprintf(stderr, " %d", int(rots[i]));
    //fprintf(stderr, "\n");

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

    fprintf(stderr, "postfix: ");
    for (int i=0; i<postfix.size(); i++)
      fprintf(stderr, " %d", postfix[i]);
    fprintf(stderr, "\n");
  }

  void build_tree() {
    vector<Node *> curr_operands;
    Node *node;

    fprintf(stderr, "build_tree()\n");

    for (int i=0; i<postfix.size(); i++) {
      int op = postfix[i];
      if (op == HORIZONTAL || op == VERTICAL) {
        assert(curr_operands.size() >= 2);

        Node *second= curr_operands.back();
          curr_operands.pop_back();

        Node *first = curr_operands.back();
          curr_operands.pop_back();

        Item item(99, 0, 0);
        if (op == HORIZONTAL) {
          item.w = first->item.w + second->item.w;
          item.h = max(first->item.h, second->item.h);
        } else {
          item.w = max(first->item.w, second->item.w);
          item.h = first->item.h + second->item.h;
        }

        node = new Node(item, first, second, op);
      } else {
        node = new Node(items[op], NULL, NULL);
      }

      curr_operands.push_back(node);
    }

    assert(curr_operands.size() == 1);
    root = curr_operands[0];
  }

  pair<int, int> print_tree(Node *v, int dx, int dy, int shift=0) {
    if (v != NULL) {
      fprintf(stderr, "%*s", shift, "");
      if (v->first == NULL && v->second == NULL) {
        Item item = v->item;
        fprintf(stderr, "leaf: #%d (%d, %d -- %d, %d)\n", item.id, dx, dy, dx + item.w, dy + item.h);
        return make_pair(item.w, item.h);
      } else {
        assert(v->cut == HORIZONTAL || v->cut == VERTICAL);
        fprintf(stderr, "cut: %s\n", v->cut == HORIZONTAL ? "H" : "V");
      }

      pair<int, int> d_first, d_second;
      if (v->first != NULL)
        d_first = print_tree(v->first, dx, dy, shift+2);

      int dx_prev = dx, dy_prev = dy;
      if (v->second != NULL) {
        if (v->cut == HORIZONTAL) {
          dy += d_first.second;
          d_second = print_tree(v->second, dx, dy, shift+2);
          dx += max(d_first.first, d_second.first);
          dy += d_second.second;
        } else {
          dx += d_first.first;
          d_second = print_tree(v->second, dx, dy, shift+2);
          dx += d_second.first;
          dy += max(d_first.second, d_second.second);
        }
      }

      return make_pair(dx-dx_prev, dy-dy_prev);
    }

    return make_pair(0, 0);
  }

  ~Tree() {
    delete_subtree(root);
  }

  void delete_subtree(Node *v) {
    if (v != NULL) {
      delete_subtree(v->first);
      delete_subtree(v->second);
      delete v;
    }
  }
};

// Hopper format
void read_bins(char *infile) {
  FILE *in = fopen(infile, "r");
  int n, curr_id=0;

  fscanf(in, "%d,,", &n);
  bins.reserve(n);

  while(bins.size() < n) {
    int id, h, w, num;

    fscanf(in, "%d,%d,%d,%d", &id, &w, &h, &num);

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

  while (fscanf(in, "%d,%d,%d", &id, &w, &h) != EOF) {
    Item item(items.size(), h, w);
    items.push_back(item);
  }

  assert(!items.empty());
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
