/******************************************************************************
 * Sistema de Cadastro e Consulta de Relatos sobre Catástrofes Naturais (Versão C)
 * Disciplina: Estrutura de Dados e Algoritmos (Adaptado)
 * Curso: Ciência da Computação - FIAP
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h> // Para isspace

// --- Constantes ---
#define PI 3.14159265358979323846
#define EARTH_RADIUS_KM 6371.0
#define MAX_STRING_LENGTH 256
#define INITIAL_CAPACITY 10
#define FILENAME "relatos_catastrofes_c.txt"
#define RAIO_MAXIMO_KM 10.0

// --- Estruturas de Dados ---
typedef struct {
    double latitude;
    double longitude;
} Coordenadas;

typedef struct {
    // Identificação do Relator
    char nomeRelator[MAX_STRING_LENGTH];
    char telefoneRelator[MAX_STRING_LENGTH];
    char emailRelator[MAX_STRING_LENGTH];

    // Detalhes da Ocorrência
    char tipoCatastrofe[MAX_STRING_LENGTH];
    char descricao[MAX_STRING_LENGTH];
    char dataOcorrencia[11]; // YYYY-MM-DD
    Coordenadas localizacao;

    // Campo auxiliar para ordenação na consulta
    double distanciaDoPontoReferencia;
} Relato;

// Estrutura para gerenciar o array dinâmico de relatos
typedef struct {
    Relato *relatos;
    int tamanho;     // Número atual de relatos
    int capacidade; // Capacidade alocada do array
} BancoDeRelatos;

// --- Protótipos das Funções ---
void inicializarBanco(BancoDeRelatos *banco);
void liberarBanco(BancoDeRelatos *banco);
int adicionarRelato(BancoDeRelatos *banco, Relato novoRelato);
void exibirMenu();
void cadastrarRelato(BancoDeRelatos *banco);
void listarRelatos(const BancoDeRelatos *banco, const Relato *relatos_a_listar, int count); // Modificado para aceitar subconjuntos
void consultarPorRaio(BancoDeRelatos *banco);
int compararRelatosPorDistancia(const void *a, const void *b);
void salvarRelatos(const BancoDeRelatos *banco, const char *nomeArquivo);
void carregarRelatos(BancoDeRelatos *banco, const char *nomeArquivo);
double calcularDistanciaHaversine(Coordenadas p1, Coordenadas p2);
void lerStringSegura(const char *prompt, char *buffer, int buffer_size);
Coordenadas lerCoordenadas(const char *prompt);
void limparBufferEntrada();

// --- Função Principal (main) ---
int main() {
    BancoDeRelatos banco;
    inicializarBanco(&banco);

    // Carregar dados existentes ao iniciar
    carregarRelatos(&banco, FILENAME);

    int opcao;
    do {
        exibirMenu();
        printf("Escolha uma opção: ");
        if (scanf("%d", &opcao) != 1) {
            printf("Entrada inválida. Por favor, insira um número.\n");
            limparBufferEntrada();
            opcao = -1; // Força a repetição do loop
        } else {
            limparBufferEntrada(); // Limpar o newline deixado pelo scanf
        }

        switch (opcao) {
            case 1:
                cadastrarRelato(&banco);
                break;
            case 2:
                printf("\n--- Lista de Todos os Relatos ---");
                listarRelatos(&banco, banco.relatos, banco.tamanho);
                break;
            case 3:
                consultarPorRaio(&banco);
                break;
            case 4:
                printf("Salvando relatos...\n");
                salvarRelatos(&banco, FILENAME);
                break;
            case 0:
                printf("Salvando relatos antes de sair...\n");
                salvarRelatos(&banco, FILENAME);
                printf("Saindo do sistema.\n");
                break;
            default:
                if (opcao != -1) { // Não mostra erro se a entrada já foi inválida
                    printf("Opção inválida. Tente novamente.\n");
                }
        }
        if (opcao != 0) {
             printf("\nPressione Enter para continuar...");
             getchar(); // Pausa até Enter ser pressionado
        }

    } while (opcao != 0);

    liberarBanco(&banco);
    return 0;
}

// --- Implementação das Funções Auxiliares ---

// Limpa o buffer de entrada do teclado
void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Lê uma string do usuário de forma segura
void lerStringSegura(const char *prompt, char *buffer, int buffer_size) {
    printf("%s", prompt);
    if (fgets(buffer, buffer_size, stdin)) {
        // Remove o newline no final, se existir
        buffer[strcspn(buffer, "\n")] = 0;
    } else {
        // Limpa o buffer em caso de erro ou EOF
        buffer[0] = '\0';
        limparBufferEntrada();
    }
    // Garante que não está vazia (exceto se o usuário só der Enter)
    if (strlen(buffer) == 0 || (strlen(buffer) == 1 && isspace(buffer[0]))) {
         printf("Entrada não pode ser vazia. Tente novamente.\n");
         lerStringSegura(prompt, buffer, buffer_size); // Tenta novamente
    }
}

// Lê coordenadas com validação básica
Coordenadas lerCoordenadas(const char *prompt) {
    Coordenadas coords;
    printf("%s\n", prompt);
    int valido = 0;
    while (!valido) {
        printf("  Latitude (-90 a 90): ");
        if (scanf("%lf", &coords.latitude) != 1 || coords.latitude < -90.0 || coords.latitude > 90.0) {
            printf("Latitude inválida. Tente novamente.\n");
            limparBufferEntrada();
        } else {
            limparBufferEntrada();
            valido = 1;
        }
    }
    valido = 0;
    while (!valido) {
        printf("  Longitude (-180 a 180): ");
        if (scanf("%lf", &coords.longitude) != 1 || coords.longitude < -180.0 || coords.longitude > 180.0) {
            printf("Longitude inválida. Tente novamente.\n");
            limparBufferEntrada();
        } else {
            limparBufferEntrada();
            valido = 1;
        }
    }
    return coords;
}

// Calcula a distância Haversine
double calcularDistanciaHaversine(Coordenadas p1, Coordenadas p2) {
    double lat1Rad = p1.latitude * PI / 180.0;
    double lon1Rad = p1.longitude * PI / 180.0;
    double lat2Rad = p2.latitude * PI / 180.0;
    double lon2Rad = p2.longitude * PI / 180.0;

    double dLat = lat2Rad - lat1Rad;
    double dLon = lon2Rad - lon1Rad;

    double a = sin(dLat / 2.0) * sin(dLat / 2.0) +
               cos(lat1Rad) * cos(lat2Rad) *
               sin(dLon / 2.0) * sin(dLon / 2.0);
    double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));

    return EARTH_RADIUS_KM * c;
}

// Função de comparação para qsort
int compararRelatosPorDistancia(const void *a, const void *b) {
    Relato *relatoA = (Relato *)a;
    Relato *relatoB = (Relato *)b;

    // Trata casos onde a distância pode não ter sido calculada (-1)
    if (relatoA->distanciaDoPontoReferencia < 0 && relatoB->distanciaDoPontoReferencia < 0) return 0;
    if (relatoA->distanciaDoPontoReferencia < 0) return 1; // Relatos sem distância vão para o final
    if (relatoB->distanciaDoPontoReferencia < 0) return -1;

    if (relatoA->distanciaDoPontoReferencia < relatoB->distanciaDoPontoReferencia) return -1;
    if (relatoA->distanciaDoPontoReferencia > relatoB->distanciaDoPontoReferencia) return 1;
    return 0;
}

// --- Funções de Gerenciamento do Banco de Dados ---

// Inicializa a estrutura do banco de relatos
void inicializarBanco(BancoDeRelatos *banco) {
    banco->relatos = (Relato *)malloc(INITIAL_CAPACITY * sizeof(Relato));
    if (banco->relatos == NULL) {
        perror("Erro ao alocar memória inicial para relatos");
        exit(EXIT_FAILURE);
    }
    banco->tamanho = 0;
    banco->capacidade = INITIAL_CAPACITY;
}

// Libera a memória alocada para o banco de relatos
void liberarBanco(BancoDeRelatos *banco) {
    free(banco->relatos);
    banco->relatos = NULL;
    banco->tamanho = 0;
    banco->capacidade = 0;
}

// Adiciona um relato ao banco, redimensionando se necessário
int adicionarRelato(BancoDeRelatos *banco, Relato novoRelato) {
    // Verifica se precisa redimensionar
    if (banco->tamanho >= banco->capacidade) {
        int novaCapacidade = banco->capacidade * 2;
        Relato *temp = (Relato *)realloc(banco->relatos, novaCapacidade * sizeof(Relato));
        if (temp == NULL) {
            perror("Erro ao redimensionar memória para relatos");
            return 0; // Falha ao adicionar
        }
        banco->relatos = temp;
        banco->capacidade = novaCapacidade;
    }

    // Adiciona o novo relato
    banco->relatos[banco->tamanho] = novoRelato;
    banco->tamanho++;
    return 1; // Sucesso
}

// --- Funções Principais do Sistema ---

// Exibe o menu principal
void exibirMenu() {
    printf("\n--- Sistema de Relatos de Catástrofes (Versão C) ---\n");
    printf("1. Cadastrar Novo Relato\n");
    printf("2. Listar Todos os Relatos\n");
    printf("3. Consultar Relatos por Raio (%.1fkm)\n", RAIO_MAXIMO_KM);
    printf("4. Salvar Relatos Manualmente\n");
    printf("0. Sair\n");
    printf("----------------------------------------------------\n");
}

// Cadastra um novo relato
void cadastrarRelato(BancoDeRelatos *banco) {
    Relato novoRelato;
    printf("\n--- Cadastro de Novo Relato ---\n");

    lerStringSegura("Nome do Relator: ", novoRelato.nomeRelator, MAX_STRING_LENGTH);
    lerStringSegura("Telefone do Relator: ", novoRelato.telefoneRelator, MAX_STRING_LENGTH);
    lerStringSegura("Email do Relator: ", novoRelato.emailRelator, MAX_STRING_LENGTH);
    lerStringSegura("Tipo da Catástrofe (ex: Enchente): ", novoRelato.tipoCatastrofe, MAX_STRING_LENGTH);
    lerStringSegura("Descrição Detalhada: ", novoRelato.descricao, MAX_STRING_LENGTH);
    lerStringSegura("Data da Ocorrência (YYYY-MM-DD): ", novoRelato.dataOcorrencia, 11); // Tamanho fixo
    // TODO: Adicionar validação de formato de data
    novoRelato.localizacao = lerCoordenadas("Localização da Ocorrência:");
    novoRelato.distanciaDoPontoReferencia = -1.0; // Valor padrão

    if (adicionarRelato(banco, novoRelato)) {
        printf("\nRelato cadastrado com sucesso!\n");
    } else {
        printf("\nErro ao cadastrar relato (falha de memória).\n");
    }
}

// Lista relatos (pode ser todos ou um subconjunto)
void listarRelatos(const BancoDeRelatos *banco, const Relato *relatos_a_listar, int count) {
    if (count == 0) {
        printf("\nNenhum relato para listar.\n");
        return;
    }

    for (int i = 0; i < count; ++i) {
        const Relato *r = &relatos_a_listar[i];
        printf("\n--- Relato %d ---", i + 1);
        printf("\nRelator: %s (Tel: %s, Email: %s)", r->nomeRelator, r->telefoneRelator, r->emailRelator);
        printf("\nCatástrofe: %s em %s", r->tipoCatastrofe, r->dataOcorrencia);
        printf("\nDescrição: %s", r->descricao);
        printf("\nLocalização: Lat %.6f, Lon %.6f", r->localizacao.latitude, r->localizacao.longitude);
        if (r->distanciaDoPontoReferencia >= 0) { // Exibe distância se calculada
             printf(" (Distância: %.2f km)", r->distanciaDoPontoReferencia);
        }
        printf("\n");
    }
    printf("----------------------------------\n");
}

// Consulta relatos dentro de um raio especificado
void consultarPorRaio(BancoDeRelatos *banco) {
    printf("\n--- Consulta por Raio (até %.1f km) ---", RAIO_MAXIMO_KM);
    if (banco->tamanho == 0) {
        printf("\nNenhum relato cadastrado para consultar.\n");
        return;
    }

    Coordenadas pontoReferencia = lerCoordenadas("Digite as coordenadas do ponto de referência:");

    // Cria um array temporário para os resultados
    Relato *relatosNoRaio = (Relato *)malloc(banco->tamanho * sizeof(Relato));
    if (relatosNoRaio == NULL) {
        perror("Erro ao alocar memória para consulta");
        return;
    }
    int countNoRaio = 0;

    // Calcula distância e filtra
    for (int i = 0; i < banco->tamanho; ++i) {
        // Calcula a distância e armazena temporariamente no relato original
        // CUIDADO: Isso modifica o banco original, mas a distância é resetada ao carregar/cadastrar.
        // Uma abordagem melhor seria armazenar a distância apenas no array temporário.
        // Vamos ajustar para armazenar no array temporário:
        double dist = calcularDistanciaHaversine(pontoReferencia, banco->relatos[i].localizacao);
        if (dist <= RAIO_MAXIMO_KM) {
            relatosNoRaio[countNoRaio] = banco->relatos[i]; // Copia o relato
            relatosNoRaio[countNoRaio].distanciaDoPontoReferencia = dist; // Armazena a distância calculada
            countNoRaio++;
        }
    }

    if (countNoRaio == 0) {
        printf("\nNenhum relato encontrado dentro de %.1f km deste ponto.\n", RAIO_MAXIMO_KM);
    } else {
        printf("\n--- Relatos encontrados (%d) (ordenados por distância) ---", countNoRaio);
        // Ordenar os resultados pela distância calculada usando qsort
        qsort(relatosNoRaio, countNoRaio, sizeof(Relato), compararRelatosPorDistancia);
        // Listar os resultados ordenados
        listarRelatos(banco, relatosNoRaio, countNoRaio); // Passa o array temporário
    }

    // Libera a memória do array temporário
    free(relatosNoRaio);
}

// Salva os relatos em um arquivo texto
void salvarRelatos(const BancoDeRelatos *banco, const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "w"); // Abre em modo de escrita (sobrescreve)
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo para escrita");
        fprintf(stderr, "Arquivo: %s\n", nomeArquivo);
        return;
    }

    // Salva o número de relatos na primeira linha
    fprintf(arquivo, "%d\n", banco->tamanho);

    // Salva cada relato, um campo por linha
    for (int i = 0; i < banco->tamanho; ++i) {
        const Relato *r = &banco->relatos[i];
        fprintf(arquivo, "%s\n", r->nomeRelator);
        fprintf(arquivo, "%s\n", r->telefoneRelator);
        fprintf(arquivo, "%s\n", r->emailRelator);
        fprintf(arquivo, "%s\n", r->tipoCatastrofe);
        fprintf(arquivo, "%s\n", r->descricao);
        fprintf(arquivo, "%s\n", r->dataOcorrencia);
        fprintf(arquivo, "%.15f\n", r->localizacao.latitude); // Maior precisão para leitura
        fprintf(arquivo, "%.15f\n", r->localizacao.longitude);
    }

    fclose(arquivo);
    printf("Relatos salvos com sucesso em %s\n", nomeArquivo);
}

// Lê uma linha inteira do arquivo, tratando possíveis problemas
int lerLinhaArquivo(FILE *arquivo, char *buffer, int buffer_size) {
    if (fgets(buffer, buffer_size, arquivo) == NULL) {
        return 0; // Erro ou fim de arquivo
    }
    // Remove o newline no final, se existir
    buffer[strcspn(buffer, "\n")] = 0;
    return 1;
}

// Carrega os relatos de um arquivo texto
void carregarRelatos(BancoDeRelatos *banco, const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        // Não é um erro fatal, apenas informa que o arquivo não existe
        printf("Arquivo %s não encontrado. Iniciando com lista vazia.\n", nomeArquivo);
        return;
    }

    int numeroDeRelatos;
    if (fscanf(arquivo, "%d", &numeroDeRelatos) != 1) {
        fprintf(stderr, "Erro ao ler o número de relatos do arquivo %s. Arquivo pode estar corrompido.\n", nomeArquivo);
        fclose(arquivo);
        return;
    }
    // Consumir o newline após o número
    fgetc(arquivo);

    // Limpa o banco atual antes de carregar (se já houver algo)
    // liberarBanco(banco); // Cuidado: isso desaloca a memória
    // inicializarBanco(banco); // Realoca - melhor só resetar o tamanho
    banco->tamanho = 0;

    char buffer[MAX_STRING_LENGTH];
    for (int i = 0; i < numeroDeRelatos; ++i) {
        Relato r;
        r.distanciaDoPontoReferencia = -1.0; // Reseta distância

        if (!lerLinhaArquivo(arquivo, r.nomeRelator, MAX_STRING_LENGTH) ||
            !lerLinhaArquivo(arquivo, r.telefoneRelator, MAX_STRING_LENGTH) ||
            !lerLinhaArquivo(arquivo, r.emailRelator, MAX_STRING_LENGTH) ||
            !lerLinhaArquivo(arquivo, r.tipoCatastrofe, MAX_STRING_LENGTH) ||
            !lerLinhaArquivo(arquivo, r.descricao, MAX_STRING_LENGTH) ||
            !lerLinhaArquivo(arquivo, r.dataOcorrencia, 11) || // Data
            !lerLinhaArquivo(arquivo, buffer, MAX_STRING_LENGTH) || // Latitude como string
            sscanf(buffer, "%lf", &r.localizacao.latitude) != 1 ||
            !lerLinhaArquivo(arquivo, buffer, MAX_STRING_LENGTH) || // Longitude como string
            sscanf(buffer, "%lf", &r.localizacao.longitude) != 1)
        {
            fprintf(stderr, "Erro ao ler dados do relato %d no arquivo %s. Arquivo pode estar incompleto ou corrompido.\n", (i + 1), nomeArquivo);
            // Decide se quer parar ou continuar tentando ler os próximos
            break; // Para a leitura se encontrar um erro
        }

        if (!adicionarRelato(banco, r)) {
             fprintf(stderr, "Erro de memória ao carregar relato %d.\n", (i + 1));
             break; // Para se não conseguir adicionar
        }
    }

    fclose(arquivo);
    printf("%d relatos carregados de %s\n", banco->tamanho, nomeArquivo);
}

