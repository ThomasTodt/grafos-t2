#include "grafo.h"
#include <graphviz/cgraph.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char rec_name[14] = "info_vertice";

typedef struct {
  Agrec_t header;
  int contado;
  int componente;
} info_vertice;

typedef struct {
  Agrec_t header;
  int criada;
  int padding;
} info_aresta;

static int backtrack_conexo(grafo g, vertice atual, int *contador_vertices); // funcao que auxilia a funcao "conexo"
static int backtrack_bipartido(grafo g, vertice v, int cor_atual);
static void multiplica_matriz_adjacencia(int **A, int **B, int **C, int tamanho);
void visit(grafo g, vertice u, vertice* stack, int *topo_stack);
void assign(grafo g, vertice u, vertice root, grafo subgrafo);

//------------------------------------------------------------------------------
grafo le_grafo(void) 
{
  grafo g = agread(stdin, NULL);
  // strcpy(rec_name, "info_vertice");
  aginit(g, AGNODE, rec_name, sizeof(info_vertice), TRUE);
  return g; 
}
//------------------------------------------------------------------------------
void destroi_grafo(grafo g) 
{
  agclose(g);
}
//------------------------------------------------------------------------------
grafo escreve_grafo(grafo g) 
{  
  agwrite(g, stdout);
  return g;
}

// -----------------------------------------------------------------------------
int n_vertices(grafo g) 
{  
  return agnnodes(g);
}

// -----------------------------------------------------------------------------
int n_arestas(grafo g) 
{  
  return agnedges(g);
}

// -----------------------------------------------------------------------------
int grau(vertice v, grafo g) 
{
  return agcountuniqedges(g, v, TRUE, TRUE);;
}

// -----------------------------------------------------------------------------
int grau_maximo(grafo g)  
{

  vertice atual = agfstnode(g);
  int grauMax = grau(atual, g);

  vertice ultimo = aglstnode(g);


  while (atual != ultimo)
  {
    int grauAtual = grau(atual, g);

    if (grauAtual > grauMax)
      grauMax = grauAtual;

    atual = agnxtnode(g, atual);
  }

  
  return grauMax;
}

// -----------------------------------------------------------------------------
int grau_minimo(grafo g)  
{
  vertice atual = agfstnode(g);
  int grauMin = grau(atual, g);

  vertice ultimo = aglstnode(g);


  while (atual != ultimo)
  {
    int grauAtual = grau(atual, g);

    if (grauAtual < grauMin)
      grauMin = grauAtual;

    atual = agnxtnode(g, atual);
  }

  
  return grauMin;  
}

// -----------------------------------------------------------------------------
int grau_medio(grafo g) 
{
  int total = n_arestas(g) * 2;
  return total / n_vertices(g);
}

// -----------------------------------------------------------------------------
int regular(grafo g) 
{
  
  vertice atual = agfstnode(g);
  int grauReg = grau(atual, g);

  vertice ultimo = aglstnode(g);


  while (atual != ultimo)
  {
    int grauAtual = grau(atual, g);

    if (grauAtual != grauReg)
      return 0;

    atual = agnxtnode(g, atual);
  }


  return 1;
}

// -----------------------------------------------------------------------------
int completo(grafo g) {
  // nao funciona com multigrafos

  int arestas = n_arestas(g);  
  int vertices = n_vertices(g);

  return arestas == (vertices * (vertices - 1)) / 2;
}

// -----------------------------------------------------------------------------
int conexo(grafo g) 
{
  info_vertice *info;
  // strcpy(rec_name, "info_vertice");

  for (vertice v = agfstnode(g); v; v = agnxtnode(g,v))
  {
    //info = agbindrec(v, rec_name, sizeof(info_vertice), TRUE);
    info = (info_vertice *) aggetrec(v, rec_name, TRUE);
    info->contado = 0;
  }

  int contador_vertices = n_vertices(g);

  return backtrack_conexo(g, agfstnode(g), &contador_vertices);
}

static int backtrack_conexo(grafo g, vertice atual, int *contador_vertices) // funcao que auxilia a funcao "conexo"
{
  info_vertice *info;
  // strcpy(rec_name, "info_vertice");
  info = (info_vertice*) aggetrec(atual, rec_name, TRUE);

  if (info->contado == 0) // contabiliza o vertice
  {
    info->contado = 1;
    (*contador_vertices)--;

    if (*contador_vertices == 0)
      return 1;


    for (vertice v = agnxtnode(g, atual); v; v = agnxtnode(g,v))
    {
      if ( agedge(g, atual, v, NULL, 0) )
      {
        if ( backtrack_conexo(g, v, contador_vertices) == 1 )
          return 1;
      }
    }

  }


  return 0;
}

