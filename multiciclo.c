#include "multiciclo.h"

// FUNÇÕES GERAIS

char memoria_instrucao[TAM_MEMORIA_INSTRUCAO][TAM_INSTRUCAO]; // Definição das variáveis
int memoria_dados[TAM_MEMORIA_DADOS]; // Definição das variáveis

int menu() {
    int m;
    printf("\n");
    printf("===========================================\n");
    printf("|       MINI-MIPS 8 BITS - UNIPAMPA       |\n");
    printf("===========================================\n");
    printf("|       1. Carregar memoriaUnica          |\n");
    printf("|       2. Imprimir memoriaUnica          |\n");
    printf("|       3. Imprimir registradores         |\n");
    printf("|       4. Imprimir todo o simulador      |\n");
    printf("|       5. Decodificar instrucao atual    |\n");
    printf("|       6. Salvar .asm                    |\n");
    printf("|       7. Salvar .data                   |\n");
    printf("|       8. Executa Programa (run)         |\n");
    printf("|       9. Step by Step                   |\n");
    printf("|       10. Volta uma instrucao (Back)    |\n");
    printf("|       0. Sair                           |\n");
    printf("===========================================\n");
    printf("Escolha uma opcao: ");
    setbuf(stdin, NULL);
    if (scanf("%d", &m) != 1) {
        printf("Erro ao ler entrada.\n");
        return -1;
    }
    printf("===========================================\n");
    return m;
}


int ula(int a, int b, int op) {
    switch (op) {
        case 0: // ADD
            return a + b;
        case 1: // SUB
            return a - b;
        case 2: // AND
            return a & b;
        case 3: // OR
            return a | b;
        default:
            printf("Operacao da ULA nao reconhecida: %d\n", op);
            return 0;
    }
}

int mux(int a, int b, int select) {
    if (select == 0) {
        return a;
    } else {
        return b;
    }
}


Instrucao codificarInstrucao(char *instrucao_string) {
    Instrucao inst;
    unsigned int instrucao_int = strtol(instrucao_string, NULL, 2); //converte a string binaria para inteiro
    // Determina o tipo de instrução com base no opcode
    int opcode = (instrucao_int >> 12) & 0xF; //intrucao_int >> 12 significa que estamos deslocando 12 bits para a direita e & 0xF significa que estamos pegando os 4 bits mais significativos
    /*if (opcode == 0) {
        inst.tipo = R_TYPE;
    } else if (opcode == 2 || opcode == 3) {
        inst.tipo = J_TYPE;
    } else {
        inst.tipo = I_TYPE;
    }*/
    if (strncmp(instrucao_string, "add", 3) == 0) {
        inst.opcode = 0;
        inst.rs = 0;
        inst.rt = 0;
        inst.rd = 0;
        inst.funct = 32;
        inst.imm = 0;
        inst.addr = 0;
    } else if (strncmp(instrucao_string, "j", 1) == 0) {
        inst.opcode = 2;
        inst.rs = 0;
        inst.rt = 0;
        inst.rd = 0;
        inst.funct = 0;
        inst.imm = 0;
        inst.addr = 0;
    } else if (strncmp(instrucao_string, "addi", 4) == 0) {
        inst.opcode = 8;
        inst.rs = 0;
        inst.rt = 0;
        inst.rd = 0;
        inst.funct = 0;
        inst.imm = 0;
        inst.addr = 0;
    }

    // Extrai os campos da instrução com base nos tipos R, I e J
    switch (inst.tipo) {
        case R_TYPE:
            inst.opcode = (instrucao_int >> 12) & 0xF; // 4 bits mais significativos
            inst.rs = (instrucao_int >> 9) & 0x7; // 3 bits seguintes
            inst.rt = (instrucao_int >> 6) & 0x7; // 3 bits seguintes
            inst.rd = (instrucao_int >> 3) & 0x7; // 3 bits seguintes
            inst.funct = instrucao_int & 0x7; // 3 bits menos significativos
            break;
        case I_TYPE:
            inst.opcode = (instrucao_int >> 12) & 0xF; // 4 bits mais significativos
            inst.rs = (instrucao_int >> 9) & 0x7; // 3 bits seguintes
            inst.rt = (instrucao_int >> 6) & 0x7; // 3 bits seguintes
            inst.imm = instrucao_int & 0x3F; // 6 bits menos significativos
            break;
        case J_TYPE:
            inst.opcode = (instrucao_int >> 12) & 0xF; // 4 bits mais significativos
            inst.addr = instrucao_int & 0xFFF; // 12 bits menos significativos
            break;
    }
    //imprimirInstrucao(inst); // Chama a função para imprimir os campos da instrução
    return inst;
}


