#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Constantes
#define CARACTERES_MAX_LINHA 250
#define TAREFAS_MAX_POR_COLUNA 1000
#define MAX_FOR 200
#define MAX_AUX 20
#define CONST_PENDENTE "pendente"
#define CONST_FAZENDO "fazendo"
#define CONST_FINALIZADO "finalizado"
#define CONST_PENDENTE_SAIDA " Pendente "
#define CONST_FAZENDO_SAIDA " Fazendo "
#define CONST_FINALIZADO_SAIDA " Finalizado "
#define NOME_ARQUIVO_SAIDA "out.txt"
#define VERDADEIRO 1
#define FALSO 0

// Structs
typedef struct {
  char codigo[4];
  char coluna[11];
  char dataEntrega[9];
  char responsavel[24];
  int aberto;
} Tarefa;

typedef struct {
  Tarefa pendente[TAREFAS_MAX_POR_COLUNA];
  Tarefa fazendo[TAREFAS_MAX_POR_COLUNA]; 
  Tarefa finalizado[TAREFAS_MAX_POR_COLUNA];

  int ultimoPendente;
  int ultimoFazendo;
  int ultimoFinalizado;
} QuadroKanban; //Struct que armazena 3 listas que representam as colunas do quadro

//Declaração das Funções
void inicializaQuadroKanban();
int leArquivoEntrada(char[]); // Função que le arquivo de entrada e armazena os cards recebidos
void escolheOperacao(); // Escolhe entre as 7 operações possíveis para o usuário
void salvarQuadro(); // Operação 1: Salva quadro em um arquivo externo out.txt
void criaNovoCartao(); // Operação 2: Cria nova tarefa no quadro kanban
void moveCartaoFazendo(); // Operação 3: Move tarefa para em andamento
void moveCartaoFinalizado(); // Operação 4: Move tarefa para finalizado
void imprimeTarefasPorResponsavel(); // Operação 6: Imprime as tarefas referente ao usuário inserido
void imprimeTarefasPorColuna(); // Operação 5: Imprime as tarefas referente a determinada coluna 
void imprimeTarefasAtrasadas(); // Operação 7: Imprime as tarefas atrasadas a partir de uma entrada de data 
void adicionaTarefa(char[]); // Adiciona tarefa no quadro kanban
void criaTarefaPendente(char[], char[]); // Adiciona tarefa a coluna pendente
void criaTarefaFazendo(char[], char[], char[]); // Adiciona tarefa a coluna fazendo
void criaTarefaFinalizada(char[], char[], char[]); // Adiciona tarefa a coluna finalizado
void imprimePendente(int); // Imprime todos os pendentes
void imprimeFazendo(int); // Imprime todos os em andamento
void imprimeFinalizado(int); // Imprime todos os finalizados
void retiraQuebraLinha(char[]); // Retira \n do final dos inputs
int totalDias(char[]); // Calcula os dias para determinada data, utilizado para verificação de atraso de tarefas

//Variaveis Globais
QuadroKanban quadroKanban;
int terminar = 0;
char terminarListaColunas[CARACTERES_MAX_LINHA] = "", terminarListaPessoa[CARACTERES_MAX_LINHA] = "";

//Funcao Principal
int main(int argc, char *argv[]) {
  inicializaQuadroKanban();
  leArquivoEntrada(argv[1]);
  for (int i =0 ; i < 35; i++){
      escolheOperacao();
  }
}

// Corpo das funções

void inicializaQuadroKanban() {
  quadroKanban.ultimoPendente = 0;
  quadroKanban.ultimoFazendo = 0;
  quadroKanban.ultimoFinalizado = 0;
}

int leArquivoEntrada(char nomeArquivo[]) {
  FILE *arquivo = fopen(nomeArquivo, "r");
  char linhaArquivo[CARACTERES_MAX_LINHA], *result;
  
  while (!feof(arquivo)) 
      if (fgets(linhaArquivo, CARACTERES_MAX_LINHA, arquivo))
        adicionaTarefa(linhaArquivo);
  
  fclose(arquivo);
}

