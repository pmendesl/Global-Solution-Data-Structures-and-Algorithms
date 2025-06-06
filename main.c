#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

// Constantes
#define PI 3.14159265358979323846
#define EARTH_RADIUS_KM 6371.0
#define RAIO_MAXIMO_KM 10.0
#define TAMANHO_MAXIMO_STRING 100
#define MAX_RELATOS 1000

// Estruturas
typedef struct {
    double latitude;
    double longitude;
} Coordenadas;

typedef struct {
    char nomeRelator[TAMANHO_MAXIMO_STRING];
    char telefoneRelator[TAMANHO_MAXIMO_STRING];
    char emailRelator[TAMANHO_MAXIMO_STRING];
    char tipoCatastrofe[TAMANHO_MAXIMO_STRING];
    char descricao[TAMANHO_MAXIMO_STRING];
    char dataOcorrencia[TAMANHO_MAXIMO_STRING];
    Coordenadas localizacao;
    double distanciaDoPontoReferencia;
} Relato;

// Protótipos das funções
void exibirMenu();
void cadastrarRelato(Relato relatos[], int* numRelatos);
void listarRelatos(Relato relatos[], int numRelatos);
void consultarPorRaio(Relato relatos[], int numRelatos);
void ordenarRelatosPorDistancia(Relato relatos[], int numRelatos);
void salvarRelatos(Relato relatos[], int numRelatos, const char* nomeArquivo);
void carregarRelatos(Relato relatos[], int* numRelatos, const char* nomeArquivo);
double calcularDistanciaHaversine(Coordenadas p1, Coordenadas p2);
Coordenadas lerCoordenadas(const char* prompt);
void lerStringComLimite(char* destino, const char* prompt, int limite);
int validarEmail(const char* email);
int validarTelefone(const char* telefone);
int validarData(const char* data);

// Função principal
int main() {
    Relato bancoDeRelatos[MAX_RELATOS];
    int numRelatos = 0;
    const char* nomeArquivo = "relatos_catastrofes.txt";

    carregarRelatos(bancoDeRelatos, &numRelatos, nomeArquivo);

    int opcao;
    do {
        exibirMenu();
        printf("Escolha uma opção: ");
        if (scanf("%d", &opcao) != 1) {
            printf("Entrada inválida. Por favor, insira um número.\n");
            while (getchar() != '\n'); // Limpar buffer
            continue;
        }
        while (getchar() != '\n'); // Limpar buffer

        switch (opcao) {
            case 1:
                cadastrarRelato(bancoDeRelatos, &numRelatos);
                break;
            case 2:
                listarRelatos(bancoDeRelatos, numRelatos);
                break;
            case 3:
                consultarPorRaio(bancoDeRelatos, numRelatos);
                break;
            case 4:
                printf("Salvando relatos...\n");
                salvarRelatos(bancoDeRelatos, numRelatos, nomeArquivo);
                break;
            case 0:
                printf("Salvando relatos antes de sair...\n");
                salvarRelatos(bancoDeRelatos, numRelatos, nomeArquivo);
                printf("Saindo do sistema.\n");
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
        }
        printf("\nPressione Enter para continuar...");
        getchar();
    } while (opcao != 0);

    return 0;
}

// Funções de validação
int validarEmail(const char* email) {
    int temArroba = 0;
    int temPonto = 0;
    int i;
    
    for (i = 0; email[i] != '\0'; i++) {
        if (email[i] == '@') temArroba = 1;
        if (temArroba && email[i] == '.') temPonto = 1;
    }
    
    return temArroba && temPonto;
}

int validarTelefone(const char* telefone) {
    int i;
    for (i = 0; telefone[i] != '\0'; i++) {
        if (!isdigit(telefone[i])) return 0;
    }
    return (i == 10 || i == 11);
}

int validarData(const char* data) {
    if (strlen(data) != 10) return 0;
    if (data[4] != '-' || data[7] != '-') return 0;
    
    int ano = atoi(data);
    int mes = atoi(data + 5);
    int dia = atoi(data + 8);
    
    if (ano < 1900 || ano > 2100) return 0;
    if (mes < 1 || mes > 12) return 0;
    if (dia < 1 || dia > 31) return 0;
    
    return 1;
}

void lerStringComLimite(char* destino, const char* prompt, int limite) {
    char temp[TAMANHO_MAXIMO_STRING];
    do {
        printf("%s", prompt);
        fgets(temp, TAMANHO_MAXIMO_STRING, stdin);
        temp[strcspn(temp, "\n")] = 0; // Remove o \n
        
        if (strlen(temp) == 0) {
            printf("Entrada não pode ser vazia. Tente novamente.\n");
            continue;
        }
        if (strlen(temp) > limite) {
            printf("Entrada muito longa. Máximo de %d caracteres.\n", limite);
            continue;
        }
        break;
    } while (1);
    
    strcpy(destino, temp);
}

void exibirMenu() {
    printf("\n--- Sistema de Relatos de Catástrofes ---\n");
    printf("1. Cadastrar Novo Relato\n");
    printf("2. Listar Todos os Relatos\n");
    printf("3. Consultar Relatos por Raio (10km)\n");
    printf("4. Salvar Relatos Manualmente\n");
    printf("0. Sair\n");
    printf("-----------------------------------------\n");
}