// Conversor de instrução binária em assembly
void converter_asm(char *instrucao_binaria, FILE *arquivo_asm, Instrucao inst) {
    if(inst.rd == 0 && inst.rt == 0 && inst.rs == 0){
        return;
    }
    switch (inst.tipo) {
        case R_TYPE:
            switch (inst.funct) {
                case 0:
                    fprintf(arquivo_asm, "add $r%d, $r%d, $r%d", inst.rd, inst.rs, inst.rt);
                    break;
                case 2:
                    fprintf(arquivo_asm, "sub $r%d, $r%d, $r%d", inst.rd, inst.rs, inst.rt);
                    break;
                case 4:
                    fprintf(arquivo_asm, "and $r%d, $r%d, $r%d", inst.rd, inst.rs, inst.rt);
                    break;
                case 5:
                    fprintf(arquivo_asm, "or $r%d, $r%d, $r%d", inst.rd, inst.rs, inst.rt);
                    break;
                default:
                    fprintf(arquivo_asm, "Funcao R nao reconhecida: %d", inst.funct);
                    break;
            }
            break;
        case I_TYPE:
            switch (inst.opcode) {
                case 4:
                    fprintf(arquivo_asm, "addi $r%d, $r%d, %d", inst.rt, inst.rs, inst.imm);
                    break;
                case 11:
                    fprintf(arquivo_asm, "lw $r%d, %d(R%d)", inst.rt, inst.imm, inst.rs);
                    break;
                case 15:
                    fprintf(arquivo_asm, "sw $r%d, %d($r%d)", inst.rt, inst.imm, inst.rs);
                    break;
                case 8:
                    fprintf(arquivo_asm, "beq $r%d, $r%d, %d", inst.rt, inst.rs, inst.imm);
                    break;
                default:
                    fprintf(arquivo_asm, "Opcode I nao reconhecido: %d", inst.opcode);
                    break;
            }
            break;
        case J_TYPE:
            fprintf(arquivo_asm, "j %d", inst.addr);
            break;
    }

}

int check_overflow(int result) {
    if (result < -128 || result > 127) {
        return 1; // Indica que houve overflow
    }
    return 0; // Sem overflow
}


int sign_extend(int value, int original_bits) {
    int shift = 32 - original_bits;
    return (value << shift) >> shift;
}

//detectar hazard
int detect_structural_hazard(PC *pc, MemoriaUnica *memoria_unica) {
    // Verificar se a instrução atual acessa a memória de dados ou de instruções
    // e se há outra instrução que também acessa a mesma memória
    int endereco_atual = pc->endereco_atual;
    for (int i = 0; i < TAM_MEMORIA; i++) {
        if (i != endereco_atual && memoria_unica->instrucoes[i] != NULL) {
            // Verificar se a instrução em i acessa a mesma memória que a instrução atual
            if (strstr((char *)memoria_unica->instrucoes[i], "lw") != NULL || strstr((char *)memoria_unica->instrucoes[i], "sw") != NULL) {
                return 1; // Hazard detectado
            }
        }
    }
    return 0; // Não há hazard
}

int detect_data_hazard(BancoRegistradores *banco_registradores, Instrucao instrucao) {
    // Verificar se a instrução atual utiliza um registrador que está sendo utilizado
    // por outra instrução
    for (int i = 0; i < TAM_REGISTRADORES; i++) {
        if (banco_registradores->registradores[i] != 0 && (i == instrucao.rs || i == instrucao.rt || i == instrucao.rd)) {
            return 1; // Hazard detectado
        }
    }
    return 0; // Não há hazard
}

