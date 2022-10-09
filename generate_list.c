#include <stdio.h>
#include <stdlib.h>

int main(void)
{
  int i = 0;
  int tam = 0; 
  printf("Digite o tamanho da lista aleatória: ");
  scanf("%d", &tam);
  int lista[tam];
  printf("Gerando lista com %d valores aleatórios:\n\n", tam);
 
  for (i ; i < tam; i++)
  {
    /* gerando valores aleatórios entre zero e tam*/
    lista[i] = rand() % tam;  
    printf("%d ",lista[i]);
  }

  return 0;
}