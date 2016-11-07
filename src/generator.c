#include <stdio.h>
#include <stdlib.h>
#include "generator.h"

//Iniciar GERADOR
//IDEIA: GERAR AO INICAR O GERADOR AS CIDADES, A LISTA DE CIADAES, A MATRIZ DE DISTANCIAS
// A MATRIZ DE COMBINAÇÕES // GERAR O NUMERO DE CAMINHÕES
// E JÁ GERAR A MELHOR ROTA DE TAL FORMA QUE O NUM DE CAMINHÕES SEJA

//INICIALIZA O GERADOR 5
void initGenerator(Generator* g){
	g->truck_capacity = 50;
	unsigned int i,possibility = 0;
	int *bestRoute;
	g->vector_Aux = (unsigned int*)malloc(MAX*sizeof(unsigned int));
	g->combinations = (int**)malloc(MAXX*sizeof(unsigned int*));
	for (i = 0; i < MAX; i++) {
		g->combinations[i] = (int*)malloc(MAX*sizeof(unsigned int));
	}
	CityStack cs;
	g->number_of_cities = 0;
	g->number_of_combinations = 0;
	g->number_of_permutations = 0;
	generateCities(g, &cs);//Apenas ENTRADA
	generateDistances(g);//Apenas ENTRADA
	while(!generateTrucks(g,&cs)){
		generateCities(g,&cs);
	}
	for(i=0;i<=g->number_of_cities;i++){
		possibility += Fatorial(i);//Calculando as possibilidades máximas de alocação de memória
	}
	g->permutations = (int**)malloc(possibility*sizeof(int*));//Alocando memória suficiente para a matiz de permutações
	generatePermutation(1, g);//gerando recursivamente as permutações recursivamente.
	for(i=0; i<g->number_of_trucks;i++){
		generateCombinations(g,i); // GERA COMBINAÇÕES COM ZEROS
	}
	generateRoute(g,&bestRoute);
    //EXIBE A MELHOR ROTA.
    for(i=0;i<=g->number_of_cities+(3+g->number_of_trucks);i++){
        if(bestRoute[i]==-2){
            break;
        }else if (i==0){
            printf("[");
           printf(" %d ", bestRoute[i]);
        }else if (bestRoute[i]==0 && bestRoute[i+1]!=-2){
            printf(" %d ", bestRoute[i]);
            printf("][");
            printf(" %d ", bestRoute[i]);
        }else if(bestRoute[i]==0){
            printf(" %d ", bestRoute[i]);
            printf("]");
        }else{
            printf(" %d ", bestRoute[i]);
        }

   }
}

//GERA TODAS AS PERMUTAÇÕES
void generatePermutation(unsigned int nivel, Generator* g, unsigned int n){
// nivel, da recursão e K é permutar de quantas casas ex: 3 cidade de K=1 seria [1] [2] [3];
    unsigned int i, j, parada = 0;
    int *vec_temp; // Vetor temporário para auxiliar a bagunça;
    vec_temp = (int*)malloc((n*2)*sizeof(int)); //Aloca dinâmicamente para usar o vetor auxiliar.
    if (nivel<=n) {
        for (i=1; i<=g->number_of_cities; i++) {
             g->vector_Aux[nivel]=i; // Usa a recursão para gerar numeros para o vetor auxiliar
             generatePermutation(nivel+1, g, n);
        }
    } else {
         for (i=1; i<=n; i++) {
              for (j=1; j<=n; j++) {
                   if((g->vector_Aux[i]==g->vector_Aux[j])&&(i != j)){
                        //estuda e remove permutações de casa repetidas
                        parada = 8;
                        break;
                   }
               }
            }
            if(parada != 8){ // se não houver numeros repetidos no vetor
                 for (i=1; i<=n; i++) {
                        vec_temp[i] =  g->vector_Aux[i]; // o vetor temporario recebe os numeros gerados na recursao
                 }
                 vec_temp[0] = 0; // Primeira posição recebe 0 por padrão;
                 vec_temp[i] = 0; // como se fosse [01230] começa e termina
                 vec_temp[i+1] = -2; // critério de parada;
                 //Aloca o vetor de vetores da matriz de combinações
                 g->permutations[g->number_of_permutations] = (int *)malloc((n*3)*sizeof(int));
                 for(j = 0; j <= i+1; j++){ // A martiz de combinações recebe todos os numeros gerados na recursão
                    g->permutations[g->number_of_permutations][j] = vec_temp[j];
                    printf(" %d ", g->permutations[g->number_of_permutations][j]);	//PRINTANDO PERMUTAÇõES
                 }
                 free(vec_temp); // Desaloca o vetor temporario;
                 g->number_of_permutations++; // incrementa o contador 'universal'
                 printf("\n");
            }
        }
}