void escolheOperacao() {
  char entrada;
  
  entrada = getchar();
  
  if (entrada == '1') salvarQuadro();
  if (entrada == '2') criaNovoCartao();
  if (entrada == '3') moveCartaoFazendo();
  if (entrada == '4') moveCartaoFinalizado();
  if (entrada == '5') imprimeTarefasPorColuna();
  if (entrada == '6') imprimeTarefasPorResponsavel();
  if (entrada == '7') imprimeTarefasAtrasadas();
}

void salvarQuadro() {
  FILE *arquivo = fopen("out.txt", "w");

  for (int indice = 0; indice < MAX_FOR; indice++) {
    for (int tarefa = 0; tarefa < quadroKanban.ultimoPendente; tarefa++) 
      if (quadroKanban.pendente[tarefa].aberto && atoi(quadroKanban.pendente[tarefa].codigo) == indice) {
        char saida[CARACTERES_MAX_LINHA] = "";
        
        strcat(saida, quadroKanban.pendente[tarefa].codigo);
        strcat(saida, CONST_PENDENTE_SAIDA);
        strcat(saida, quadroKanban.pendente[tarefa].dataEntrega);

        fprintf(arquivo, "%s\n", saida);
      }
    for (int tarefa = 0; tarefa < quadroKanban.ultimoFazendo; tarefa++) 
      if (quadroKanban.fazendo[tarefa].aberto && atoi(quadroKanban.fazendo[tarefa].codigo) == indice) {
        char saida[CARACTERES_MAX_LINHA] = "";
        
        strcat(saida, quadroKanban.fazendo[tarefa].codigo);
        strcat(saida, CONST_FAZENDO_SAIDA);
        strcat(saida, quadroKanban.fazendo[tarefa].dataEntrega);
        strcat(saida, " ");
        strcat(saida, quadroKanban.fazendo[tarefa].responsavel);

        fprintf(arquivo, "%s\n", saida);
      }
    for (int tarefa = 0; tarefa < quadroKanban.ultimoFinalizado; tarefa++)
      if (quadroKanban.finalizado[tarefa].aberto && atoi(quadroKanban.finalizado[tarefa].codigo) == indice) {
        char saida[CARACTERES_MAX_LINHA] = "";
        
        strcat(saida, quadroKanban.finalizado[tarefa].codigo);
        strcat(saida, CONST_FINALIZADO_SAIDA);
        strcat(saida, quadroKanban.finalizado[tarefa].dataEntrega);
        strcat(saida, " ");
        strcat(saida, quadroKanban.finalizado[tarefa].responsavel);

        fprintf(arquivo, "%s\n", saida);
      }
  }

  fclose(arquivo);
  terminar = 1;
}

void criaNovoCartao() {
  char entradaCodigo[MAX_AUX], entradaData[MAX_AUX];
  scanf("%s %s", entradaCodigo, entradaData);

  retiraQuebraLinha(entradaData);
  
  criaTarefaPendente(entradaCodigo, entradaData);
}

void moveCartaoFazendo() {
  int indiceTarefaDesativada = -1;
  char entradaCodigo[MAX_AUX], entradaResponsavel[MAX_AUX];
  scanf("%s %s", entradaCodigo, entradaResponsavel);

  for (int tarefa = 0; tarefa < TAREFAS_MAX_POR_COLUNA; tarefa++) 
    if (strcmp(quadroKanban.pendente[tarefa].codigo, entradaCodigo) == 0) 
      indiceTarefaDesativada = tarefa;

  if (indiceTarefaDesativada == -1) {
    printf("Operacao invalida\n");
    return;
  }
  quadroKanban.pendente[indiceTarefaDesativada].aberto = FALSO;
  criaTarefaFazendo(entradaCodigo, quadroKanban.pendente[indiceTarefaDesativada].dataEntrega, entradaResponsavel);
}

void moveCartaoFinalizado() {
  int indiceTarefaDesativada = -1;
  char entradaCodigo[MAX_AUX];
  scanf("%s", entradaCodigo);

  for (int tarefa = 0; tarefa < TAREFAS_MAX_POR_COLUNA; tarefa++) 
    if (strcmp(quadroKanban.fazendo[tarefa].codigo, entradaCodigo) == 0) 
      indiceTarefaDesativada = tarefa;

  if (indiceTarefaDesativada == -1) {
    printf("Operacao invalida\n");
    return;
  }
  quadroKanban.fazendo[indiceTarefaDesativada].aberto = FALSO;
  criaTarefaFinalizada(entradaCodigo, quadroKanban.fazendo[indiceTarefaDesativada].dataEntrega, quadroKanban.fazendo[indiceTarefaDesativada].responsavel);
}

