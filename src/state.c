#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

// Definiciones de funciones de ayuda.
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Tarea 1 */
game_state_t* create_default_state() {
  // TODO: Implementar esta funcion.
      //Se asigna memoria para los parametros iniciales, se usa calloc para inicializar a 0
      game_state_t* state = (game_state_t*)calloc(1, sizeof(game_state_t));
      if (state == NULL) {
          return NULL;
      }

      //Se asigna el numero de filas y de serpientes
      state->num_rows = 18;
      state->num_snakes = 1;

      //Se asigna memoria para el tablero, se usa calloc para inicializar a 0
      state->board = (char**)calloc(state->num_rows, sizeof(char*));
      //Se valida que no sea null la posicion de state, para evitar inconvenientes
      if (state->board == NULL) {
          free(state);
          return NULL;
      }
  
      //Se crea y se inicializa cada fila del tablero
      for (unsigned int i = 0; i < state->num_rows; i++) {
          //Se asigna memoria para la fila (20 chars + 1 para '\0' String NULL)
          state->board[i] = (char*)calloc(21, sizeof(char));
          //Se valida que no sea null la posicion de state, para evitar inconvenientes
          if (state->board[i] == NULL) {
              //Se libera memoria en caso de null
              for (unsigned int j = 0; j < i; j++) {
                  free(state->board[j]);
              }
              free(state->board);
              free(state);
              return NULL;
          }
  
          //Se llenan las posiciones de las paredes con #
          if (i == 0 || i == state->num_rows - 1) {
              //Este es para la primera fila y la ultima
              strcpy(state->board[i], "####################");
          } else {
              strcpy(state->board[i], "#                  #");    
          }          
      }
  
      //Se coloca la fruta * en posición (2,9)
      state->board[2][9] = '*';
  
      //Se asigna memoria para la serpiente, se usa calloc para inicializar a 0
      state->snakes = (snake_t*)calloc(state->num_snakes, sizeof(snake_t));
      //Se valida que no sea null la posicion de snake, para evitar inconvenientes
      if (state->snakes == NULL) {
          //Se libera memoria en caso de null
          for (unsigned int i = 0; i < state->num_rows; i++) {
              free(state->board[i]);
          }
          free(state->board);
          free(state);
          return NULL;
      }
  
      //Se asigna las posiciones de la serpiente al iniciar
      state->snakes[0].tail_row = 2;
      state->snakes[0].tail_col = 2;
      state->snakes[0].head_row = 2;
      state->snakes[0].head_col = 4;
      state->snakes[0].live = true;
  
      //Se coloca la serpiernte en el tablero
      state->board[2][2] = 'd';  // cola
      state->board[2][3] = '>';  // cuerpo
      state->board[2][4] = 'D';  // cabeza
  
      return state;
  //return NULL;
}


/* Tarea 2 */
void free_state(game_state_t* state) {
  // TODO: Implementar esta funcion.
  // Verificamos si state es NULL
  if (state == NULL) {
    return;
  }
  
  //Liberamos snakes de state
  free(state->snakes);

  //Ciclo para liberar las filas del tablero
  for (unsigned int i = 0; i < state->num_rows; i++) {
    if (state->board[i] != NULL) {
      //printf("Valor de i: %d\n", i);
      free(state->board[i]);
    }
  }
  //Liberamos el board de state
  free (state->board);
  //Liberamos state
  free(state);

  
  /*if (state == NULL) {
    return;
  }

  //Validamos que board no sea null para liberar memoria
  if (state->board != NULL) {
    printf("Segundo: %d\n", state->num_rows);

    //Ciclo que libera cada fila del tablero
      for (unsigned int i = 0; i < state->num_rows; i++) {
        if (state->board[i] != NULL) {
          //printf("Valor de i: %d\n", i);
          free(state->board[i]);
        }
      }
    
      //printf("Valor de i: 0");
    //free (state->board[0]);
    printf("Tercero: %d\n", state->num_rows);
    //Se libera las filas del tablero
    free (state->board);
    }

    //Validamos que snakes no sea null para liberar memoria
  if (state->snakes != NULL) {
    printf("Primero: %d\n", state->num_rows);
    free(state->snakes);  
  }


  printf("Cuarto: %d\n", state->num_rows);
  //Se libera state
  free(state);
  */
}


/* Tarea 3 */
void print_board(game_state_t* state, FILE* fp) {
  // TODO: Implementar esta funcion.
  // Verificamos si state es NULL y si fp es NULL
  if (state == NULL || fp == NULL) {
    return;
  }
  
  //Ciclo para imprimir las filas del tablero
  for (unsigned int i = 0; i < state->num_rows; i++) {
    if (state->board[i] != NULL) {
      for (unsigned int j = 0; state->board[i][j]; j++) {
        int a = state->board[i][j];
        char c = (char)a;
        //printf("%c", c);
        fprintf(fp, "%c", c);
      }
      //printf("\n");
      fprintf(fp, "\n");
    }
  }

  return;
}


