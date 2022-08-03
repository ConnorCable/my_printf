/*
This program needs to:
1. Write the argument to stdout
2. Ordinary characters need to be copied unchanged to the output
3. The percent sign + the following argument changes the behavior of the function:
    a. doux: converted to signed decimal (d), unsigned decimal (u), unsigned hexadecimal (x)
    b. c: the int argument is converted to an unsigned char, and the char is written
    c. s: the char* argument is expected to be a pointer to an array of character type. Characters from the array are written up to a terminating null character
    d. p: the void* pointer argument is printed in hexadecimal
4. Handle invalid argument types.
5. Handle variable number of arguments
*/
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h> 
#include <stdio.h>
#include <stdint.h>

/* 

SIMPLE FUNCTIONS

*/
char* my_downcase(char* param_1)
{
    int i;
    i = 0;

    while(param_1[i] != '\0'){
     //   printf("\ncomparing character %c\n", param_1[i]);
        if(param_1[i] >= 'A' && param_1[i] <= 'Z') {
            param_1[i] = param_1[i] + 32;
        }
        i++;
    }
    return param_1;

}

int my_strlen(char* param_1)
{
    int i = 0;
    while(param_1[i] != '\0'){
        i++;
    }

    return i;
}

void revstr(char *str1)  
{  
    int i;
    int len;
    int temp;  
    len = my_strlen(str1);
      
    for (i = 0; i < len/2; i++)  
    {  
        temp = str1[i];  
        str1[i] = str1[len - i - 1];  
        str1[len - i - 1] = temp;  
    }  
}  

int my_putchar(char c) {
  return write(1, &c, 1);
}

int my_putstr(char* param_1)
{
   int i = 0;
    while(param_1[i] != '\0'){
        my_putchar(param_1[i]);
        i++;
    }
    return i;
}

int get_number_digits(int number){ // get number of digits
    int count = 0;
    int n = number;
    while(n != 0){ 
        n /= 10;
        count++;
    }
    return count;
}


/* 

HELPER FUNCTIONS

*/

void put_digits_in_array(int number, int *array) // splits the integer apart into digits and places them into an array
{
    int count = get_number_digits(number) - 1; // # of spaces in array is -1 the # of digits

    while(count >= 0){ // put digits into array
        int digit = number % 10;
        number /= 10;
        //printf("\nadding %d to array at %d \n", digit, count);
        array[count] = digit;
        count--;
    }
    
}

char * process_signed(char * arr, int count){ // add '-' for signed integer
    char * signedarr = malloc(sizeof arr + 1);
    signedarr[0] = '-';
    count++;
    for(int i = 1; i <  count; i++){
        signedarr[i] = arr[i-1];
    }
    return signedarr;
}

char * digits_to_asciichar(int * array, int count, int flag){ // convers integers into their ascii character by adding 0 and casting to char
    char * chararr = malloc(count * sizeof(char));
    int i = 0;
    while(i < count){
        char digitischar = (char) array[i] + '0';
        chararr[i] = digitischar;
        i++;
    }
    if(flag){
        char * signedarr = process_signed(chararr, count);
        return signedarr;
    }
    return chararr;
    
}

/* 

CONVERTER FUNCTIONS FOR DIFFERENT BASES

*/

int *convert_octal(int num){ // converts the number to octal form
    if(num < 0){
        num *= -1;
    }
    int *oct = malloc(get_number_digits(num) * sizeof( num));
    for(int i = get_number_digits(num) - 1; num > 0; i--){
        oct[i] = num % 8;
        num = num / 8;
    }
    return oct;
}

char *convert_hex(int num){ // convert integer to hexadecimal
    char *hex = malloc(get_number_digits(num) * sizeof (int));
    int i = 0;
    while (num != 0) {
        int temp = 0;
        temp = num % 16;
        if (temp < 10) {
            hex[i] = temp + 48;
            i++;
        }
        else {
            hex[i] = temp + 55;
            i++;
        }
        num = num / 16;
    }
    revstr(hex);
    return hex;

}

char *convert_hex_p(long num){ // convert pointer to hexadecimal
    char *hex = malloc(get_number_digits(num) * sizeof (long));
    int i = 0;
    while (num != 0) {
        int temp = 0;
        temp = num % 16;
        if (temp < 10) {
            hex[i] = temp + 48;
            i++;
        }
        else {
            hex[i] = temp + 55;
            i++;
        }
        num = num / 16;
    }
    revstr(hex);
    my_downcase(hex);
    return hex;

}