//GERA AS CIDADES IDS E DEMANDAS 2
void generateCities(Generator* g, CityStack* cs){
    City c;
    unsigned int i;
    unsigned int req;
    initCityStack(cs); // inicializa a pilha de cidades
    g->cities = (City*)malloc(g->number_of_cities*sizeof(City)); // aloca o vetor de cidades
    for(i=0;i<g->number_of_cities;i++){
	scanf("%u",&req);
        while(!initCity(g,&c,req)){ // inicializa a cidade
		printf("Valor Inválido!\nMotivo: Superior a capacidade dos caminhões\n");
		scanf("%u",&req);
	}
        push(cs, c); // insere a cidade na pilha
    }
    for(i=0;i<g->number_of_cities;i++){
        g->cities[i] = cs->cities[i]; // atiribui o vetor cidade ao vetor no tad generator
        printf("\nDemanda: %d ID: %d meet: %d\n" ,g->cities[i].requirements, g->cities[i].id, g->cities[i].met_requirements );
    }

}

//CRIA A MATRIZ DE DISTÂNCIAS 3
void generateDistances(Generator* g){
    unsigned int i, j;
    //gerar Distancias espelhadas
    for(i=0;i<=g->number_of_cities;i++){
        for(j=0;j<=g->number_of_cities;j++){
            if(i<j)
                g->distances[i][j] = rand()%100+1;
            else if(i>j)
                g->distances[i][j] = g->distances[j][i]; // ESPELHA A MATRIZ
            else // if (i==j)
                g->distances[i][j] = 0;
        }
    }
    for(i=0;i<=g->number_of_cities;i++){
        for(j=0;j<=g->number_of_cities;j++){
               printf(" %d " , g->distances[i][j]);
        }
        printf("\n");
    }

}

//GERA QUANTIDADE DE CAMINHÃO
int generateTrucks(Generator* g, CityStack* cs){
	if(cs->end_requirements % g->truck_capacity == 0){
		g->number_of_trucks = cs->end_requirements/g->truck_capacity;
		return 1;
	}
	return 0;
}

