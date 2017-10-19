#include <stdio.h> 
#include <stdlib.h> 
#include <ctype.h>
#include <time.h>
#define TAM_MEMORIA 100
#define TEMP_MAX 40 
#define TEMP_MIN 20
#define HOLE 'H'
#define PROCESS 'P'

typedef struct Node {
	char tipo;
	char label[5];
	int hora_inicio, min_inicio, seg_inicio; 
	int tempo_execucao, tamanho;
	struct Node *prox, *ant;
}Node;

typedef struct Cabecalho {
	Node *inicio, *fim;
	int qtd_process;
	int tam_holes;
	int qtd_espacos; //Armazena a quantidade de processos e holes na memória
}Cabecalho;

void abrir_arquivo(Cabecalho *c) {
	 /*FILE *file;
	 file = fopen("conteudo.txt", "r");
	
	 if(file == NULL) {
		fclose(file);
	 }else {
		char frase[100];

		while(fgets(frase, 100, file) != NULL) {
		  printf("%s", frase);
		}
		
		//Atualizar cabeçalho
		int item_linha, i = 0, conta_espaco = 0;
		
		frase[0];
		while(frase[i] != ' ') {
			item_linha = frase[i];
			i++;
		}
		
		
		fclose(file);
	 }
	 */ 
}

Cabecalho *iniciar_cabecalho() {
	Cabecalho *c = (Cabecalho*) malloc (sizeof (Cabecalho));
	c -> inicio = c -> fim = NULL; 
	c -> qtd_process = 0;
	c -> qtd_espacos = 0;
	c -> tam_holes = 0;

	return c;

}/*Fim-iniciar_cabecalho*/

Cabecalho *iniciar_memoria(Cabecalho *c) { //É o mesmo que iniciar a lista
	
	Node *memoria = (Node*) malloc (sizeof(Node));

	time_t t = time(NULL);
	struct tm horario = *localtime(&t);

	memoria -> tipo = HOLE;
	memoria -> label[0] = 'H';
	memoria -> label[1] = 'O';
	memoria -> label[2] = 'L';
	memoria -> label[3] = 'E';
	memoria -> label[4] = '\0';
	memoria -> tempo_execucao = 0;
	memoria -> hora_inicio = horario.tm_hour;
	memoria -> min_inicio = horario.tm_min;
	memoria -> seg_inicio = horario.tm_sec;
	memoria -> tamanho = TAM_MEMORIA;
	memoria -> prox = memoria; //Aponta para ele mesmo porque é uma lista circular
	memoria -> ant = memoria;
	
	//Colocando informações no cabecalho 
	c -> inicio = memoria; 
	c -> fim = memoria;
	c -> tam_holes = TAM_MEMORIA;
	c -> qtd_espacos = 1;

	return c;
	
}/*Fim-iniciar_memoria*/

Cabecalho *unir_holes(Cabecalho *c) { //Função que une todos os holes que estiverem apontando para outros holes
	Node *aux, *aux2;
	int j;
	
	
	if(c -> inicio != c -> fim && (c -> qtd_espacos - c -> qtd_process) > 1) {
		aux = c -> inicio;
		
		for(j = 1; j < c -> qtd_espacos; j++){
			
			if(aux -> tipo == HOLE && aux -> prox -> tipo == HOLE && aux -> prox != c -> inicio) {

				aux2 = aux -> prox;
				
				aux -> tamanho += aux2 -> tamanho;
				
				//Ajustando ponteiros
				if(aux2 == c -> fim) {
					c -> fim = aux;
				}
				
				aux -> prox = aux2 -> prox;
				aux2 -> prox -> ant = aux;
				
				aux = aux2 -> prox;
				//Excluindo hole a mais
				free(aux2);
				c -> qtd_espacos--;
				
			}
			
			else {
				aux = aux -> prox;
			}
			
			
		}/*Fim-for*/
		
	}
	
	
	return c;
	
}/*Fim-unir_holes*/

