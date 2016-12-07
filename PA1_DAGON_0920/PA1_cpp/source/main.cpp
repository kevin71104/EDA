/* Copyright (c) 2002 U Wisc. ECE 756*/
/* The source code is originally downloaded from ECE 756 website */
#define DEBUG
#include "parser.h"
#include <iostream>
using namespace std;
/* --------------------------------------------------------------------------*/
/*				 Cell Library (pattern graph)		     */
/* --------------------------------------------------------------------------*/

/* Entry of library cell table. */
typedef struct {
  Type type;			/* logic gate type, e.g. AOI21 */
  Tree g;			/* pattern: normalized tree (NAND2/INV) */
  Area area;			/* chip area footprint */
} Libcell;

/* The library cells table, 1 normalized pattern per entry.
   Pattern trees (now NULL) still need to be filled in!
*/
static Libcell libcells[] = {

  {   INV, NULL, 2 },
  { NAND2, NULL, 3 },
  { NAND3, NULL, 4 }, /* Has 2 different topologies. */
  { NAND3, NULL, 4 },
  { NAND4, NULL, 5 }, /* Has 5 different topologies. */
  { NAND4, NULL, 5 },
  { NAND4, NULL, 5 },
  { NAND4, NULL, 5 },
  { NAND4, NULL, 5 },
  { AOI21, NULL, 4 }, /* Has 2 different topologies. */
  { AOI21, NULL, 4 },
  { AOI22, NULL, 5 }
};

/* The size of the libcells table (nr. of entries). */
static int nr_libcells = sizeof(libcells) / sizeof(libcells[0]);

/* Fills in the pattern trees into the library cell table. */
static void
tree_libcells_init(void)
{
  int i = 0;

#define x		NULL
#define INV(a)		tree_mk_inv(a)
#define ND2(a,b)	tree_mk_nand2(a,b)

  /* INV: */
  libcells[i++].g = INV(x);
  /* NAND2: */
  libcells[i++].g = ND2(x,x);
  /* NAND3: */
  libcells[i++].g = ND2(INV(ND2(x,x)),x);
  libcells[i++].g = ND2(x,INV(ND2(x,x)));
  /* NAND4: */
  libcells[i++].g = ND2(INV(ND2(INV(ND2(x,x)),x)),x);
  libcells[i++].g = ND2(INV(ND2(x,INV(ND2(x,x)))),x);
  libcells[i++].g = ND2(x,INV(ND2(INV(ND2(x,x)),x)));
  libcells[i++].g = ND2(x,INV(ND2(x,INV(ND2(x,x)))));
  libcells[i++].g = ND2(INV(ND2(x,x)),INV(ND2(x,x)));
  /* AOI21: */
  libcells[i++].g = INV(ND2(ND2(x,x),INV(x)));
  libcells[i++].g = INV(ND2(INV(x), ND2(x,x)));
  /* AOI22: */
  libcells[i++].g = INV(ND2(ND2(x,x),ND2(x,x)));

#undef x
#undef INV
#undef ND2
}

/* --------------------------------------------------------------------------*/
/*				 Subject Graph (normalization)		     */
/* --------------------------------------------------------------------------*/

/* Inverts t but avoids double inverters. */
static Tree
tree_mk_INV(Tree t)
{
  if (t != NULL && T_TYPE(t) == INV) {
    Tree n = t;

    t = T_LEFT(t);
    T_LEFT(n) = NULL;
    tree_free(n);
    return t;
  }
  return tree_mk_inv(t);
}