// -----------------------------------------------------------------------------
/// funcao auxiliar que checa se o componente do vertice v ?? um subgrafo bipartido.
int bipartido(grafo g) {
  info_vertice *info;
  // strcpy(rec_name, "info_vertice");

  for (vertice v = agfstnode(g); v; v = agnxtnode(g,v))
  {
    info = (info_vertice *) aggetrec(v, rec_name, TRUE);
    info->contado = -1;
  }

  // temos que fazer isso, para garantir que todos os componentes
  // sao subgrafos bipartidos 
  for (vertice v = agfstnode(g); v; v = agnxtnode(g,v)) {
    if (!backtrack_bipartido(g, agfstnode(g), 0))
      return 0;
  }

  return 1;
}

static int backtrack_bipartido(grafo g, vertice v, int cor_atual) {
  info_vertice *info;
  // strcpy(rec_name, "info_vertice");
  info = (info_vertice *) aggetrec(v, rec_name, TRUE);
  if (info == NULL) {
    printf("eita\n");
  }

  if (info->contado != -1) {
    // se ja passamos pelo vertice a cor deve ser a mesma que colocamos antes
    return cor_atual == info->contado;
  } else {
    // se ainda nao passamos entao colocamos a cor esperada
    info->contado = cor_atual;
  }

  // para todos os vizinhos...
  for (Agedge_t *e = agfstedge(g, v); e; e = agnxtedge(g, e, v)) {
    if (!backtrack_bipartido(g, e->node, (cor_atual + 1) % 2)) {
      return 0;
    }
  }

  return 1;
}

// -----------------------------------------------------------------------------
static void multiplica_matriz_adjacencia(int **A, int **B, int **C, int tamanho)
{
  for (int i=0; i < tamanho; i++)
  {
    for (int j=0; j < tamanho; j++)
    {
      C[i][j] = 0;
      for (int k=0; k < tamanho; k++)
        C[i][j] += A[i][k] * B[k][j];
    }
  }
}

int n_triangulos(grafo g) 
{
  int **mat = matriz_adjacencia(g);
  int **mat2, **mat3;

  int tamanho = n_vertices(g);

  mat2 = (int **) calloc ((unsigned long) tamanho, sizeof (int*)) ;
  for (int i = 0; i < tamanho; i++)
    mat2[i] = (int *) calloc ((unsigned long) tamanho, sizeof (int)) ;

  mat3 = (int **) calloc ((unsigned long) tamanho, sizeof (int*)) ;
  for (int i = 0; i < tamanho; i++)
    mat3[i] = (int *) calloc ((unsigned long) tamanho, sizeof (int)) ;


  multiplica_matriz_adjacencia(mat, mat, mat2, tamanho);
  multiplica_matriz_adjacencia(mat2, mat, mat3, tamanho);

  int traco = 0 ;
  for (int i = 0; i < tamanho; i++)
    traco += mat3[i][i];


  // liberar matrizes
  for (int i = 0; i < tamanho; i++)
  {
    free(mat[i]);
    free(mat2[i]);
    free(mat3[i]);
  }
  free(mat);
  free(mat2);
  free(mat3);
  
  return traco / 6;
}

// -----------------------------------------------------------------------------
int **matriz_adjacencia(grafo g) 
{
  Agnode_t *n;
  Agnode_t *m;
  Agedge_t *e;
  int i, j;
  int num_vertices = n_vertices(g);

  int **mat;

  mat = malloc ((unsigned long) num_vertices * sizeof (int*)) ;

  for (i=0; i < num_vertices; i++)
    mat[i] = malloc ((unsigned long) num_vertices * sizeof (int)) ;
  
  i=0;
  j=0;
  for (n = agfstnode(g); n; n = agnxtnode(g,n)) {
    j=0;
    for (m = agfstnode(g); m; m = agnxtnode(g,m)) {
      e = agedge(g,n,m,NULL,FALSE);


      if(e != NULL){
        mat[i][j]=1;
      }
      else{
        mat[i][j]=0;
      }
        
      j++;
    }

    i++;
  }
  
  return mat;
}