//GERAND COMBINAÇÕES COM ZEROS 9
void generateCombinations(Generator* g, unsigned int last){
    //Last é usado para dize quantos caminhões serão. Ex, last = 2 quer dizer que deve haver 3 zeros 010230
    // Mas last depende de um vetor nessa primeira implmentação, ou seja executa a sua primeira versão com 1
    // e guarda a matriz, executa a sua segunda função com 2 caminhões e gusrda a sua matriz
    // executa a sua função com a matriz guardada na ultima excução e gera uma nova matriz.
    unsigned int i,j,l,a,  aux, cont=0, position=0;

    if(last == 0){
	    g->number_of_combinations = g->number_of_permutations;
    }else if(last == 1){//Primeira Execução
	    g->number_of_combinations = 0;
		for(l=0;l<g->number_of_permutations;l++){
		    for(i=0;i<=g->number_of_cities+3;i++){
			if(i<2){
				g->vector_Aux[i] = g->permutations[l][i]; //copia o vetor passado normalmente no vertor auxilixar
			}
			 else if(i==2){
				 g->vector_Aux[i] = 0; // insere 0 na terceira posição
			 }
			 else{
				g->vector_Aux[i] = g->permutations[l][i-1]; // na quarta posição em diante recebe o vetor passado -1, pois ele está descolado
			 }
			// uma função a menos que o auxiliar por ter recebido o zero
		     }
		    for(i=1;i<=g->number_of_cities-1;i++){
			if(g->vector_Aux[i+1]!=0){
				// verifica se o proximo numero não é zero, ou seja não está no final.
				// caso não ele inverte os números de posição andado com o zero.
				aux = g->vector_Aux[i];
				g->vector_Aux[i] = g->vector_Aux[i+1];
				g->vector_Aux[i+1] = aux;
			}
			for(j=0;j<=g->number_of_cities+(last+4);j++){
				g->combinations[g->number_of_combinations][j] = g->vector_Aux[j]; // atribui a linha do vetor na matriz combinação
			}
			g->number_of_combinations++; // incrementa o contador 'universal' do array
		    }
		}
		for(i=0;i<g->number_of_combinations;i++){
			if(i >= g->number_of_permutations){
			    g->permutations[i] = (int *)malloc((g->number_of_cities*3)*sizeof(int)); // Se acavar o vetor tal que devemos alocar mais memoria
			}
			for(j=0;j<g->number_of_cities+last+3;j++){// Las
				g->permutations[i][j] = g->combinations[i][j]; // atribui as combinações zeradas com os zeros na matriz permutação
				printf(" comb:%d perm:%d ", g->combinations[i][j], g->permutations[i][j]);
			}
			printf("\n");
		}
	}else{
            // IDEIA: Sabe-se que se há 2 caminhões deve haver 2 zeros antes do ultimo zero ex:
            // 012034560 ... tal que se usar o numero de caminhões para contar esses zeros sabemos onde mover
            // logo enquanto não encontrar o segundo zero a matriz se repete... 012...
            // Quando a enconta deve repetir o zero e a próxima posição. 01203, ou seja
            // na posição+2 depois do segundo zero encontrado deve ser inserido mais um zero
            // 012030 ... e apartir dai recebe normalmente o vetor uma posição descolada;
            // 0120304560.
            // Segunda ida, MOVER O ZERO INSERIDO// Deve-se guardar a posição do ultimo zero, para que
            // a movimentação do vetor so ocorra após o zero ser inserido
            // Ex. 0120304560 -> 0120340560 -> 0120345060 e break quando encontrar um zero final ou -2;
	    a = g->number_of_combinations;
	    g->number_of_combinations = 0;
         for(l=0;l<a;l++){
                cont=0; position=0;
                for(i=0;i<=g->number_of_cities+last+2;i++){
                    if(g->permutations[l][i] == 0){
                        cont++;
                        if(cont == last){
                            position=i+2;
                            cont++;
                        }

                    }
                    if((i<position || last >= cont))
                        g->vector_Aux[i] = g->permutations[l][i]; //copia o vetor passado normalmente no vertor auxilixar
                     else if(i==position && 1!=0)
                         g->vector_Aux[i] = 0; // insere 0 na terceira posição
                     else
                        g->vector_Aux[i] = g->permutations[l][i-1]; // na quarta posição em diante recebe o vetor passado -1, pois ele está descolado
                        // uma função a menos que o auxiliar por ter recebido o zero
                 }
                for(i=position;i<g->number_of_cities+last+3;i++){
                    	if(g->vector_Aux[i+1]==0){
                        	break;
		    	}
                	for(j=0;j<=g->number_of_cities+(last+4);j++){
                            g->combinations[g->number_of_combinations][j] = g->vector_Aux[j];
                        }
                            aux = g->vector_Aux[i];
                            g->vector_Aux[i] = g->vector_Aux[i+1];
                            g->vector_Aux[i+1] = aux;
                        g->number_of_combinations++;
                }
            }
        for(i=0;i<g->number_of_combinations;i++){
                if(i >= g->number_of_permutations){
                    g->permutations[i] = (int *)malloc((g->number_of_cities*3)*sizeof(int));
		}
            for(j=0;j<g->number_of_cities+last+3;j++){//Last é o numero de casas a mais do vetor, com o acrécimo de zeros.
                //Sabe-se que o vetor possui normlamente além da combinação mais 3 casas, tal que 123 ->
                // 01230-2 para dizer o final do vetor, logo a cada last, ou seja há uma insersão a mais de casa no vetor,
                 //logo 01230-2 -> 010230-2
                 g->permutations[i][j] = g->combinations[i][j];
                 // Transfere as combinações para a matriz de permutações
                 // Sobre escrevendo as antigas permutações de 01234560 para 010234560
                 printf(" %d ", g->permutations[i][j]);
            }
            printf("\n");
        }
    }
}

//FATORIAL USADO PARA CALCULAR O TAMANHO DA ALOCAÇÃO DE DEVE SER FEITA NA MATRIZ DE COMBINAÇÕES
unsigned long int Fatorial(unsigned long int n){
    if(n==0 || n==1)
    return 1;
    return n*Fatorial(n-1);
}

//GERA AS ROTAS E VERIFICA AS DEMANDAS
int generateRoute(Generator* g, int **bestRoute){
	int current_city = g->permutations[0][0];
	unsigned int last_city, acumulator = 0, *comparator, comp_top = 0, *melhor, i, j, capacity = 0, last_comp_keep, last_best_keep;
	melhor = (unsigned int*)malloc(g->number_of_combinations*sizeof(unsigned int));
	comparator = (unsigned int*)malloc(g->number_of_combinations*sizeof(unsigned int));
	//PEGANDO AS DEMANDAS DAS CIDADES
    	g->vector_Aux[0]=0;
	for(i=1;i<=g->number_of_cities+g->number_of_trucks+1;i++){
		g->vector_Aux[i] = g->cities[i-1].requirements;
	}
        for (i = 1; i < g->number_of_cities+g->number_of_trucks+2; i++){//Numeros de caminhões diz quantos zeros há a mais na linha da matriz, além dos 2 por padõres
            last_city = current_city;
            if(current_city != -2 && g->permutations[0][i] != -2){
                current_city = g->permutations[0][i];
                acumulator += g->distances[last_city][current_city];
            }
        }
	melhor[0] = 0;
        comparator[0] = acumulator;	//Atribui a comparator um valor inicial como se a primeira permutação fosse a melhor
	comp_top = 1;
        for (i = 1; i < g->number_of_combinations; i++) {
            acumulator = 0;
            for (j = 1; j < g->number_of_cities+g->number_of_trucks+1; j++) {
                last_city = g->permutations[i][j-1];
                if(current_city != -2 && g->permutations[i][j] != -2){
                    current_city = g->permutations[i][j];
                    acumulator += g->distances[last_city][current_city];
                }
            }
            if(acumulator < comparator[comp_top - 1]){
                comparator[comp_top] = acumulator;	//Insere ao final(como o melhor)
		melhor[comp_top++] = i;
            }
	    else if(comp_top > 1 && acumulator < comparator[comp_top - 2]){
		last_comp_keep = comparator[comp_top - 1];	//Insere na penultima posição(como segundo melhor)
	    	comparator[comp_top - 1] = acumulator;
		last_best_keep = melhor[comp_top - 1];
		melhor[comp_top - 1] = i;
		comparator[comp_top] = last_comp_keep;
		melhor[comp_top++] = last_best_keep;
	    }
        }
        for(i=0;i<g->number_of_cities+g->number_of_trucks+1;i++){
            if(g->permutations[melhor[comp_top - 1]][i]==0){ // Verifica se não há zero, tal que 0120 é uma capacidade e 03450 é outra
                capacity = 0;
	    }
            capacity += g->vector_Aux[g->permutations[melhor[comp_top - 1]][i]];
            printf(" %d ", capacity); // conta a capacidade até encontrar um zero
            if(capacity > g->truck_capacity && comp_top > 1){
		    comp_top--;	//recomeça porem testando a combinação um pouco pior em nivel de distancia
		    i = 0;
		    capacity = 0;
            }
	    else if (capacity > g->truck_capacity){
	    	return 0;	//exigira um numero de caminhoes inferior
	    }
        }
    	*bestRoute =  g->permutations[melhor[comp_top - 1]];
	return 1;
}