Coordenadas lerCoordenadas(const char* prompt) {
    Coordenadas coords;
    printf("%s\n", prompt);
    
    do {
        printf("  Latitude (-90 a 90): ");
        if (scanf("%lf", &coords.latitude) != 1 || 
            coords.latitude < -90.0 || coords.latitude > 90.0) {
            printf("Latitude inválida. Tente novamente.\n");
            while (getchar() != '\n');
        } else {
            while (getchar() != '\n');
            break;
        }
    } while (1);
    
    do {
        printf("  Longitude (-180 a 180): ");
        if (scanf("%lf", &coords.longitude) != 1 || 
            coords.longitude < -180.0 || coords.longitude > 180.0) {
            printf("Longitude inválida. Tente novamente.\n");
            while (getchar() != '\n');
        } else {
            while (getchar() != '\n');
            break;
        }
    } while (1);
    
    return coords;
}

void cadastrarRelato(Relato relatos[], int* numRelatos) {
    if (*numRelatos >= MAX_RELATOS) {
        printf("Limite máximo de relatos atingido!\n");
        return;
    }

    Relato novoRelato;
    printf("\n--- Cadastro de Novo Relato ---\n");

    lerStringComLimite(novoRelato.nomeRelator, "Nome do Relator: ", TAMANHO_MAXIMO_STRING);
    
    do {
        lerStringComLimite(novoRelato.telefoneRelator, "Telefone do Relator: ", TAMANHO_MAXIMO_STRING);
        if (!validarTelefone(novoRelato.telefoneRelator)) {
            printf("Telefone inválido. Use apenas números (10 ou 11 dígitos).\n");
        }
    } while (!validarTelefone(novoRelato.telefoneRelator));
    
    do {
        lerStringComLimite(novoRelato.emailRelator, "Email do Relator: ", TAMANHO_MAXIMO_STRING);
        if (!validarEmail(novoRelato.emailRelator)) {
            printf("Email inválido. Use o formato: exemplo@dominio.com\n");
        }
    } while (!validarEmail(novoRelato.emailRelator));
    
    lerStringComLimite(novoRelato.tipoCatastrofe, "Tipo da Catástrofe (ex: Enchente, Deslizamento): ", TAMANHO_MAXIMO_STRING);
    lerStringComLimite(novoRelato.descricao, "Descrição Detalhada: ", TAMANHO_MAXIMO_STRING);
    
    do {
        lerStringComLimite(novoRelato.dataOcorrencia, "Data da Ocorrência (YYYY-MM-DD): ", TAMANHO_MAXIMO_STRING);
        if (!validarData(novoRelato.dataOcorrencia)) {
            printf("Data inválida. Use o formato YYYY-MM-DD.\n");
        }
    } while (!validarData(novoRelato.dataOcorrencia));
    
    novoRelato.localizacao = lerCoordenadas("Localização da Ocorrência:");
    novoRelato.distanciaDoPontoReferencia = -1;

    relatos[*numRelatos] = novoRelato;
    (*numRelatos)++;
    printf("\nRelato cadastrado com sucesso!\n");
}

void listarRelatos(Relato relatos[], int numRelatos) {
    printf("\n--- Lista de Todos os Relatos ---");
    if (numRelatos == 0) {
        printf("\nNenhum relato cadastrado ainda.\n");
        return;
    }

    for (int i = 0; i < numRelatos; i++) {
        const Relato* r = &relatos[i];
        printf("\n--- Relato %d ---", (i + 1));
        printf("\nRelator: %s (Tel: %s, Email: %s)", r->nomeRelator, r->telefoneRelator, r->emailRelator);
        printf("\nCatástrofe: %s em %s", r->tipoCatastrofe, r->dataOcorrencia);
        printf("\nDescrição: %s", r->descricao);
        printf("\nLocalização: Lat %.4f, Lon %.4f", r->localizacao.latitude, r->localizacao.longitude);
        if (r->distanciaDoPontoReferencia >= 0) {
            printf(" (Distância: %.2f km)", r->distanciaDoPontoReferencia);
        }
        printf("\n");
    }
    printf("----------------------------------\n");
}

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

void consultarPorRaio(Relato relatos[], int numRelatos) {
    printf("\n--- Consulta por Raio (até %.1f km) ---", RAIO_MAXIMO_KM);
    if (numRelatos == 0) {
        printf("\nNenhum relato cadastrado para consultar.\n");
        return;
    }

    Coordenadas pontoReferencia = lerCoordenadas("Digite as coordenadas do ponto de referência:");

    Relato relatosNoRaio[MAX_RELATOS];
    int numRelatosNoRaio = 0;

    for (int i = 0; i < numRelatos; i++) {
        relatos[i].distanciaDoPontoReferencia = calcularDistanciaHaversine(pontoReferencia, relatos[i].localizacao);
        if (relatos[i].distanciaDoPontoReferencia <= RAIO_MAXIMO_KM) {
            relatosNoRaio[numRelatosNoRaio] = relatos[i];
            numRelatosNoRaio++;
        }
    }

    if (numRelatosNoRaio == 0) {
        printf("\nNenhum relato encontrado dentro de %.1f km deste ponto.\n", RAIO_MAXIMO_KM);
    } else {
        printf("\n--- Relatos encontrados (ordenados por distância) ---");
        ordenarRelatosPorDistancia(relatosNoRaio, numRelatosNoRaio);
        listarRelatos(relatosNoRaio, numRelatosNoRaio);
    }
}

