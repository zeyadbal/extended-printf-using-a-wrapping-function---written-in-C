/* toy-io.c
 * Limited versions of printf and scanf
 *
 * Programmer: Mayer Goldberg, 2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/* the states in the printf state-machine */
enum printf_state {
  st_printf_init=0,
  st_printf_percent=1,
  st_printf_array=2
};

typedef struct state_args {
    char* fs;
    int chars_printed;
    int int_value;
    int array_size;
    int* integers_array;
    char** strings_array;
    char* chars_array;
    char* min_ammount;             
    int* min_ammount_idx;                           
    int spaces;                                 
    int min_ammount_value;                          
    char *string_value;
    char char_value;
}state_args;

struct state_result {
  int printed_chars;
  enum printf_state next_state;
}state_result;

struct state_result init_handler(va_list args, struct state_args* state);
struct state_result percent_handler(va_list args, struct state_args* state);
struct state_result array_handler(va_list args, struct state_args* state);

   
#define MAX_NUMBER_LENGTH 64
#define is_octal_char(ch) ('0' <= (ch) && (ch) <= '7')

int toy_printf(char *fs, ...);

const char *digit = "0123456789abcdef";
const char *DIGIT = "0123456789ABCDEF";

//
int print_int_helper(unsigned int n, int radix, const char *digit);
int print_int(int n, int radix, const char * digit);
int print_unsigned_int_arr(int integers_array[],int array_size,int radix,const char * digit);
int print_signed_int_arr(int integers_array[],int array_size,int radix,const char * digit);
int print_strings_arr(char** strings_array, int array_size);
int print_chars_arr(char* chars_array, int array_size);
int getMinimalValue(char min_ammount[25] ,int  min_ammount_idx);
int numOfDigits(int num);


/* SUPPORTED:
 *   %b, %d, %o, %x, %X -- 
 *     integers in binary, decimal, octal, hex, and HEX
 *   %s -- strings
 */

int toy_printf(char *fs, ...) {
  int chars_printed = 0;
  int int_value = 0;
  int array_size=0;                       
  int* integers_array=0;                         
  char** strings_array=0;                       
  char* chars_array=0;                         
  char min_ammount[25];                             
  int min_ammount_idx=0;                               
   int spaces=0;                                  
   int min_ammount_value=0;                           
  char *string_value=0;
  char char_value=0;
  va_list args;
  enum printf_state state;
  
  //
  struct state_args strgs;
  struct state_result result;
  //

  va_start(args, fs);

  state = st_printf_init; 

 // function_return_type (*pointer_name)(arguments_list);
  struct state_result(*functions[3])(va_list args, struct state_args* state);
  

  functions[0]= &init_handler;
  functions[1]= &percent_handler;
  functions[2]= &array_handler;
  
  for (; *fs != '\0'; ++fs) {
      //load the state
        strgs.fs= fs;
        strgs.int_value= int_value;
        strgs.array_size= array_size;                        
        strgs.integers_array= integers_array;                           
        strgs.strings_array= strings_array;                         
        strgs.chars_array= chars_array;                            
        strgs.min_ammount= min_ammount;                       
        strgs.min_ammount_idx= &min_ammount_idx;                       
        strgs.spaces= spaces;        
        strgs.min_ammount_value= min_ammount_value;
        strgs.string_value= string_value;
        strgs.char_value= char_value;
                
        result= (*functions[state]) (args, &strgs);
        state= result.next_state;
        chars_printed+= result.printed_chars;
  }

  va_end(args);

  return chars_printed;
}

struct state_result init_handler(va_list args, struct state_args* state){
    struct state_result result;
    result.printed_chars=0;

    switch (*(state->fs)) {
        case '%':
            result.next_state= st_printf_percent;
            break;
    default:
        putchar(*(state->fs));
        ++(result.printed_chars);
        result.next_state= st_printf_init;
        break;
    }
    return result;
}

struct state_result percent_handler(va_list args, struct state_args* state){
                
    struct state_result result;
    result.printed_chars=0;
    
