// Sistema de Cadastro e Consulta de Relatos sobre Catástrofes Naturais
// Disciplina: Estrutura de Dados e Algoritmos (Adaptado de Técnicas Avançadas de Programação)
// Curso: Ciência da Computação - FIAP

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <limits>
#include <algorithm> // Para std::sort
#include <iomanip> // Para std::setprecision

// Constante para conversão de graus para radianos
const double PI = 3.14159265358979323846;
const double EARTH_RADIUS_KM = 6371.0;

// --- Definição da Estrutura de Dados ---
struct Coordenadas {
    double latitude;
    double longitude;
};

struct Relato {
    // Identificação do Relator
    std::string nomeRelator;
    std::string telefoneRelator;
    std::string emailRelator;

    // Detalhes da Ocorrência
    std::string tipoCatastrofe;
    std::string descricao;
    std::string dataOcorrencia; // Formato YYYY-MM-DD para facilitar ordenação
    Coordenadas localizacao;
    // Adicionado para cálculo de distância na consulta
    double distanciaDoPontoReferencia; 
};

// --- Protótipos das Funções ---
void exibirMenu();
void cadastrarRelato(std::vector<Relato>& relatos);
void listarRelatos(const std::vector<Relato>& relatos);
void consultarPorRaio(std::vector<Relato>& relatos);
void ordenarRelatosPorDistancia(std::vector<Relato>& relatos); // Ordena pelo campo 'distanciaDoPontoReferencia'
void salvarRelatos(const std::vector<Relato>& relatos, const std::string& nomeArquivo);
void carregarRelatos(std::vector<Relato>& relatos, const std::string& nomeArquivo);
double calcularDistanciaHaversine(Coordenadas p1, Coordenadas p2);
Coordenadas lerCoordenadas(const std::string& prompt);
std::string lerStringNaoVazia(const std::string& prompt);

// --- Função Principal (main) ---
int main() {
    std::vector<Relato> bancoDeRelatos;
    std::string nomeArquivo = "relatos_catastrofes.txt";

    // Carregar dados existentes ao iniciar
    carregarRelatos(bancoDeRelatos, nomeArquivo);

    int opcao;
    do {
        exibirMenu();
        std::cout << "Escolha uma opção: ";
        while (!(std::cin >> opcao)) {
            std::cout << "Entrada inválida. Por favor, insira um número: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Limpar buffer

        switch (opcao) {
            case 1:
                cadastrarRelato(bancoDeRelatos);
                break;
            case 2:
                listarRelatos(bancoDeRelatos);
                break;
            case 3:
                consultarPorRaio(bancoDeRelatos);
                break;
            case 4:
                std::cout << "Salvando relatos...\n";
                salvarRelatos(bancoDeRelatos, nomeArquivo);
                break;
            case 0:
                std::cout << "Salvando relatos antes de sair...\n";
                salvarRelatos(bancoDeRelatos, nomeArquivo);
                std::cout << "Saindo do sistema.\n";
                break;
            default:
                std::cout << "Opção inválida. Tente novamente.\n";
        }
        std::cout << "\nPressione Enter para continuar...";
        std::cin.get(); // Pausa

    } while (opcao != 0);

    return 0;
}

// --- Implementação das Funções ---

// Função auxiliar para ler string não vazia
std::string lerStringNaoVazia(const std::string& prompt) {
    std::string input;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, input);
        if (!input.empty()) {
            return input;
        }
        std::cout << "Entrada não pode ser vazia. Tente novamente.\n";
    }
}

// Função auxiliar para ler coordenadas com validação básica
Coordenadas lerCoordenadas(const std::string& prompt) {
    Coordenadas coords;
    std::cout << prompt << std::endl;
    while (true) {
        std::cout << "  Latitude (-90 a 90): ";
        if (!(std::cin >> coords.latitude) || coords.latitude < -90.0 || coords.latitude > 90.0) {
            std::cout << "Latitude inválida. Tente novamente.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Limpar buffer
            break;
        }
    }
    while (true) {
        std::cout << "  Longitude (-180 a 180): ";
        if (!(std::cin >> coords.longitude) || coords.longitude < -180.0 || coords.longitude > 180.0) {
            std::cout << "Longitude inválida. Tente novamente.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Limpar buffer
            break;
        }
    }
    return coords;
}