void ordenarRelatosPorDistancia(Relato relatos[], int numRelatos) {
    for (int i = 0; i < numRelatos - 1; i++) {
        for (int j = 0; j < numRelatos - i - 1; j++) {
            if (relatos[j].distanciaDoPontoReferencia > relatos[j + 1].distanciaDoPontoReferencia) {
                Relato temp = relatos[j];
                relatos[j] = relatos[j + 1];
                relatos[j + 1] = temp;
            }
        }
    }
}

void salvarRelatos(Relato relatos[], int numRelatos, const char* nomeArquivo) {
    // Criar backup
    char backupFile[TAMANHO_MAXIMO_STRING];
    strcpy(backupFile, nomeArquivo);
    strcat(backupFile, ".backup");
    
    FILE* arquivoOriginal = fopen(nomeArquivo, "r");
    if (arquivoOriginal) {
        FILE* arquivoBackup = fopen(backupFile, "w");
        if (arquivoBackup) {
            char c;
            while ((c = fgetc(arquivoOriginal)) != EOF) {
                fputc(c, arquivoBackup);
            }
            fclose(arquivoBackup);
        }
        fclose(arquivoOriginal);
    }

    FILE* arquivo = fopen(nomeArquivo, "w");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo %s para escrita.\n", nomeArquivo);
        return;
    }

    fprintf(arquivo, "%d\n", numRelatos);

    for (int i = 0; i < numRelatos; i++) {
        fprintf(arquivo, "%s\n", relatos[i].nomeRelator);
        fprintf(arquivo, "%s\n", relatos[i].telefoneRelator);
        fprintf(arquivo, "%s\n", relatos[i].emailRelator);
        fprintf(arquivo, "%s\n", relatos[i].tipoCatastrofe);
        fprintf(arquivo, "%s\n", relatos[i].descricao);
        fprintf(arquivo, "%s\n", relatos[i].dataOcorrencia);
        fprintf(arquivo, "%.6f\n", relatos[i].localizacao.latitude);
        fprintf(arquivo, "%.6f\n", relatos[i].localizacao.longitude);
    }

    fclose(arquivo);
    printf("Relatos salvos com sucesso em %s\n", nomeArquivo);
}

void carregarRelatos(Relato relatos[], int* numRelatos, const char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        printf("Arquivo %s não encontrado ou não pode ser aberto. Iniciando com lista vazia.\n", nomeArquivo);
        return;
    }

    if (fscanf(arquivo, "%d", numRelatos) != 1) {
        printf("Erro ao ler o número de relatos do arquivo. Arquivo pode estar corrompido.\n");
        fclose(arquivo);
        return;
    }
    fgetc(arquivo); // Consumir o newline

    for (int i = 0; i < *numRelatos; i++) {
        char linha[TAMANHO_MAXIMO_STRING];
        
        if (!fgets(linha, TAMANHO_MAXIMO_STRING, arquivo)) break;
        linha[strcspn(linha, "\n")] = 0;
        strcpy(relatos[i].nomeRelator, linha);
        
        if (!fgets(linha, TAMANHO_MAXIMO_STRING, arquivo)) break;
        linha[strcspn(linha, "\n")] = 0;
        strcpy(relatos[i].telefoneRelator, linha);
        
        if (!fgets(linha, TAMANHO_MAXIMO_STRING, arquivo)) break;
        linha[strcspn(linha, "\n")] = 0;
        strcpy(relatos[i].emailRelator, linha);
        
        if (!fgets(linha, TAMANHO_MAXIMO_STRING, arquivo)) break;
        linha[strcspn(linha, "\n")] = 0;
        strcpy(relatos[i].tipoCatastrofe, linha);
        
        if (!fgets(linha, TAMANHO_MAXIMO_STRING, arquivo)) break;
        linha[strcspn(linha, "\n")] = 0;
        strcpy(relatos[i].descricao, linha);
        
        if (!fgets(linha, TAMANHO_MAXIMO_STRING, arquivo)) break;
        linha[strcspn(linha, "\n")] = 0;
        strcpy(relatos[i].dataOcorrencia, linha);
        
        if (fscanf(arquivo, "%lf", &relatos[i].localizacao.latitude) != 1) break;
        if (fscanf(arquivo, "%lf", &relatos[i].localizacao.longitude) != 1) break;
        fgetc(arquivo); // Consumir o newline
        
        relatos[i].distanciaDoPontoReferencia = -1;
    }

    fclose(arquivo);
    printf("Relatos carregados de %s\n", nomeArquivo);
}