    switch (*state->fs) {
        case '%':
            putchar('%');
            ++(result.printed_chars);
            result.next_state = st_printf_init;
            break;
        
        case 'A':
            result.next_state = st_printf_array;                            //here
            break;
    
        case 'd':
                                                            //here
	state->int_value = va_arg(args, int);
        state->min_ammount_value= getMinimalValue(state->min_ammount, (*state->min_ammount_idx));

        if(state->min_ammount_value==0 || numOfDigits(state->int_value)>=state->min_ammount_value){
            if(state->int_value>=0){
                (result.printed_chars) += print_int(state->int_value, 10, digit);
            }else{
                putchar('-');
                (result.printed_chars)++;
                (result.printed_chars) += print_int(-state->int_value, 10, digit);
            } 
        }else{
        
        state->spaces= state->min_ammount_value - numOfDigits(state->int_value);
        
        if(state->min_ammount[0]!='-' && state->min_ammount[0]!='0'){
                if(state->int_value>=0){
                    (result.printed_chars) += print_int(state->int_value, 10, digit);
                }else{
                    putchar('-');
                    (result.printed_chars)++;
                    (result.printed_chars) += print_int(-state->int_value, 10, digit);
                } 
            for(int i=0; i<state->spaces; i++){
                putchar(' ');
            }
            putchar('#');
            (result.printed_chars)+=state->spaces+1;
        }else if(state->min_ammount[0]=='0'){
                    
                    if(state->int_value>=0){
                    for(int i=0; i<state->spaces; i++){
                        putchar('0');
                    }
                    (result.printed_chars)+=state->spaces;
                    (result.printed_chars) += print_int(state->int_value, 10, digit);
                    }else{
                        putchar('-');
                        (result.printed_chars)++;
                        for(int i=0; i<state->spaces; i++){
                            putchar('0');
                        }
                        (result.printed_chars)+=state->spaces;
                        (result.printed_chars) += print_int(-state->int_value, 10, digit);
                    } 
        }else{
                for(int i=0; i<state->spaces; i++){
                    putchar(' ');
                }
                (result.printed_chars)+=state->spaces;
                if(state->int_value>=0){
                    (result.printed_chars) += print_int(state->int_value, 10, digit);
                }else{
                    putchar('-');
                    (result.printed_chars)++;
                    (result.printed_chars) += print_int(-state->int_value, 10, digit);
                } 
        }
    }
        (*state->min_ammount_idx)=0;
        state->min_ammount_value=0;
        state->min_ammount[0]='\0';
        result.next_state = st_printf_init;
        break;


        case 'b':
            state->int_value = va_arg(args, int);
            (result.printed_chars) += print_int(state->int_value, 2, digit);
            result.next_state = st_printf_init;
            break;

      case 'o':
            state->int_value = va_arg(args, int);
            (result.printed_chars) += print_int(state->int_value, 8, digit);
            result.next_state = st_printf_init;
            break;
	
      case 'x':
            state->int_value = va_arg(args, int);
            (result.printed_chars) += print_int(state->int_value, 16, digit);
            result.next_state = st_printf_init;
            break;

      case 'X':
            state->int_value = va_arg(args, int);
            (result.printed_chars) += print_int(state->int_value, 16, DIGIT);
            result.next_state = st_printf_init;
            break;

        case 's':                                      
          
            state->string_value = va_arg(args, char *);
            state->min_ammount_value= getMinimalValue(state->min_ammount, (*state->min_ammount_idx));
            if(state->min_ammount_value==0 || strlen(state->string_value)>=state->min_ammount_value){
                (result.printed_chars) += toy_printf(state->string_value);
            }else{
        
                state->spaces= state->min_ammount_value - strlen(state->string_value);
        
                if(state->min_ammount[0]!='-'){
                    (result.printed_chars) += toy_printf(state->string_value);
                    for(int i=0; i<state->spaces; i++){
                        putchar(' ');
                    }
                putchar('#');
                (result.printed_chars)+=state->spaces+1;
                }else{
                        for(int i=0; i<state->spaces; i++){
                        putchar(' ');
                    }
                (result.printed_chars)+=state->spaces;
                (result.printed_chars) += toy_printf(state->string_value);
                }
            }
            (*state->min_ammount_idx)=0;
            state->min_ammount_value=0;
            result.next_state = st_printf_init;
            break;

      case 'c':
            state->char_value = (char)va_arg(args, int);
            putchar(state->char_value);
            ++(result.printed_chars);
            result.next_state = st_printf_init;
            break;
        
        case 'u':                                                   // here
            state->int_value = va_arg(args, int);
            (result.printed_chars) += print_int(state->int_value, 10, digit);
            result.next_state = st_printf_init;
            break;
    
    
      case '0':
          state->min_ammount[(*state->min_ammount_idx)++]='0';
          result.next_state=st_printf_percent;
          break;
      case '1':
          state->min_ammount[(*state->min_ammount_idx)++]='1';
          result.next_state=st_printf_percent;
          break;
      case '2':
          state->min_ammount[(*state->min_ammount_idx)++]='2';
          result.next_state=st_printf_percent;
          break;
      case '3':
          state->min_ammount[(*state->min_ammount_idx)++]='3';
          result.next_state=st_printf_percent;
          break;
      case '4':
          state->min_ammount[(*state->min_ammount_idx)++]='4';
          result.next_state=st_printf_percent;
          break;
      case '5':
          state->min_ammount[(*state->min_ammount_idx)++]='5';
          result.next_state=st_printf_percent;
          break;
      case '6':
          state->min_ammount[(*state->min_ammount_idx)++]='6';
          result.next_state=st_printf_percent;
          break;
      case '7':
          state->min_ammount[(*state->min_ammount_idx)++]='7';
          result.next_state=st_printf_percent;
          break;
      case '8':
          state->min_ammount[(*state->min_ammount_idx)++]='8';
          result.next_state=st_printf_percent;
          break;
      case '9':
          state->min_ammount[(*state->min_ammount_idx)++]='9';
          result.next_state=st_printf_percent;
          break;
      case '-':
          state->min_ammount[(*state->min_ammount_idx)++]='-';
          result.next_state=st_printf_percent;
          break;

     default:
	toy_printf("Unhandled format...\n", *state->fs);
	exit(-1);
      }
    