int detect_control_hazard(PC *pc, Instrucao instrucao) {
    // Verificar se a instrução atual altera o fluxo de controle do programa
    // e se há outra instrução que depende do resultado dessa instrução
    if (instrucao.opcode == 2 || instrucao.opcode == 3) {
        // Instrução de jump ou branch
        return 1; // Hazard detectado
    }
    return 0; // Não há hazard
}

// Implemente as funções para executar o programa e as instruções
//executar programa com todos os ciclos necessarios FETCH, DECODE, EXECUTE, MEMORY, WRITEBACK
void executarCicloInstrucao(PC *pc, BancoRegistradores *banco_registradores, MemoriaUnica *memoria_unica) {
    static int estado = 0; 
    static Instrucao instrucao; 
    int rs_teste, rt_teste, resultado;
    if_id_register if_id; 
    id_ex_register id_ex;  
    RegistradoresEstado registradores_estado; 
    ex_mem_register ex_mem; 
    mem_wb_register mem_wb; 

    char instrucao_string[TAM_INSTRUCAO + 1]; // Declare a character array to hold the instruction string

    if (memoria_unica->instrucoes[pc->endereco_atual] == 0) {

    } else {
        char* instrucao_string; // declare como um array de caracteres
        sprintf(instrucao_string, "%63s", (char *)memoria_unica->instrucoes[pc->endereco_atual]);


        printf("Instrução buscada: %s\n", instrucao_string); 
        codificarInstrucao(instrucao_string); 
        converter_asm(instrucao_string, stdout, instrucao); 
    }    

    // Verificar se há hazard
    if (detect_structural_hazard(pc, memoria_unica) || detect_data_hazard(banco_registradores, instrucao) || detect_control_hazard(pc, instrucao)) {
        // Inserir bolha
        pc->endereco_atual = pc->endereco_atual + 1;
        pc->endereco_proximo = pc->endereco_atual + 1;
        estado = 0; // Reinicia o ciclo
        return;
    }
    
            switch (estado) {
                case 0: // ETAPA DE BUSCA DA INSTRUÇÃO
                    printf("\n");
                    printf("--------Executando ciclo FETCH-----------\n");
                    if (memoria_unica->instrucoes[pc->endereco_atual] == NULL || strlen((char *)memoria_unica->instrucoes[pc->endereco_atual]) == 0) {
                        pc->endereco_atual = pc->endereco_proximo;
                        pc->endereco_proximo = pc->endereco_atual + 1;
                        estado = 0; // Reinicia o ciclo
                        break;
                    }
                    printf("Instrução buscada: %s\n", (char *)memoria_unica->instrucoes[pc->endereco_atual]);
                    // RI = Mem[PC]
                    instrucao = codificarInstrucao((char *)memoria_unica->instrucoes[pc->endereco_atual]);
                    // PC = PC + 1
                    converter_asm((char *)memoria_unica->instrucoes[pc->endereco_atual], stdout, instrucao);
                    if_id.instruction = codificarInstrucao((char *)memoria_unica->instrucoes[pc->endereco_atual]);
                    if_id.pc = pc->endereco_atual + 1;
                    estado = 1;
                    break;
                    printf("\n");
                    break;

                case 1: // ETAPA DE DECODIFICAÇÃO DA INSTRUÇÃO E BUSCA DOS REGISTRADORES
                    printf("\n");
                    printf("--------Executando ciclo DECODE-----------\n");
                    // Decodificação da instrução
                    switch (instrucao.tipo) {
                        case R_TYPE:
                            printf("\n");
                            printf("|--------------------------------|\n");
                            printf("|Instrução do tipo R identificada|\n");
                            printf("|--------------------------------|\n");
                            imprimirInstrucao(instrucao);
                            printf("Instrução do tipo R identificada\n");
                            banco_registradores->registradores[instrucao.rs];
                            banco_registradores->registradores[instrucao.rt];
                            banco_registradores->registradores[instrucao.rd];
                            printf("Registrador A: %d\n", banco_registradores->registradores[instrucao.rs]);
                            printf("Registrador B: %d\n", banco_registradores->registradores[instrucao.rt]);
                            printf("Registrador de saída da ALU: %d\n", banco_registradores->registradores[instrucao.rd]);
                            printf("\n");
                            estado = 7;
                            break;
                        case I_TYPE:
                            printf("\n");
                            printf("|--------------------------------|\n");
                            printf("|Instrução do tipo I identificada|\n");
                            printf("|--------------------------------|\n");
                            imprimirInstrucao(instrucao);
                            banco_registradores->registradores[instrucao.rs];
                            banco_registradores->registradores[instrucao.rt];
                            printf("Registrador A: %d\n", instrucao.rs);
                            printf("Registrador B: %d\n", instrucao.rt);
                            printf("valor do imediato: %d\n", instrucao.imm);
                            printf("Registrador de saída da ALU: %d\n", banco_registradores->registradores[instrucao.rt]);
                            printf("\n");
                            estado = 2;
                            break;
                        case J_TYPE:
                            printf("\n");
                            printf("|--------------------------------|\n");
                            printf("|Instrução do tipo J identificada|\n");
                            printf("|--------------------------------|\n");
                            imprimirInstrucao(instrucao);
                            printf("\n");
                            estado = 10;
                            break;
                        default:
                            printf("\n");
                            printf("Tipo de instrução desconhecido\n");
                            estado = 0;
                            break;
                    }
                    id_ex.opcode = instrucao.opcode;
                    id_ex.rs = instrucao.rs;
                    id_ex.rt = instrucao.rt;
                    id_ex.rd = instrucao.rd;
                    id_ex.imm = instrucao.imm;
                    estado = 2;
                    break;

        case 2: // EXECUTE (TIPO I)
            printf("\n");
            printf("--------Executando ciclo EXECUTE (TIPO I)-----------\n");
            switch (instrucao.opcode) {
                case 4: // ADDI
                    printf("\n");
                    printf("Instrução ADDI\n");
                    registradores_estado.registradorSaidaALU = registradores_estado.registradorA + instrucao.imm;
                    printf("Registrador de saída da ALU + imm: %d\n", registradores_estado.registradorSaidaALU);
                    printf("executando instrução...\n");
                    printf("\n");
                    estado = 6; // Vai para o ciclo de WRITEBACK
                    break;
                case 11: // LW
                    printf("\n");
                    printf("Instrução LW\n");
                    registradores_estado.registradorSaidaALU = registradores_estado.registradorA + instrucao.imm;
                    printf("executando instrução...\n");
                    printf("O valor da ALUSaida é: [%d]\n", registradores_estado.registradorSaidaALU);
                    printf("\n");
                    estado = 3;
                    break;
                case 15: // SW
                    printf("\n");
                    printf("Instrução SW\n");
                    registradores_estado.registradorSaidaALU = registradores_estado.registradorA + instrucao.imm;
                    printf("executando instrução...\n");
                    printf("O valor da ALUSaida é: [%d]\n", registradores_estado.registradorSaidaALU);
                    printf("\n");
                    estado = 5;
                    break;
                case 8: // BEQ
                    printf("\n");
                    printf("Instrução BEQ\n");
                    rs_teste = banco_registradores->registradores[instrucao.rs];
                    rt_teste = banco_registradores->registradores[instrucao.rt];
                    if (rs_teste == rt_teste) {
                        pc->endereco_proximo = pc->endereco_atual + instrucao.imm;
                        printf("BEQ tomado. Novo endereço próximo: %d\n", pc->endereco_proximo);
                    } else {
                        printf("BEQ não tomado. Endereço próximo: %d\n", pc->endereco_proximo);
                    }
                    printf("\n");
                    estado = 9;
                    break;
                default:
                    printf("Opcode desconhecido\n");
                    estado = 0;
                    break;
            }
            ex_mem.result = registradores_estado.registradorA + instrucao.imm;
                ex_mem.data = registradores_estado.registradorA + instrucao.imm;
                estado = 3;
                break;

        case 3: // MEMORY LW
            printf("\n");
            printf("--------Executando ciclo MEMORY LW-----------\n");
            // LW: RDM = Mem[ALUout]
            printf("Acessando a memória de dados...\n");
            registradores_estado.registradorSaidaALU = memoria_dados[registradores_estado.registradorSaidaALU];
            printf("Valor lido da memória de dados: %d\n", registradores_estado.registradorSaidaALU);
            printf("\n");
                mem_wb.result = memoria_dados[ex_mem.data];
                mem_wb.data = memoria_dados[ex_mem.data];
                estado = 4;
                break;

        case 4: // WRITEBACK (LW)
            printf("\n");
            printf("--------Executando ciclo WRITEBACK LW-----------\n");
            // Escreve o resultado de uma instrução LW no registrador
            banco_registradores->registradores[instrucao.rt] = registradores_estado.registradorSaidaALU;
            printf("valor adicionado ao registrador: %d\n", banco_registradores->registradores[instrucao.rt]);
            printf("Escreveu no registrador com sucesso!!\n");
            printf("\n");
            estado = 0;
            break;

        case 5: // WRITEBACK (SW)
            printf("\n");
            printf("--------Executando ciclo WRITEBACK SW-----------\n");
            // Escrever resultados de volta à memória (SW)
            // SW: Mem[ALUout] = B
            memoria_dados[registradores_estado.registradorSaidaALU] = registradores_estado.registradorB;
            printf("O valor que vai ser escrito na memória de dados: %d\n", registradores_estado.registradorB);
            printf("Escreveu na memória de dados com sucesso!!\n");
            printf("\n");
            estado = 0;
            break;

        case 6: // WRITEBACK (ADDI)
            printf("\n");
            printf("--------Executando ciclo WRITEBACK-----------\n");
            // Escrever resultados de volta aos registradores (ADDI)
            // ADDI: reg[RI] = ULAout
            banco_registradores->registradores[instrucao.rt] = registradores_estado.registradorSaidaALU;
            printf("Esse valor vai ser adicionado no registador: %d\n", registradores_estado.registradorSaidaALU);
            printf("Escreveu no registrador com sucesso!!\n");
            printf("\n");
            estado = 0;
            break;

        case 7: // Estado do tipo R
            printf("\n");
            printf("-------- Executando ciclo EXECUTE (TIPO R)-----------\n");
            switch (instrucao.funct) {
                case 0: // ADD
                    printf("\n");
                    printf("Instrução ADD\n");
                    registradores_estado.registradorSaidaALU = ula(registradores_estado.registradorA, registradores_estado.registradorB, 0);
                    printf("Registrador de saída da ALU: %d\n", registradores_estado.registradorSaidaALU);
                    estado = 8; // Vai para o ciclo de WRITEBACK
                    break;
                case 2: // SUB
                    printf("\n");
                    printf("Instrução SUB\n");
                    registradores_estado.registradorSaidaALU = ula(registradores_estado.registradorA, registradores_estado.registradorB, 1);
                    estado = 8; // Vai para o ciclo de WRITEBACK
                    break;
                case 4: // AND
                    printf("\n");
                    printf("Instrução AND\n");
                    registradores_estado.registradorSaidaALU = ula(registradores_estado.registradorA, registradores_estado.registradorB, 2);
                    estado = 8; // Vai para o ciclo de WRITEBACK
                    break;
                case 5: // OR
                    printf("\n");
                    printf("Instrução OR\n");
                    registradores_estado.registradorSaidaALU = ula(registradores_estado.registradorA, registradores_estado.registradorB, 3);
                    estado = 8; // Vai para o ciclo de WRITEBACK
                    break;
                default:
                    printf("\n");
                    printf("Função desconhecida\n");
                    estado = 0;
                    break;
            }
            break;

        case 8: // WRITEBACK (tipo R)
            printf("\n");
            printf("--------Executando ciclo WRITEBACK (TIPO R)-----------\n");
            // Escrever resultados de volta aos registradores (tipo R)
            banco_registradores->registradores[instrucao.rd] = registradores_estado.registradorSaidaALU;
            printf("O valor vai ser adicionado no registrador: %d\n", registradores_estado.registradorSaidaALU);
            printf("Escreveu no registrador com sucesso!!\n");
            printf("\n");
            estado = 0;
            break;

        case 9: // Condição BEQ
            printf("\n");
            printf("--------Executando condição BEQ-----------\n");
            // Verificação de condição BEQ
            printf("\n");
            estado = 0;
            break;

        case 10: // Estado do tipo J
            printf("\n");
            printf("--------Executando ciclo EXECUTE (TIPO J)-----------\n");
            pc->endereco_atual = instrucao.addr;
            pc->endereco_proximo = pc->endereco_atual + 1;
            printf("Jump executado com sucesso\n");
            printf("\n");
            estado = 0;
            break;

        default:
            printf("Estado desconhecido\n");
            estado = 0;
            break;
    }
}


