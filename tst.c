#include "tst.h"
#include "listC.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

struct node {
	int x;
	char c;
	struct node *left;
	struct node *middle;
	struct node *right;
	struct node *prev;
	bool sentinel;
};

typedef struct node node;

struct tst {
	node *first;
	list *allNodes;
};

//------------------------------------------------------------------------------

static void fail(char *m) {
	fprintf(stderr, "%s\n", m);
	exit(1);
}

tst *newTst() {
	tst *t = malloc(sizeof(tst));
	node *start = malloc(sizeof(node));
//	node *senLeft = malloc(sizeof(node));
//	node *senMiddle = malloc(sizeof(node));
//	node *senRight = malloc(sizeof(node));
//	*senLeft = (node) {-1, '0', NULL, NULL, NULL, start, true};
//	*senMiddle = (node) {-1, '0', NULL, NULL, NULL, start, true};
//	*senRight = (node) {-1, '0', NULL, NULL, NULL, start, true};
//	*start = (node) {-1, '0', senLeft, senMiddle, senRight, NULL, true};
	*start = (node) {-1, '0', NULL, NULL, NULL, NULL, true};
	list *l = newList();
	t->allNodes = l;
	t->first = start;
	/*insertBefore(l, start);
	insertBefore(l, senLeft);
	insertBefore(l, senMiddle);
	insertBefore(l, senRight);*/
	return t;
}

//Turn a sentinel into a normal node and add missing sentinels
static void createNode(node *n, int x, char c) {
	n->sentinel = false;
	node *senLeft = malloc(sizeof(node));
	node *senMiddle = malloc(sizeof(node));
	node *senRight = malloc(sizeof(node));
	*senLeft = (node) {-1, '0', NULL, NULL, NULL, n, true};
	*senMiddle = (node) {-1, '0', NULL, NULL, NULL, n, true};
	*senRight = (node) {-1, '0', NULL, NULL, NULL, n, true};
	n->left = senLeft;
	n->middle = senMiddle;
	n->right = senRight;
	n->x = x;
	n->c = c;
}

static node *chooseNextNode(char c, node* current) {
	node *n = NULL;
	if (c == current->c) {
		n = current->middle;
		printf("Chose Middle\n");
	}
	else if (c < current->c) {
		n = current->left;
		printf("Chose Left\n");
	}
	else if (c > current->c) {
		n = current->right;
		printf("Chose Right\n");
	}
	return n;
}

void insertString(tst *t, int x, char *c) {
	node *current = t->first;
	int stringLength = strlen(c);
	int i = 0;
	bool finished = false;
	bool atLastChar = false;
	while (!finished) {
		atLastChar = (stringLength - i) == 1;
		if (current->sentinel) createNode(current, -1, c[i]);
		else {
			node *n = chooseNextNode(c[i], current);
			if (n == current->middle) {
				if (atLastChar) {
					current->x = x;
					finished = true;
				}
				else {
					if (n->sentinel) createNode(n, -1, c[i+1]);
					i++;
				}
			}
			current = n;
		}
	}
}

static node *findNode(tst *t, char *c) {
	int i = 0;
	int stringLength = strlen(c);
	bool done = false;
	node *n = t->first;
	node *current = NULL;
	while ((! done) && (i < stringLength)) {
		printf("c[i] = %c\n", c[i]);
		current = n;
		n = chooseNextNode(c[i], current);
		bool A = n == current->middle;
		bool B = n->sentinel;
		if (A) i++;
		if (B) done = true;
		if (!A && B) current = NULL;
	}
	return current;
}

int search(tst *t, char *c) {
	node *n = findNode(t, c);
	if (n == NULL) fail("Not found");
	if (n->x == -1) fail("Not found");
	printf("%d\n", n->x);
	return n->x;
}

void removeString(tst *t, char *c) {
	node *n = findNode(t, c);
	if (n == NULL) fail("Not found");
	if (n->x == -1) fail("Not found");
	while (true) {
		printf("Current node x = %d\n", n->x);
		node *left = n->left, *middle = n->middle, *right = n->right;
		bool lS = left->sentinel, mS = middle->sentinel, rS = right->sentinel;
		//which nodes are not empty?
		bool onlyLeft = !lS && mS && rS, onlyRight = !rS && mS && lS;
		bool middleOrMore = !mS;
		bool bothLeftAndRight = !lS && mS && !rS;
		bool none = lS && mS && rS;
		if (onlyLeft) {
			printf("Only Left\n");
			n->prev->middle = left;
			free(middle);
			free(right);
			free(n);
			break;
		}
		else if (onlyRight) {
			printf("Only Right\n");
			n->prev->middle = right;
			free(middle);
			free(left);
			free(n);
			break;
		}
		else if (bothLeftAndRight) {
			printf("Both\n");
			n->prev->middle = left;
			free(middle);
			if (right->c > left->c) {
				left->right = right;
			}
			if (right->c < left->c) {
				left->left = right;
			}
			free(n);
			break;
		}
		else if (none) {
			printf("None\n");
			free(left);
			free(middle);
			free(right);
			*n = (node) {-1, '0', NULL, NULL, NULL, n->prev, true};
			n = n->prev;
		}
		else if (middleOrMore) {
			printf("Middle or more\n");
			break;
		}
	}
}
//------------------------------------------------------------------------------