/* Normalizes tree t by reexpressing its Boolean function in terms of
   NAND2 and INV gates only. Returns the newly constructed tree.
   Does NOT consider permutations of NAND2 inputs; expects library cells
   to be dependent on this to have multiple representations.
*/
static Tree
tree_normalize(Tree t)
{
  Tree L, R;

  if (t == NULL)
    return t;

  switch (T_TYPE(t)) {
  case NOT:			/* => INV(-) */
    L = tree_normalize(T_LEFT(t));
    return tree_mk_INV(L);

  case AND:			/* => INV(NAND2(-,-)) */
    L = tree_normalize(T_LEFT(t));
    R = tree_normalize(T_RIGHT(t));
    return tree_mk_inv(tree_mk_nand2(L, R));

  case NAND:			/* => NAND2(-,-) */
    L = tree_normalize(T_LEFT(t));
    R = tree_normalize(T_RIGHT(t));
    return tree_mk_nand2(L, R);

  case OR:			/* => NAND2(INV(-),INV(-)) */
    L = tree_normalize(T_LEFT(t));
    R = tree_normalize(T_RIGHT(t));
    return tree_mk_nand2(tree_mk_INV(L), tree_mk_INV(R));

  default:
    assert(1);
  }

  return NULL;
}

/* --------------------------------------------------------------------------*/
/*			    Tree Mapper (filled by you)			     */
/* --------------------------------------------------------------------------*/
/* Tries to match library cell tree g with circuit tree t.
   (Expects normalized trees.) If sucessfully finds a match, then
   adds total area of subtrees induced by the library cell match to *area.

   Returns 1 when successful, 0 otherwise.
*/
static int
tree_match(Tree t, Tree g, Area *area)
{
	//deal with nullptr
	if(!t && g )
		return false;
	if(!g)
	{
		if(t)
			*area+=T_AREA(t);
		return 1;
	}
	
	if(T_TYPE(g)!=T_TYPE(t))
		return false;
		
	bool LLRR=false,LRRL=false;
	Area a1=*area,a2=*area;
	//t_left(right)=g_left(right)
	if( tree_match( T_LEFT(t) , T_LEFT(g) , &a1 ) && \
		tree_match( T_RIGHT(t) , T_RIGHT(g) , &a1 ))
		LLRR=true;
	//t_left(right)=g_right(left)
	if( tree_match( T_LEFT(t) , T_RIGHT(g) , &a2 ) && \
		tree_match( T_RIGHT(t) , T_LEFT(g) , &a2 ))
		LRRL=true;
		
	if(LLRR && LRRL)
	{
		if (a1<=a2)
			*area=a1;
		else 
			*area=a2;
		return true;
	}
	if(LLRR && !LRRL)
	{
		*area=a1;
		return true;
	}
	if(LRRL && !LLRR)
	{
		*area=a2;
		return true;
	}
	if(!LRRL && !LLRR)
		return false;

}

/* Finds minimum area mapping of circuit t on to given library.
   Annotates each node of t with best matched library cell (index).
   Returns area for t.
*/
Area tree_mapper(Tree t)
{
	if(T_LEFT(t) )
		if(!T_AREA(T_LEFT(t)))
			tree_mapper(T_LEFT(t));
	if(T_RIGHT(t) )
		if(!T_AREA(T_RIGHT(t)))
			tree_mapper(T_RIGHT(t));
		
	Area areacomp=INT_MAX,temp=0;
	for(int i=0;i<nr_libcells;i++,temp=0)
		if(tree_match(t,libcells[i].g,&temp) && temp+libcells[i].area<areacomp)
		{
			areacomp=temp+libcells[i].area;
			T_CELL(t)=libcells[i].type;
		}
	T_AREA(t)=areacomp;
	
	cout<<"Tree node: "<<gate_names[T_TYPE(t)]<<", area: "<<T_AREA(t)\
		<<", cell: "<<gate_names[T_CELL(t)]<<endl;
	
	return T_AREA(t);
}


/* --------------------------------------------------------------------------*/
/*			    Main					     */
/* --------------------------------------------------------------------------*/

int
main(int argc, char *argv[])
{
  Tree s, t;

  if (argc > 1 && argv[1])
    freopen(argv[1], "r", stdin);

  tree_libcells_init();

  t = parse();

#ifdef DEBUG
//  printf("Input tree circuit: ");
//  tree_print(t);
//  printf("\n");
#endif

  t = tree_normalize(s = t);
  tree_free(s);

#ifdef DEBUG
  printf("Normalized tree   : ");
  tree_print(t);
  printf("\n");
#endif

  printf("%d\n", tree_mapper(t));

  tree_free(t);

  return EXIT_SUCCESS;
}