// FUNÇÕES DE INICIAÇÃO E ENTRADA
void inicializarBancoRegistradores(BancoRegistradores *banco_registradores) {
    for(int i = 0; i < TAM_REGISTRADORES; i++) {
        banco_registradores->registradores[i] = 0;
    }
}

void inicializarPC(PC *pc) {
    pc->endereco_atual = 0;
    pc->endereco_proximo = 1;
}

void inicializarMemoriaDados() {
    // Inicializa a memória de dados preenchendo ela com zeros
    for (int i = 0; i < TAM_MEMORIA_DADOS; i++) {
        memoria_dados[i] = 0;
    }
}
// FUNÇÕES DE CARREGAR MEMORIA DE INSTRUÇÕES E DADOS
void carregarMemoriaUnica() {
    char nome_arquivo[50]; // Declarar como um array de caracteres
    int opcao;
    printf("\n");
    printf("Escolha entre carregar a memoria de instrucoes (0) ou a memoria de dados (1): ");
    if (scanf("%d", &opcao) != 1) {
        printf("Erro ao ler entrada.\n");
        return;
    }
    printf("Digite o nome do arquivo: ");
    if (scanf("%49s", nome_arquivo) != 1) {
        printf("Erro ao ler entrada.\n");
        return;
    }

    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo.\n");
        return;
    }

    if (opcao == 0) {
        // Carrega a memória de instruções
        int i = 0;
        char linha[TAM_INSTRUCAO + 1];
        while (fgets(linha, TAM_INSTRUCAO + 1, arquivo) && i < TAM_MEMORIA_INSTRUCAO) {
            linha[strcspn(linha, "\n")] = '\0'; // Remove o caractere de nova linha
            strncpy(memoria_instrucao[i], linha, TAM_INSTRUCAO);
            i++;
        }
        printf("Memoria de instrucoes carregada com sucesso!\n");
    } else if (opcao == 1) {
        // Carrega a memória de dados
        int endereco, valor;
        // Lê cada par de valores endereço e valor do arquivo e carrega na memória de dados
        for (int i = 0; i < TAM_MEMORIA_DADOS; i++) {
            if (fscanf(arquivo, "Endereço de memoria[%d]:%d\n", &endereco, &valor) != 2) {
                printf("Erro ao ler arquivo.\n");
                break;
            }
            memoria_dados[endereco] = valor;
        }

        printf("Memória de dados carregada com sucesso\n");
    } else {
        printf("Opcao inválida\n");
    }

    fclose(arquivo);
}