/**
 * Guarda el estado actual a un archivo. No modifica el objeto/struct state.
 * (ya implementada para que la utilicen)
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Tarea 4.1 */


/**
 * Funcion de ayuda que obtiene un caracter del tablero dado una fila y columna
 * (ya implementado para ustedes).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}


/**
 * Funcion de ayuda que actualiza un caracter del tablero dado una fila, columna y
 * un caracter.
 * (ya implementado para ustedes).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}


/**
 * Retorna true si la variable c es parte de la cola de una snake.
 * La cola de una snake consiste de los caracteres: "wasd"
 * Retorna false de lo contrario.
*/
static bool is_tail(char c) {
  // TODO: Implementar esta funcion.
  if(c == 'w' || c == 'a' || c == 's' || c == 'd'){
    return true;
  }else{
    return false;
  }
}


/**
 * Retorna true si la variable c es parte de la cabeza de una snake.
 * La cabeza de una snake consiste de los caracteres: "WASDx"
 * Retorna false de lo contrario.
*/
static bool is_head(char c) {
  // TODO: Implementar esta funcion.
  if(c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x'){
    return true;
  }else{
    return false;
  }
  //return true;
}


/**
 * Retorna true si la variable c es parte de una snake.
 * Una snake consiste de los siguientes caracteres: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implementar esta funcion.
  if(c == '^' || c == '<' || c == 'v' || c == '>' || is_head(c) || is_tail(c)){
    return true;
  }else{
    return false;
  }
  //return true;
}


/**
 * Convierte un caracter del cuerpo de una snake ("^<v>")
 * al caracter que correspondiente de la cola de una
 * snake ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implementar esta funcion.
  switch(c) {
    case '^': return 'w';
    case '<': return 'a';
    case 'v': return 's';
    case '>': return 'd';
    default: return '?'; // Carácter no válido
  }
  //return '?';
}


/**
 * Convierte un caracter de la cabeza de una snake ("WASD")
 * al caracter correspondiente del cuerpo de una snake
 * ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implementar esta funcion.
  switch(c) {
    case 'W': return '^';
    case 'A': return '<';
    case 'S': return 'v';
    case 'D': return '>';
    default: return '?'; // Carácter no válido
  }
  //return '?';
}


/**
 * Retorna cur_row + 1 si la variable c es 'v', 's' o 'S'.
 * Retorna cur_row - 1 si la variable c es '^', 'w' o 'W'.
 * Retorna cur_row de lo contrario
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implementar esta funcion.
  switch(c) {
    case 'v': return cur_row + 1;
    case 's': return cur_row + 1;
    case 'S': return cur_row + 1;
    case '^': return cur_row - 1;
    case 'w': return cur_row - 1;
    case 'W': return cur_row - 1;
    default: return cur_row;
  }
  //return cur_row;
}


/**
 * Retorna cur_col + 1 si la variable c es '>' or 'd' or 'D'.
 * Retorna cur_col - 1 si la variable c es '<' or 'a' or 'A'.
 * Retorna cur_col de lo contrario
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implementar esta funcion.
  switch(c) {
    case '>': return cur_col + 1;
    case 'd': return cur_col + 1;
    case 'D': return cur_col + 1;
    case '<': return cur_col - 1;
    case 'a': return cur_col - 1;
    case 'A': return cur_col - 1;
    default: return cur_col;
  }
  //return cur_col;
}


/**
 * Tarea 4.2
 *
 * Funcion de ayuda para update_state. Retorna el caracter de la celda
 * en donde la snake se va a mover (en el siguiente paso).
 *
 * Esta funcion no deberia modificar nada de state.
*/
static char next_square(game_state_t* state, unsigned int snum) {
  // TODO: Implementar esta funcion.
  //Validamos que sean valores validos
  if(state != NULL || snum <= state->num_snakes){
    //Creamos variables para guardar los valores del head col,row,char
    unsigned int shr = state->snakes[snum].head_row;
    unsigned int shc = state->snakes[snum].head_col;
    char h = get_board_at(state, shr, shc);
    //Validamos que efectivamente sea un head de una serpiente
    if(is_head(h)){
      //obtenemos los valores del siguiente state del head de la serpiente
      unsigned int i = get_next_col(shc, h);
      unsigned int j = get_next_row(shr, h);
      //Retornamos el char de la celda
      return get_board_at(state, j, i);
    }
  }
  return '?';
}