void imprimir_posicoes(Cabecalho *c);

Cabecalho *parar_process(Cabecalho *c, char label[5]) {
	Node *aux;
	int i;
	
	c = unir_holes(c);
	
	aux = c -> inicio;
	
	//Verifica no inicio
	if(aux -> label[0] == label[0]) { 
		aux -> tipo = HOLE;				
		aux -> label[0] = 'H';
		aux -> label[1] = 'O';
		aux -> label[2] = 'L';
		aux -> label[3] = 'E';
		aux -> label[5] = '\0';
		aux -> tempo_execucao = 0;
		c -> tam_holes += aux -> tamanho;
		c -> qtd_process--;
	} 
	
	else {
		
		aux = aux -> prox;
		
		for(i = 2; i <= c -> qtd_espacos; i++){
			if(aux -> label[0] == label[0]) { 
				aux -> tipo = HOLE;				
				aux -> label[0] = 'H';
				aux -> label[1] = 'O';
				aux -> label[2] = 'L';
				aux -> label[3] = 'E';
				aux -> label[5] = '\0';
				aux -> tempo_execucao = 0;
				c -> tam_holes += aux -> tamanho;
				c -> qtd_process--;
				break;
			}
			aux = aux -> prox;
		}
	}
	
	c = unir_holes(c);
	
	imprimir_posicoes(c);
	
	return c;
	
}/*Fim-parar_process*/

Cabecalho *verificar_tempo(Cabecalho *c){
	Node *aux = c->inicio;
	
	c = unir_holes(c);
	
	int hora_atual, hora_inicio;
	time_t t = time(NULL);
	struct tm horario = *localtime(&t);

	hora_atual = (horario.tm_hour * 3600) + (horario.tm_min * 60) + (horario.tm_sec);
	
	//Primeiro loop
	hora_inicio = (aux -> hora_inicio * 3600) + (aux -> min_inicio * 60) + aux -> seg_inicio;
		
	if((hora_inicio + aux -> tempo_execucao) <= hora_atual && aux -> tipo == PROCESS)
		c = parar_process(c, aux -> label);
		
	aux = aux->prox;
	
	//Proximos loops
	while(aux != c->inicio){
		hora_inicio = (aux -> hora_inicio * 3600) + (aux -> min_inicio * 60) + aux -> seg_inicio;
		
		if((hora_inicio + aux -> tempo_execucao) <= hora_atual && aux -> tipo == PROCESS)
			c = parar_process(c, aux -> label);
			
		aux = aux->prox;			
	}
	
	return c;

}/*Fim-verificar_tempo*/

Node *criar_novo_process(Cabecalho *c, char label[5], int tamanho) {
	Node *novo = (Node*) malloc (sizeof (Node)); 
	int i = 0;
	time_t t = time(NULL);
	struct tm horario = *localtime(&t);

	novo -> tipo = PROCESS;  //Acrescenta o processo
	novo -> tamanho = tamanho; 
	novo -> tempo_execucao = (rand() % TEMP_MAX) + TEMP_MIN; //Para o tempo nunca ser 0, e ter valor mínimo 3
	novo -> hora_inicio = horario.tm_hour;
	novo -> min_inicio = horario.tm_min;
	novo -> seg_inicio = horario.tm_sec;

	while (label[i] != '\0') {
		novo -> label[i] = label[i];
		i++;
	}

	return novo; 
	
}/*Fim-criar_novo_process*/