    return result;
}

struct state_result array_handler(va_list args, struct state_args* state){
    
    struct state_result result;
    result.printed_chars=0;
    
              switch (*state->fs) {
              
              case 'd':
                  state->integers_array= (int*)va_arg(args, int*);
                  state->array_size= va_arg(args, int);
                  (result.printed_chars)+= print_signed_int_arr(state->integers_array, state->array_size, 10, digit);
                  result.next_state = st_printf_init;
                  break;
            case 'b':
                  state->integers_array= (int*)va_arg(args, int*);
                  state->array_size= va_arg(args, int);
                  (result.printed_chars)+= print_unsigned_int_arr(state->integers_array, state->array_size, 2, digit);
                  result.next_state = st_printf_init;
                  break;
            case 'o':
                  state->integers_array= (int*)va_arg(args, int*);
                  state->array_size= va_arg(args, int);
                  (result.printed_chars)+= print_unsigned_int_arr(state->integers_array, state->array_size, 8, digit);
                  result.next_state = st_printf_init;
                  break;
            case 'x':
                  state->integers_array= (int*)va_arg(args, int*);
                  state->array_size= va_arg(args, int);
                  (result.printed_chars)+= print_unsigned_int_arr(state->integers_array, state->array_size, 16, digit);
                  result.next_state = st_printf_init;
                  break;
            case 'X':
                  state->integers_array= (int*)va_arg(args, int*);
                  state->array_size= va_arg(args, int);
                  (result.printed_chars)+= print_unsigned_int_arr(state->integers_array, state->array_size, 16, DIGIT);
                  result.next_state = st_printf_init;
                  break;
            case 's':
                  state->strings_array= (char**)va_arg(args, char**);
                  state->array_size= va_arg(args, int);
                  (result.printed_chars)+= print_strings_arr(state->strings_array, state->array_size);
                  result.next_state = st_printf_init;
                  break;
            case 'u':
                  state->integers_array= (int*)va_arg(args, int*);
                  state->array_size= va_arg(args, int);
                  (result.printed_chars)+= print_unsigned_int_arr(state->integers_array, state->array_size, 10, digit);
                  result.next_state = st_printf_init;
                  break;
           case 'c':
                  state->chars_array= va_arg(args, char*);
                  state->array_size= va_arg(args, int);
                  (result.printed_chars)+= print_chars_arr(state->chars_array, state->array_size);
                  result.next_state = st_printf_init;
                  break;
                  

      default:
	toy_printf("Unhandled format...\n", *state->fs);
	exit(-1);
      }
    
