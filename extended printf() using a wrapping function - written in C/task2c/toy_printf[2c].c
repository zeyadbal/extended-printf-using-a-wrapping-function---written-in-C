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

// init_handler_helpers *start*
void (*init_handler_helpers[128]) (struct state_args* state, struct state_result* result);
void is_percent(struct state_args* state, struct state_result* result);
void print_this_char(struct state_args* state, struct state_result* result);
// init_handler_helpers *end*
   
// percent_handler_helpers *start*
void (*percent_handler_helpers[128]) (va_list args, struct state_args* state, struct state_result* result);
void print_percent(va_list args, struct state_args* state, struct state_result* result);
void is_A(va_list args, struct state_args* state, struct state_result* result);
void is_d(va_list args, struct state_args* state, struct state_result* result);
void is_b(va_list args, struct state_args* state, struct state_result* result);
void is_o(va_list args, struct state_args* state, struct state_result* result);
void is_x(va_list args, struct state_args* state, struct state_result* result);
void is_X(va_list args, struct state_args* state, struct state_result* result);
void is_s(va_list args, struct state_args* state, struct state_result* result);
void is_c(va_list args, struct state_args* state, struct state_result* result);
void is_u(va_list args, struct state_args* state, struct state_result* result);
void is_0(va_list args, struct state_args* state, struct state_result* result);
void is_1(va_list args, struct state_args* state, struct state_result* result);
void is_2(va_list args, struct state_args* state, struct state_result* result);
void is_3(va_list args, struct state_args* state, struct state_result* result);
void is_4(va_list args, struct state_args* state, struct state_result* result);
void is_5(va_list args, struct state_args* state, struct state_result* result);
void is_6(va_list args, struct state_args* state, struct state_result* result);
void is_7(va_list args, struct state_args* state, struct state_result* result);
void is_8(va_list args, struct state_args* state, struct state_result* result);
void is_9(va_list args, struct state_args* state, struct state_result* result);
void is_minus(va_list args, struct state_args* state, struct state_result* result);
void unknown_identifiers(va_list args, struct state_args* state, struct state_result* result);
// percent_handler_helpers *end*

// array_handler_helpers *start*
void (*array_handler_helpers[128]) (va_list args, struct state_args* state, struct state_result* result);
void is_d_array(va_list args, struct state_args* state, struct state_result* result);
void is_b_array(va_list args, struct state_args* state, struct state_result* result);
void is_o_array(va_list args, struct state_args* state, struct state_result* result);
void is_x_array(va_list args, struct state_args* state, struct state_result* result);
void is_X_array(va_list args, struct state_args* state, struct state_result* result);
void is_s_array(va_list args, struct state_args* state, struct state_result* result);
void is_c_array(va_list args, struct state_args* state, struct state_result* result);
void is_u_array(va_list args, struct state_args* state, struct state_result* result);
void unknown_identifiers(va_list args, struct state_args* state, struct state_result* result);
// array_handler_helpers *end*