Cabecalho *compactar_memoria(Cabecalho *c) { //Função que pode ser usada quando se inserir um process
	Node *aux, *aux2;
	
	int tam_hole = 0;
	int i = 1;
	aux = c -> inicio;
	
	//Percorrer a lista do seu inicio até o seu penúltimo node
	for(i = 1; i < c -> qtd_espacos; i++) { 
		
		if(aux -> tipo == HOLE) {
			
			//Caso o inicio seja um hole
			if(aux == c -> inicio) {
				c -> inicio = aux -> prox;
			} 
			
			aux2 = aux;
			tam_hole += aux2 -> tamanho;
			
			//Ajustando apontadores
			aux -> ant -> prox = aux -> prox;
			aux -> prox -> ant = aux -> ant;
			
			free(aux2); 
		}
		
		aux = aux -> prox;
	}/*Fim-for*/
	
	//Tratar último node
	if(aux -> prox -> tipo == PROCESS) {
		
		//Criando novo hole para colocar no final da memória
		aux2 = criar_novo_process(c, "HOLE", tam_hole);
		
		aux2 -> tipo = HOLE;
		
		//Ajustando ponteiros
		aux2 -> prox = c -> inicio;
		aux2 -> ant = aux;
		aux -> prox = aux2;
		c -> inicio -> ant = aux2;
		c -> fim = aux2;
		
		
	}
	
	//Último node é um hole
	else {
		aux2 = aux -> prox;
		aux2 -> tamanho = tam_hole;
		
		//Ajustando ponteiros
		aux2 -> prox = c -> inicio;
		aux2 -> ant = aux;
		c -> fim = aux2;
		c -> inicio -> ant = aux2;
		
	}/*Fim-if else*/
	
	//Atualizando qtd_espacos
	i = 1;
	for(aux = c -> inicio; aux -> prox != c -> fim; aux = aux -> prox)
		i++;
	
	i++; //incremento correspondente ao último espaço
	
	c -> qtd_espacos = i;
	
	return c;
	
}/*Fim-compactar_memoria*/