/* 

PARSER FUNCTIONS FOR DIFFERENT FLAGS

*/

int parse_d_or_u(int num){
    int signedflag = 0;
    if(num < 0){ // determine if num is signed, behavior of digits_to_asciichar is changed if so
        num *=-1;
        signedflag = 1;
    }
    int numberdigits = get_number_digits(num);
    int *intarray = malloc(numberdigits * sizeof num);
    put_digits_in_array(num, intarray); 
    char *writeabledigits = digits_to_asciichar(intarray, numberdigits, signedflag);
    int digitswritten = my_putstr(writeabledigits);
    return digitswritten;
}

int parse_o(int num){
    int signedflag = 0;
    int *num2oct = convert_octal(num); // convert to octal
    int numberdigits = get_number_digits(num); // get number of digits
    char *writeabledigits = digits_to_asciichar(num2oct, numberdigits, signedflag); // turn digits into writeable string
    int digitswritten = my_putstr(writeabledigits);
    return digitswritten;

}

int parse_x(int num){
    if(num < 0){
        num *= -1;
    }
    char *hex = convert_hex_p(num); // convert to hexadecimal
    int digitswritten = my_putstr(hex);
    return digitswritten;
}


int parse_p(unsigned long num){ // pointers need long to parse correctly
  // printf("%d", num);
   char * hex = convert_hex_p((uintptr_t) num); // convert to hexadecimal ( unique function for p )
   char * address = "0x7"; // initialize whatever this is
   my_putstr(address);
   int digitswritten = my_putstr(hex);
   return digitswritten + 3;

}

/* 

DISPATCHER FUNCTION FLAG_DETECTOR

*/

int flag_detector(char ch, int arg ){ // used to detect which flag is used for formatting (douxc) return digits written
    int digitswritten = 0;
    switch(ch){
        case 'd' :
            digitswritten += parse_d_or_u(arg);
            break;
        case 'o':
            digitswritten += parse_o(arg);
            break;
        case 'u':
            digitswritten += parse_d_or_u(arg);
            break;
        case 'x':
            digitswritten += parse_x((long)arg);
            break;
        case 'c' :
            my_putchar(arg);
            digitswritten++;
            break;

    }
    return digitswritten;
}

/* 

MY_PRINTF

*/

int my_printf(char * restrict string, ...){
    int length = my_strlen(string);
    int formatcounter = 0; // used to count the number of '%'s present in the string
    va_list list; // initialize multivariate argument list
    va_start(list, string); // initialize list to be read with the arguments provided
    int writecounter = 0; // count number of bytes written to return

    for(int i = 0; i < length; i++){ // get number of % in string
        if(string[i] == '%'){
            formatcounter++;
        }

    }
    // This function needs to be long because I cannot access va_arg elsewere, va_list is initialized here.
    for(int i = 0; i < length; i++){ // if the set of characters are a '%flag' , parse them in their own functions. If not, write it to the string
        if(string[i] == '%' && string[i+1] == 's'){ // detect string flag, easy implementation
            writecounter += my_putstr(va_arg(list, char *));
            i++;
            continue;
        }
        if(string[i] == '%' && string[i+1] == 'p'){ // detect void pointer
            writecounter += parse_p(va_arg(list, unsigned long));
            i++;
            continue;
        }
        if(string[i] == '%'){ // detect douxc
            writecounter += flag_detector(string[i+1], va_arg(list, int));
            i++;
            continue;
        }
        else{
            write(1,&string[i],1);
            writecounter++;
        }
    }
    return writecounter;
}


int main(){
    
    int integer_1 = -123;
    int integer_2 = 456;
    int *ptr_int1 = &integer_1;
    int *ptr_int2 = &integer_2;
    char ch_1 = 'a';
    char ch_2 = 'b';
    char str_1[6] = "hello\0"; 
    char str_2[6] = "world\0";

   my_printf("test string u:%u d:%d o:%o x:%x c:%c%c s:%s %s p:%p\n", integer_1, integer_2, integer_1, integer_1, ch_1, ch_2, str_1, str_2, ptr_int1);
   return 0;
}
