# Sistema de Cadastro e Consulta de Relatos sobre Catástrofes Naturais (Versão C)

## Descrição do Projeto

Este projeto implementa um sistema de linha de comando em linguagem C para cadastrar e consultar relatos sobre catástrofes naturais ocorridas em um raio de até 10 km de um ponto de referência. A aplicação permite registrar informações detalhadas sobre o relator e a ocorrência, armazenando os dados de forma persistente em um arquivo de texto (`relatos_catastrofes_c.txt`).

O sistema foi desenvolvido como parte da disciplina de Estrutura de Dados e Algoritmos (adaptado do enunciado original de Técnicas Avançadas de Programação) do curso de Ciência da Computação da FIAP, como uma adaptação da versão original em C++.

## Funcionalidades Implementadas

*   **Cadastro de Relatos:** Permite inserir informações completas sobre o relator (nome, telefone, email) e a catástrofe (tipo, descrição, data, localização por coordenadas).
*   **Listagem de Relatos:** Exibe todos os relatos cadastrados no sistema.
*   **Consulta por Raio:** Busca e exibe relatos que ocorreram dentro de um raio de 10 km de coordenadas fornecidas pelo usuário. Os resultados são ordenados pela distância ao ponto de referência (do mais próximo ao mais distante) usando `qsort`.
*   **Persistência de Dados:** Salva automaticamente os relatos em um arquivo `relatos_catastrofes_c.txt` ao sair do programa e os carrega ao iniciar. Há também uma opção para salvar manualmente.
*   **Validação de Entradas:** Realiza validações básicas para garantir a qualidade dos dados inseridos (campos não vazios, formato numérico e limites para coordenadas).
*   **Cálculo de Distância:** Utiliza a fórmula de Haversine para calcular a distância geodésica entre as coordenadas do relato e o ponto de referência da consulta.
*   **Gerenciamento Dinâmico de Memória:** Utiliza `malloc` e `realloc` para gerenciar um array dinâmico de relatos, permitindo que o sistema armazene um número variável de registros.

## Estrutura de Dados e Conceitos Aplicados

*   **`struct`:** A entidade `Relato` e `Coordenadas` são modeladas utilizando estruturas (`struct`) para agrupar logicamente os dados.
*   **Ponteiros e Alocação Dinâmica:** O armazenamento dos relatos é feito em um array alocado dinamicamente na heap (`Relato *relatos`), gerenciado pela estrutura `BancoDeRelatos`. A capacidade do array é aumentada conforme necessário usando `realloc`.
*   **Algoritmos de Busca e Ordenação:** A consulta por raio implementa uma busca linear filtrada pela distância. Os resultados da consulta são ordenados usando a função `qsort` da biblioteca padrão C, com uma função de comparação customizada (`compararRelatosPorDistancia`).
*   **Manipulação de Arquivos:** As funções `salvarRelatos` e `carregarRelatos` utilizam as funções padrão de I/O de C (`fopen`, `fprintf`, `fscanf`, `fgets`, `fclose`) para interagir com o arquivo de persistência `.txt`.
*   **Validação e Tratamento de Erros:** O código inclui verificações para entradas inválidas (como letras onde números são esperados), tratamento de buffer de entrada (`limparBufferEntrada`) e verificações básicas de erros em alocação de memória e operações de arquivo.

## Como Compilar e Executar

1.  **Pré-requisitos:** É necessário ter um compilador C instalado (como GCC ou Clang).
2.  **Compilação:** Abra um terminal ou prompt de comando, navegue até o diretório onde o arquivo `main.c` foi salvo e execute o seguinte comando (usando GCC como exemplo):
    ```bash
    gcc main.c -o sistema_relatos_c -lm -std=c99
    ```
    *   `gcc`: Nome do compilador C.
    *   `main.c`: Arquivo fonte.
    *   `-o sistema_relatos_c`: Define o nome do arquivo executável gerado.
    *   `-lm`: Linka a biblioteca matemática (libm), necessária para funções como `sqrt`, `sin`, `cos`, `atan2`.
    *   `-std=c99`: Especifica o padrão C99 (ou mais recente, como c11) para garantir compatibilidade com recursos utilizados.

3.  **Execução:** Após a compilação bem-sucedida, execute o programa com:
    ```bash
    ./sistema_relatos_c
    ```

## Exemplo de Uso

Ao executar o programa, um menu será exibido:

```
--- Sistema de Relatos de Catástrofes (Versão C) ---
1. Cadastrar Novo Relato
2. Listar Todos os Relatos
3. Consultar Relatos por Raio (10.0km)
4. Salvar Relatos Manualmente
0. Sair
----------------------------------------------------
Escolha uma opção:
```

*   **Opção 1:** Siga as instruções para inserir os dados do relator e da ocorrência.
*   **Opção 2:** Veja a lista de todos os relatos já cadastrados.
*   **Opção 3:** Informe as coordenadas de um ponto de referência (latitude e longitude) para buscar relatos próximos.
*   **Opção 4:** Salva o estado atual dos relatos no arquivo `relatos_catastrofes_c.txt`.
*   **Opção 0:** Salva os relatos e encerra o programa.

## Formato do Arquivo de Persistência (`relatos_catastrofes_c.txt`)

O arquivo `relatos_catastrofes_c.txt` armazena os dados da seguinte forma:

1.  A primeira linha contém o número total de relatos salvos.
2.  As linhas subsequentes contêm os dados de cada relato, um campo por linha, na seguinte ordem:
    *   Nome do Relator
    *   Telefone do Relator
    *   Email do Relator
    *   Tipo da Catástrofe
    *   Descrição
    *   Data da Ocorrência (YYYY-MM-DD)
    *   Latitude (com alta precisão)
    *   Longitude (com alta precisão)

**Observação:** O campo `distanciaDoPontoReferencia` não é salvo no arquivo, pois é calculado dinamicamente durante a consulta por raio.