void imprimeTarefasPorResponsavel() {
  char entrada[MAX_AUX];
  scanf("%s", entrada);

  if (strcmp(entrada, terminarListaPessoa) == 0) {
    terminar = 1;
    return;
  }
  
  for (int indice = 0; indice < MAX_FOR; indice++) {
    for (int tarefa = 0; tarefa < quadroKanban.ultimoFazendo; tarefa++) 
      if (quadroKanban.fazendo[tarefa].aberto && strcmp(quadroKanban.fazendo[tarefa].responsavel, entrada) == 0 && atoi(quadroKanban.fazendo[tarefa].codigo) == indice)
       imprimeFazendo(tarefa);
    for (int tarefa = 0; tarefa < quadroKanban.ultimoFinalizado; tarefa++)
      if (quadroKanban.finalizado[tarefa].aberto && strcmp(quadroKanban.finalizado[tarefa].responsavel, entrada) == 0 && atoi(quadroKanban.finalizado[tarefa].codigo) == indice)
        imprimeFinalizado(tarefa);
  }
    
}

void imprimeTarefasPorColuna() {
  char entrada[MAX_AUX];
  scanf("%s", entrada);
  if (strcmp(entrada, terminarListaColunas) == 0) {
    terminar = 1;
    return;
  }
  for (int indice = 0; indice < MAX_FOR; indice++) {
    if (strcmp("Pendente", entrada) == 0) {
      for (int tarefa = 0; tarefa < quadroKanban.ultimoPendente; tarefa++)
        if(quadroKanban.pendente[tarefa].aberto && atoi(quadroKanban.pendente[tarefa].codigo) == indice)
          imprimePendente(tarefa);
    } else if (strcmp("Fazendo", entrada) == 0) {
      for (int tarefa = 0; tarefa < quadroKanban.ultimoFazendo; tarefa++) 
        if (quadroKanban.fazendo[tarefa].aberto && atoi(quadroKanban.fazendo[tarefa].codigo) == indice)
         imprimeFazendo(tarefa);
    } else if (strcmp("Finalizado", entrada) == 0) {
      for (int tarefa = 0; tarefa < quadroKanban.ultimoFinalizado; tarefa++)
        if (quadroKanban.finalizado[tarefa].aberto && atoi(quadroKanban.finalizado[tarefa].codigo) == indice)
          imprimeFinalizado(tarefa);
    }
  }
  
}

void imprimeTarefasAtrasadas() {
  char entradaData[MAX_AUX];
  scanf("%s", entradaData);
  
  if (strcmp(entradaData, "") == 0) {
    terminar = 1;
    return;
  }
  int entradaTotalDias = totalDias(entradaData);

  for (int indice = 0; indice < MAX_FOR; indice++) {
    for (int tarefa = 0; tarefa < quadroKanban.ultimoPendente; tarefa++) 
      if (quadroKanban.pendente[tarefa].aberto && totalDias(quadroKanban.pendente[tarefa].dataEntrega) < entradaTotalDias && atoi(quadroKanban.pendente[tarefa].codigo) == indice)
       imprimePendente(tarefa);
    for (int tarefa = 0; tarefa < quadroKanban.ultimoFazendo; tarefa++) 
      if (quadroKanban.fazendo[tarefa].aberto && totalDias(quadroKanban.fazendo[tarefa].dataEntrega) < entradaTotalDias && atoi(quadroKanban.fazendo[tarefa].codigo) == indice)
       imprimeFazendo(tarefa);
  }
  printf("\n");
}