void init_arrays(){
    //init init_handler_helpers
    for(int i=0; i<128; i++){
        init_handler_helpers[i]= &print_this_char;
    }
    init_handler_helpers['%']= &is_percent;
    
    //init percent_handler_helpers
    
    for(int i=0; i<128; i++){
        percent_handler_helpers[i]= &unknown_identifiers;
    }
    percent_handler_helpers['%']= &print_percent;
    percent_handler_helpers['A']= &is_A;
    percent_handler_helpers['d']= &is_d;
    percent_handler_helpers['b']= &is_b;
    percent_handler_helpers['o']= &is_o;
    percent_handler_helpers['x']= &is_x;
    percent_handler_helpers['X']= &is_X;
    percent_handler_helpers['s']= &is_s;
    percent_handler_helpers['c']= &is_c;
    percent_handler_helpers['u']= &is_u;
    percent_handler_helpers['0']= &is_0;
    percent_handler_helpers['1']= &is_1;
    percent_handler_helpers['2']= &is_2;
    percent_handler_helpers['3']= &is_3;
    percent_handler_helpers['4']= &is_4;
    percent_handler_helpers['5']= &is_5;
    percent_handler_helpers['6']= &is_6;
    percent_handler_helpers['7']= &is_7;
    percent_handler_helpers['8']= &is_8;
    percent_handler_helpers['9']= &is_9;
    percent_handler_helpers['-']= &is_minus;
    
    //init array_handler_helpers
    
    for(int i=0; i<128; i++){
        array_handler_helpers[i]= &unknown_identifiers;
    }
    array_handler_helpers['d']= &is_d_array;
    array_handler_helpers['b']= &is_b_array;
    array_handler_helpers['o']= &is_o_array;
    array_handler_helpers['x']= &is_x_array;
    array_handler_helpers['X']= &is_X_array;
    array_handler_helpers['s']= &is_s_array;
    array_handler_helpers['c']= &is_c_array;
    array_handler_helpers['u']= &is_u_array;
    
}
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
    init_arrays();
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
  struct state_args strgs;
  struct state_result result;

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
    init_handler_helpers[(int)*state->fs] (state, &result);
    return result;

}

struct state_result percent_handler(va_list args, struct state_args* state){
    struct state_result result;
    result.printed_chars=0;
    percent_handler_helpers[(int)*state->fs] (args, state, &result);
    return result;
}

struct state_result array_handler(va_list args, struct state_args* state){
    struct state_result result;
    result.printed_chars=0;
    array_handler_helpers[(int)*state->fs] (args, state, &result);
    return result;
}

// init_handler_helpers impl //start
void is_percent(struct state_args* state, struct state_result* result){
    result->next_state= st_printf_percent;
}
void print_this_char(struct state_args* state, struct state_result* result){
    putchar(*(state->fs));
    ++(result->printed_chars);
    result->next_state= st_printf_init;
}
// init_handler_helpers impl //end