// -----------------------------------------------------------------------------
grafo complemento(grafo g)
{
  char complemento[13] = "complemento"; 
  char aresta[7] = "aresta";
  grafo h = agopen(complemento, g->desc, NULL);

  // criando o mesmo numero de nodos no novo grafo
  for (vertice v = agfstnode(g); v; v = agnxtnode(g, v)) {
    agnode(h, agnameof(v), TRUE);
  }

  for (vertice ga = agfstnode(g), ha = agfstnode(h); ga && ha; ga = agnxtnode(g, ga), ha = agnxtnode(h, ha)) {
    for (vertice gb = agfstnode(g), hb = agfstnode(h); gb && hb; gb = agnxtnode(g, gb), hb = agnxtnode(h, hb)) {
      if (ga != gb && ha != hb) {
        if (!agedge(g, ga, gb, NULL, 0)) {
          // se nao existe no original, existe no complemento
          agedge(h, ha, hb, aresta, 1);
        }
      }
    }
  }

  return h;
}

//------------------------------------------------------------------------------
void visit(grafo g, vertice u, vertice* stack, int* topo_stack)
{
  info_vertice *info;
  // strcpy(rec_name, "info_vertice");
  info = (info_vertice *) aggetrec(u, rec_name, TRUE);

  // If u is unvisited then:
  if (info->contado == 0)
  {
    // Mark u as visited
    info->contado = 1;

    // For each out-neighbour v of u, if v is unvisited, do Visit(v)
    for (Agedge_t *e = agfstout(g, u); e; e = agnxtout(g, e)) // esse agnxtout funciona? 
    {
        visit(g, e->node, stack, topo_stack);
    }

    // prepend u to L
    stack[(*topo_stack)++] = u;
  }
}

void assign(grafo g, vertice u, vertice root, grafo subgrafo)
{
  info_vertice *info_root;
  // strcpy(rec_name, "info_vertice");
  info_root = (info_vertice *) aggetrec(root, rec_name, TRUE);


  // For each in-neighbour v of u, do Assign(v,root)
  for (Agedge_t *e = agfstin(g, u); e; e = agnxtin(g, e)) // esse agnxtout funciona? 
  {
    info_vertice *info_prox = (info_vertice *) aggetrec(e->node, rec_name, TRUE);

    if (info_prox->componente == 0) {
      // n??o definiu o componente ainda

      // adiciona ao componente do root.
      info_prox->componente = info_root->componente;

      // adiciona arestas e seus v??rtices ao subgrafo do componente
      agsubedge(subgrafo, e, TRUE);

      // faz o mesmo pros vizinhos
      assign(g, e->node, root, subgrafo);
    }

    if (info_prox->componente == info_root->componente) {
      // adiciona a aresta ao subgrafo/componente
      agsubedge(subgrafo, e, TRUE);
    }
  }
  
}

grafo decompoe(grafo g) 
{
  info_vertice *info;
  // strcpy(rec_name, "info_vertice");

  // For each vertex u of the graph, mark u as unvisited. Let L be empty.
  for (vertice v = agfstnode(g); v; v = agnxtnode(g,v))
  {
    info = (info_vertice *) aggetrec(v, rec_name, TRUE);
    info->contado = 0; // nao visitado
    info->componente = 0; // nao faz parte de nenhum componente
  }

  // "stack" L?
  vertice *stack = (vertice*) calloc((size_t)agnnodes(g), sizeof(vertice));
  int topo_stack = 0;

  // For each vertex u of the graph do Visit(u)
  for (vertice u = agfstnode(g); u; u = agnxtnode(g,u))
  {
    visit(g, u, stack, &topo_stack);
  }

  int id_componente = 0;

  // para cada u em L, em ordem
  while (topo_stack != 0) {
    vertice u = stack[--topo_stack];

    info_vertice *info2 = (info_vertice *) aggetrec(u, rec_name, TRUE);

    if (info2->componente == 0) {
      // Cria um subgrafo de g para servir de componente.
      grafo s = agsubg(g, NULL, TRUE);

      // adiciona u a seu pr??prio componente
      agsubnode(s, u, TRUE);

      // seta um novo id para o componente
      info2->componente = ++id_componente;

      assign(g, u, u, s);
    }
    
  }

  free(stack);

  return g;
}

// vim: set sts=2 ts=2 sw=2 et:
