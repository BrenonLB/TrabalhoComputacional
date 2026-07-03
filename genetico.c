#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// PARÂMETROS DO ALGORITMO
#define GERACOES 50
#define BITS 16
#define TAXA_CRUZAMENTO 0.85
#define TAXA_MUTACAO 0.02
#define SEED 42 // Seed fixa para permitir reprodução

// Constantes do problema
#define PI 3.14159265358979323846
#define MAX_VAL_16BIT 65535.0 // 2^16 - 1

// Estrutura do Indivíduo
typedef struct {
    int genes[BITS];
    double x;         
    double fitness;   
} Individuo;

double rand_double() {
    return (double)rand() / RAND_MAX;
}

double decodificar(int genes[]) {
    unsigned int dec = 0;
    for(int i = 0; i < BITS; i++) {
        dec = (dec << 1) | genes[i];
    }
    return -1.0 + (3.0 * dec) / MAX_VAL_16BIT;
}

double avaliar_fitness(double x) {
    return x * sin(10.0 * PI * x) + 1.0;
}

void inicializar_populacao(Individuo pop[], int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < BITS; j++) {
            pop[i].genes[j] = rand() % 2; 
        }
    }
}

void calcular_fitness_populacao(Individuo pop[], int N) {
    for (int i = 0; i < N; i++) {
        pop[i].x = decodificar(pop[i].genes);
        pop[i].fitness = avaliar_fitness(pop[i].x);
    }
}

int selecao_roleta(Individuo pop[], int N, double fitness_total, double offset) {
    double r = rand_double() * fitness_total;
    double probabilidade_acumulada = 0.0;
    
    for (int i = 0; i < N; i++) {
        probabilidade_acumulada += (pop[i].fitness + offset);
        if (probabilidade_acumulada >= r) {
            return i;
        }
    }
    return N - 1; 
}

void cruzamento(Individuo p1, Individuo p2, Individuo *f1, Individuo *f2) {
    if (rand_double() < TAXA_CRUZAMENTO) {
        int ponto_corte = rand() % BITS;
        for (int i = 0; i < BITS; i++) {
            if (i < ponto_corte) {
                f1->genes[i] = p1.genes[i];
                f2->genes[i] = p2.genes[i];
            } else {
                f1->genes[i] = p2.genes[i];
                f2->genes[i] = p1.genes[i];
            }
        }
    } else {
        *f1 = p1;
        *f2 = p2;
    }
}

void mutacao(Individuo *ind) {
    for (int i = 0; i < BITS; i++) {
        if (rand_double() < TAXA_MUTACAO) {
            ind->genes[i] ^= 1; 
        }
    }
}

void executar_ag(int N, double historico_fitness[]) {
    srand(SEED); 
    
    Individuo *populacao = (Individuo *)malloc(N * sizeof(Individuo));
    Individuo *nova_populacao = (Individuo *)malloc(N * sizeof(Individuo));
    
    inicializar_populacao(populacao, N);
    
    Individuo melhor_global;
    melhor_global.fitness = -9999.0;

    for (int g = 0; g < GERACOES; g++) {
        calcular_fitness_populacao(populacao, N);
        
        int idx_melhor = 0;
        double min_fitness = populacao[0].fitness; 
        
        for (int i = 1; i < N; i++) {
            if (populacao[i].fitness > populacao[idx_melhor].fitness) {
                idx_melhor = i;
            }
            if (populacao[i].fitness < min_fitness) {
                min_fitness = populacao[i].fitness;
            }
        }
        
        Individuo elite = populacao[idx_melhor];
        if (elite.fitness > melhor_global.fitness) {
            melhor_global = elite;
        }

        historico_fitness[g] = melhor_global.fitness;

        double offset = (min_fitness < 0) ? fabs(min_fitness) + 0.1 : 0.0;
        double fitness_total = 0.0;
        
        for (int i = 0; i < N; i++) {
            fitness_total += (populacao[i].fitness + offset);
        }

        nova_populacao[0] = elite; 
        int count = 1;
        
        while (count < N) {
            int p1 = selecao_roleta(populacao, N, fitness_total, offset);
            int p2 = selecao_roleta(populacao, N, fitness_total, offset);
            
            Individuo f1, f2;
            cruzamento(populacao[p1], populacao[p2], &f1, &f2);
            mutacao(&f1);
            mutacao(&f2);
            
            nova_populacao[count++] = f1;
            if (count < N) {
                nova_populacao[count++] = f2;
            }
        }
        
        for (int i = 0; i < N; i++) {
            populacao[i] = nova_populacao[i];
        }
    }

    printf("População N = %-3d | Melhor x = %8.5f | Sustentação Máxima f(x) = %8.5f\n", 
           N, melhor_global.x, melhor_global.fitness);
           
    free(populacao);
    free(nova_populacao);
}

// Função para exportar os dados para um script Octave/MATLAB
void gerar_script_octave(double hist_10[], double hist_30[], double hist_100[]) {
    FILE *fp = fopen("gerar_grafico.m", "w");
    if (fp == NULL) {
        printf("Erro ao criar o arquivo de gráfico.\n");
        return;
    }

    fprintf(fp, "%% Script gerado automaticamente para o Octave/MATLAB\n");
    fprintf(fp, "geracoes = 1:%d;\n\n", GERACOES);

    // Escreve os vetores de dados
    fprintf(fp, "fit_10 = [");
    for(int i=0; i<GERACOES; i++) fprintf(fp, "%f ", hist_10[i]);
    fprintf(fp, "];\n");

    fprintf(fp, "fit_30 = [");
    for(int i=0; i<GERACOES; i++) fprintf(fp, "%f ", hist_30[i]);
    fprintf(fp, "];\n");

    fprintf(fp, "fit_100 = [");
    for(int i=0; i<GERACOES; i++) fprintf(fp, "%f ", hist_100[i]);
    fprintf(fp, "];\n\n");

    // Comandos de plotagem
    fprintf(fp, "figure;\n");
    fprintf(fp, "plot(geracoes, fit_10, '-o', 'LineWidth', 1.5, 'DisplayName', 'N = 10');\n");
    fprintf(fp, "hold on;\n");
    fprintf(fp, "plot(geracoes, fit_30, '-s', 'LineWidth', 1.5, 'DisplayName', 'N = 30');\n");
    fprintf(fp, "plot(geracoes, fit_100, '-^', 'LineWidth', 1.5, 'DisplayName', 'N = 100');\n");
    
    fprintf(fp, "xlabel('Gerações');\n");
    fprintf(fp, "ylabel('Melhor Fitness f(x)');\n");
    fprintf(fp, "title('Convergência do AG: Aerodesign (Flape)');\n");
    fprintf(fp, "legend('Location', 'southeast');\n");
    fprintf(fp, "grid on;\n");

    fclose(fp);
    printf("\nArquivo 'gerar_grafico.m' criado com sucesso! Abra no Octave e execute.\n");
}

int main() {
    printf("=== AVALIAÇÃO DE DESEMPENHO AERODINÂMICO (ALGORITMO GENÉTICO) ===\n\n");
    
    double historico_10[GERACOES];
    double historico_30[GERACOES];
    double historico_100[GERACOES];
    
    executar_ag(10, historico_10);
    executar_ag(30, historico_30);
    executar_ag(100, historico_100);
    
    // Chama a função que cria o arquivo do Octave
    gerar_script_octave(historico_10, historico_30, historico_100);
    
    return 0;
}