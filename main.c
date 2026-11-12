#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int x, y, z;
    int rank, prev;
} Bloco;

typedef struct {
    int h_max;
    int index;
} Node;

int min(int a, int b) {
    if (a<b) return a;
    return b;
}

int max(int a, int b) {
    if (a>b) return a;
    return b;
}

int comparaBlocos(const void *bloco1, const void *bloco2) {
    /*
    A função recebe 2 blocos e retorna -1 se x do bloco1 for menor que o do
    bloco2, e 1 se for maior. Se forem iguais, retorna 0.
    */
    const Bloco *b1 = (const Bloco*) bloco1;
    const Bloco *b2 = (const Bloco*) bloco2;
    return (b1->x < b2->x) - (b1->x > b2->x);
}

int comparaY(const void *ya, const void *yb) {
    /*
    A função recebe 2 valores de y e retorna -1 se ya for menor que o yb e 1 se
    for maior. Se forem iguais, retorna 0.
    */
    int y1 = *(const int *) ya;
    int y2 = *(const int *) yb;
    return (y1 < y2) - (y1 > y2);
}
int *lstYNotRepeated(Bloco *lst, int len, int *len_lst) {
    /*
    A função recebe uma lista de blocos e o comprimento da lista, e retorna um
    array com os valores únicos de y, ordenados em ordem crescente.
    */
    int i = 0;
    int k = 1;

    int *y_in_order = malloc(len*sizeof(int)); 
    for (i = 0; i < len; i++) {
        // copia os valores de y para o array y_in_order
        y_in_order[i] = lst[i].y;
    }

    // ordena o array y_in_order em ordem crescente
    qsort(y_in_order, len, sizeof(int), comparaY);

    int *not_repeated = malloc(len*sizeof(int)); 
    not_repeated[0] = y_in_order[0];
    for (i = 1; i < len; i++) {
        if (y_in_order[i] != not_repeated[k-1]) {
            // se o valor de y não for repetido, adiciona ao array not_repeated
            not_repeated[k] = y_in_order[i];
            k++;
        }
    }

    free(y_in_order);
    *len_lst = k;
    return not_repeated;
}

int encontra_rank(int y, int *not_repeated, int len_lista) {
    /*
    A função recebe um valor de y, uma lista de valores únicos de y e o
    comprimento da lista. Retorn -1 se o valor de y não estiver na lista, ou o
    índice do valor de y na lista.
    */
    int ini = 0, end = len_lista - 1;
    while (ini <= end) {
        int i = (ini + end)/2;
        if (not_repeated[i] == y) return i;
        if (not_repeated[i] > y) ini = i + 1;
        else end = i - 1;
    }
    return -1;
}

void atualiza(Node *arvore, int len, int i, Node novo) {
    /*
    A função recebe uma árvore, o comprimento da árvore, um índice e um novo nó.
    Atualiza a árvore com o novo nó, se o valor de h_max do novo nó for maior
    que o valor de h_max do nó atual na árvore.
    */
    while (i < len + 1) {
        if (novo.h_max > arvore[i].h_max) {
            arvore[i].h_max = novo.h_max;
            arvore[i].index = novo.index;
        }
        i += i & -i; // vai buscar o pai do nó atual
    }
}
Node consulta(Node* arvore, int p) {
    /*
    Esta função recebe uma árvore e um índice, e retorna o nó com o maior valor
    de h_max na árvore até o índice p.
    */
    Node melhor;
    melhor.h_max = 0;
    melhor.index = -1;    
    while (p>0) {
        if (arvore[p].h_max >= melhor.h_max) {
            melhor = arvore[p];
        }
        p -= p & -p; // vai buscar o primeiro bit = 1 e passa a 0, ou seja, vai buscar o pai do nó atual
    }
    return melhor;
}
void construir_torre(Bloco *lst, int len, int *n_rep_y, int len_lista) {
    int i, m = 1, k = 0, fim, topo = -1;
    int melhor_h_max = 0;
    Node melhor;
    Node novo;
    int *h_max = malloc(len*sizeof(int));
    Bloco *lst_final = (Bloco*) malloc(len*sizeof(Bloco));
    Node *arvore = (Node*) malloc((len_lista+1)*sizeof(Node));
    for (i = 0; i < len; i++) {
        lst[i].rank = encontra_rank(lst[i].y,n_rep_y,len_lista) + 1;
    }
    for (i = 0; i < len_lista + 1; i++) {
        arvore[i].h_max = 0;
        arvore[i].index = -1;
    }
    i = 0;
    while (i < len) {
        fim = i;
        while (fim < len && lst[fim].x == lst[i].x) {
            fim++;
        }
        k = i;
        while (k < fim) {
            melhor = consulta(arvore,lst[k].rank-1);
            h_max[k] = lst[k].z + melhor.h_max;
            lst[k].prev = melhor.index;
            if (h_max[k] > melhor_h_max) {
                melhor_h_max = h_max[k];
                topo = k;
            }
            k++;
        }
        k = i;
        while (k < fim) {
            novo.h_max = h_max[k];
            novo.index = k;
            atualiza(arvore, len_lista, lst[k].rank, novo);
            k++;
        }
        i = fim;
    }

    lst_final[0] = lst[topo];
    for(i = 1; i < len; i++) {
        k = lst[topo].prev;
        if (k == -1) break;
        m++;
        lst_final[i] = lst[k];
        topo = k;   
    }
    printf("%d %d\n", melhor_h_max, m);
    for (i = m-1; i >= 0; i--) {
        printf("%d %d %d\n", lst_final[i].x, lst_final[i].y, lst_final[i].z);
    }
    free(h_max);
    free(arvore);
    free(lst_final);
}

int main() {
    int n, len_lista, i;
    int *n_rep_y;

    if (scanf("%d", &n) == -1) return -1;
    int len = 3*n;

    Bloco *lst = (Bloco*) malloc(sizeof(Bloco)*3*n);
    for (i = 0; i < 3*n; i = i + 3) {
        int x, y, z;
        if (scanf("%d %d %d", &x,&y,&z) == -1) {
            return -1;
        }
        Bloco b1, b2, b3;

        b1.x=min(x,y);
        b1.y=max(x,y);
        b1.z=z;

        b2.x=min(x,z);
        b2.y=max(x,z);
        b2.z=y;

        b3.x=min(y,z);
        b3.y=max(y,z);
        b3.z=x;

        lst[i]=b1;
        lst[i+1]=b2;
        lst[i+2]=b3;

    }

    qsort(lst, len, sizeof(Bloco), comparaBlocos);
    n_rep_y = lstYNotRepeated(lst, len, &len_lista);
    construir_torre(lst, len, n_rep_y, len_lista);

    free(lst);
    free(n_rep_y);
    return 0;
}