    return result;
    
}

//general functions

int print_int_helper(unsigned int n, int radix, const char *digit) { //here
  int result;

  if (n < radix) {
    putchar(digit[n]);
    return 1;
  }
  else {
    result = print_int_helper(n / radix, radix, digit);
    putchar(digit[n % radix]);
    return 1+ result;
    
  }
}

int print_int(int n, int radix, const char * digit) {
  if (radix < 2 || radix > 16) {
    toy_printf("Radix must be in [2..16]: Not %d\n", radix);
    exit(-1);
  }

  if (n == 0) {
    putchar('0');
    return 1;
  }
  else {
    return print_int_helper(n, radix, digit);
  }
}

int print_unsigned_int_arr(int integers_array[],        //here
              int array_size,
              int radix,
              const char * digit){  
    
    if (radix < 2 || radix > 16) {
    toy_printf("Radix must be in [2..16]: Not %d\n", radix);
    exit(-1);
   }
   if (array_size == 0) {
    toy_printf("{}");
    return 2;
  }else {
    int c=2;
    putchar('{');
    for(int i=0; i<array_size; i++){
       c+= print_int_helper(integers_array[i], radix, digit);
       if(i<array_size-1){
            putchar(',');  c++;
       }
    }
    putchar('}');
    return c;
  }
}

int print_signed_int_arr(int integers_array[],        //here
              int array_size,
              int radix,
              const char * digit){  
    
    if (radix < 2 || radix > 16) {
    toy_printf("Radix must be in [2..16]: Not %d\n", radix);
    exit(-1);
   }
   if (array_size == 0) {
    toy_printf("{}");
    return 2;
  }else {
    int c=2;
    putchar('{');
    for(int i=0; i<array_size; i++){
        if(integers_array[i]>=0){
            c+= print_int_helper(integers_array[i], radix, digit);
        }else{
            putchar('-'); c++;
            c+= print_int_helper(-integers_array[i], radix, digit);
        }
       if(i<array_size-1){
            putchar(',');  c++;
       }
    }
    putchar('}');
    return c;
  }
}

int print_strings_arr(char** strings_array, int array_size){
    int c=2;
    putchar('{');
   for(int i=0; i<array_size; i++){
       putchar('"'); c++;
       c+=toy_printf("%s",strings_array[i]);
       putchar('"'); c++;
       if(i<array_size-1){
           putchar(',');  c++;
       }
   }
   putchar('}');
   return c;
}

int print_chars_arr(char* chars_array, int array_size){
    int c=2;
    putchar('{');
   for(int i=0; i<array_size; i++){
       putchar('\''); c++;
       c+=toy_printf("%c",chars_array[i]);
       putchar('\''); c++;
       if(i<array_size-1){
           putchar(',');  c++;
       }
   }
   putchar('}');
   return c;
}

int getMinimalValue(char min_ammount[25] ,int  min_ammount_idx){
    
    int start=0;
   
    if(min_ammount_idx==0){
        return 0;
    }
    if(min_ammount[0]=='-'){
            start=1;
    }
    if(min_ammount_idx>=2 && min_ammount[min_ammount_idx]=='0'){
            start=2;
    }
    char arr[min_ammount_idx-start];
    
    for(int i=start; i<min_ammount_idx; i++){
            arr[i-start]=min_ammount[i];
    }
    int minimalVal= atoi(arr);
    arr[0]='\0'; // clean the arr
    return minimalVal;
}

int numOfDigits(int num){
    
    int c=0;
    if(num<0){
        c++;
        num=num*-1;
    }
    while(num>0){
        num=num/10;
        c++;
    }
    return c;
}