// FUNÇÕES DE SALVAMENTO
void salvar_asm() {
    FILE *arquivo_asm = fopen("programa.asm", "w"); //w = write / ponteiro para escrever no arquivo
    if (arquivo_asm == NULL) {
        printf("Erro ao criar o arquivo\n");
        return;
    }
    // Iterar sobre cada instrução na memória e converter para assembly
    for (int i = 0; i < TAM_MEMORIA; i++) {
        converter_asm(memoria_instrucao[i], arquivo_asm, codificarInstrucao(memoria_instrucao[i]));
        fprintf(arquivo_asm, "\n");
    }
    fclose(arquivo_asm);
    printf("Arquivo .asm salvo com sucesso!\n");
}

// Salvar configuração de memória em .data

void salvar_data() {
    FILE *arquivo_memoria = fopen("programa.data", "w"); //w = write / ponteiro para escrever no arquivo
    if (arquivo_memoria == NULL) {
        printf("Erro ao criar o arquivo\n");
        return;
    }
    // Iterar sobre cada endereço na memória de dados e salvar o conteúdo
    for (int i = 0; i < TAM_MEMORIA_DADOS; i++) {
        fprintf(arquivo_memoria, "Endereço de memoria[%d]:%d\n", i, memoria_dados[i]);
    /* Verificar para não sobrecarregar 
    if (pc.endereco_atual >= TAM_MEMORIA_DADOS) {
            printf("Erro: endereco de memoria excedeu o limite\n");
            return;
        }*/
    }
    fclose(arquivo_memoria);
    printf("Arquivo .mem salvo com sucesso!\n");
}