Cabecalho *inserir_process(Cabecalho *c, char label[5], int tamanho) {

	Node *aux, *novo, *aux2; 
	int i; 
	aux = c -> inicio;
	
	c = unir_holes(c);
	
	novo = criar_novo_process(c, label, tamanho);
	
	if(aux -> prox == c -> inicio && aux -> tipo == HOLE) { //Caso 1: O início aponta para ele mesmo, quando não há nenhum process ainda, só um hole

		if (novo -> tamanho == TAM_MEMORIA) { //Se entrar nesse if, não terá buraco, e as informações do cabeçalho são todas do novo processo
			aux = c -> inicio; //aux tem o endereço do hole 
			c -> inicio = c -> fim = novo;
			novo -> prox = novo -> ant = c -> inicio;
			c -> tam_holes = 0;
			
			//Excluindo hole
			free(aux);
		}
		
		else { //Aqui o fim vai ser o buraco memoria, que é igual a c->fim e o inicio é igual ao novo processo
			c -> fim -> tamanho = c -> tam_holes = TAM_MEMORIA - novo -> tamanho;
			c -> inicio = novo;
			novo -> prox = novo -> ant = c -> fim;
			c -> fim -> prox = c -> fim -> ant = novo; //Como a lista é circular o proximo no do fim vai ser o inicio e o inicio é o novo
			c -> qtd_espacos++;
		}/*Fim-if_else*/
		
		c -> qtd_process++;
		
		imprimir_posicoes(c);
		
	}/*Fim-if Caso 1*/

	else if(c -> qtd_process == 1 && c -> qtd_espacos == 2) {//Caso 2: A memória só tem um process apontando para um hole 

		if (novo -> tamanho == c -> tam_holes) {
			aux = c -> inicio -> prox;
			c -> inicio -> prox = c -> inicio -> ant = novo; //Lista circular
			novo -> ant = novo -> prox = c -> inicio;
			c -> fim = novo;
			c -> qtd_espacos = 2;
			c -> tam_holes = 0;
			free(aux);
		}

		else if(novo -> tamanho < c -> tam_holes) {
			novo -> prox = c -> fim;
			novo -> ant = c -> inicio;
			c -> inicio -> prox = novo;
			c -> fim -> ant = novo;
			c -> fim -> tamanho = (c -> fim -> tamanho - novo -> tamanho);
			c -> qtd_espacos = 3;
			c -> tam_holes = c -> fim -> tamanho;
		}
		
		c -> qtd_process++;
		
		imprimir_posicoes(c);
		
	}/*Fim-if Caso 2*/

	else if(novo -> tamanho <= c -> tam_holes) { //Caso 3: Casos gerais para inserir um process
	
		//Criando novo process
		novo = criar_novo_process(c, label, tamanho);
	
		int verifica_inseriu = 0;
		i = 1;
		
		aux = c -> inicio; 
		while(i <= c -> qtd_espacos) { //Percorre a lista de acordo com a sua quantidade de nós
				
			//Verifica se o node do momento é um hole && se ele tem o mesmo tamanho do process a ser inserido
			if(aux -> tipo == HOLE && aux -> tamanho == novo -> tamanho) {
				//OBS.: aux tem o endereço do buraco
				//Ajustando ponteiros do novo process
				novo -> prox = aux -> prox;
				novo -> ant = aux -> ant;
				
				if(aux == c -> inicio) {
					c -> inicio = novo;
				} 
				
				else if(aux == c -> fim) {
					c -> fim = novo;
				}
				
				//Ajustando ponteiros dos vizinhos do novo process
				aux -> ant -> prox = aux -> prox -> ant = novo;
				
				//Excluindo hole
				free(aux);
				
				//Atualizando qtd
				c -> qtd_process++;
				
				imprimir_posicoes(c);
				verifica_inseriu = 1;
				break;
			}
				
			//Verifica se o node do momento é um hole && se ele tem o tamanho menor do que o do process a ser inserido
			else if(aux -> tipo == HOLE && novo -> tamanho < aux -> tamanho) {
				//OBS.: aux tem o endereço do buraco
				if(aux == c -> inicio) {
					c -> inicio = novo;
				} 
				
				//Diminuindo tamanho do hole
				aux -> tamanho = (aux -> tamanho - novo -> tamanho);
				c -> tam_holes = (c -> tam_holes - novo -> tamanho);
				
				//Ajustando ponteiros do novo process
				novo -> prox = aux;
				novo -> ant = aux -> ant;
				
				//Ajustando ponteiros dos vizinhos do novo process
				aux -> ant -> prox = novo;
				aux -> ant = novo;
				
				//Atualizando qtd
				c -> qtd_process++;
				c -> qtd_espacos++;
				
				imprimir_posicoes(c);
				verifica_inseriu = 1;
				break;
			}
			
			else {
				aux = aux -> prox;
				i++;
			}
			
		}
		
		//Caso não tenha inserido no while acima, este if vai compactar a memória para depois inserir o process
		if(i == c -> qtd_espacos && verifica_inseriu == 0) {
			c = compactar_memoria(c);
			
			c -> fim -> tamanho -= novo -> tamanho; 
			
			//Inserindo process
			if(c -> fim -> tamanho == 0) {
				aux2 = c -> fim;
				
				aux2 -> ant -> prox = c -> fim = novo;
				
				novo -> ant = aux2 -> ant;
				novo -> prox = c -> inicio;
				c -> inicio -> ant = novo;
				
				free(aux2);
				
			}
			
			else {
				novo -> prox = c -> fim;
				novo -> ant = c -> fim -> ant;
				c -> fim -> ant -> prox = novo;
				c -> fim -> ant = novo;
			}
			
			imprimir_posicoes(c);
			
		}
	
	}/*Fim-if Caso 3*/
	
	else {
		printf("\nNAO FOI POSSIVEL INSERIR UM PROCESSO NESTE MOMENTO\n\n");
	}
	
	return c;

}/*Fim-inserir_process*/