// Exibe o menu principal
void exibirMenu() {
    std::cout << "\n--- Sistema de Relatos de Catástrofes ---\n";
    std::cout << "1. Cadastrar Novo Relato\n";
    std::cout << "2. Listar Todos os Relatos\n";
    std::cout << "3. Consultar Relatos por Raio (10km)\n";
    std::cout << "4. Salvar Relatos Manualmente\n";
    std::cout << "0. Sair\n";
    std::cout << "-----------------------------------------\n";
}

// Cadastra um novo relato
void cadastrarRelato(std::vector<Relato>& relatos) {
    Relato novoRelato;
    std::cout << "\n--- Cadastro de Novo Relato ---\n";

    novoRelato.nomeRelator = lerStringNaoVazia("Nome do Relator: ");
    novoRelato.telefoneRelator = lerStringNaoVazia("Telefone do Relator: ");
    novoRelato.emailRelator = lerStringNaoVazia("Email do Relator: ");
    novoRelato.tipoCatastrofe = lerStringNaoVazia("Tipo da Catástrofe (ex: Enchente, Deslizamento): ");
    novoRelato.descricao = lerStringNaoVazia("Descrição Detalhada: ");
    novoRelato.dataOcorrencia = lerStringNaoVazia("Data da Ocorrência (YYYY-MM-DD): "); // Validação de formato pode ser adicionada
    novoRelato.localizacao = lerCoordenadas("Localização da Ocorrência:");
    novoRelato.distanciaDoPontoReferencia = -1; // Valor padrão, será calculado na consulta

    relatos.push_back(novoRelato);
    std::cout << "\nRelato cadastrado com sucesso!\n";
}

// Lista todos os relatos cadastrados
void listarRelatos(const std::vector<Relato>& relatos) {
    std::cout << "\n--- Lista de Todos os Relatos ---";
    if (relatos.empty()) {
        std::cout << "\nNenhum relato cadastrado ainda.\n";
        return;
    }

    for (size_t i = 0; i < relatos.size(); ++i) {
        const auto& r = relatos[i];
        std::cout << "\n--- Relato " << (i + 1) << " ---";
        std::cout << "\nRelator: " << r.nomeRelator << " (Tel: " << r.telefoneRelator << ", Email: " << r.emailRelator << ")";
        std::cout << "\nCatástrofe: " << r.tipoCatastrofe << " em " << r.dataOcorrencia;
        std::cout << "\nDescrição: " << r.descricao;
        std::cout << std::fixed << std::setprecision(6);
        std::cout << "\nLocalização: Lat " << r.localizacao.latitude << ", Lon " << r.localizacao.longitude;
        if (r.distanciaDoPontoReferencia >= 0) { // Exibe distância se calculada
             std::cout << std::fixed << std::setprecision(2);
             std::cout << " (Distância: " << r.distanciaDoPontoReferencia << " km)";
        }
        std::cout << std::endl;
    }
    std::cout << "----------------------------------\n";
}

// Calcula a distância entre dois pontos usando a fórmula de Haversine
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

// Consulta relatos dentro de um raio de 10 km
void consultarPorRaio(std::vector<Relato>& relatos) {
    std::cout << "\n--- Consulta por Raio (até 10 km) ---";
    if (relatos.empty()) {
        std::cout << "\nNenhum relato cadastrado para consultar.\n";
        return;
    }

    Coordenadas pontoReferencia = lerCoordenadas("Digite as coordenadas do ponto de referência:");
    double raioMaximo = 10.0;

    std::vector<Relato> relatosNoRaio;
    for (auto& r : relatos) { // Usar referência para modificar a distância
        r.distanciaDoPontoReferencia = calcularDistanciaHaversine(pontoReferencia, r.localizacao);
        if (r.distanciaDoPontoReferencia <= raioMaximo) {
            relatosNoRaio.push_back(r);
        }
    }

    if (relatosNoRaio.empty()) {
        std::cout << "\nNenhum relato encontrado dentro de " << raioMaximo << " km deste ponto.\n";
    } else {
        std::cout << "\n--- Relatos encontrados (ordenados por distância) ---";
        // Ordenar os resultados pela distância calculada
        ordenarRelatosPorDistancia(relatosNoRaio);
        // Listar os resultados ordenados
        listarRelatos(relatosNoRaio); // Reutiliza a função de listagem
    }
}