//FUNÇÕES DE IMPRESSÃO
void imprimirInstrucao(Instrucao inst) {
    switch (inst.tipo) {
        case R_TYPE:
            printf("|     Tipo: [TIPO R]  |\n");
            printf("|      Opcode: [%d]   |\n", inst.opcode);
            printf("|      Rs: [%d]        |\n", inst.rs);
            printf("|      Rt: [%d]        |\n", inst.rt);
            printf("|      Rd: [%d]        |\n", inst.rd);
            printf("|      Funct: [%d]     |\n", inst.funct);
            printf("|---------------------|\n");
            break;
        case I_TYPE:
            printf("|     Tipo: [TIPO I]    |\n");
            printf("|      Opcode: [%d]     |\n", inst.opcode);
            printf("|      Rs: [%d]         |\n", inst.rs);
            printf("|      Rt: [%d]         |\n", inst.rt);
            printf("|     Imediato: [%d]    |\n", inst.imm);
            printf("|-----------------------|\n");
            break;
        case J_TYPE:
            printf("|    Tipo: [TIPO J] |\n");
            printf("|     Opcode: [%d]  |\nt", inst.opcode);
            printf("|     Addr: [%d]     |\n", inst.addr);
            printf("|-------------------|\n");
            break;
    }
}

void imprimirMemoriaUnica(MemoriaUnica *memoria_unica) {
    printf("Memoria de Instrucoes:\n");
    for (int i = 0; i < TAM_MEMORIA_INSTRUCAO; i++) {
        printf("Instrucao[%d]: %s\n", i, memoria_instrucao[i]);
    }

    printf("\nMemoria de Dados:\n");
    for (int i = 0; i < TAM_MEMORIA_DADOS; i++) {
        printf("Dados[%d]: %d\n", i, memoria_dados[i]);
    }
}