void adicionaTarefa(char input[]) {
    const char divisor[2] = " ";
    char *total, status[MAX_AUX], dataEntrega[MAX_AUX], responsavel[1000], codigo[MAX_AUX];

    total = strtok(input, divisor);
    strcpy(codigo, total);
    total = strtok(NULL, divisor);
    strcpy(status, total);
    total = strtok(NULL, divisor);
    strcpy(dataEntrega, total);
  
    if (strcmp(status, "Pendente") == 0) criaTarefaPendente(codigo, dataEntrega);
    else {
      total = strtok(NULL, divisor);
      strcpy(responsavel, total);
      
      if (strcmp(status, "Fazendo") == 0) criaTarefaFazendo(codigo, dataEntrega, responsavel);
    if (strcmp(status, "Finalizado") == 0) criaTarefaFinalizada(codigo, dataEntrega, responsavel);  
    }
    
    
     
  }

void criaTarefaPendente (char codigo[], char dataEntrega[]) {
    retiraQuebraLinha(dataEntrega);
    
    strcpy(quadroKanban.pendente[quadroKanban.ultimoPendente].codigo, codigo);
    strcpy(quadroKanban.pendente[quadroKanban.ultimoPendente].coluna, CONST_PENDENTE);
    strcpy(quadroKanban.pendente[quadroKanban.ultimoPendente].dataEntrega, dataEntrega);
    strcpy(quadroKanban.pendente[quadroKanban.ultimoPendente].responsavel, "");
    quadroKanban.pendente[quadroKanban.ultimoPendente].aberto = VERDADEIRO;

    quadroKanban.ultimoPendente++;
  }

void criaTarefaFazendo (char codigo[], char dataEntrega[], char responsavel[]) {
    retiraQuebraLinha(responsavel);
    strcpy(quadroKanban.fazendo[quadroKanban.ultimoFazendo].codigo, codigo);
    strcpy(quadroKanban.fazendo[quadroKanban.ultimoFazendo].coluna, CONST_FAZENDO);
    strcpy(quadroKanban.fazendo[quadroKanban.ultimoFazendo].dataEntrega, dataEntrega);
    strcpy(quadroKanban.fazendo[quadroKanban.ultimoFazendo].responsavel, responsavel);
    quadroKanban.fazendo[quadroKanban.ultimoFazendo].aberto = VERDADEIRO;

    quadroKanban.ultimoFazendo++;
  }

void criaTarefaFinalizada (char codigo[], char dataEntrega[], char responsavel[]) {
    retiraQuebraLinha(responsavel);
    
    strcpy(quadroKanban.finalizado[quadroKanban.ultimoFinalizado].codigo, codigo);
    strcpy(quadroKanban.finalizado[quadroKanban.ultimoFinalizado].coluna, CONST_FINALIZADO);
    strcpy(quadroKanban.finalizado[quadroKanban.ultimoFinalizado].dataEntrega, dataEntrega);
    strcpy(quadroKanban.finalizado[quadroKanban.ultimoFinalizado].responsavel, responsavel);
    quadroKanban.finalizado[quadroKanban.ultimoFinalizado].aberto = VERDADEIRO;

    quadroKanban.ultimoFinalizado++;
  }

void retiraQuebraLinha(char entrada[]) {
  for (int caracter = 0; entrada[caracter] != '\0'; caracter++) {
    if (entrada[caracter] == '\n') {
      entrada[caracter] = '\0';
    }
  }
}

void imprimePendente(int indice) {
  printf("\n%s Pendente %s", quadroKanban.pendente[indice].codigo, quadroKanban.pendente[indice].dataEntrega);
}

void imprimeFazendo(int indice) {
  printf("\n%s Fazendo %s %s", quadroKanban.fazendo[indice].codigo, quadroKanban.fazendo[indice].dataEntrega, quadroKanban.fazendo[indice].responsavel);
}

void imprimeFinalizado(int indice) {
  printf("\n%s Finalizado %s %s", quadroKanban.finalizado[indice].codigo, quadroKanban.finalizado[indice].dataEntrega, quadroKanban.finalizado[indice].responsavel);
}

int totalDias(char data[]) {
  char dia[3], mes[3], ano[3];

  dia[0] = data[0];
  dia[1] = data[1];
  mes[0] = data[3];
  mes[1] = data[4];
  ano[0] = data[6];
  ano[1] = data[7];

  return atoi(dia) + (atoi(mes)*30) + (atoi(ano)*365);
}
