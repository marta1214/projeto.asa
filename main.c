#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int x, y, z;
    int rank, prev;
} Bloco;

typedef struct {
    int h_max;
    int index ;
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

int compara_y(const void *pa, const void *pb) {
    int y1 = *(const int *) pa;
    int y2 = *(const int *) pb;
    return (y1 < y2) - (y1 > y2);
}
int *lst_y_nrep(Bloco *lst, int len, int *len_lista) {
    int i=0;
    int *ord_y= malloc(len*sizeof(int));
    for (i=0; i<len; i++) {
        ord_y[i]=lst[i].y;
    }
    qsort(ord_y,len,sizeof(int),compara_y);
    int *n_rep= malloc(len*sizeof(int));
    int k=1;
    n_rep[0]=ord_y[0];
    for (i=1; i<len;i++) {
        if (ord_y[i]!=n_rep[k-1]) {
            n_rep[k]=ord_y[i];
            k++;
        }
    }
    free(ord_y);
    *len_lista=k;
    return n_rep;
}
int encontra_rank(int y, int *n_rep_y,int len_lista) {
    int ini=0, end=len_lista-1;
    while (ini <= end) {
        int i = (ini+end)/2;
        if (n_rep_y[i]==y) {
            return i;
        }
        if (n_rep_y[i]>y) {
            ini=i+1;
        } 
        else {
            end=i-1;
        }
    }
    return -1;
}
void atualiza(Node *arvore, int len, int i, Node novo) {
    while (i<len+1) {
        if (novo.h_max>arvore[i].h_max) {
            arvore[i].h_max=novo.h_max;
            arvore[i].index = novo.index;
        }
        i+= i & -i;
    }
}
Node consulta(Node* arvore, int p) {
    Node melhor;
    melhor.h_max=0;
    melhor.index=-1;    
    while (p>0) {
        if (arvore[p].h_max>=melhor.h_max) {
            melhor=arvore[p];
        }
        p-= p & -p;
    }
    return melhor;
}
void construir_torre(Bloco *lst, int len, int *n_rep_y, int len_lista) {
    int i,m=1,k=0,fim,topo=-1;
    int melhor_h_max=0;
    Node melhor;
    Node novo;
    int *h_max=malloc(len*sizeof(int));
    Bloco *lst_final=(Bloco*)malloc(len*sizeof(Bloco));
    Node *arvore= (Node*)malloc((len_lista+1)*sizeof(Node));
    for (i=0; i<len; i++) {
        lst[i].rank=encontra_rank(lst[i].y,n_rep_y,len_lista)+1;
    }
    for (i=0; i<len_lista+1;i++) {
        arvore[i].h_max=0;
        arvore[i].index=-1;
    }
    i=0;
    while (i<len) {
        fim=i;
        while (fim<len && lst[fim].x==lst[i].x) {
            fim++;
        }
        k=i;
        while (k<fim) {
            melhor=consulta(arvore,lst[k].rank-1);
            h_max[k]=lst[k].z+melhor.h_max;
            lst[k].prev=melhor.index;
            if (h_max[k]>melhor_h_max) {
                melhor_h_max=h_max[k];
                topo=k;
            }
            k++;
        }
        k=i;
        while (k<fim) {
            novo.h_max=h_max[k];
            novo.index=k;
            atualiza(arvore, len_lista, lst[k].rank, novo);
            k++;
        }
        i=fim;
    }
    lst_final[0]=lst[topo];
    for(i=1;i<len;i++) {
        k=lst[topo].prev;
        if (k==-1) {
            break;
        }
        m++;
        lst_final[i]=lst[k];
        topo=k;   
    }
    printf("%d %d\n",melhor_h_max,m);
    for (i=m-1;i>=0;i--) {
        printf("%d %d %d\n", lst_final[i].x, lst_final[i].y, lst_final[i].z);
    }
    free(h_max);
    free(arvore);
    free(lst_final);
}
int main() {
    int n;
    int len_lista;
    int *n_rep_y;
    if (scanf("%d", &n)==-1) {
        return -1;
    }
    int len=3*n;
    Bloco *lst = (Bloco*)malloc(sizeof(Bloco)*3*n);
    int i;
    for (i=0; i<3*n; i=i+3) {
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
    qsort(lst,len,sizeof(Bloco),comparaBlocos);
    n_rep_y=lst_y_nrep(lst, len, &len_lista);
    construir_torre(lst,len,n_rep_y,len_lista);
    free(lst);
    free(n_rep_y);
    return 0;
}