void imprimirRegistradores(BancoRegistradores *banco_registradores) {
    printf("Conteudo do banco de registradores:\n");
    for (int i = 0; i < TAM_REGISTRADORES; i++) {
        printf("R%d: %d\n", i, banco_registradores->registradores[i]);
    }
}



//BACK
void inicializePilha(descritor *desc) {
    desc->topo = NULL;
}

void save_backup(PC *pc, descritor *topo, BancoRegistradores *banco_registradores, MemoriaUnica *memoria_unica) {
    nodo *novo_nodo = (nodo *)malloc(sizeof(nodo));
    if (novo_nodo == NULL) {
        printf("Erro ao alocar memória para o backup.\n");
        return;
    }

    // Salvar o estado atual
    novo_nodo->banco_undo = *banco_registradores;
    novo_nodo->pc_undo = *pc;
    memcpy(novo_nodo->mem_undo.dados, memoria_unica->dados, sizeof(memoria_unica->dados));

    // Inserir no topo da pilha
    novo_nodo->prox = topo->topo;
    topo->topo = novo_nodo;
}

void undo(PC *pc, descritor *topo, BancoRegistradores *banco_registradores, MemoriaUnica *memoria_unica) {
    if (topo->topo == NULL) {
        printf("Não há estados para desfazer.\n");
        return;
    }

    nodo *nodo_remover = topo->topo;

    // Restaurar o estado salvo
    *banco_registradores = nodo_remover->banco_undo;
    *pc = nodo_remover->pc_undo;
    memcpy(memoria_unica->dados, nodo_remover->mem_undo.dados, sizeof(memoria_unica->dados));

    // Remover o nodo do topo da pilha
    topo->topo = nodo_remover->prox;
    free(nodo_remover);
}