void imprimir_dados_process(Cabecalho *c, char label[5]) {
	Node *aux;
	int h_final, m_final, s_final, i = 1;
	
	if(c -> qtd_espacos == 1 && c -> inicio -> tipo == HOLE) {
		printf("Nao ha processo a ser impresso no momento!");
	}

	else {
		aux = c -> inicio;
		while(i <= c -> qtd_espacos) {
			if(aux -> label == label) { //Pesquisa na lista
				break;
			}
			
			aux = aux -> prox;
			i++;
		}
		
		//Ajustando hora final
		s_final = aux -> seg_inicio + aux -> tempo_execucao;
		m_final = 0;
		while(s_final > 60) {
			s_final -= 60;
			m_final++;
		}
		
		m_final += aux -> min_inicio;
		h_final = 0;
		while(m_final > 60) {
			m_final -= 60;
			h_final++;
		}
		
		h_final += aux -> hora_inicio;
		
		if(h_final >= 24){
			h_final -= 24;
		}
		
		
		printf("O processo %c possui %dKB.\n", aux -> label[0], aux -> tamanho); 
		printf("O processo %c leva %ds para ser executado.\n", aux -> label[0], aux -> tempo_execucao);
		printf("O processo foi iniciado as %d:%d:%d e sera encerrado as %d:%d:%d.", aux -> hora_inicio, aux -> min_inicio, aux -> seg_inicio, h_final, m_final, s_final);
	}
	
	
	
}/*Fim-imprimir_dados_process*/

void imprimir_posicoes(Cabecalho *c) {
	
	printf("\n Posicoes na memoria: %d\n Processos em execucao: %d\n Tamanho holes: %dKB \n", c -> qtd_espacos, c -> qtd_process, c -> tam_holes);
	
	Node *aux;
	int i, j = 0;
	
	printf("\n <<<<ESTADO DA MEMORIA>>>>\n\n");
	//Imprimindo 1º node
	aux = c -> inicio;
	for(i = 1; i <= aux -> tamanho; i++) {
		if(j == 10) {
			printf("\n");
			j = 0;
		}
		j++;
		
		if(aux -> tipo == HOLE) {
			printf(" %s", aux -> label);
		}
		
		else {
			printf(" %c   ", aux -> label[0]);
		}
		
	}
	printf(" AcabouNode ");
	aux = aux -> prox;
	
	
	while(aux != c -> inicio) {
		
		for(i = 1; i <= aux -> tamanho; i++) {
			if(j == 10) {
				printf("\n");
				j = 0;
			}
			j++;
			
			if(aux -> tipo == HOLE) {
				printf(" %s", aux -> label);
			}
			
			else {
				printf(" %c   ", aux -> label[0]);
			}
			
		}
		
		//printf("\n");
		printf(" AcabouNode ");
		aux = aux -> prox;
		
	}
	
	printf("\n\n");
	
}/*Fim-imprimir_posicoes_memoria*/

void gravar_arquivo (Cabecalho *c) {
	
	Node *aux = c -> inicio;
	FILE *file;
	file = fopen("memoria.txt", "w"); // w é de write
	int i;
	
	if(file == NULL) { 
		printf("O arquivo nao pode ser criado\n"); 
		system("pause");
	}
	
	int hora_atual, hora_inicio;
	time_t t = time(NULL);
	struct tm horario = *localtime(&t);

	hora_atual = (horario.tm_hour * 3600) + (horario.tm_min * 60) + (horario.tm_sec);
	hora_inicio = (aux -> hora_inicio * 3600) + (aux -> min_inicio * 60) + aux -> seg_inicio;
	hora_inicio += aux -> tempo_execucao;
	
	hora_inicio -= hora_atual;

	
	if(c -> qtd_espacos == 1) {
		
		fprintf(file, "%d %d %d\n", c -> qtd_espacos, c -> qtd_process, c-> tam_holes);
		fprintf(file, "%c %s %d %d\n", aux -> tipo, aux -> label, aux -> tamanho, hora_inicio);
	}

	else {
		
		fprintf(file, "%d %d %d\n", c -> qtd_espacos, c -> qtd_process, c-> tam_holes);
		fprintf(file, "%c %s %d %d\n", aux -> tipo, aux -> label, aux -> tamanho, hora_inicio);

		for(i = 2; i <= c -> qtd_espacos; i++){
			hora_atual = (horario.tm_hour * 3600) + (horario.tm_min * 60) + (horario.tm_sec);
			hora_inicio = (aux -> hora_inicio * 3600) + (aux -> min_inicio * 60) + aux -> seg_inicio;
			hora_inicio += aux -> tempo_execucao;
			
			hora_atual -= hora_inicio;
			
			fprintf(file, "%c %s %d %d\n", aux -> tipo, aux -> label, aux -> tamanho, hora_inicio);
			aux = aux -> prox;
		}

	}
	
	fclose(file); // salvar e fechar o arquivo
	system("pause");
}

