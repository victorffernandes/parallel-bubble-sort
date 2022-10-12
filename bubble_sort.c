#include <stdio.h>
#include <stdlib.h>

void listGenerator(int* lista, int size){
  for (int i = 0 ; i < size; i++)
  {
    /* gerando valores aleatórios entre zero e tam*/
    lista[i] = rand() % size;  

  }

}

void bubble_sort (int *vetor, int n) {
    int i, j, aux;

    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            if (vetor[j] > vetor[j + 1]) {
                aux = vetor[j];
                vetor[j] = vetor[j + 1];
                vetor[j + 1] = aux;
            }
        }
    }
}

int main(void)
{
  int tam = 16;
  int lista[tam];
 
  listGenerator(lista, tam);
      
  for (int j = 0; j < tam; j++)
  {
    // imprimindo valores da lista aleatória
    printf("%d ",lista[j]);
  }

  bubble_sort(lista,tam);
  printf("\n\n");
  
  for (int j = 0; j < tam; j++)
  {
    // imprimindo valores ordenados pelo bubble sort
    printf("%d ",lista[j]);
  }

  return 0;
}