// Ordena um vetor de relatos pela distância (menor para maior)
void ordenarRelatosPorDistancia(std::vector<Relato>& relatos) {
    std::sort(relatos.begin(), relatos.end(), [](const Relato& a, const Relato& b) {
        // Trata casos onde a distância pode não ter sido calculada (-1)
        if (a.distanciaDoPontoReferencia < 0) return false;
        if (b.distanciaDoPontoReferencia < 0) return true;
        return a.distanciaDoPontoReferencia < b.distanciaDoPontoReferencia;
    });
}


// Salva os relatos em um arquivo texto
void salvarRelatos(const std::vector<Relato>& relatos, const std::string& nomeArquivo) {
    std::ofstream arquivo(nomeArquivo); // Abre em modo de escrita (sobrescreve)
    if (!arquivo.is_open()) {
        std::cerr << "Erro ao abrir o arquivo " << nomeArquivo << " para escrita.\n";
        return;
    }

    // Salva o número de relatos na primeira linha
    arquivo << relatos.size() << std::endl;

    // Salva cada relato em linhas separadas
    for (const auto& r : relatos) {
        arquivo << r.nomeRelator << std::endl;
        arquivo << r.telefoneRelator << std::endl;
        arquivo << r.emailRelator << std::endl;
        arquivo << r.tipoCatastrofe << std::endl;
        arquivo << r.descricao << std::endl;
        arquivo << r.dataOcorrencia << std::endl;
        arquivo << r.localizacao.latitude << std::endl;
        arquivo << r.localizacao.longitude << std::endl;
    }

    arquivo.close();
    std::cout << "Relatos salvos com sucesso em " << nomeArquivo << std::endl;
}

// Carrega os relatos de um arquivo texto
void carregarRelatos(std::vector<Relato>& relatos, const std::string& nomeArquivo) {
    std::ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        std::cerr << "Arquivo " << nomeArquivo << " não encontrado ou não pode ser aberto. Iniciando com lista vazia.\n";
        return;
    }

    int numeroDeRelatos;
    arquivo >> numeroDeRelatos;
    arquivo.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Consumir o newline após o número

    if (arquivo.fail()) {
         std::cerr << "Erro ao ler o número de relatos do arquivo. Arquivo pode estar corrompido.\n";
         arquivo.close();
         return;
    }

    relatos.clear(); // Limpa o vetor atual antes de carregar
    relatos.reserve(numeroDeRelatos); // Opcional: aloca espaço

    for (int i = 0; i < numeroDeRelatos; ++i) {
        Relato r;
        if (!std::getline(arquivo, r.nomeRelator) ||
            !std::getline(arquivo, r.telefoneRelator) ||
            !std::getline(arquivo, r.emailRelator) ||
            !std::getline(arquivo, r.tipoCatastrofe) ||
            !std::getline(arquivo, r.descricao) ||
            !std::getline(arquivo, r.dataOcorrencia) ||
            !(arquivo >> r.localizacao.latitude) ||
            !(arquivo >> r.localizacao.longitude)) {
            std::cerr << "Erro ao ler dados do relato " << (i + 1) << ". Arquivo pode estar incompleto ou corrompido.\n";
            // Decide se quer parar ou continuar tentando ler os próximos
            break; // Para a leitura se encontrar um erro
        }
        arquivo.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Consumir o newline após a longitude
        r.distanciaDoPontoReferencia = -1; // Reseta a distância ao carregar
        relatos.push_back(r);
    }

    arquivo.close();
    std::cout << "Relatos carregados de " << nomeArquivo << std::endl;
}