// percent_handler_helpers impl //start
void print_percent(va_list args, struct state_args* state, struct state_result* result){
    putchar('%');
    ++(result->printed_chars);
    result->next_state = st_printf_init;   
}
void is_A(va_list args, struct state_args* state, struct state_result* result){
     result->next_state = st_printf_array;    
}
void is_d(va_list args, struct state_args* state, struct state_result* result){
    state->int_value = va_arg(args, int);
    state->min_ammount_value= getMinimalValue(state->min_ammount, *(state->min_ammount_idx));
    if(state->min_ammount_value==0 || numOfDigits(state->int_value)>=state->min_ammount_value){
    if(state->int_value>=0){
    (result->printed_chars) += print_int(state->int_value, 10, digit);
    }else{
    putchar('-');
    (result->printed_chars)++;
    (result->printed_chars) += print_int(-state->int_value, 10, digit);
    } 
    }else{
        
    state->spaces= state->min_ammount_value - numOfDigits(state->int_value);
        
    if(state->min_ammount[0]!='-' && state->min_ammount[0]!='0'){
    if(state->int_value>=0){
    (result->printed_chars) += print_int(state->int_value, 10, digit);
    }else{
    putchar('-');
    (result->printed_chars)++;
    (result->printed_chars) += print_int(-state->int_value, 10, digit);
    } 
    for(int i=0; i<state->spaces; i++){
    putchar(' ');
    }
    putchar('#');
    (result->printed_chars)+=state->spaces+1;
    }else if(state->min_ammount[0]=='0'){
                    
    if(state->int_value>=0){
    for(int i=0; i<state->spaces; i++){
    putchar('0');
    }
    (result->printed_chars)+=state->spaces;
    (result->printed_chars) += print_int(state->int_value, 10, digit);
    }else{
    putchar('-');
    (result->printed_chars)++;
    for(int i=0; i<state->spaces; i++){
    putchar('0');
    }
    (result->printed_chars)+=state->spaces;
    (result->printed_chars) += print_int(-state->int_value, 10, digit);
    } 
    }else{
    for(int i=0; i<state->spaces; i++){
    putchar(' ');
    }
    (result->printed_chars)+=state->spaces;
    if(state->int_value>=0){
    (result->printed_chars) += print_int(state->int_value, 10, digit);
    }else{
    putchar('-');
    (result->printed_chars)++;
    (result->printed_chars) += print_int(-state->int_value, 10, digit);
    } 
    }
    }
    *(state->min_ammount_idx)=0;
    state->min_ammount_value=0;
    state->min_ammount[0]='\0';
    result->next_state = st_printf_init;
    
}
void is_b(va_list args, struct state_args* state, struct state_result* result){
    state->int_value = va_arg(args, int);
    (result->printed_chars) += print_int(state->int_value, 2, digit);
    result->next_state = st_printf_init;
}
void is_o(va_list args, struct state_args* state, struct state_result* result){
    state->int_value = va_arg(args, int);
    (result->printed_chars) += print_int(state->int_value, 8, digit);
    result->next_state = st_printf_init;
}
void is_x(va_list args, struct state_args* state, struct state_result* result){
            state->int_value = va_arg(args, int);
            (result->printed_chars) += print_int(state->int_value, 16, digit);
            result->next_state = st_printf_init;
}
void is_X(va_list args, struct state_args* state, struct state_result* result){
                state->int_value = va_arg(args, int);
            (result->printed_chars) += print_int(state->int_value, 16, DIGIT);
            result->next_state = st_printf_init;
}
void is_s(va_list args, struct state_args* state, struct state_result* result){
             state->string_value = va_arg(args, char *);
            state->min_ammount_value= getMinimalValue(state->min_ammount, *(state->min_ammount_idx));
            if(state->min_ammount_value==0 || strlen(state->string_value)>=state->min_ammount_value){
                (result->printed_chars) += toy_printf(state->string_value);
            }else{
        
                state->spaces= state->min_ammount_value - strlen(state->string_value);
                if(state->min_ammount[0]!='-'){
                    (result->printed_chars) += toy_printf(state->string_value);
                    for(int i=0; i<state->spaces; i++){
                        putchar(' ');
                    }
                putchar('#');
                (result->printed_chars)+=state->spaces+1;
                }else{
                        for(int i=0; i<state->spaces; i++){
                        putchar(' ');
                    }
                (result->printed_chars)+=state->spaces;
                (result->printed_chars) += toy_printf(state->string_value);
                }
            }
            *(state->min_ammount_idx)=0;
            state->min_ammount_value=0;
            result->next_state = st_printf_init;   
}
void is_c(va_list args, struct state_args* state, struct state_result* result){
             state->char_value = (char)va_arg(args, int);
            putchar(state->char_value);
            ++(result->printed_chars);
            result->next_state = st_printf_init;   
}
void is_u(va_list args, struct state_args* state, struct state_result* result){
             state->int_value = va_arg(args, int);
            (result->printed_chars) += print_int(state->int_value, 10, digit);
            result->next_state = st_printf_init;   
}
void is_0(va_list args, struct state_args* state, struct state_result* result){
    state->min_ammount[(*state->min_ammount_idx)++]='0';
    result->next_state = st_printf_percent;   
}
void is_1(va_list args, struct state_args* state, struct state_result* result){
    state->min_ammount[(*state->min_ammount_idx)++]='1';
    result->next_state = st_printf_percent;   
}
void is_2(va_list args, struct state_args* state, struct state_result* result){
    state->min_ammount[(*state->min_ammount_idx)++]='2';
    result->next_state = st_printf_percent;   
}
void is_3(va_list args, struct state_args* state, struct state_result* result){
    state->min_ammount[(*state->min_ammount_idx)++]='3';
    result->next_state = st_printf_percent;   
}
void is_4(va_list args, struct state_args* state, struct state_result* result){
    state->min_ammount[(*state->min_ammount_idx)++]='4';
    result->next_state = st_printf_percent;   
}
void is_5(va_list args, struct state_args* state, struct state_result* result){
    state->min_ammount[(*state->min_ammount_idx)++]='5';
    result->next_state = st_printf_percent;   
}
void is_6(va_list args, struct state_args* state, struct state_result* result){
    state->min_ammount[(*state->min_ammount_idx)++]='6';
    result->next_state = st_printf_percent;   
}
void is_7(va_list args, struct state_args* state, struct state_result* result){
    state->min_ammount[(*state->min_ammount_idx)++]='7';
    result->next_state = st_printf_percent;   
}
void is_8(va_list args, struct state_args* state, struct state_result* result){
    state->min_ammount[(*state->min_ammount_idx)++]='8';
    result->next_state = st_printf_percent;   
}
void is_9(va_list args, struct state_args* state, struct state_result* result){
    state->min_ammount[(*state->min_ammount_idx)++]='9';
    result->next_state = st_printf_percent;   
}
void is_minus(va_list args, struct state_args* state, struct state_result* result){
    state->min_ammount[(*state->min_ammount_idx)++]='-';
    result->next_state = st_printf_percent;
    
}
void unknown_identifiers(va_list args, struct state_args* state, struct state_result* result){
    toy_printf("Unhandled format...\n", *(state->fs));
	exit(-1);
}
// percent_handler_helpers impl //end