void menu(){
	printf("\n <<<< MEMORIA COM TAMANHO TOTAL DE %dKB >>>>\n", TAM_MEMORIA);
	printf("\n <<<<< MENU >>>>>\n");
	printf(" 0 - SAIR\n");
	printf(" 1 - INICIAR PROCESSO\n");
	printf(" 2 - TERMINAR PROCESSO\n");
	printf(" 3 - IMPRIMIR OS DADOS DO PROCESSO\n");
	printf(" 4 - MOSTRAR POSIÇÕES LIVRES E OCUPADAS NA MEMÓRIA\n");
	printf("\tOPÇÃO: ");
}/*Fim-menu*/

int main () {
	Cabecalho *c;
	int opcao = 2, i, tam;
	char label[5], gravar;

	c = iniciar_cabecalho();
	c = iniciar_memoria(c);
	
	//abrir_arquivo(c);

	while(opcao != 0){
			menu();
			scanf("%d", &opcao);
			/*while(opcao < 0 || opcao > 4) {
				printf("\n\t\t\tDIGITE UMA OPCAO VALIDA: ");
				scanf("%d", &opcao);
			}*/
			switch(opcao){
				case 0: 
					setbuf(stdin, NULL);
					printf("Deseja gravar a memoria em um arquivo (S/N)?\n");
					scanf("%c", &gravar);
					c = verificar_tempo(c);
					
					if(gravar == 's') {
						gravar = 'S';
					}
					
					if(gravar == 'S') {
						gravar_arquivo(c);
					}
					break;
				case 1:
					setbuf(stdin, NULL);
					printf("Qual o nome do processo que deseja iniciar\n");
					scanf("%s", label);
					
					i = 0;
					while (label[i] != '\0'){
						label[i] = toupper(label[i]);
						i++;
					}
					label[1] = ' ';
					label[2] = ' ';
					label[3] = ' ';
					label[4] = '\0';
					
					printf("Qual o tamanho do processo?\n");
					scanf("%d", &tam);
					
					opcao++;
					c = verificar_tempo(c);
					c = inserir_process(c, label, tam);
					break;
				case 2:
					setbuf(stdin, NULL); //função para limpar o teclado, pq ele pode ter guardado o '\n' da ultima string lida
					printf("Qual processo deseja fechar? ");
					scanf("%s", label);
					
					i = 0;
					while (label[i] != '\0'){
						label[i] = toupper(label[i]);
						i++;
					}
					label[1] = ' ';
					label[2] = ' ';
					label[3] = ' ';
					label[4] = '\0';
					
					opcao++;
					c = verificar_tempo(c);
					c = parar_process(c, label);
					break;
				case 3:
					setbuf(stdin, NULL);
					printf("Qual o nome do processo deseja imprimir?\n");
					scanf("%s", label);
					
					i = 0;
					while (label[i] != '\0'){
						label[i] = toupper(label[i]);
						i++;
					}
					label[1] = ' ';
					label[2] = ' ';
					label[3] = ' ';
					label[4] = '\0';
					
					opcao++;
					c = verificar_tempo(c);
					imprimir_dados_process(c, label);
					
					break;
				case 4:
					c = unir_holes(c);
					printf("\nLegenda: H = hole, qualquer outra letra = label do processo.\n\n");
					
					opcao = 3;
					c = verificar_tempo(c);
					imprimir_posicoes(c);
					break;					
			}
		}

	return 0;
}/*Fim-main*/
