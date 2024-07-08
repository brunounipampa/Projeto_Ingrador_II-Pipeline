#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_MEMORIA_INSTRUCAO 256
#define TAM_INSTRUCAO 16
#define TAM_MEMORIA_DADOS 128
#define TAM_REGISTRADORES 32
#define TAM_MEMORIA 256

typedef struct {
    int opcode;
    int rs;
    int rt;
    int rd;
    int funct;
    int imm;
    int addr;
    int tipo;
} Instrucao;

typedef struct {
    Instrucao instruction;
    int pc;
} if_id_register;

typedef struct {
    int opcode;
    int rs;
    int rt;
    int rd;
    int imm;
} id_ex_register;

typedef struct {
    int result;
    int data;
} ex_mem_register;

typedef struct {
    int result;
    int data;
} mem_wb_register;

typedef struct {
    int registradorSaidaALU;
    int registradorA;
    int registradorB;
} RegistradoresEstado;

typedef struct {
    int registradores[TAM_REGISTRADORES];
} BancoRegistradores;

typedef struct {
    int endereco_atual;
    int endereco_proximo;
} PC;

typedef struct {
    int instrucoes[TAM_MEMORIA][TAM_INSTRUCAO];
    int dados[TAM_MEMORIA_DADOS];
} MemoriaUnica; 

typedef enum { 
    R_TYPE,
    I_TYPE,
    J_TYPE
} InstrucaoTipo;

typedef struct nodo {
    BancoRegistradores banco_undo;
    PC pc_undo;
    MemoriaUnica mem_undo;
    struct nodo *prox;
} nodo;

typedef struct {
    nodo *topo;
} descritor;

void inicializarPC(PC *pc);
void inicializePilha(descritor *desc);
int menu();
void carregarMemoriaUnica();
Instrucao codificarInstrucao(char *instrucao);
void salvar_asm();
void salvar_data();
void save_backup(PC *pc, descritor *topo, BancoRegistradores *banco_registradores, MemoriaUnica *memoria_unica);
void undo(PC *pc, descritor *topo, BancoRegistradores *banco_registradores, MemoriaUnica *memoria_unica);
void imprimirInstrucao(Instrucao inst);
void imprimirMemoriaUnica(MemoriaUnica *memoria_unica);
void imprimirRegistradores(BancoRegistradores *banco_registradores);
void executarCicloInstrucao(PC *pc, BancoRegistradores *banco_registradores, MemoriaUnica *memoria_unica);
void inicializarBancoRegistradores(BancoRegistradores *banco_registradores);