//  array_handler_helpers impl //start
void is_d_array(va_list args, struct state_args* state, struct state_result* result){
                    state->integers_array= (int*)va_arg(args, int*);
                state->array_size= va_arg(args, int);
                (result->printed_chars)+= print_signed_int_arr(state->integers_array, state->array_size, 10, digit);
                result->next_state = st_printf_init;
}
void is_b_array(va_list args, struct state_args* state, struct state_result* result){
                    state->integers_array= (int*)va_arg(args, int*);
                state->array_size= va_arg(args, int);
                (result->printed_chars)+= print_unsigned_int_arr(state->integers_array, state->array_size, 2, digit);
                result->next_state = st_printf_init;
}
void is_o_array(va_list args, struct state_args* state, struct state_result* result){
                    state->integers_array= (int*)va_arg(args, int*);
                state->array_size= va_arg(args, int);
                (result->printed_chars)+= print_unsigned_int_arr(state->integers_array, state->array_size, 8, digit);
                result->next_state = st_printf_init;
}
void is_x_array(va_list args, struct state_args* state, struct state_result* result){
                    state->integers_array= (int*)va_arg(args, int*);
                state->array_size= va_arg(args, int);
                (result->printed_chars)+= print_unsigned_int_arr(state->integers_array, state->array_size, 16, digit);
                result->next_state = st_printf_init;
}
void is_X_array(va_list args, struct state_args* state, struct state_result* result){
                    state->integers_array= (int*)va_arg(args, int*);
                state->array_size= va_arg(args, int);
                (result->printed_chars)+= print_unsigned_int_arr(state->integers_array, state->array_size, 16, DIGIT);
                result->next_state = st_printf_init;
}
void is_s_array(va_list args, struct state_args* state, struct state_result* result){
                    state->strings_array= (char**)va_arg(args, char**);
                state->array_size= va_arg(args, int);
                (result->printed_chars)+= print_strings_arr(state->strings_array, state->array_size);
                result->next_state = st_printf_init;
}
void is_c_array(va_list args, struct state_args* state, struct state_result* result){
                    state->chars_array= va_arg(args, char*);
                state->array_size= va_arg(args, int);
                (result->printed_chars)+= print_chars_arr(state->chars_array, state->array_size);
                result->next_state = st_printf_init;
}
void is_u_array(va_list args, struct state_args* state, struct state_result* result){
                    state->integers_array= (int*)va_arg(args, int*);
                state->array_size= va_arg(args, int);
                (result->printed_chars)+= print_unsigned_int_arr(state->integers_array, state->array_size, 10, digit);
                result->next_state = st_printf_init;
}

// array_handler_helpers impl //end

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