/**
 * Tarea 4.3
 *
 * Funcion de ayuda para update_state. Actualiza la cabeza de la snake...
 *
 * ... en el tablero: agregar un caracter donde la snake se va a mover (¿que caracter?)
 *
 * ... en la estructura del snake: actualizar el row y col de la cabeza
 *
 * Nota: esta funcion ignora la comida, paredes, y cuerpos de otras snakes
 * cuando se mueve la cabeza.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  // TODO: Implementar esta funcion.
  
  //Validamos que sean valores validos y que este viva snake
  if(state != NULL || snum <= state->num_snakes || state->snakes[snum].live){
    
    //Creamos variables para guardar los valores del head col,row,char
    unsigned int shr = state->snakes[snum].head_row;
    unsigned int shc = state->snakes[snum].head_col;
    char h = get_board_at(state, shr, shc);
    
    //Creamos variables para guardar los valores nuevos del head col,row,char
    unsigned int i = get_next_row(shr, h);
    unsigned int j = get_next_col(shc, h);
    char h_new;

    //Actualizamos la posición de la cabeza
    state->snakes[snum].head_row = i;
    state->snakes[snum].head_col = j;

    //Obtenemos el nuevo carácter para la cabeza
    //Validamos si esta muerta y si lo esta se asigna x
    if (h == 'x') {
      h_new = 'x';
    } else {
      // es el mismo caracter head  
      h_new = h; 
    }

    // Actualizamos el tablero:
    //Convertimos la cabeza anterior a cuerpo
    h = head_to_body(h);
    set_board_at(state, shr, shc, h);
    
    //Colocamos la nueva cabeza
    set_board_at(state, i, j, h_new);  
  }      
  return;
}


/**
 * Tarea 4.4
 *
 * Funcion de ayuda para update_state. Actualiza la cola de la snake...
 *
 * ... en el tablero: colocar un caracter blanco (spacio) donde se encuentra
 * la cola actualmente, y cambiar la nueva cola de un caracter de cuerpo (^<v>)
 * a un caracter de cola (wasd)
 *
 * ...en la estructura snake: actualizar el row y col de la cola
*/
static void update_tail(game_state_t* state, unsigned int snum) {
  // TODO: Implementar esta funcion.

    //Validamos que sean valores validos y que este viva snake
    if(state != NULL || snum <= state->num_snakes || state->snakes[snum].live){
    
      //Creamos variables para guardar los valores de tail col,row,char
      unsigned int str = state->snakes[snum].tail_row;
      unsigned int stc = state->snakes[snum].tail_col;
      char t = get_board_at(state, str, stc);
      
      //Creamos variables para guardar los valores nuevos de tail col,row
      unsigned int i = get_next_row(str, t);
      unsigned int j = get_next_col(stc, t);
  
      //Actualizamos la posición de la tail
      state->snakes[snum].tail_row = i;
      state->snakes[snum].tail_col = j;
    
      //Actualizamos el tablero:
      //Actualizamos la tail anterior a ' '
      set_board_at(state, str, stc, ' ');
      
      //Colocamos la nueva tail
      char t_new = get_board_at(state, i, j);
      t_new = body_to_tail(t_new);
      set_board_at(state, i, j, t_new);  
    }      
  

  return;
}

/* Tarea 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  // TODO: Implementar esta funcion.
  
  //Ciclo para actualizar cada serpiente
  for(unsigned int i = 0; i< state->num_snakes; i++){

      //Validamos si las serpiente esta viva
      if(state->snakes->live){

        //Creamos una valiable para ver el valor de la casilla a la que se va a mover
        char c_next = next_square(state, i);

        //Validamos si choca contra la pared o una serpiente
        if (c_next == '#' || is_snake(c_next)) {
          
          //Si choca se muere la serpiente, debemos asignar x al head y live pasa a ser falso
          set_board_at(state, state->snakes->head_row, state->snakes->head_col, 'x');
          state->snakes->live = false;
          continue;
        }



         // Si no ha muerto la serpiente se mueve el head
         update_head(state, i);
        
         //al commer fruta no se debe actualizar el tail, validamos si es fruta la casilla
         if (c_next == '*') {
             
            //Si es fruta, debemos colocar una nueva
            add_food(state);

         } else {

            //Si no era fruta la casilla se debe mover la cola
            update_tail(state, i);
         }


      }
  }

  return;
}

/* Tarea 5 */
game_state_t* load_board(char* filename) {
  // TODO: Implementar esta funcion.
  return NULL;
}


/**
 * Tarea 6.1
 *
 * Funcion de ayuda para initialize_snakes.
 * Dada una structura de snake con los datos de cola row y col ya colocados,
 * atravezar el tablero para encontrar el row y col de la cabeza de la snake,
 * y colocar esta informacion en la estructura de la snake correspondiente
 * dada por la variable (snum)
*/
static void find_head(game_state_t* state, unsigned int snum) {
  // TODO: Implementar esta funcion.
  return;
}

/* Tarea 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  // TODO: Implementar esta funcion.
  return NULL;
}
