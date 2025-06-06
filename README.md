# Global-Solution-Data-Structures-and-Algorithms
# Sistema de Cadastro e Consulta de Relatos sobre Catástrofes Naturais

## Descrição do Projeto

Este projeto implementa um sistema de linha de comando em C para cadastrar e consultar relatos sobre catástrofes naturais ocorridas em um raio de até 10 km de um ponto de referência. A aplicação permite registrar informações detalhadas sobre o relator e a ocorrência, armazenando os dados de forma persistente em um arquivo de texto (`relatos_catastrofes.txt`).

O sistema foi desenvolvido como parte da disciplina de Estrutura de Dados e Algoritmos (adaptado do enunciado original de Técnicas Avançadas de Programação) do curso de Ciência da Computação da FIAP.

## Funcionalidades Implementadas

*   **Cadastro de Relatos:** Permite inserir informações completas sobre o relator (nome, telefone, email) e a catástrofe (tipo, descrição, data, localização por coordenadas).
*   **Listagem de Relatos:** Exibe todos os relatos cadastrados no sistema.
*   **Consulta por Raio:** Busca e exibe relatos que ocorreram dentro de um raio de 10 km de coordenadas fornecidas pelo usuário. Os resultados são ordenados pela distância ao ponto de referência (do mais próximo ao mais distante).
*   **Persistência de Dados:** Salva automaticamente os relatos em um arquivo `relatos_catastrofes.txt` ao sair do programa e os carrega ao iniciar. Há também uma opção para salvar manualmente.
*   **Validação de Entradas:** Realiza validações básicas para garantir a qualidade dos dados inseridos (campos não vazios, formato numérico e limites para coordenadas).
*   **Cálculo de Distância:** Utiliza a fórmula de Haversine para calcular a distância geodésica entre as coordenadas do relato e o ponto de referência da consulta.

## Estrutura de Dados e Conceitos Aplicados

*   **`struct`:** A entidade `Relato` e `Coordenadas` são modeladas utilizando estruturas (`struct`) para agrupar logicamente os dados.
*   **`std::vector`:** Os relatos são armazenados em memória utilizando um vetor dinâmico (`std::vector<Relato>`), permitindo um número flexível de registros.
*   **Algoritmos de Busca e Ordenação:** A consulta por raio implementa uma busca linear filtrada pela distância. Os resultados da consulta são ordenados usando `std::sort` com uma função lambda baseada na distância calculada.
*   **Persistência em Arquivo:** As funções `salvarRelatos` e `carregarRelatos` manipulam um arquivo `.txt` para guardar e recuperar os dados entre execuções do programa.
*   **Validação e Tratamento de Erros:** O código inclui verificações para entradas inválidas (como letras onde números são esperados) e tratamento básico de erros na leitura/escrita de arquivos.

## Como Compilar e Executar

1.  **Pré-requisitos:** É necessário ter um compilador C++ instalado (como g++).
2.  **Compilação:** Abra um terminal ou prompt de comando, navegue até o diretório onde o arquivo `main.cpp` foi salvo e execute o seguinte comando:
    ```bash
    g++ main.cpp -o sistema_relatos -lm -std=c++11
    ```
    *   `g++`: Nome do compilador (pode variar).
    *   `main.cpp`: Arquivo fonte.
    *   `-o sistema_relatos`: Define o nome do arquivo executável gerado.
    *   `-lm`: (Em alguns sistemas Linux/macOS) Linka a biblioteca matemática necessária para `sqrt`, `sin`, `cos`, `atan2`.
    *   `-std=c++11`: Garante a compilação com o padrão C++11 ou superior (necessário para `std::to_string`, `std::stoi`, lambdas, etc., embora o código atual use principalmente recursos mais antigos, é uma boa prática).

3.  **Execução:** Após a compilação bem-sucedida, execute o programa com:
    ```bash
    ./sistema_relatos
    ```

## Exemplo de Uso

Ao executar o programa, um menu será exibido:

```
--- Sistema de Relatos de Catástrofes ---
1. Cadastrar Novo Relato
2. Listar Todos os Relatos
3. Consultar Relatos por Raio (10km)
4. Salvar Relatos Manualmente
0. Sair
-----------------------------------------
Escolha uma opção:
```

*   **Opção 1:** Siga as instruções para inserir os dados do relator e da ocorrência.
*   **Opção 2:** Veja a lista de todos os relatos já cadastrados.
*   **Opção 3:** Informe as coordenadas de um ponto de referência (latitude e longitude) para buscar relatos próximos.
*   **Opção 4:** Salva o estado atual dos relatos no arquivo `relatos_catastrofes.txt`.
*   **Opção 0:** Salva os relatos e encerra o programa.

## Formato do Arquivo de Persistência (`relatos_catastrofes.txt`)

O arquivo `relatos_catastrofes.txt` armazena os dados da seguinte forma:

1.  A primeira linha contém o número total de relatos salvos.
2.  As linhas subsequentes contêm os dados de cada relato, um campo por linha, na seguinte ordem:
    *   Nome do Relator
    *   Telefone do Relator
    *   Email do Relator
    *   Tipo da Catástrofe
    *   Descrição
    *   Data da Ocorrência (YYYY-MM-DD)
    *   Latitude
    *   Longitude

**Observação:** O campo `distanciaDoPontoReferencia` não é salvo no arquivo, pois é calculado dinamicamente durante a consulta por raio.