// L, M, R - follow left, middle or right pointer
// Any int or (lowercase) char - assert it is saved in the current node
//  * - assert this is a sentinel
//  # - assert this is NOT a sentinel
static bool compare(tst *t, char *s) {
	printf("Now checking: %s \n\n", s);
	int i = 0;
	int length = strlen(s);
	node *current = t->first;
	while (i < length) {
		printf("i = %d\n", i);
		printf("s[i] = %c\n", s[i]);
		printf("Tree->c says %c\n", current->c);
		if ((s[i] >= 'a') && (s[i] <= 'z')) assert(current->c == s[i]);
		if ((s[i] >= '0') && (s[i] <= '9')) assert(current->x == (s[i] - '0'));
		if (s[i] == '*') assert(current->sentinel == true);
		if (s[i] == '#') assert(current->sentinel == false);
		if (s[i] == 'L') current = current->left;
		if (s[i] == 'M') current = current->middle;
		if (s[i] == 'R') current = current->right;
		printf("step %d passed\n\n", i);
		i++;
	}
	printf("%s PASSED \n\n\n\n", s);
	return true;
}

static void testNew() {
	tst *t = newTst();
	compare(t, "*");
	createNode(t->first, 5, 'g');
	compare(t, "#g5L*");
	compare(t, "#g5M*");
	compare(t, "#g5R*");
	/*list *l = t->allNodes;
	node *s = t->first;
	start(l);
	assert(atStart(l));
	assert(s == getAfter(l));
	forward(l);
	assert((s->left) == getAfter(l));
	forward(l);
	assert(s->middle == getAfter(l));
	forward(l);
	assert(s->right == getAfter(l));
	forward(l);*/
	printf("testNew passed #################\n\n");
}

static void testInsert() {
	tst *t = newTst();
	insertString(t, 6, "abc");
	compare(t, "#aM#bM#c6M*");
	insertString(t, 2, "abd");
	compare(t, "#aMbMc6M*");
	compare(t, "#aMbMcRd2M*");
	insertString(t, 4, "omega");
	compare(t, "aRoMmMeMgMa4M*");
	insertString(t, 3, "gamble");
	compare(t, "aRoLgMaMmMbMlMe3M*");
	insertString(t, 3, "game");
	compare(t, "aRoLgMaMmMbRe3M*");
	insertString(t, 9, "gamers");
	compare(t, "aRoLgMaMmMbReMrMs9M*");
	printf("testInsert passed #################\n");
}

static void testSearch() {
	tst *t = newTst();
	insertString(t, 6, "abc");
	insertString(t, 2, "abd");
	insertString(t, 4, "omega");
	insertString(t, 3, "gamble");
	insertString(t, 3, "game");
	insertString(t, 9, "gamers");
	assert(search(t, "abc") == 6);
	assert(search(t, "abd") == 2);
	assert(search(t, "omega") == 4);
	assert(search(t, "gamble") == 3);
	assert(search(t, "game") == 3);
	assert(search(t, "gamers") == 9);
	assert(findNode(t, "gamerz") == NULL);
	//search(t, "gamer");
	printf("testSearch passed #################\n");
}

static void testRemove() {
	tst *t = newTst();
	insertString(t, 6, "abc");
	insertString(t, 2, "abd");
	insertString(t, 4, "omega");
	insertString(t, 3, "gamble");
	insertString(t, 3, "game");
	insertString(t, 9, "gamers");
	removeString(t, "gamble");
	compare(t, "#aM#bM#c6M*");
	compare(t, "#aMbMcRd2M*");
	compare(t, "aRoMmMeMgMa4M*");
	compare(t, "aRoLgMaMmMe3");
	compare(t, "aRoLgMaMmMeMrMs9M*");
	tst *r = newTst();
	insertString(r, 5, "ebo");
	insertString(r, 2, "eax");
	insertString(r, 7, "eco");
	compare(r, "eMbLaMx2M*");
	compare(r, "eMbMo5M*");
	compare(r, "eMbRcMo7M*");
	removeString(r, "ebo");
	compare(r, "#eM#aM#x2M*");
	compare(r, "#eM#aR#cM#o7M*");
	compare(r, "#eR*");
	compare(r, "#eMaL*");
	compare(r, "#eMaMxL*");
	compare(r, "#eMaRcR*");
	compare(r, "#eMaRcMoR*");
	printf("testRemove passed #################\n");
}

int tstMain() {
	testNew();
	testInsert();
	testSearch();
	testRemove();
	printf("ALL TESTS PASSED\n");
	return 0;
}